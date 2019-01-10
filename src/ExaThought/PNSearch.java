package ExaThought;

import chesspresso.Chess;
import chesspresso.move.IllegalMoveException;
import chesspresso.position.FEN;
import chesspresso.position.Position;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

import static chesspresso.move.Move.getFromSqi;
import static chesspresso.move.Move.getToSqi;

class PNSearch {

    PNSnode root;
    private Position rootboard;
    private HashMap<PNSkey, PNSnode> tree;
    private result_enum goal;
    private HashSet<Long> hist;


    PNSearch(Position startpos, result_enum goal){
        this.hist = new HashSet<>();
        type_enum root_type = startpos.getToPlay() == Chess.BLACK ? type_enum.AND : type_enum.OR;
        if(goal == result_enum.BLACK_W){
            root_type = startpos.getToPlay() == Chess.BLACK ? type_enum.OR : type_enum.AND;
        }
        this.root = new PNSnode(startpos.getHashCode(), eval(startpos), root_type, goal);
        this.rootboard = startpos;
        this.tree = new HashMap<>();
        this.tree.put(new PNSkey(startpos.getHashCode(), -1), this.root);
        this.hist.add(startpos.getHashCode());
        this.goal = goal;
    }

    private void expand(PNSnode child, Position childboard) throws IllegalMoveException {
        short[] moves = childboard.getAllMoves();
        child.expanded = true;
        long cpnum = child.type == type_enum.AND ? 0 : Integer.MAX_VALUE;
        long cdpnum = child.type == type_enum.AND ? Integer.MAX_VALUE : 0;
        hist.add(childboard.getHashCode());
        for(short m : moves){
            childboard.doMove(m);
            long chash = childboard.getHashCode();
            PNSnode newchild = new PNSnode(chash, eval(childboard), invert(child.type), goal);
            if(child.type == type_enum.AND){
                cpnum += newchild.pnum;
                cdpnum = Math.min(cdpnum, newchild.dpnum);
            } else {
                cpnum = Math.min(cpnum, newchild.pnum);
                cdpnum += newchild.dpnum;
            }
            this.tree.put(new PNSkey(chash, child.index), newchild);
            childboard.undoMove();
        }
        hist.remove(childboard.getHashCode());
        child.children++;
        child.pnum = (int) Math.min(cpnum, Integer.MAX_VALUE);
        child.dpnum = (int) Math.min(cdpnum, Integer.MAX_VALUE);
    }

    private void step(PNSnode child, Position childboard) throws IllegalMoveException{
        if(!child.expanded){
            expand(child, childboard);
            return;
        }

        short[] moves = childboard.getAllMoves();
        if(moves.length == 0){
            System.out.println("re-expanding terminal node " + child.index);
            return; //already expanded, no legal moves, but most proving node. How did we get here?
        }
        int bestpval = Integer.MAX_VALUE;
        int secbpval = Integer.MAX_VALUE;
        long sumpval = 0;
        int bestdpval = Integer.MAX_VALUE;
        int secbdpval = Integer.MAX_VALUE;
        long sumdpval = 0;
        PNSnode bestnode = null;
        short bestmove = 0;
        for(short m : moves){
            childboard.doMove(m);
            long chash = childboard.getHashCode();
            PNSkey key = new PNSkey(chash, child.index);
            PNSnode grandc = this.tree.get(key);
            sumpval += grandc.pnum;
            sumdpval += grandc.dpnum;
            if(bestdpval > grandc.dpnum || (bestmove == 0)){
                secbdpval = bestdpval;
                bestdpval = grandc.dpnum;
                if(child.type == type_enum.AND){
                    bestnode = grandc;
                    bestmove = m;
                }
            } else if (secbdpval > grandc.dpnum) {
                secbdpval = grandc.dpnum;
            }
            if(bestpval > grandc.pnum || (bestmove == 0)){
                secbpval = bestpval;
                bestpval = grandc.pnum;
                if(child.type == type_enum.OR){
                    bestnode = grandc;
                    bestmove = m;
                }
            } else if (secbpval > grandc.pnum) {
                secbpval = grandc.pnum;
            }
            childboard.undoMove();
        }
        //bestnode should now be the most proving child node, and bestmove the move that produces it
        if(bestmove == 0){
            System.out.println(" " + child.index);
        }
//        if(bestnode.index == 4862){
//            System.out.println("debug");
//        }
        int oldpval = bestnode.pnum;
        int olddpval = bestnode.dpnum;
        hist.add(childboard.getHashCode());
        childboard.doMove(bestmove);
        step(bestnode, childboard);
        childboard.undoMove();
        //System.out.println(childboard.getFEN());
        hist.remove(childboard.getHashCode());

        child.children++;
        if(child.type == type_enum.AND){
            child.pnum = (int) Math.min(sumpval - oldpval + bestnode.pnum, Integer.MAX_VALUE);
            child.dpnum = Math.min(bestnode.dpnum, secbdpval);
        } else {
            child.pnum = Math.min(bestnode.pnum, secbpval);
            child.dpnum = (int) Math.min(sumdpval - olddpval + bestnode.dpnum, Integer.MAX_VALUE);
        }
        if(child.pnum == Integer.MAX_VALUE || child.dpnum == Integer.MAX_VALUE){
            if(child.children >= Math.sqrt(root.children)){
                System.out.println(child);
            }
            if(child != root) {
                delChildren(child, childboard);
            }
        }
        //step sets child values, so child pnum and dpnum should now be correct (even in expansion case)
        //these should be based on iteration over immediate children, not grandchildren, no?
        //yeah. I can record the initial parent/child p/dp values and use subtraction and max/min to update
    }


