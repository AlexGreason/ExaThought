import chess
from eval import evaluateBoard
import time
import random
from chess.polyglot import zobrist_hash as zhash


scores = {'0-1':-float("inf"), '1-0':float("inf"), "1/2-1/2":0}

def alphabeta(board, depth, alpha, beta, transtable):
    totalnodes = 0
    hash = zhash(board)
    if (hash, depth) in transtable:
        return transtable[(hash, depth)]
    moves = list(board.generate_legal_moves())
    #random.shuffle(moves)
    if len(moves) == 0:
        result = board.result()
        score = scores[result]
        if not board.turn:
            score *= -1
        return score, [], 1
    if board.can_claim_draw():
        return 0, [], 1
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
    value, pv, nodes = alphabeta(chess.Board("rn5k/ppp1p2p/6p1/1P6/8/r3b3/7K/5q2 b - - 12 52"), 3, -float("inf"), float("inf"), {})
    print(value, pv)
    endtime = time.time()
    print(nodes, endtime-starttime)