import chess
from search import alphabeta
import time
import sys

def printandlog(string):
    logfile.write(string)
    sys.stdout.write(string)

def mainloop():
    board = chess.Board()
    while True:
        message = input()

        logfile.write(message + "\n")
        message = message.split(" ")
        if message[0] == "isready":
            printandlog("readyok" + "\n")
        if message[0] == "position":
            # assumes position startpos moves
            # cutechess gives that for every move, so whatever
            board = chess.Board()
            for move in message[3:]:
                board.push_uci(move)
        if message[0] == "go":
            #only supports go depth
            starttime = time.time()
            value, pv, nodes = alphabeta(board, int(message[2]), -float("inf"), float("inf"), {}, {})
            endtime = time.time()
            totaltime = int((endtime - starttime) * 1000)
            pvstring = ""
            for move in pv:
                pvstring += " " + str(move)
            printandlog("info depth %d nodes %d nps %d tbhits 0 score cp %d time %d pv%s\n" %
                  (int(message[2]), nodes, int(nodes/(totaltime/1000)), value, totaltime, pvstring))
            printandlog("bestmove " + str(pv[0]) + "\n")
        if message[0] == "uci":
            printandlog("id name ExaThought v0.0.1" + "\n")
            printandlog("id author Exa" + "\n")
            printandlog("uciok" + "\n")
        if message[0] == "ucinewgame":
            board = chess.Board()
        sys.stdout.flush()
        if message[0] == "printfen":
            printandlog(board.fen())
        if message[0] == "quit" or message[0] == "stop":
            return

if __name__ == "__main__":
    logfile = open("log.txt", "w")
    mainloop()