    int search(int nodelimit) throws IllegalMoveException{
        if(rootboard.isTerminal()){
            root.result = eval(rootboard);
            root.pnum = getpnum(root.result, goal);
            root.dpnum = getdpnum(root.result, goal);
            if (root.dpnum == Integer.MAX_VALUE){
                return 1;
            } else {
                return -1;
            }
        }
        while(nodelimit > 0 && root.pnum < Integer.MAX_VALUE && root.dpnum < Integer.MAX_VALUE){
            if(root.children % 10000 == 0){
                System.out.println(root);
            }
            nodelimit--;
            Position childboard = new Position(FEN.getFEN(this.rootboard));
            step(root, childboard);
            //System.out.println("searchstep");
        }
        if (root.pnum < Integer.MAX_VALUE && root.dpnum < Integer.MAX_VALUE){
            return 0;
        } else if (root.dpnum == Integer.MAX_VALUE){
            return 1;
        } else {
            return -1;
        }
    }

    private Short getBestMove(PNSnode child, Position childboard) throws IllegalMoveException{
        short[] moves = childboard.getAllMoves();
        if(moves.length == 0 || !child.expanded){
            return 0;
        }
        int bestpval = Integer.MAX_VALUE;
        int mostchildren = 0;
        short bestmove = 0;
        for(short m : moves){
            childboard.doMove(m);
            long chash = childboard.getHashCode();
            PNSkey key = new PNSkey(chash, child.index);
            PNSnode grandc = this.tree.get(key);
            if (grandc == null) {
                return 0;
            }
            if(mostchildren < grandc.children || (bestmove == 0)){
                mostchildren = grandc.children;
                if(child.type == type_enum.AND){
                    bestmove = m;
                }
            }
            if(bestpval > grandc.pnum || (bestmove == 0)){
                bestpval = grandc.pnum;
                if(child.type == type_enum.OR){
                    bestmove = m;
                }
            }
            childboard.undoMove();
        }
        return bestmove;
    }

    void printChildren(short[] moves) throws IllegalMoveException{
        Position childboard = rootboard;
        PNSnode node = root;
        for(short m : moves) {
            childboard.doMove(m);
            long chash = childboard.getHashCode();
            PNSkey key = new PNSkey(chash, node.index);
            node = this.tree.get(key);
        }
        short[] cmoves = childboard.getAllMoves();
        for(short m : cmoves) {
            childboard.doMove(m);
            long chash = childboard.getHashCode();
            PNSkey key = new PNSkey(chash, node.index);
            PNSnode grandc = this.tree.get(key);
            System.out.println(grandc);
            childboard.undoMove();
        }
    }

    void delChildren(PNSnode node, Position childboard) throws IllegalMoveException{
        short[] cmoves = childboard.getAllMoves();
        for(short m : cmoves) {
            childboard.doMove(m);
            long chash = childboard.getHashCode();
            PNSkey key = new PNSkey(chash, node.index);
            PNSnode grandc = this.tree.get(key);
            if(grandc != null) {
                delChildren(grandc, childboard);
                if (grandc.children < Math.sqrt(root.children)) {
                    this.tree.remove(key);
                }
            }

            childboard.undoMove();
        }
    }

