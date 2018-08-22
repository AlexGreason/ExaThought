import chess
import random
import numpy as np
import pickle
import collections
from numpy import array

def rep_count(board):
    transposition_key = board._transposition_key()
    transpositions = collections.Counter()
    transpositions.update((transposition_key,))

    # Count positions.
    switchyard = collections.deque()
    while board.move_stack:
        move = board.pop()
        switchyard.append(move)

        if board.is_irreversible(move):
            break

        transpositions.update((board._transposition_key(),))
        if transpositions[transposition_key] > 1:
            break

    while switchyard:
        board.push(switchyard.pop())

    return transpositions[transposition_key]

def is_drawn(board):
    if board.can_claim_fifty_moves():
        return True
    if rep_count(board) >= 3:
        return True
    return False


def bitarray_to_numpy(bitarray):
    str = np.binary_repr(bitarray, width=64)
    arr = np.fromstring(str, np.int8) - 48
    arr = arr.reshape((8, 8))
    arr = np.flip(arr, axis=1)
    return arr.astype(np.float32)

def diffpieces(board, type):
    ours = np.sum(bitarray_to_numpy(board.pieces_mask(type, board.turn)))
    theirs = np.sum(bitarray_to_numpy(board.pieces_mask(type, not board.turn)))
    return ours - theirs

def evaluateBoard(board):
    pawnval, knightval, bishopval, rookval, queenval = PSQTables[0]

    eval = 0
    eval += pawnval * diffpieces(board, chess.PAWN)
    eval += knightval * diffpieces(board, chess.KNIGHT)
    eval += bishopval * diffpieces(board, chess.BISHOP)
    eval += rookval * diffpieces(board, chess.ROOK)
    eval += queenval * diffpieces(board, chess.QUEEN)

    eval += psqt(board, board.turn)
    eval -= psqt(board, not board.turn)
    #eval +=  # tempo
    return eval

def psqt(board, side):
    sum = 0
    for type in [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN, chess.KING]:
        locations = bitarray_to_numpy(board.pieces_mask(type, side))
        if not side:
            locations = np.flip(locations, axis=0)
        sum += np.sum(PSQTables[type] * locations)
    return sum

PSQTables = \
{0: array([ 148,  236,  279,  515, 1279]),

 1: array([[  0,   0,   0,   0,   0,   0,   0,   0],
        [134, 176, 195, 197, 343, 307, 259, 220],
        [-49, -12,   0, -45,   0, 146, 120,   6],
        [-56, -45, -36, -45, -54, -45, -42, -60],
        [-77, -66, -59, -49, -57, -57, -71, -80],
        [-75, -63, -52, -56, -49, -50, -52, -69],
        [-77, -58, -65, -76, -74, -64, -49, -79],
        [  0,   0,   0,   0,   0,   0,   0,   0]]),

 2: array([[-110, -164,  -25,  -44,   23, -122,   76, -162],
        [ -34,   -8,   15,  -10,   99,   49,  -39,  -23],
        [   3,    8,   35,   36,   92,  136,   18,   47],
        [  -9,   19,   21,   52,   53,  101,   89,  112],
        [  -6,   -1,   13,    8,   31,   22,    4,    1],
        [ -26,    0,    5,   24,   32,   26,   26,  -16],
        [ -40,  -15,    1,   -7,   -9,   -7,   -2,  -10],
        [ -83,  -33,  -35,  -23,  -30,    7,  -32,  -43]]),

 3: array([[ -65,  -85, -104,  -30,    4,  -86,  -21,   24],
        [ -30,  -16,   36,  -37,   -1,   30,  -26,  -38],
        [ -32,    5,   -3,   10,   24,   80,   68,   60],
        [   6,    0,   27,   57,   71,   24,   19,   31],
        [ -11,    3,   25,   26,   48,    5,   13,   26],
        [  -3,    7,   21,   25,    7,   25,   16,   15],
        [ -10,   19,   14,  -11,    7,    0,   29,  -17],
        [ -22,   13,  -16,  -23,  -19,  -14,  -30,  -37]]),

 4: array([[-16,  63,  40,  56,  24,  49,  72,  56],
        [ 65,  71,  93,  75,  92, 143, 120, 135],
        [  2, -36,  -6,  29,   2,  17,  33,  37],
        [-38, -62, -23, -17,   0,  31,  42,   1],
        [-68, -60, -39, -16, -24, -28,   2, -30],
        [-67, -41, -42, -36, -26,   4,  58, -10],
        [-52, -46, -34, -39, -35, -17,   3, -68],
        [-34, -33, -18,  -9,  -4,  -6, -26, -49]]),

 5: array([[ -86,  -90,  -91,  -36,  -52,  103,  116,  -99],
        [ -77,    9,   67,  -12,  204,   71,  -40,  -58],
        [ -81, -120, -104,  -65,    4,  248,  121,  175],
        [ -67,  -64,  -86,    4,  118,  129,  158,  155],
        [ -43,  -80,  -13,   57,   26,   57,   82,   53],
        [ -58,   -6,   64,    6,   13,   54,  123,   71],
        [-100,    0,   -6,  -11,    4,    1,   56,   -4],
        [ -20,  -48,  -30,   -4,  -27,  -12,    1,  -18]]),

 6: array([[ -314,  -143,    85,   -89,   -96,   167,  -135, -1363],
        [  -97,   129,   223,    43,   132,   249,    70,  -259],
        [  100,   161,   219,   158,   282,   355,   260,  -125],
        [  -94,    78,   149,   126,    42,   147,    11,  -179],
        [  -62,    84,    88,    51,    54,    83,    17,  -342],
        [  -34,    45,   103,    71,    60,    52,    53,   -95],
        [  -17,    58,    53,    30,    18,    29,    23,   -38],
        [ -220,   -23,   -23,   -50,   -34,   -87,   -46,  -198]])}
#def loadtables():
#    global PSQTables
#    PSQTables = pickle.load(open("psqt-1M-adj.dmp", "rb"))
#loadtables()

if __name__ == "__main__":
    a = chess.Board()
    a.push_uci("c2c4")
    print(evaluateBoard(a))