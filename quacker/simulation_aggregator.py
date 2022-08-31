import pandas as pd
import numpy as np
from scipy import stats
import sys

DEBUG = False
STOP_AFTER_N_MANY_NOT_FOUND = 5
out_file_name = "results"

if DEBUG:
    titles = ["Player", "Rack", "Play", "Score gained", "Cumulative score"]

dirs_and_names = [
    ("Static1_v_Static2_results/", "Static1", "Static2"),
    ("Greedy1_v_Greedy2_results/", "Greedy1", "Greedy2"),
    ("Champ_v_Greedy_results/", "Champ", "Greedy"),
    ("Champ1_v_Champ2_results/", "Champ1", "Champ2"),
    ("Static_v_Champ_results/", "Champ", "Static"),
    ("Static_v_Greedy_results/", "Static", "Greedy"),
]


class Play:
    def __init__(self):
        self.is_exchange = False
        self.is_placement = False


class Exchange(Play):
    def __init__(self, letters):
        Play.__init__(self)
        self.letters = letters
        self.is_exchange = True

    def __str__(self):
        return "-" + self.letters


class Placement(Play):
    def __init__(self, location, word):
        Play.__init__(self)
        self.location = location
        self.word = word
        self.is_placement = True

    def __str__(self):
        return self.location + " " + self.word


class Final(Play):
    def __init__(self, leftovers):
        Play.__init__(self)
        self.leftovers = leftovers

    def __str__(self):
        return "(" + self.leftovers + ")"


def whoWins(p1, p2):
    if p1[1] > p2[1]:
        return p1[0]
    elif p1[1] < p2[1]:
        return p2[0]
    else:
        return "Draw"


