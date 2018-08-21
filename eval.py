import chess
import random
import numpy as np


def bitarray_to_numpy(bitarray):
    str = np.binary_repr(bitarray, width=64)
    arr = np.fromstring(str, np.int8) - 48
    arr = arr.reshape((8, 8))
    return arr.astype(np.float32)

def diffpieces(board, type):
    ours = np.sum(bitarray_to_numpy(board.pieces_mask(type, board.turn)))
    theirs = np.sum(bitarray_to_numpy(board.pieces_mask(type, not board.turn)))
    return ours - theirs

def evaluateBoard(board):
    pawnval = 1
    knightval = 3
    bishopval = 3
    rookval = 5
    queenval = 9

    eval = 0
    eval += pawnval * diffpieces(board, chess.PAWN)
    eval += knightval * diffpieces(board, chess.KNIGHT)
    eval += bishopval * diffpieces(board, chess.BISHOP)
    eval += rookval * diffpieces(board, chess.ROOK)
    eval += queenval * diffpieces(board, chess.QUEEN)
    return eval

if __name__ == "__main__":
    a = chess.Board()
    print(evaluateBoard(a))