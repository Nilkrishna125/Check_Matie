#include <iostream>
#include "chess.hpp"
#include <vector>
#include <math.h>
using namespace std;
using namespace chess;

struct to_return{
    int val;
    string best_string;
};


int count_words(const string& s, char delimiter) {
    int count = 0;
    bool in_word = false;

    for (char ch : s) {
        if (ch == delimiter) {
            count++;
        }
    }

    // Add 1 to count the last word (since there's no delimiter after it)
    return s.empty() ? 0 : count - 1;
}

to_return backind(Board board, int max_player, int i, int alpha, int beta){
    Movelist moves;
    movegen::legalmoves(moves,board);
    to_return values;

   

    if (moves.empty()){
        if (board.inCheck()) {
            int a = (board.sideToMove() == chess::Color::WHITE) ? -100000 : 100000;
            values.val = a;
            values.best_string = ",";
            return values;
        } else {
            values.val = 0;
            values.best_string = ",";
            return values;
        }
    }

    if(i==0){
        values.val = -1000;
        values.best_string = ",";
        return values;
    }

    if (max_player) {
        int utility=-10000;
        to_return temp;
        for (Move action : moves) {
            board.makeMove(action);
            values = backind(board, 0, i-1, alpha, beta);
            board.unmakeMove(action);
            if(values.val >= utility){
                temp.best_string = values.best_string + uci::moveToSan(board, action) + ",";
                utility = values.val;
                temp.val = utility;
            }
            alpha = max(alpha,utility);
            if(alpha>=beta)break;
        }
        return temp;
    }

    else {
        int utility=10000;
        to_return temp;
        for (Move action : moves) {
            board.makeMove(action);
            values = backind(board, 1, i-1, alpha, beta);
            board.unmakeMove(action);
            if(values.val < utility){
                temp.best_string = values.best_string + uci::moveToSan(board, action) + ",";
                utility = values.val;
                temp.val = utility;
            }
            beta = min(beta, utility);
            if(alpha>=beta)break;
        }
        
        return temp;
    }
}

int main(){
    string fen = "3n1k2/5p2/2p1bb2/1p2pN1q/1P2P3/2P3Q1/5PB1/3R2K1 w - - 1 0";
    chess::Board board(fen);
    to_return see_move;
    see_move = backind(board, 1, 3, -100000, 100000);
    cout<<see_move.best_string<<endl;
    return 0;
}