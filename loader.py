import numpy as np


def loadfile(file, threshold=100000, flipscores=False, loadmax=float("inf")):
    result = []
    with open(file, "r") as f:
        for line in f:
            fen, scores = line.split(" |  ")
            scorelist = scores.split(" ")
            scorelist = [int(x) for x in scorelist]
            board, side, castling, dash, halfmove, turn, winner = fen.split(" ")
            fen = "{0} {1} {2} - {3} {4}".format(board, side, castling, halfmove, turn)
            if side == 'b' and flipscores:
                scorelist = [-x for x in scorelist]
            if abs(scorelist[-1]) <= threshold:
                result.append((fen, winner, scorelist))
            if len(result) >= loadmax:
                return result
    return result

def savefens(vals, file):
    with open(file, "w") as f:
        for val in vals:
            line = val[0] + " " + val[1] + "\n"
            f.write(line)

if __name__ == "__main__":

    file = "/home/exa/Documents/Ethereal-master/FENS-Scored"
    vals = loadfile(file)
    print(vals[:10])