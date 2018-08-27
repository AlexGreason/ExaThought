package ExaThought;
import chesspresso.move.IllegalMoveException;
import chesspresso.position.Position;
import ExaThought.search;

import java.util.ArrayDeque;
import java.util.HashMap;
import java.util.Scanner;

public class ExaThought {


    public static void main(String[] args) throws IllegalMoveException{
        //Position pos = new Position("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
        //pos.setStart();
        //long startime = System.currentTimeMillis();
        //int depth = 6;
        //search.searchResult val = search.alphabeta(pos, depth, -128000, 128000, new HashMap<>(), new HashMap<>(), new HashMap<>(), new ArrayDeque<>());
        //long time = System.currentTimeMillis() - startime;
        //System.out.println(val.toString() +", took " + Long.toString(time) + " milliseconds");
        //System.out.println(eval.evaluateBoard(pos));
        //System.out.println(Integer.parseInt("3") + 1);
        UCI ucihandler = new UCI();
        ucihandler.handleloop();
        //info depth 8 nodes 7718355 nps 559909 tbhits 0 score cp 22 time 13785 pv e2e4 b8c6 b1c3 e7e6 d1h5 g7g6 h5g4 d8h4
    }
}