for dir_and_names in dirs_and_names:

    base_dir = dir_and_names[0]
    p1_name = dir_and_names[1]
    p2_name = dir_and_names[2]

    if DEBUG:
        print("-------------------------------------")
        print(base_dir)
        print("-------------------------------------")

    out_file = base_dir + out_file_name

    p1_wins = 0
    p2_wins = 0
    draws = 0

    turns_per_game = []

    p1_score_means_per_turn_per_game = []
    p2_score_means_per_turn_per_game = []

    p1_bingos_per_game = []
    p2_bingos_per_game = []

    p1_scores_per_game = []
    p2_scores_per_game = []

    not_found_count = 0
    for i in range(1, 2048):
        table = []
        p1_final_score = 0
        p2_final_score = 0
        added_score = 0

        filename = str(i)
        file_location = base_dir + filename

        try:
            with open(file_location, 'r') as f:
                f.readline()
                f.readline()
                f.readline()
                for line in f:
                    line = line.strip()
                    str_record = line.split(" ")
                    if len(str_record) == 6:  # Placement
                        table.append([
                            str_record[0][1:-1]  # Remove the '>' and ':'
                            , str_record[1]
                            , Placement(str_record[2], str_record[3])
                            , int(str_record[4])
                            , int(str_record[5])
                        ])
                    elif len(str_record[
                                 1]) == 0:  # Last line - score gain due to other player having tiles
                        added_score = int(int(str_record[3]) / 2)
                        table.append([
                            str_record[0][1:-1]  # Remove the '>' and ':'
                            , str_record[1]  # Should be the empty string
                            , Final(str_record[2][1:-1])
                            # Remove the '(' and ')'
                            , added_score
                            , int(str_record[4]) - added_score
                        ])
                    elif len(str_record) == 5:  # Exchange
                        table.append([
                            str_record[0][1:-1]  # Remove the '>' and ':'
                            , str_record[1]
                            , Exchange(str_record[2][1:])  # Remove the '-'
                            , int(str_record[3])  # Should be a '0'
                            , int(str_record[4])
                        ])
        except FileNotFoundError as e:
            not_found_count += 1
            print(e)
            if not_found_count >= STOP_AFTER_N_MANY_NOT_FOUND:
                break
            continue
        except Exception as e:
            print("Something went wrong:")
            print(e)
            print("In file " + str(i))
            continue

        not_found_count = 0

        if table[-1][0] == p1_name:
            p1_final_score = table[-1][-1]
            p2_final_score = table[-3][-1] - added_score
        else:
            p1_final_score = table[-3][-1] - added_score
            p2_final_score = table[-1][-1]

        p1 = (p1_name, p1_final_score)
        p2 = (p2_name, p2_final_score)

        if whoWins(p1, p2) == p1_name:
            p1_wins += 1
        elif whoWins(p1, p2) == p2_name:
            p2_wins += 1
        else:
            if DEBUG:
                print("draw at {0}".format(i))
            draws += 1

        p1_scores_per_game.append(p1_final_score)
        p2_scores_per_game.append(p2_final_score)

        p1_number_of_turns = 0
        p2_number_of_turns = 0
        p1_bingos = 0
        p2_bingos = 0
        for row in table[:-1]:
            p1_number_of_turns += int(row[0] == p1_name)
            p2_number_of_turns += int(row[0] == p2_name)
            p1_bingos += (int(
                row[-3].is_placement and len(
                    row[-3].word.replace('.', '')) == 7 and
                row[0] == p1_name))
            p2_bingos += (int(
                row[-3].is_placement and len(
                    row[-3].word.replace('.', '')) == 7 and
                row[0] == p2_name))
        p1_bingos_per_game.append(p1_bingos)
        p2_bingos_per_game.append(p2_bingos)

        p1_mean_score_per_turn = p1_final_score / p1_number_of_turns
        p2_mean_score_per_turn = p2_final_score / p2_number_of_turns
        p1_score_means_per_turn_per_game.append(p1_mean_score_per_turn)
        p2_score_means_per_turn_per_game.append(p2_mean_score_per_turn)
        turns_per_game.append(p1_number_of_turns + p2_number_of_turns)

        if DEBUG:
            df = pd.DataFrame(table, columns=titles)
            print(p1_name + "'s final score was " + str(p1_final_score))
            print(p2_name + "'s final score was " + str(p2_final_score))
            print("The winner is " + whoWins(p1, p2) + "\n")
            print(df)


    def calculate_stats(ls):
        ret = {}
        ret["mean"] = np.mean(ls)
        ret["median"] = np.median(ls)
        ret["mode"] = stats.mode(ls)[0][0]
        ret["mode_count"] = stats.mode(ls)[1][0]
        ret["std"] = np.std(ls)
        ret["range"] = max(ls) - min(ls)
        ret["max"] = max(ls)
        ret["min"] = min(ls)
        ret["q1"] = np.percentile(ls, 25)
        ret["q3"] = np.percentile(ls, 75)
        ret["iqr"] = ret["q3"] - ret["q1"]

        return ret


    number_of_games = len(p1_scores_per_game)
    non_draw_games = number_of_games - draws

    number_of_turns_stats = calculate_stats(turns_per_game)

    p1_score_stats = calculate_stats(p1_scores_per_game)
    p2_score_stats = calculate_stats(p2_scores_per_game)
    scores_combined_mean = (p1_score_stats["mean"] + p2_score_stats["mean"]) / 2

    p1_mean_turn_score_stats = calculate_stats(p1_score_means_per_turn_per_game)
    p2_mean_turn_score_stats = calculate_stats(p2_score_means_per_turn_per_game)
    mean_score_per_turn_combined_mean = (p1_mean_turn_score_stats["mean"] +
                                         p2_mean_turn_score_stats["mean"]) / 2

    p1_bingos_stats = calculate_stats(p1_bingos_per_game)
    p2_bingos_stats = calculate_stats(p2_bingos_per_game)
    bingos_combined_mean = (p1_bingos_stats["mean"] + p2_bingos_stats[
        "mean"]) / 2

    with open(base_dir + out_file_name, 'w') as f:
        f.write(
            "Generated by 'simulation_aggregator.py'\n---------------------------------------------\n\n")
        f.write("A total of {0} games were played, with {1} draws.\n".format(
            number_of_games, draws))

        f.write(
            "The mode number of turns per game is {0} with a count of {1}\n".format(
                number_of_turns_stats["mode"],
                number_of_turns_stats["mode_count"]))
        f.write("The mean number of turns per game is {0}\n".format(
            number_of_turns_stats["mean"]))
        f.write(
            "The standard deviation of number of turns per game is {0}\n".format(
                number_of_turns_stats["std"]))
        f.write("The median number of turns per game is {0}\n".format(
            number_of_turns_stats["median"]))
        f.write(
            "The inter-quartile range of number of turns per game is {0} - from {1} to {2}\n".format(
                number_of_turns_stats["iqr"], number_of_turns_stats["q1"],
                number_of_turns_stats["q3"]))
        f.write(
            "The range of number of turns per game is {0} - from {1} to {2}\n\n".format(
                number_of_turns_stats["range"], number_of_turns_stats["min"],
                number_of_turns_stats["max"]))

        f.write(p1_name + " wins " + str(
            p1_wins) + " times ({1}% of games), which is {0}% of all non-draw games.\n".format(
            100 * p1_wins / non_draw_games, 100 * p1_wins / number_of_games))
        f.write(p2_name + " wins " + str(
            p2_wins) + " times ({1}% of games), which is {0}% of all non-draw games.\n".format(
            100 * p2_wins / non_draw_games, 100 * p2_wins / number_of_games))
        f.write(
            "There were " + str(
                draws) + " draws ({0}% of all games).\n\n".format(
                100 * draws / number_of_games))

        f.write("{0}'s mode score is {1} with a count of {2}\n".format(p1_name,
                                                                       p1_score_stats[
                                                                           "mode"],
                                                                       p1_score_stats[
                                                                           "mode_count"]))
        f.write(
            "{0}'s mean score is {1}\n".format(p1_name, p1_score_stats["mean"]))
        f.write("{0}'s standard deviation in score is {1}\n".format(p1_name,
                                                                    p1_score_stats[
                                                                        "std"]))
        f.write(
            "{0}'s median score is {1}\n".format(p1_name,
                                                 p1_score_stats["median"]))
        f.write(
            "{0}'s inter-quartile range score is {1} - from {2} to {3}\n".format(
                p1_name, p1_score_stats["iqr"], p1_score_stats["q1"],
                p1_score_stats["q3"]))
        f.write(
            "{0}'s range in score is {1} - from {2} to {3}\n".format(p1_name,
                                                                     p1_score_stats[
                                                                         "range"],
                                                                     p1_score_stats[
                                                                         "min"],
                                                                     p1_score_stats[
                                                                         "max"]))
        f.write("{0}'s mode score is {1} with a count of {2}\n".format(p2_name,
                                                                       p2_score_stats[
                                                                           "mode"],
                                                                       p2_score_stats[
                                                                           "mode_count"]))
        f.write(
            "{0}'s mean score is {1}\n".format(p2_name, p2_score_stats["mean"]))
        f.write("{0}'s standard deviation in score is {1}\n".format(p2_name,
                                                                    p2_score_stats[
                                                                        "std"]))
        f.write(
            "{0}'s median score is {1}\n".format(p2_name,
                                                 p2_score_stats["median"]))
        f.write(
            "{0}'s inter-quartile range score is {1} - from {2} to {3}\n".format(
                p2_name, p2_score_stats["iqr"], p2_score_stats["q1"],
                p2_score_stats["q3"]))
        f.write(
            "{0}'s range in score is {1} - from {2} to {3}\n".format(p2_name,
                                                                     p2_score_stats[
                                                                         "range"],
                                                                     p2_score_stats[
                                                                         "min"],
                                                                     p2_score_stats[
                                                                         "max"]))
        f.write(
            "They have a combined mean of {0}\n\n".format(scores_combined_mean))

        f.write(
            "{0}'s mean score per turn mode is {1} with a count of {2}\n".format(
                p1_name, p1_mean_turn_score_stats["mode"],
                p1_mean_turn_score_stats["mode_count"]))
        f.write("{0}'s mean score per turn mean is {1}\n".format(p1_name,
                                                                 p1_mean_turn_score_stats[
                                                                     "mean"]))
        f.write(
            "{0}'s mean score per turn standard deviation is {1}\n".format(
                p1_name,
                p1_mean_turn_score_stats[
                    "std"]))
        f.write("{0}'s mean score per turn median is {1}\n".format(p1_name,
                                                                   p1_mean_turn_score_stats[
                                                                       "median"]))
        f.write(
            "{0}'s mean score per turn inter-quartile range is {1} - from {2} to {3}\n".format(
                p1_name, p1_mean_turn_score_stats["iqr"],
                p1_mean_turn_score_stats["q1"], p1_mean_turn_score_stats["q3"]))
        f.write(
            "{0}'s mean score per turn range is {1} - from {2} to {3}\n".format(
                p1_name, p1_mean_turn_score_stats["range"],
                p1_mean_turn_score_stats["min"],
                p1_mean_turn_score_stats["max"]))
        f.write(
            "{0}'s mean score per turn mode is {1} with a count of {2}\n".format(
                p2_name, p2_mean_turn_score_stats["mode"],
                p2_mean_turn_score_stats["mode_count"]))
        f.write("{0}'s mean score per turn mean is {1}\n".format(p2_name,
                                                                 p2_mean_turn_score_stats[
                                                                     "mean"]))
        f.write(
            "{0}'s mean score per turn standard deviation is {1}\n".format(
                p2_name,
                p2_mean_turn_score_stats[
                    "std"]))
        f.write("{0}'s mean score per turn median is {1}\n".format(p2_name,
                                                                   p2_mean_turn_score_stats[
                                                                       "median"]))
        f.write(
            "{0}'s mean score per turn inter-quartile range is {1} - from {2} to {3}\n".format(
                p2_name, p2_mean_turn_score_stats["iqr"],
                p2_mean_turn_score_stats["q1"], p2_mean_turn_score_stats["q3"]))
        f.write(
            "{0}'s mean score per turn range is {1} - from {2} to {3}\n".format(
                p2_name, p2_mean_turn_score_stats["range"],
                p2_mean_turn_score_stats["min"],
                p2_mean_turn_score_stats["max"]))
        f.write("They have a combined mean of {0}\n\n".format(
            mean_score_per_turn_combined_mean))

        f.write(
            "{0} has a mode of {1} bingos per game with a count of {2}.\n".format(
                p1_name, p1_bingos_stats["mode"],
                p1_bingos_stats["mode_count"]))
        f.write("{0} has a mean of {1} bingos per game.\n".format(p1_name,
                                                                  p1_bingos_stats[
                                                                      "mean"]))
        f.write(
            "{0}'s standard deviation in bingos per game is {1}.\n".format(
                p1_name,
                p1_bingos_stats[
                    "std"]))
        f.write("{0} has a median of {1} bingos per game.\n".format(p1_name,
                                                                    p1_bingos_stats[
                                                                        "median"]))
        f.write(
            "{0}'s inter-quartile range in bingos per game is {1} - from {2} to {3}.\n".format(
                p1_name, p1_bingos_stats["iqr"], p1_bingos_stats["q1"],
                p1_bingos_stats["q3"]))
        f.write(
            "{0}'s range in bingos per game is {1} - from {2} to {3}.\n".format(
                p1_name, p1_bingos_stats["range"], p1_bingos_stats["min"],
                p1_bingos_stats["max"]))
        f.write(
            "{0} has a mode of {1} bingos per game with a count of {2}.\n".format(
                p2_name, p2_bingos_stats["mode"],
                p2_bingos_stats["mode_count"]))
        f.write("{0} has a mean of {1} bingos per game.\n".format(p2_name,
                                                                  p2_bingos_stats[
                                                                      "mean"]))
        f.write(
            "{0}'s standard deviation in bingos per game is {1}.\n".format(
                p2_name,
                p2_bingos_stats[
                    "std"]))
        f.write("{0} has a median of {1} bingos per game.\n".format(p2_name,
                                                                    p2_bingos_stats[
                                                                        "median"]))
        f.write(
            "{0}'s inter-quartile range in bingos per game is {1} - from {2} to {3}.\n".format(
                p2_name, p2_bingos_stats["iqr"], p2_bingos_stats["q1"],
                p2_bingos_stats["q3"]))
        f.write(
            "{0}'s range in bingos per game is {1} - from {2} to {3}.\n".format(
                p2_name, p2_bingos_stats["range"], p2_bingos_stats["min"],
                p2_bingos_stats["max"]))
        f.write(
            "They have a combined mean of {0}\n".format(bingos_combined_mean))
