package ExaThought;

import chesspresso.position.Position;
import chesspresso.Chess;

import java.util.HashMap;

public class eval {

    private static HashMap<Short, Integer> typeconvert = new HashMap<>();
    static {
        typeconvert.put(Chess.PAWN, 0);
        typeconvert.put(Chess.KNIGHT, 1);
        typeconvert.put(Chess.BISHOP, 2);
        typeconvert.put(Chess.ROOK, 3);
        typeconvert.put(Chess.QUEEN, 4);
        typeconvert.put(Chess.KING, 5);
    }


    //private static final int[] piecevalues = {148,  236,  279,  515, 1279};
    private static final int[] piecevalues = {100,  300,  300,  500, 900};

    private static final int[][][] PSQT =
            {{{  0,   0,   0,   0,   0,   0,   0,   0},
            {  6,   6,   6,   6,   6,   6,   6,   6,},
            {  5,   5,   5,   5,   5,   5,   5,   5,},
            {  4,   4,   4,   4,   4,   4,   4,   4,},
            {  3,   3,   3,   3,   3,   3,   3,   3,},
            {  2,   2,   2,   2,   2,   2,   2,   2,},
            {  1,   1,   1,   1,   1,   1,   1,   1,},
            {  0,   0,   0,   0,   0,   0,   0,   0}}};


    static int scoreTerminal(Position board) throws IllegalStateException{
        //need a way to handle insufficient mating material and threefold repetition
        if(board.isStaleMate() || board.getHalfMoveClock() >= 100){
            return 0;
        } else if(board.isMate()){
            return -12800;
        }
        throw new IllegalStateException("scoreTerminal called, but doesn't appear to be terminal");
    }

    private static int diffpieces(Position board, int piecetype){
        long bitboardwhite = board.getBitBoard(piecetype, 0);
        long bitboardblack = board.getBitBoard(piecetype, 1);
        return Long.bitCount(bitboardwhite) - Long.bitCount(bitboardblack);
    }

    private static int findPSQT(Position board, short piecetype){
        int psqtindex = typeconvert.get(piecetype);
        long bitboardwhite = board.getBitBoard(piecetype, 0);
        long bitboardblack = board.getBitBoard(piecetype, 1);
        int whitepieces = 0;
        int blackpieces = 0;
        for(int i = 0; i < 64; i++){
            int rank = i/8;
            int file = i%8;
            int adjrank = -(rank-7);
            long whitepiece = (bitboardwhite >> i & 1);
            long blackpiece = (bitboardblack >> i & 1);
            whitepieces += whitepiece * PSQT[psqtindex][adjrank][file];
            blackpieces += blackpiece * PSQT[psqtindex][rank][file];
        }
        return whitepieces - blackpieces;
    }

    public static int evaluateBoard(Position board){
        int eval = 0;
        eval += piecevalues[0] * diffpieces(board, Chess.PAWN);
        eval += piecevalues[1] * diffpieces(board, Chess.KNIGHT);
        eval += piecevalues[2] * diffpieces(board, Chess.BISHOP);
        eval += piecevalues[3] * diffpieces(board, Chess.ROOK);
        eval += piecevalues[4] * diffpieces(board, Chess.QUEEN);

        eval += findPSQT(board, Chess.PAWN);
        //eval += findPSQT(board, Chess.KNIGHT);
        //eval += findPSQT(board, Chess.BISHOP);
        //eval += findPSQT(board, Chess.ROOK);
        //eval += findPSQT(board, Chess.QUEEN);
        //eval += findPSQT(board, Chess.KING);

        if(board.getToPlay() != Chess.WHITE){
            eval *= -1;
        }
        return eval;
    }
}
