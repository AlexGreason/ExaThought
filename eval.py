import chess
import random
import numpy as np
import pickle
import collections

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
    pawnval, knightval, bishopval, rookval, queenval = 1, 3, 3, 5, 9 #PSQTables[0]

    eval = 0
    eval += pawnval * diffpieces(board, chess.PAWN)
    eval += knightval * diffpieces(board, chess.KNIGHT)
    eval += bishopval * diffpieces(board, chess.BISHOP)
    eval += rookval * diffpieces(board, chess.ROOK)
    eval += queenval * diffpieces(board, chess.QUEEN)

    #eval += psqt(board, board.turn)
    #eval -= psqt(board, not board.turn)
    return eval

def psqt(board, side):
    sum = 0
    for type in [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN, chess.KING]:
        locations = bitarray_to_numpy(board.pieces_mask(type, side))
        for rank in range(8):
            if side:
                adjrank = rank
            else:
                adjrank = -(rank - 7)
            for file in range(8):
                sum += PSQTables[type][adjrank][file] * locations[rank][file]
    return sum


PSQTables = {}
#def loadtables():
#    global PSQTables
#    PSQTables = pickle.load("psqt.dmp")
#loadtables()

if __name__ == "__main__":
    a = chess.Board()
    print(evaluateBoard(a))