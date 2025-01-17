#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <vector>
#include "piece.h"

class Engine {
public:
    Engine();
    void setBoardState(const std::string &fen);
    std::string getBestMove();
    std::string generateFen() const;

private: 
    Piece board[8][8];
    bool isWhiteTurn;

    void parseFen(const std::string &fen);
    std::vector<std::string> generateLegalMoves();
    int evaluateBoard() const;
    std::string searchBestMove(int depth);

    // Move generation functions
    std::vector<std::string> generatePawnMoves(int x, int y);
    std::vector<std::string> generateRookMoves(int x, int y);
    std::vector<std::string> generateKnightMoves(int x, int y);
    std::vector<std::string> generateBishopMoves(int x, int y);
    std::vector<std::string> generateQueenMoves(int x, int y);
    std::vector<std::string> generateKingMoves(int x, int y);
};
#endif // ENGINE_H