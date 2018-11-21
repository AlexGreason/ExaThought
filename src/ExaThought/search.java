package ExaThought;

import chesspresso.Chess;
import chesspresso.move.IllegalMoveException;
import chesspresso.move.Move;
import chesspresso.position.Position;

import javax.naming.directory.SearchResult;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.Iterator;

import static chesspresso.move.Move.getFromSqi;
import static chesspresso.move.Move.getToSqi;

class search {

    static long perft(Position board, int depth) throws IllegalMoveException {
        if (depth <= 0) {
            return 1;
        }
        long sum = 0;
        short[] moves = board.getAllMoves();
        for (short move : moves) {
            board.doMove(move);
            sum += perft(board, depth - 1);
            board.undoMove();
        }
        return sum;
    }

    static long uniquepos(Position board, int depth, HashMap<Long, Long> transtable) throws IllegalMoveException {


        long boardhash = board.getHashCode();
        //tableHash hash = new tableHash(boardhash, depth);
        if(transtable.containsKey(boardhash)){
            return 0;
        }
        if(transtable.size() % 1000000 == 0){
            System.out.println(transtable.size());
        }
        transtable.put(boardhash, 1L);
        if (board.isTerminal() || depth < 0) {
            //print("terminal: " + depth);
            transtable.put(boardhash, 1L);
            return 1;
        }
        long sum = 1;
        short[] moves = board.getAllMoves();
        for (short move : moves) {
            if(depth >= 350){
                print("move: " + move + " depth: " + depth);
            }
            board.doMove(move);
            sum += uniquepos(board, depth - 1, transtable);
            board.undoMove();
        }
        return transtable.size();
    }

    static long perft_nocaps(Position board, int depth, HashMap<tableHash, Long> transtable, int startmaterial) throws IllegalMoveException {
        if(Math.abs(board.getMaterial() - startmaterial) > 200){
            return 0;
        }
        if (depth <= 0) {
            return 1;
        }
        long boardhash = board.getHashCode();
        tableHash hash = new tableHash(boardhash, depth);
        if(transtable.containsKey(hash)){

            return transtable.get(hash);
        }

        long sum = 0;
        short[] moves = board.getAllMoves();
        for (short move : moves) {
            board.doMove(move);
            sum += perft_nocaps(board, depth - 1, transtable, startmaterial);
            board.undoMove();
        }
        transtable.put(hash, sum);
        return sum;
    }

    private static ArrayList<Short> moveordering(Position board, int depth, HashMap<Integer, Short> killermoves, HashMap<Long, bestMoveEntry> bestmoves) throws IllegalMoveException {
        long boardhash = board.getHashCode();
        ArrayList<Short> moves = new ArrayList<Short>();
        short[] notcapturemoves = board.getAllNonCapturingMoves();
        short[] capturemoves = board.getAllCapturingMoves();
        for (short cmove : capturemoves) {
            moves.add(cmove);
        }
        for (short move : notcapturemoves) {
            moves.add(move);
        }
        if (killermoves.containsKey(depth)) {
            Short killermove = killermoves.get(depth);
            boolean islegal = moves.remove(killermove);
            if (islegal) {
                moves.add(0, killermove);
            }
        }
        if (bestmoves.containsKey(boardhash)) {
            short bestmove = bestmoves.get(boardhash).bestmove;
            int index = moves.indexOf(bestmove);
            if (index != -1) {
                moves.remove(index);
                moves.add(0, bestmove);
            }
        }
        return moves;
    }

    static boolean isThreefold(long boardhash, Deque<Long> history){
        if (history.contains(boardhash)) {
            //at least a twofold

            Iterator<Long> it = history.iterator();
            short count = 0;
            for (long tmp = it.next(); it.hasNext(); tmp = it.next()) {
                if (tmp == boardhash) {
                    count++;
                }
            }
            if (count >= 2) {
                return true;
            }
        }
        return false;
    }

    static boolean isTwofold(long boardhash, Deque<Long> history){
        return history.contains(boardhash);
    }

    static void print(String str){
        System.out.println(str);
    }

    static searchResult alphabeta(Position board, int depth, int alpha, int beta,
                                  HashMap<tableHash, searchResult> transtable, HashMap<Integer, Short> killermoves,
                                  HashMap<Long, bestMoveEntry> bestmoves, Deque<Long> history) throws IllegalMoveException {
        searchResult result = new searchResult();
        result.nodes = 1;
        if (board.isTerminal()) {
            result.eval = eval.scoreTerminal(board);
            return result;
        }
        if (depth == 0) {
            result.eval = eval.evaluateBoard(board);
            return result;
        }
        long boardhash = board.getHashCode();
        if(isTwofold(boardhash, history)){
            result.eval = 0;
            return result;
        }
        history.addFirst(boardhash);
        ArrayList<Short> moves = moveordering(board, depth, killermoves, bestmoves);
        ArrayList<Short> pv = new ArrayList<>();
        pv.add(moves.get(0));
        for (Short move : moves) {
            board.doMove(move);

            searchResult val = alphabeta(board, depth - 1, -beta, -alpha, transtable, killermoves, bestmoves, history);

            board.undoMove();
            int eval = val.eval * -1;
            result.nodes += val.nodes;
            if (eval > alpha) {
                pv = (ArrayList<Short>) val.pv.clone();
                pv.add(0, move);
                alpha = eval;
            }
            if (alpha >= beta) {
                if (!Move.isCapturing(move)) {
                    killermoves.put(depth, move);
                }
                result.eval = beta;
                result.pv = pv;
                if (!bestmoves.containsKey(boardhash) || bestmoves.get(boardhash).depth < depth) {
                    bestmoves.put(boardhash, new bestMoveEntry(pv.get(0), depth));
                }
                history.removeFirst();
                return result;
            }
        }
        result.pv = pv;
        result.eval = alpha;
        if (!bestmoves.containsKey(boardhash) || bestmoves.get(boardhash).depth < depth) {
            bestmoves.put(boardhash, new bestMoveEntry(pv.get(0), depth));
        }
        history.removeFirst();

        return result;
    }

