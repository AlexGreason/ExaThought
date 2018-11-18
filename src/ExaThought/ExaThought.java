package ExaThought;

import chesspresso.move.IllegalMoveException;
import chesspresso.position.Position;

import java.util.ArrayDeque;
import java.util.HashMap;

public class ExaThought {


    public static void main(String[] args) throws IllegalMoveException{
//        Position pos = new Position("5k2/6p1/5p2/5PPK/P3n2P/3B4/8/4b3 b - - 5 57");
////        pos.setStart();
//        long starttime = System.currentTimeMillis();
//        for(int i = 0; i < 100; i++){
//            long val  = search.perft_nocaps(pos, i, new HashMap<>(), pos.getMaterial());
//            long elapsedtime = System.currentTimeMillis() - starttime;
//            System.out.println(i + " " + val + " " + elapsedtime);
//
//        }

        //pos.setStart();
        //search.searchResult val = search.moveTime(pos, 10000, new ArrayDeque<>());
        //System.out.println(eval.evaluateBoard(pos));
        //System.out.println(Integer.parseInt("3") + 1);
        UCI ucihandler = new UCI();
        ucihandler.handleloop();
        //info depth 8 nodes 7718355 nps 559909 tbhits 0 score cp 22 time 13785 pv e2e4 b8c6 b1c3 e7e6 d1h5 g7g6 h5g4 d8h4
    }
}
