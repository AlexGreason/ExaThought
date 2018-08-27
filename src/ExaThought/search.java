package ExaThought;

import chesspresso.Chess;
import chesspresso.move.IllegalMoveException;
import chesspresso.move.Move;
import chesspresso.position.Position;

import java.util.HashMap;
import java.util.ArrayList;
import ExaThought.eval;

import javax.naming.directory.SearchResult;

import static chesspresso.move.Move.getFromSqi;
import static chesspresso.move.Move.getToSqi;

class search {

    static long perft(Position board, int depth) throws IllegalMoveException {
        if( depth <= 0){
            return 1;
        }
        long sum = 0;
        short[] moves = board.getAllMoves();
        for(short move : moves){
            board.doMove(move);
            sum += perft(board, depth - 1);
            board.undoMove();
        }
        return sum;
    }

    private static ArrayList<Short> moveordering(Position board, int depth, HashMap<Integer, Short> killermoves){
        ArrayList<Short> moves = new ArrayList<Short>();
        short[] notcapturemoves = board.getAllNonCapturingMoves();
        short[] capturemoves = board.getAllCapturingMoves();
        for(short cmove : capturemoves){
            moves.add(cmove);
        }
        for(short move: notcapturemoves){
            moves.add(move);
        }
        if(killermoves.containsKey(depth)){
            Short killermove = killermoves.get(depth);
            boolean islegal = moves.remove(killermove);
            if(islegal){
                moves.add(0, killermove);
            }
        }
        return moves;
    }

    static searchResult alphabeta(Position board, int depth, int alpha, int beta,
                                  HashMap<tableHash, searchResult> transtable, HashMap<Integer, Short> killermoves, HashMap<Long, bestMoveEntry>bestmoves) throws IllegalMoveException{
        searchResult result = new searchResult();
        long boardhash = board.getHashCode();
        tableHash hash = new tableHash(boardhash, depth);
        if(transtable.containsKey(hash)){
            searchResult tableentry = transtable.get(hash);
            tableentry.nodes = 1;
            return transtable.get(hash);
        }
        result.nodes = 1;
        if(board.isTerminal()){
            result.eval = eval.scoreTerminal(board);
            return result;
        }
        if( depth == 0){
            result.eval = eval.evaluateBoard(board);
            return result;
        }
        ArrayList<Short> moves = moveordering(board, depth, killermoves);
        if(bestmoves.containsKey(boardhash)){
            short bestmove = bestmoves.get(boardhash).bestmove;
            int index = moves.indexOf(bestmove);
            if(index != -1){
                moves.remove(index);
                moves.add(0, bestmove);
            }
        }
        ArrayList<Short> pv = new ArrayList<>();
        pv.add(moves.get(0));
        for(Short move: moves){
            board.doMove(move);

            searchResult val = alphabeta(board, depth-1, -beta, -alpha, transtable, killermoves, bestmoves);

            board.undoMove();
            val.eval *= -1;
            result.nodes += val.nodes;
            if(val.eval > alpha){
                pv = (ArrayList<Short>) val.pv.clone();
                pv.add(0, move);
                alpha = val.eval;
            }
            if(alpha >= beta){
                if(!Move.isCapturing(move)){
                    killermoves.put(depth, move);
                    result.eval = beta;
                    result.pv = pv;
                    transtable.put(hash, result);
                    if(!bestmoves.containsKey(boardhash) || bestmoves.get(boardhash).depth < depth) {
                        bestmoves.put(boardhash, new bestMoveEntry(pv.get(0), depth));
                    }
                    return result;
                }
            }
        }
        result.pv = pv;
        result.eval = alpha;
        transtable.put(hash, result);
        if(!bestmoves.containsKey(boardhash) || bestmoves.get(boardhash).depth < depth) {
            bestmoves.put(boardhash, new bestMoveEntry(pv.get(0), depth));
        }
        return result;
    }

    static class bestMoveEntry{
        public short bestmove;
        public int depth;

        bestMoveEntry(short move, int depth){
            bestmove = move;
            depth = depth;
        }
    }


    static class searchResult{
        int eval;
        ArrayList<Short> pv = new ArrayList<>();
        long nodes;

        @Override
        public String toString(){
            StringBuilder sb =  new StringBuilder();
            sb.append("Eval: ");
            sb.append(Integer.toString(eval));
            sb.append(" Nodes: ");
            sb.append(Long.toString(nodes));
            sb.append(" PV: ");
            for(Short move: pv){
                sb.append(Chess.sqiToStr(getFromSqi(move)));
                sb.append(Chess.sqiToStr(getToSqi(move)));
                sb.append(" ");
            }
            return sb.toString();
        }

        String infostring(int depth, long time){
            StringBuilder sb = new StringBuilder();
            sb.append("info depth ");
            sb.append(Integer.toString(depth));
            sb.append(" nodes ");
            sb.append(Long.toString(nodes));
            sb.append(" nps ");
            sb.append(Integer.toString((int)((double)nodes/((double)time/1000.0))));
            sb.append(" tbhits 0 score cp ");
            sb.append(Integer.toString(eval));
            sb.append(" time ");
            sb.append(Long.toString(time));
            sb.append(" pv ");
            for(Short move: pv){
                sb.append(movetouci(move));
                sb.append(" ");
            }
            return sb.toString();
        }

        String bestmove(){
            short move = pv.get(0);
            return movetouci(move);
        }

        String movetouci(short move){
            int promo = Move.getPromotionPiece(move);
            if(promo == Chess.NO_PIECE) {
                return Chess.sqiToStr(getFromSqi(move)) + Chess.sqiToStr(getToSqi(move));
            } else {
                return Chess.sqiToStr(getFromSqi(move)) + Chess.sqiToStr(getToSqi(move)) + Character.toLowerCase(Chess.pieceToChar(promo));
            }
        }
    }

    static class tableHash{
        long hash;
        int depth;

        tableHash(long hash, int depth){
            this.hash = hash;
            this.depth = depth;
        }
    }
}
