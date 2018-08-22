import chess
from eval import bitarray_to_numpy, diffpieces
import numpy as np
import pickle
from loader import loadfile

def psqtgrad(board, side):
    grad = {}
    for type in [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN, chess.KING]:
        tmpgrad = np.zeros((8,8))
        locations = bitarray_to_numpy(board.pieces_mask(type, side))
        for rank in range(8):
            if side:
                adjrank = rank
            else:
                adjrank = -(rank - 7)
            for file in range(8):
                tmpgrad[adjrank][file] = locations[rank][file]
        grad[type] = tmpgrad
    return grad

def fullpsqtgrad(board):
    grad = psqtgrad(board, board.turn)
    grad2 = psqtgrad(board, not board.turn)
    for type in grad:
        grad[type] = grad[type] - grad2[type]
    return grad

def getfeatures(board):
    pawndiff = diffpieces(board, chess.PAWN)
    knightdiff = diffpieces(board, chess.KNIGHT)
    bishopdiff = diffpieces(board, chess.BISHOP)
    rookdiff = diffpieces(board, chess.ROOK)
    queendiff = diffpieces(board, chess.QUEEN)
    piecegrads = fullpsqtgrad(board)
    piecegrads[0] = np.array([pawndiff, knightdiff, bishopdiff, rookdiff, queendiff])
    return piecegrads

def calceval(grads, params):
    sum = 0
    for key in grads:
        sum += np.sum(grads[key] * params[key])
    return sum

def zerosdict():
    result = {}
    result[0] = np.zeros(5, dtype=np.float32)
    for i in [1, 2, 3, 4, 5, 6]:
        result[i] = np.zeros((8, 8), dtype="float32")
    return result

def flatten(dict):
    result = np.zeros(5 + 6 * 64, dtype=np.float32)
    result[0:5] = dict[0]
    for i in range(1, 7):
        result[5 + (i - 1) * 64: 5 + i * 64] = dict[i].reshape(64)
    return result

def unflatten(array):
    dict = {}
    dict[0] = array[0:5]
    for i in range(1, 7):
        dict[i] = array[5 + (i - 1) * 64: 5 + i * 64].reshape((8,8))
    return dict

def data_as_array(npositions):
    data = loadfile("/home/exa/Documents/Ethereal-master/FENS/FENS-cleared-10-all", flipscores=True, loadmax=npositions)
    processed = []
    i = 0
    for pos in data:
        i += 1
        print(i)
        board = chess.Board(pos[0])
        posgrads = flatten(getfeatures(board))
        searchedeval = pos[2][-1]
        processed.append((posgrads, searchedeval, pos[1]))
    return processed

def tune():
    npositions = 1000
    data = loadfile("/home/exa/Documents/Ethereal-master/FENS/FENS-cleared-10-all", flipscores=True, loadmax=npositions)
    processed = []
    i = 0
    for pos in data:
        i += 1
        print(i)
        board = chess.Board(pos[0])
        posgrads = getfeatures(board)
        searchedeval = pos[2][-1]
        processed.append((posgrads, searchedeval))
    lr = 0.01
    params = zerosdict()
    params[0] = np.array([100, 300, 300, 500, 900], dtype="float32")
    while True:
        error= 0
        grads = zerosdict()
        i = 0
        for pos in processed:
            i += 1
            if i % 10000 == 0:
                print(i)
            eval = calceval(pos[0], params)
            error += abs(eval - pos[1])
            for key in [1]:
                grads[key] += (pos[1] - eval) * pos[0][key]
        for key in [1]:
            params[key] += grads[key] * lr * (1/len(processed))
        file = open("psqt-tmp.dmp", "wb")
        pickle.dump(params, file)
        print("Total error ", error)
        print(params[1])



if __name__ == "__main__":
    tune()
