package ExaThought;

import chesspresso.move.IllegalMoveException;
import chesspresso.position.Position;

import java.util.ArrayDeque;
import java.util.HashMap;

public class ExaThought {


    public static void main(String[] args) throws IllegalMoveException{
        Position pos = new Position("8/8/8/2K5/1P1P4/2Pk4/8/8 w - - 0 1");
        PNSearch test =  new PNSearch(pos, PNSearch.result_enum.WHITE_W);
        System.out.println(test.search(100000000));
        System.out.println(test.root + "\n");
        test.printChildren(new short[]{});
        System.out.println(test.printpv());
        //System.out.println(" " + pos.getCardinality());
//        pos.setStart();
//        long starttime = System.currentTimeMillis();
//        long val  = search.uniquepos(pos, 10, new HashMap<>());
//        long elapsedtime = System.currentTimeMillis() - starttime;
//        System.out.println(100 + " " + val + " " + elapsedtime);

        //pos.setStart();
        //search.searchResult val = search.moveTime(pos, 10000, new ArrayDeque<>());
        //System.out.println(eval.evaluateBoard(pos));
        //System.out.println(Integer.parseInt("3") + 1);
        //UCI ucihandler = new UCI();
        //ucihandler.handleloop();
        //info depth 8 nodes 7718355 nps 559909 tbhits 0 score cp 22 time 13785 pv e2e4 b8c6 b1c3 e7e6 d1h5 g7g6 h5g4 d8h4
    }
}
