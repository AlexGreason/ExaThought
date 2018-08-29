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
            {134, 176, 195, 197, 343, 307, 259, 220},
            {-49, -12,   0, -45,   0, 146, 120,   6},
            {-56, -45, -36, -45, -54, -45, -42, -60},
            {-77, -66, -59, -49, -57, -57, -71, -80},
            {-75, -63, -52, -56, -49, -50, -52, -69},
            {-77, -58, -65, -76, -74, -64, -49, -79},
            {  0,   0,   0,   0,   0,   0,   0,   0}},

            {{-110, -164,  -25,  -44,   23, -122,   76, -162},
            { -34,   -8,   15,  -10,   99,   49,  -39,  -23},
            {   3,    8,   35,   36,   92,  136,   18,   47},
            {  -9,   19,   21,   52,   53,  101,   89,  112},
            {  -6,   -1,   13,    8,   31,   22,    4,    1},
            { -26,    0,    5,   24,   32,   26,   26,  -16},
            { -40,  -15,    1,   -7,   -9,   -7,   -2,  -10},
            { -83,  -33,  -35,  -23,  -30,    7,  -32,  -43}},

            {{ -65,  -85, -104,  -30,    4,  -86,  -21,   24},
            { -30,  -16,   36,  -37,   -1,   30,  -26,  -38},
            { -32,    5,   -3,   10,   24,   80,   68,   60},
            {   6,    0,   27,   57,   71,   24,   19,   31},
            { -11,    3,   25,   26,   48,    5,   13,   26},
            {  -3,    7,   21,   25,    7,   25,   16,   15},
            { -10,   19,   14,  -11,    7,    0,   29,  -17},
            { -22,   13,  -16,  -23,  -19,  -14,  -30,  -37}},

            {{-16,  63,  40,  56,  24,  49,  72,  56},
            { 65,  71,  93,  75,  92, 143, 120, 135},
            {  2, -36,  -6,  29,   2,  17,  33,  37},
            {-38, -62, -23, -17,   0,  31,  42,   1},
            {-68, -60, -39, -16, -24, -28,   2, -30},
            {-67, -41, -42, -36, -26,   4,  58, -10},
            {-52, -46, -34, -39, -35, -17,   3, -68},
            {-34, -33, -18,  -9,  -4,  -6, -26, -49}},

            {{ -86,  -90,  -91,  -36,  -52,  103,  116,  -99},
            { -77,    9,   67,  -12,  204,   71,  -40,  -58},
            { -81, -120, -104,  -65,    4,  248,  121,  175},
            { -67,  -64,  -86,    4,  118,  129,  158,  155},
            { -43,  -80,  -13,   57,   26,   57,   82,   53},
            { -58,   -6,   64,    6,   13,   54,  123,   71},
            {-100,    0,   -6,  -11,    4,    1,   56,   -4},
            { -20,  -48,  -30,   -4,  -27,  -12,    1,  -18}},

            {{ -314,  -143,    85,   -89,   -96,   167,  -135, -1363},
            {  -97,   129,   223,    43,   132,   249,    70,  -259},
            {  100,   161,   219,   158,   282,   355,   260,  -125},
            {  -94,    78,   149,   126,    42,   147,    11,  -179},
            {  -62,    84,    88,    51,    54,    83,    17,  -342},
            {  -34,    45,   103,    71,    60,    52,    53,   -95},
            {  -17,    58,    53,    30,    18,    29,    23,   -38},
            { -220,   -23,   -23,   -50,   -34,   -87,   -46,  -198}}};

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

        //eval += findPSQT(board, Chess.PAWN);
        //eval += findPSQT(board, Chess.KNIGHT);
        //eval += findPSQT(board, Chess.BISHOP);
        //eval += findPSQT(board, Chess.ROOK);
        //eval += findPSQT(board, Chess.QUEEN);
        //eval += findPSQT(board, Chess.KING);

        if(board.getToPlay() != Chess.WHITE){
            eval *= -1;
        }
        eval += 50; //tempo
        return eval;
    }
}
