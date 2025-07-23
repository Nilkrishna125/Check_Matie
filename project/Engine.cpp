#include <iostream>
#include "chess.hpp"
#include <vector>
#include <math.h>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace chess;

string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
Board board(fen);

struct to_return{
    int val;
    string best_string;
};

int stop = 0;

// Material values in centipawns
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;

// Piece-Square Tables (from white's perspective)
const int pawn_table[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int knight_table[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int bishop_table[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int rook_table[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

const int queen_table[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int king_middlegame_table[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

const int king_endgame_table[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

// Helper function to get piece value
int getPieceValue(PieceType piece) {
    switch(piece) {
        case PieceType(PieceType::underlying::PAWN) : return PAWN_VALUE;
        case PieceType(PieceType::underlying::KNIGHT): return KNIGHT_VALUE;
        case PieceType(PieceType::underlying::BISHOP): return BISHOP_VALUE;
        case PieceType(PieceType::underlying::ROOK): return ROOK_VALUE;
        case PieceType(PieceType::underlying::QUEEN): return QUEEN_VALUE;
        case PieceType(PieceType::underlying::KING): return KING_VALUE;
        default: return 0;
    }
}

// Helper function to get piece-square table value
int getPieceSquareValue(PieceType piece, Square square, Color color, bool endgame = false) {
    int sq = square.index();
    if (color == Color::BLACK) {
        sq = 63 - sq; // Flip square for black
    }
    
    switch(piece) {
        case PieceType(PieceType::underlying::PAWN): return pawn_table[sq];
        case PieceType(PieceType::underlying::KNIGHT): return knight_table[sq];
        case PieceType(PieceType::underlying::BISHOP): return bishop_table[sq];
        case PieceType(PieceType::underlying::ROOK): return rook_table[sq];
        case PieceType(PieceType::underlying::QUEEN): return queen_table[sq];
        case PieceType(PieceType::underlying::KING): 
            return endgame ? king_endgame_table[sq] : king_middlegame_table[sq];
        default: return 0;
    }
}

// Count material to determine game phase
int countMaterial(const Board& board) {
    int material = 0;
    for (int sq = 0; sq < 64; sq++) {
        Square square = static_cast<Square>(sq);
        Piece piece = board.at(square);
        if (piece != Piece(Piece::underlying::NONE)) {
            PieceType type = piece.type();
            if (type != PieceType(PieceType::underlying::PAWN) && type != PieceType(PieceType::underlying::KING)) {
                material += getPieceValue(type);
            }
        }
    }
    return material;
}

// Count bits in bitboard (number of ones in the bitstring)
int popcount(Bitboard bb) {
    return bb.count();
}

// Get least significant bit square (gives trailing zeros)
Square getLSB(Bitboard bb) {
    return Square(bb.lsb());
}

// Pop least significant bit
Bitboard popLSB(Bitboard bb) {
    uint8_t temp1 = bb.pop();
    return bb;
}

// Create bitboard from square
Bitboard squareToBitboard(Square sq) {
    return Bitboard::fromSquare(sq);
}

// Get file bitboard
Bitboard getFileBitboard(File file) {
    return Bitboard(file);
}

// Evaluate pawn structure
int evaluatePawns(const Board& board) {
    int score = 0;
    Bitboard whitePawns = board.pieces(PieceType::PAWN, Color::WHITE);
    Bitboard blackPawns = board.pieces(PieceType::PAWN, Color::BLACK);
    
    // Check for doubled, isolated, and passed pawns
    for (int fileNum = 0; fileNum < 8; fileNum++) {
        File file = static_cast<File>(fileNum);
        Bitboard fileBB = getFileBitboard(file);
        
        // Count pawns on this file
        Bitboard whitePawnsOnFile = whitePawns & fileBB;
        Bitboard blackPawnsOnFile = blackPawns & fileBB;
        
        // Doubled pawns penalty
        if (popcount(whitePawnsOnFile) > 1) {
            score -= 50;
        }
        if (popcount(blackPawnsOnFile) > 1) {
            score += 50;
        }
        
        // Adjacent files for isolation check
        Bitboard adjacentFiles = Bitboard(0);
        if (fileNum > 0) adjacentFiles |= getFileBitboard(static_cast<File>(fileNum - 1));
        if (fileNum < 7) adjacentFiles |= getFileBitboard(static_cast<File>(fileNum + 1));
        
        // Isolated pawns penalty
        if (whitePawnsOnFile && !(whitePawns & adjacentFiles)) {
            score -= 20; // Isolated white pawn
        }
        if (blackPawnsOnFile && !(blackPawns & adjacentFiles)) {
            score += 20; // Isolated black pawn
        }
        
        // Simple passed pawn detection
        if (whitePawnsOnFile) {
            Square pawnSq = getLSB(whitePawnsOnFile);
            Rank rank = pawnSq.rank();
            int rankNum = static_cast<int>(rank);
            
            Bitboard frontSpan = Bitboard(0);
            // Create front span
            for (int r = rankNum + 1; r < 8; r++) {
                frontSpan |= Rank(static_cast<Rank>(r)).bb();
            }
            Bitboard passedArea = frontSpan & (fileBB | adjacentFiles);
            if (!(blackPawns & passedArea)) {
                score += 50 + (rankNum - 1) * 10; // Bonus increases with advancement
            }
        }
        
        if (blackPawnsOnFile) {
            Square pawnSq = getLSB(blackPawnsOnFile);
            Rank rank = pawnSq.rank();
            int rankNum = static_cast<int>(rank);
            
            Bitboard frontSpan = Bitboard(0);
            // Create front span for black
            for (int r = rankNum - 1; r >= 0; r--) {
                frontSpan |= Rank(static_cast<Rank>(r)).bb();
            }
            Bitboard passedArea = frontSpan & (fileBB | adjacentFiles);
            if (!(whitePawns & passedArea)) {
                score -= 50 + (6 - rankNum) * 10; // Bonus increases with advancement
            }
        }
    }
    
    return score;
}

// Evaluate piece mobility
int evaluateMobility(const Board& board) {
    int score = 0;
    Movelist moves;
    
    // Count legal moves for current side
    movegen::legalmoves(moves, board);
    int mobility = moves.size();
    
    // Mobility bonus/penalty
    if (board.sideToMove() == Color(Color::underlying::WHITE)) {
        score += mobility * 4;
    } else {
        score -= mobility * 4;
    }
    
    return score;
}

// Evaluate king safety
int evaluateKingSafety(const Board& board) {
    int score = 0;
    
    // Bonus for having castling rights
    Board::CastlingRights rights = board.castlingRights();
    if (rights.has(Color::WHITE, Board::CastlingRights::Side::KING_SIDE) ||
        rights.has(Color::WHITE, Board::CastlingRights::Side::QUEEN_SIDE)) {
        score += 30;
    }
    if (rights.has(Color::BLACK, Board::CastlingRights::Side::KING_SIDE) ||
        rights.has(Color::BLACK, Board::CastlingRights::Side::QUEEN_SIDE)) {
        score -= 30;
    }
    
    // Simple king safety evaluation based on pawn shield
    Square whiteKing = board.kingSq(Color::WHITE);
    Square blackKing = board.kingSq(Color::BLACK);
    
    Bitboard whitePawns = board.pieces(PieceType::PAWN, Color::WHITE);
    Bitboard blackPawns = board.pieces(PieceType::PAWN, Color::BLACK);
    
    // Check pawn shield around kings (simplified)
    File whiteKingFile = whiteKing.file();
    Rank whiteKingRank = whiteKing.rank();
    File blackKingFile = blackKing.file();
    Rank blackKingRank = blackKing.rank();
    
    int whiteKingFileNum = static_cast<int>(whiteKingFile);
    int whiteKingRankNum = static_cast<int>(whiteKingRank);
    int blackKingFileNum = static_cast<int>(blackKingFile);
    int blackKingRankNum = static_cast<int>(blackKingRank);
    
    // Bonus for pawns in front of king
    for (int file = max(0, whiteKingFileNum - 1); file <= min(7, whiteKingFileNum + 1); file++) {
        for (int rank = whiteKingRankNum + 1; rank <= min(7, whiteKingRankNum + 2); rank++) {
            Square sq = Square(static_cast<File>(file), static_cast<Rank>(rank));
            if (whitePawns & squareToBitboard(sq)) {
                score += 10;
            }
        }
    }
    
    for (int file = max(0, blackKingFileNum - 1); file <= min(7, blackKingFileNum + 1); file++) {
        for (int rank = max(0, blackKingRankNum - 2); rank < blackKingRankNum; rank++) {
            Square sq = Square(static_cast<File>(file), static_cast<Rank>(rank));
            if (blackPawns & squareToBitboard(sq)) {
                score -= 10;
            }
        }
    }
    
    return score;
}

// Check for basic endgame patterns
int evaluateEndgame(const Board& board) {
    int score = 0;
    
    // Check for insufficient material
    if (board.isInsufficientMaterial()) {
        return 0; // Force draw
    }
    
    // In endgames, centralize the king
    Square whiteKing = board.kingSq(Color::WHITE);
    Square blackKing = board.kingSq(Color::BLACK);
    
    File whiteKingFile = whiteKing.file();
    Rank whiteKingRank = whiteKing.rank();
    File blackKingFile = blackKing.file();
    Rank blackKingRank = blackKing.rank();
    
    int whiteKingFileNum = static_cast<int>(whiteKingFile);
    int whiteKingRankNum = static_cast<int>(whiteKingRank);
    int blackKingFileNum = static_cast<int>(blackKingFile);
    int blackKingRankNum = static_cast<int>(blackKingRank);
    
    // Distance to center bonus
    double whiteCenterDist = abs(whiteKingFileNum - 3.5) + abs(whiteKingRankNum - 3.5);
    double blackCenterDist = abs(blackKingFileNum - 3.5) + abs(blackKingRankNum - 3.5);
    
    score += (7 - whiteCenterDist) * 5;
    score -= (7 - blackCenterDist) * 5;
    
    return score;
}

// Main evaluation function
int evaluate(const Board& board) {
    // Check for game over conditions
    auto gameResult = board.isGameOver();
    if (gameResult.first != GameResultReason::NONE) {
        if (gameResult.second == GameResult::WIN) {
            return board.sideToMove() == Color::WHITE ? KING_VALUE : -KING_VALUE;
        } else if (gameResult.second == GameResult::LOSE) {
            return board.sideToMove() == Color::WHITE ? -KING_VALUE : KING_VALUE;
        } else {
            return 0; // Draw
        }
    }
    
    int score = 0;
    int totalMaterial = countMaterial(board);
    bool endgame = totalMaterial < 1300; // Endgame threshold
    
    // Material and piece-square tables
    for (int sq = 0; sq < 64; sq++) {
        Square square = static_cast<Square>(sq);
        Piece piece = board.at(square);
        
        if (piece != Piece::NONE) {
            PieceType type = piece.type();
            Color color = piece.color();
            
            int pieceValue = getPieceValue(type);
            int squareValue = getPieceSquareValue(type, square, color, endgame);
            
            if (color == Color::WHITE) {
                score += pieceValue + squareValue;
            } else {
                score -= pieceValue + squareValue;
            }
        }
    }
    
    // Positional evaluation
    score += evaluatePawns(board);
    score += evaluateMobility(board);
    score += evaluateKingSafety(board);
    
    if (endgame) {
        score += evaluateEndgame(board);
    }
    
    // Return score from current player's perspective
    return board.sideToMove() == Color::WHITE ? score : -score;
}

to_return alpha_beta_pruning(Board board, int max_player, int alpha, int beta, int depth){
    Movelist moves;
    movegen::legalmoves(moves, board);
    to_return values;
    values.best_string = "";
    values.val = 0;

    if(depth < stop){
        values.val = evaluate(board);
        values.best_string = " ";
        return values;
    }

    if (moves.empty()){
        if (board.inCheck()) {
            int a = (board.sideToMove() == chess::Color::WHITE) ? -KING_VALUE : KING_VALUE;
            values.val = a;
            values.best_string = " ";
            stop = depth;
            return values;
        } else {
            values.val = 0;
            values.best_string = " ";
            stop = depth;
            return values;
        }
    }

    if(depth == 0){
        values.val = evaluate(board);
        values.best_string = " ";
        return values;
    }

    if (max_player) {
        int utility = -KING_VALUE;
        to_return temp;
        temp.best_string = "";
        temp.val = 0;
        for (Move action : moves) {
            board.makeMove(action);
            values = alpha_beta_pruning(board, 0, alpha, beta, depth-1);
            board.unmakeMove(action);
            
            if(values.val >= utility){
                temp.best_string = values.best_string + uci::moveToSan(board, action) + " ";
                utility = values.val;
                temp.val = utility;
            }
            alpha = max(alpha, utility);
            if(alpha >= beta) break;
        }
        return temp;
    }
    else {
        int utility = KING_VALUE;
        to_return temp;
        temp.best_string = "";
        temp.val = 0;
        for (Move action : moves) {
            board.makeMove(action);
            values = alpha_beta_pruning(board, 1, alpha, beta, depth-1);
            board.unmakeMove(action);
            
            if(values.val < utility){
                temp.best_string = values.best_string + uci::moveToSan(board, action) + " ";
                utility = values.val;
                temp.val = utility;
            }
            beta = min(beta, utility);
            if(alpha >= beta) break;
        }
        return temp;
    }
}

string best_move(){
    int a = (board.sideToMove() == Color::WHITE) ? 1 : 0;
    to_return move_and_val = alpha_beta_pruning(board, a, -KING_VALUE, KING_VALUE, 5);
    istringstream move_list(move_and_val.best_string);
    string move;
    while(move_list >> move);
    Move move_object = uci::parseSan(board, move);
    move = uci::moveToUci(move_object);
    return move;
}

void uci_loop() {
    string line;
    while (getline(cin, line)) {
        if (line == "uci") {
            cout << "id name TinguChess" << endl;
            cout << "id author Tingi" << endl;
            cout << "uciok" << endl;
        } else if (line == "isready") {
            cout << "readyok" << endl;
        } else if (line == "ucinewgame") {
            string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            board.setFen(fen);
        } else if (line.rfind("position", 0) == 0) {
            istringstream words(line);
            string word;
            words >> word;
            words >> word;
            if(word == "startpos"){
                string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
                board.setFen(fen);
                words >> word;
                while(words >> word){
                    Move move = uci::uciToMove(board, word);
                    board.makeMove(move);
                }
            }
            else if(word == "fen"){
                string fen_string;
                for(int i = 0; i < 5; i++){
                    words >> word;
                    fen_string += word + " ";
                }
                words >> word;
                fen_string += word;
                words >> word;
                board.setFen(fen_string);
                while(words >> word){
                    Move move = uci::uciToMove(board, word);
                    board.makeMove(move);
                }
            }
            else {
                cout << "invalid position type" << endl;
            }
        } else if (line.rfind("go", 0) == 0) {
            string best_mov = best_move();
            cout << "bestmove " << best_mov << endl;  
        } else if (line == "quit") {
            break;
        }
    }
}

int main(){
    uci_loop();
    return 0;
}