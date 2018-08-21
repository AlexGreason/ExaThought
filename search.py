import chess
from eval import evaluateBoard
import time
import random
from chess.polyglot import zobrist_hash as zhash


scores = {'0-1':float("inf"), '1-0':-float("inf"), "1/2-1/2":0}
nodes = 0

def alphabeta(board, depth, alpha, beta, maxplayer, transtable):
    global nodes
    nodes += 1
    hash = zhash(board)
    if (hash, depth) in transtable:
        return transtable[(hash, depth)]
    moves = list(board.generate_legal_moves())
    #random.shuffle(moves)
    if len(moves) == 0:
        result = board.result()
        return scores[result]
    if depth == 0:
        return evaluateBoard(board)
    if maxplayer:
        for move in moves:
            board.push(move)
            value = alphabeta(board, depth - 1, alpha, beta, not maxplayer, transtable)
            board.pop()
            alpha = max(alpha, value)
            if alpha >= beta:
                transtable[(hash, depth)] = beta
                return beta
        transtable[(hash, depth)] = alpha
        return alpha
    else:
        for move in moves:
            board.push(move)
            value = alphabeta(board, depth - 1, alpha, beta, not maxplayer, transtable)
            board.pop()
            beta = min(beta, value)
            if alpha >= beta:
                transtable[(hash, depth)] = alpha
                return alpha
        transtable[(hash, depth)] = beta
        return beta

if __name__ == "__main__":
    starttime = time.time()
    print(alphabeta(chess.Board(), 5, -float("inf"), float("inf"), True, {}))
    endtime = time.time()
    print(nodes, endtime-starttime)