    public static searchResult iterativeDeepening(Position currpos, int depth, Deque<Long> history) throws IllegalMoveException {
        HashMap<Long, search.bestMoveEntry> hashtable = new HashMap<>();
        long starttime = System.currentTimeMillis();
        search.searchResult val = search.alphabeta(currpos, 1, -128000, 128000, new HashMap<>(), new HashMap<>(), hashtable, history);
        long elapsedtime = System.currentTimeMillis() - starttime;
        System.out.println(val.infostring(1, elapsedtime));
        for(int i = 2; i <= depth; i++) {
            val = search.alphabeta(currpos, i, -128000, 128000, new HashMap<>(), new HashMap<>(), hashtable, history);
            elapsedtime = System.currentTimeMillis() - starttime;
            System.out.println(val.infostring(i, elapsedtime));
        }
        return val;
    }

    public static searchResult moveTime(Position currpos, long milliseconds, Deque<Long> history) throws IllegalMoveException {
        HashMap<Long, search.bestMoveEntry> hashtable = new HashMap<>();
        long starttime = System.currentTimeMillis();
        search.searchResult val = search.alphabeta(currpos, 1, -128000, 128000, new HashMap<>(), new HashMap<>(), hashtable, history);
        long elapsedtime = System.currentTimeMillis() - starttime;
        System.out.println(val.infostring(1, elapsedtime));
        long prevtime = elapsedtime;
        double ratio = (double)elapsedtime/(double)prevtime;
        boolean enoughtime = true;
        int i = 1;
        while(enoughtime) {
            i++;
            val = search.alphabeta(currpos, i, -128000, 128000, new HashMap<>(), new HashMap<>(), hashtable, history);
            elapsedtime = System.currentTimeMillis() - starttime + 1;
            ratio = (double)elapsedtime/((double)prevtime + 100);
            System.out.println("info string " + + prevtime + " " + elapsedtime + " " + ratio + " " + (1.2*((double)elapsedtime * ratio)) + " " + milliseconds);
            prevtime = elapsedtime;
            if(i >= 3) {
                enoughtime = 1.2 * ((double) elapsedtime * ratio) < (double) milliseconds;
            }
            System.out.println(val.infostring(i, elapsedtime));
        }
        return val;
    }

    static class bestMoveEntry {
        public short bestmove;
        public int depth;

        bestMoveEntry(short move, int depth) {
            bestmove = move;
            depth = depth;
        }
    }


    static class searchResult {
        int eval;
        ArrayList<Short> pv = new ArrayList<>();
        long nodes;
        long index;
        static long maxindex = 0;

        public searchResult(){
            this.eval = 0;
            this.pv = new ArrayList<>();
            this.nodes = 0;
            this.index = maxindex;
            maxindex++;
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("Eval: ");
            sb.append(Integer.toString(eval));
            sb.append(" Nodes: ");
            sb.append(Long.toString(nodes));
            sb.append(" PV: ");
            for (Short move : pv) {
                sb.append(Chess.sqiToStr(getFromSqi(move)));
                sb.append(Chess.sqiToStr(getToSqi(move)));
                sb.append(" ");
            }
            return sb.toString();
        }

        String infostring(int depth, long time) {
            StringBuilder sb = new StringBuilder();
            sb.append("info depth ");
            sb.append(Integer.toString(depth));
            sb.append(" nodes ");
            sb.append(Long.toString(nodes));
            sb.append(" nps ");
            sb.append(Integer.toString((int) ((double) nodes / ((double) time / 1000.0))));
            sb.append(" tbhits 0 score cp ");
            sb.append(Integer.toString(eval));
            sb.append(" time ");
            sb.append(Long.toString(time));
            sb.append(" pv ");
            for (Short move : pv) {
                sb.append(movetouci(move));
                sb.append(" ");
            }
            return sb.toString();
        }

        String bestmove() {
            short move = pv.get(0);
            return movetouci(move);
        }

        String movetouci(short move) {
            int promo = Move.getPromotionPiece(move);
            if (promo == Chess.NO_PIECE) {
                return Chess.sqiToStr(getFromSqi(move)) + Chess.sqiToStr(getToSqi(move));
            } else {
                return Chess.sqiToStr(getFromSqi(move)) + Chess.sqiToStr(getToSqi(move)) + Character.toLowerCase(Chess.pieceToChar(promo));
            }
        }
    }

    static class tableHash {
        long hash;
        int depth;

        tableHash(long hash, int depth) {
            this.hash = hash;
            this.depth = depth;
        }

        @Override
        public int hashCode(){
            return (int) (this.hash % Integer.MAX_VALUE) - 13527*this.depth;
        }

        @Override
        public boolean equals(Object other){
            if (getClass() != other.getClass()){
                return false;
            }
            tableHash otherhash = (tableHash) other;
            return this.hash == otherhash.hash && otherhash.depth == this.depth;
        }
    }
}
