import pandas as pd

DEBUG = False

base_dir = "Champ1_v_Champ2_results/"
p1_name = "Champ1"
p2_name = "Champ2"

# base_dir = "Static_v_Champ_results/"
# p1_name = "Champ"
# p2_name = "Static"

# base_dir = "Static1_v_Static2_results/"
# p1_name = "Static1"
# p2_name = "Static2"

out_file_name = "results"
out_file = base_dir + out_file_name


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


if DEBUG:
    titles = ["Player", "Rack", "Play", "Score gained", "Cumulative score"]

p1_wins = 0
p2_wins = 0
draws = 0

number_of_games = 0
p1_total_score = 0
p2_total_score = 0
p1_score_averages_per_turn = 0
p2_score_averages_per_turn = 0
p1_scores_over_51 = 0
p2_scores_over_51 = 0
total_number_of_turns = 0
for i in range(1, 1001):
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
                elif len(str_record[1]) == 0:  # Last line - score gain due to other player having tiles
                    added_score = int(int(str_record[3]) / 2)
                    table.append([
                        str_record[0][1:-1]  # Remove the '>' and ':'
                        , str_record[1]     # Should be the empty string
                        , Final(str_record[2][1:-1])  # Remove the '(' and ')'
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
    except Exception as e:
        print("Something went wrong:")
        print(e)
        continue

    number_of_games += 1

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

    p1_total_score += p1_final_score
    p2_total_score += p2_final_score

    p1_number_of_turns = 0
    p2_number_of_turns = 0
    for row in table[:-1]:
        p1_number_of_turns += row[0] == p1_name
        p2_number_of_turns += row[0] == p2_name
        p1_scores_over_51 += row[-2] > 50 and row[0] == p1_name
        p2_scores_over_51 += row[-2] > 50 and row[0] == p2_name
    p1_average_score_per_turn = p1_final_score / p1_number_of_turns
    p2_average_score_per_turn = p2_final_score / p2_number_of_turns
    p1_score_averages_per_turn += p1_average_score_per_turn
    p2_score_averages_per_turn += p2_average_score_per_turn
    total_number_of_turns += p1_number_of_turns + p2_number_of_turns

    if DEBUG:
        df = pd.DataFrame(table, columns=titles)
        print(p1_name + "'s final score was " + str(p1_final_score))
        print(p2_name + "'s final score was " + str(p2_final_score))
        print("The winner is " + whoWins(p1, p2) + "\n")
        print(df)


with open(base_dir + out_file_name, 'w') as f:
    non_draws = number_of_games - draws
    average_total_number_of_turns = total_number_of_turns / number_of_games
    f.write("A total of {0} games were played, with {1} draws.\n".format(number_of_games, draws))
    f.write("The average number of turns per game is {0}\n\n".format(average_total_number_of_turns))
    f.write(p1_name + " wins " + str(p1_wins) + " times ({1}% of games), which is {0}% of all non-draw games.\n".format(100*p1_wins/non_draws, 100*p1_wins/number_of_games))
    f.write(p2_name + " wins " + str(p2_wins) + " times ({1}% of games), which is {0}% of all non-draw games.\n".format(100*p2_wins/non_draws, 100*p2_wins/number_of_games))
    f.write("There were " + str(draws) + " draws ({0}% of all games).\n\n".format(100*draws/number_of_games))

    p1_average_score = p1_total_score / number_of_games
    p2_average_score = p2_total_score / number_of_games
    combined_average = (p1_average_score + p2_average_score) / 2
    f.write("{0}'s average score is {1}\n".format(p1_name, p1_average_score))
    f.write("{0}'s average score is {1}\n".format(p2_name, p2_average_score))
    f.write("They have a combined average of {0}\n\n".format(combined_average))

    p1_score_averages_per_turn_average = p1_score_averages_per_turn / number_of_games
    p2_score_averages_per_turn_average = p2_score_averages_per_turn / number_of_games
    combined_average = (p1_score_averages_per_turn_average + p2_score_averages_per_turn_average) / 2
    f.write("{0}'s average score per turn is {1}\n".format(p1_name, p1_score_averages_per_turn_average))
    f.write("{0}'s average score per turn is {1}\n".format(p2_name, p2_score_averages_per_turn_average))
    f.write("They have a combined average of {0}\n\n".format(combined_average))

    p1_average_bingos_per_game = p1_scores_over_51 / number_of_games
    p2_average_bingos_per_game = p2_scores_over_51 / number_of_games
    combined_average = (p1_average_bingos_per_game + p2_average_bingos_per_game) / 2
    f.write("{0} has an average of {1} times when the move scores >50 points per game.\n".format(p1_name, p1_average_bingos_per_game))
    f.write("{0} has an average of {1} times when the move scores >50 points per game.\n".format(p2_name, p2_average_bingos_per_game))
    f.write("They have a combined average of {0}\n".format(combined_average))

