package ExaThought;
import chesspresso.move.IllegalMoveException;
import chesspresso.position.Position;
import ExaThought.search;

import java.util.HashMap;
import java.util.Scanner;

public class ExaThought {


    public static void main(String[] args) throws IllegalMoveException{
        //Position pos = new Position("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
        //pos.setStart();
        //long startime = System.currentTimeMillis();
        //int depth = 5;
        //search.searchResult val = search.alphabeta(pos, depth, -128000, 128000, new HashMap<>(), new HashMap<>());
        //long time = System.currentTimeMillis() - startime;
        //System.out.println(val.toString() +", took " + Long.toString(time) + " milliseconds");
        //System.out.println(eval.evaluateBoard(pos));
        //System.out.println(Integer.parseInt("3") + 1);
        UCI ucihandler = new UCI();
        ucihandler.handleloop();
    }
}