    private ArrayList<Short> getpv() throws IllegalMoveException{
        PNSnode node = root;
        Position childboard = new Position(FEN.getFEN(this.rootboard));
        ArrayList<Short> pv = new ArrayList<>();
        Short newmove = getBestMove(node, childboard);
        while(newmove != 0){
            pv.add(newmove);
            childboard.doMove(newmove);
            PNSkey key = new PNSkey(childboard.getHashCode(), node.index);
            node = this.tree.get(key);
            if (node == null) {
                return pv;
            }
            newmove = getBestMove(node, childboard);
        }
        return pv;
    }

    String printpv() throws IllegalMoveException{
        ArrayList<Short> pv = getpv();
        StringBuilder sb = new StringBuilder();
        for (Short move : pv) {
            sb.append(Chess.sqiToStr(getFromSqi(move)));
            sb.append(Chess.sqiToStr(getToSqi(move)));
            sb.append(" ");
        }
        return sb.toString();
    }

    public enum result_enum {
        BLACK_W, WHITE_W, D, UNKNOWN
    }

    private result_enum eval(Position board){
        //need a way to handle repetition
        if(board.isStaleMate() || board.getHalfMoveClock() >= 100 || board.isInsufficentMaterial() || hist.contains(board.getHashCode())){
            if( hist.contains(board.getHashCode())){
                //System.out.println("hit!");
                //System.out.println(board.getFEN());
            }
            return result_enum.D;
        }
        if(!board.isTerminal()){
            return result_enum.UNKNOWN;
        }
        if(board.isMate() && board.getToPlay() == Chess.WHITE){
            return result_enum.BLACK_W;
        } else if(board.isMate() && board.getToPlay() == Chess.BLACK) {
            return result_enum.WHITE_W;
        }
        throw new IllegalStateException("scoreTerminal called, but doesn't appear to be terminal");
    }

    private static int getpnum(result_enum val, result_enum goal){
        if (val == result_enum.UNKNOWN){
            return 1;
        }
        if(val == goal){
            return 0;
        }
        return Integer.MAX_VALUE;
    }

    private static int getdpnum(result_enum val, result_enum goal){
        if (val == result_enum.UNKNOWN){
            return 1;
        }
        if(val == goal){
            return Integer.MAX_VALUE;
        }
        return 0;
    }

    public enum type_enum {
        AND, OR
    }

    private type_enum invert(type_enum val){
        return val == type_enum.AND ? type_enum.OR : type_enum.AND;
    }

    static class PNSnode {
        long hash;
        result_enum result;
        type_enum type;
        int pnum;
        int dpnum;
        int children;
        int index;
        boolean expanded;
        static int maxindex;

        PNSnode(long hash, result_enum result, type_enum type, int pnum, int dpnum, int children, boolean expanded) {
            this.hash = hash;
            this.result = result;
            this.type = type;
            this.index = maxindex;
            maxindex++;
            this.children = children;
            this.pnum = pnum;
            this.dpnum = dpnum;
            this.expanded = expanded;
        }

        PNSnode(long hash, result_enum result, type_enum type, result_enum goal){
            this(hash, result, type, 1, 1, 0, false);
            this.pnum = getpnum(result, goal);
            this.dpnum = getdpnum(result, goal);
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("index: ");
            sb.append(index);
            sb.append(" result: ");
            sb.append(result);
            sb.append(" type: ");
            sb.append(type);
            sb.append(" pnum: ");
            sb.append(pnum);
            sb.append(" dpnum: ");
            sb.append(dpnum);
            sb.append(" children: ");
            sb.append(children);
            return sb.toString();
        }

        @Override
        public int hashCode(){
            return (int) (this.hash % Integer.MAX_VALUE);
        }

        @Override
        public boolean equals(Object other){
            if (getClass() != other.getClass()){
                return false;
            }
            PNSnode othernode = (PNSnode) other;
            return this.index == othernode.index;
        }
    }

    static class PNSkey {
        long hash;
        int index;

        PNSkey(long hash, int index) {
            this.hash = hash;
            this.index = index;
        }

        @Override
        public int hashCode(){
            return (int) ((this.hash % Integer.MAX_VALUE) - 13527*this.index);
        }

        @Override
        public boolean equals(Object other){
            if (getClass() != other.getClass()){
                return false;
            }
            PNSkey otherkey = (PNSkey) other;
            return this.hash == otherkey.hash && otherkey.index == this.index;
        }
    }
}