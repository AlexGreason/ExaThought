import chess
from eval import evaluateBoard
import time
import random
from chess.polyglot import zobrist_hash as zhash


scores = {'0-1':float("inf"), '1-0':-float("inf"), "1/2-1/2":0}
nodes = 0

def alphabeta(board, depth, alpha, beta, transtable):
    global nodes
    nodes += 1
    hash = zhash(board)
    if (hash, depth) in transtable:
        return transtable[(hash, depth)]
    moves = list(board.generate_legal_moves())
    #random.shuffle(moves)
    if len(moves) == 0:
        result = board.result()
        return scores[result], []
    if depth == 0:
        return evaluateBoard(board), []
    ourpv = []
    for move in moves:
        board.push(move)
        value, pv = alphabeta(board, depth - 1, -beta, -alpha, transtable)
        value *= -1
        board.pop()
        if value > alpha or len(ourpv) == 0:
            pv.insert(0, move)
            ourpv = pv
        alpha = max(alpha, value)
        if alpha >= beta:
            transtable[(hash, depth)] = beta, ourpv
            return beta, ourpv
    transtable[(hash, depth)] = alpha, ourpv
    return alpha, ourpv

if __name__ == "__main__":
    starttime = time.time()
    value, pv = alphabeta(chess.Board("8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"), 4, -float("inf"), float("inf"), {})
    print(value, pv)
    endtime = time.time()
    print(nodes, endtime-starttime)