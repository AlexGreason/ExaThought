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
            return Move.getRegularMove(fromsquare, tosquare, capturing);
        } else {
            int promotion = Chess.charToPiece(move.charAt(4));
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
                }
                break;
            case "go":
                int depth = Integer.parseInt(messagelist.get(2));
                long starttime = System.currentTimeMillis();
                search.searchResult val = search.alphabeta(currpos, depth, -128000, 128000, new HashMap<>(), new HashMap<>());
                long elapsedtime = System.currentTimeMillis() - starttime;
                System.out.println(val.infostring(depth, elapsedtime));
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
