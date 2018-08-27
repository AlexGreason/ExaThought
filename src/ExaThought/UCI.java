package ExaThought;

import chesspresso.Chess;
import chesspresso.move.IllegalMoveException;
import chesspresso.move.Move;
import chesspresso.position.Position;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class UCI {

    private Position currpos;
    private Scanner input;

    UCI(){
        currpos = new Position();
        currpos.setStart();
        input = new Scanner(System.in);
    }


    private short ucitoMove(String move){
        HashMap<Character, Integer> charmap = new HashMap<>();
        String sfromsquare = move.substring(0, 2);
        String stosquare = move.substring(2, 4);

        int fromsquare = Chess.strToSqi(sfromsquare);
        int tosquare = Chess.strToSqi(stosquare);
        boolean capturing = currpos.getStone(tosquare) != 0;
        if(move.length() == 4) {
            int piece = Math.abs(currpos.getStone(fromsquare)) ;
            if(piece == Chess.KING){
                if(Chess.sqiToCol(fromsquare) == Chess.charToCol('e')) {
                    int col = Chess.sqiToCol(tosquare);
                    if(col == Chess.charToCol('g')){
                        return Move.getShortCastle(currpos.getToPlay());
                    } else if(col == Chess.charToCol('c')) {
                        return Move.getLongCastle(currpos.getToPlay());
                    }
                }
            } else if(piece == Chess.PAWN){
                if((Chess.sqiToCol(fromsquare) != Chess.sqiToCol(tosquare)) && !capturing){
                    return Move.getEPMove(fromsquare, tosquare);
                }
            }
            return Move.getRegularMove(fromsquare, tosquare, capturing);
        } else {
            int promotion = Chess.charToPiece(Character.toUpperCase(move.charAt(4)));
            return Move.getPawnMove(fromsquare, tosquare, capturing, promotion);
        }
    }

    void handleloop() throws IllegalMoveException {
        boolean done = false;
        while(!done){
            done = handlestep();
        }
    }

    private boolean handlestep() throws IllegalMoveException {
        String message = input.nextLine();
        List<String> messagelist = Arrays.asList(message.split("\\s"));
        String type = messagelist.get(0);
        switch(type){
            case "isready":
                System.out.println("readyok");
                break;
            case "position":
                currpos.setStart();
                for(int i = 3; i < messagelist.size(); i++){
                    String token = messagelist.get(i);
                    short move = ucitoMove(token);
                    currpos.doMove(move);
                    //System.out.println(currpos.getFEN());
                }
                break;
            case "go":
                int depth = Integer.parseInt(messagelist.get(2));
                HashMap<Long, search.bestMoveEntry> hashtable = new HashMap<>();
                long starttime = System.currentTimeMillis();
                search.searchResult val = search.alphabeta(currpos, 1, -128000, 128000, new HashMap<>(), new HashMap<>(), hashtable);
                for(int i = 2; i <= depth; i++) {
                    val = search.alphabeta(currpos, i, -128000, 128000, new HashMap<>(), new HashMap<>(), hashtable);
                    long elapsedtime = System.currentTimeMillis() - starttime;
                    System.out.println(val.infostring(i, elapsedtime));
                }
                System.out.println("bestmove " + val.bestmove());
                break;
            case "uci":
                System.out.println("id name ExaThought v0.0.1");
                System.out.println("id author Exa");
                System.out.println("uciok");
                break;
            case "ucinewgame":
                currpos.setStart();
                break;
            case "printfen":
                System.out.println(currpos.getFEN());
                break;
            case "quit":
                return true;
            case "stop":
                return true;
        }
        return false;
    }
}
