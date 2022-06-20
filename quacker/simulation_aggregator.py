import pandas as pd

base_dir = "Static1_v_Static2_results/"

out_file_name = "Static1_v_Static2_results"
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


titles = ["Player", "Rack", "Play", "Score gained", "Cumulative score"]
for i in range(1, 1001):
    table = []
    p1_final_score = 0
    p2_final_score = 0

    filename = str(i)
    file_location = base_dir + filename

    p1_name = ""
    p2_name = ""
    try:
        with open(file_location, 'r') as f:
            f.readline()
            p1_name = f.readline().split()[1]
            p2_name = f.readline().split()[1]
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
                elif len(str_record) == 5:  # Exchange
                    table.append([
                        str_record[0][1:-1]  # Remove the '>' and ':'
                        , str_record[1]
                        , Exchange(str_record[2][1:])  # Remove the '-'
                        , int(str_record[3])  # Should be a '0'
                        , int(str_record[4])
                    ])
                else:  # len(str_record) == 4; Last line - score gain due to other player having tiles
                    added_score = int(str_record[3]) / 2
                    table.append([
                        str_record[0][1:-1]  # Remove the '>' and ':'
                        , str_record[1]     # Should be the empty string
                        , Final(str_record[2][1:-1])  # Remove the '(' and ')'
                        , added_score
                        , int(str_record[4])
                    ])
    except Exception as e:
        print("Something went wrong:")
        print(e)
        continue

    if table[-1][0] == p1_name:
        p1_final_score = table[-1][-1] + added_score
        p2_final_score = table[-3][-1] - added_score
    else:
        p1_final_score = table[-3][-1] + added_score
        p2_final_score = table[-1][-1] - added_score

    df = pd.DataFrame(table, columns=titles)
    print(p1_name + "'s final score was " + str(p1_final_score))
    print(p2_name + "'s final score was " + str(p2_final_score))
    print(df)
