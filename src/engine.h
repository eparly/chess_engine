#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <vector>
#include "piece.h"
#include "move.h"
#include "board.h"
#include "bitboard.h"

class Engine {
public:
    Engine();
    void setBoardState(const std::string &fen);
    std::string getBestMove();
    std::string generateFen() const;

private:
    Bitboard bitboard;
    Board board;
    bool isWhiteTurn;

    void parseFen(const std::string &fen);
    std::vector<Move> generateLegalMoves();
    int evaluateBoard() const;
    Move searchBestMove(int depth);
    int minimax(int depth, int alpha, int beta, bool isMaximizing);

    void applyMove(const Move &move);
    void undoMove(const Move &move);
    void updateBitboard();
    uint64_t pieceToBitboard(const Piece &piece) const;
};
#endif // ENGINE_H