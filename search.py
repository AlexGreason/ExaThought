import chess
from eval import evaluateBoard
import time
import random
from chess.polyglot import zobrist_hash as zhash


scores = {'0-1':float("inf"), '1-0':-float("inf"), "1/2-1/2":0}

def alphabeta(board, depth, alpha, beta, transtable):
    totalnodes = 0
    hash = zhash(board)
    if (hash, depth) in transtable:
        return transtable[(hash, depth)]
    moves = list(board.generate_legal_moves())
    #random.shuffle(moves)
    if len(moves) == 0:
        result = board.result()
        return scores[result], [], 1
    if depth == 0:
        return evaluateBoard(board), [], 1
    ourpv = [moves[0]]
    for move in moves:
        board.push(move)
        value, pv, nodes = alphabeta(board, depth - 1, -beta, -alpha, transtable)
        totalnodes += nodes
        value *= -1
        board.pop()
        if value > alpha or len(ourpv) == 0:
            ourpv = [move] + pv
        alpha = max(alpha, value)
        if alpha >= beta:
            transtable[(hash, depth)] = beta, ourpv, totalnodes
            return beta, ourpv, totalnodes
    transtable[(hash, depth)] = alpha, ourpv, totalnodes
    return alpha, ourpv, totalnodes

if __name__ == "__main__":
    starttime = time.time()
    value, pv, nodes = alphabeta(chess.Board("8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"), 4, -float("inf"), float("inf"), {})
    print(value, pv)
    endtime = time.time()
    print(nodes, endtime-starttime)