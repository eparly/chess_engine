#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <vector>
#include <stack>
#include <utility>
#include "bitboard.h"

class Engine {
public:
    Engine();
    void setBoardState(const std::string &fen);
    std::string getBestMove();
    std::string generateFen() const;
    std::vector<std::pair<int, int>> generateLegalMoves(bool isSearch = false);
    int evaluateBoard() const; // Update the declaration to reflect the combined function
    std::pair<int, int> searchBestMove(int depth);
    void applyMove(const std::pair<int, int>& move, bool isSearch = false);
    void undoMove();
    const Bitboard& getBitboard() const;
    void promotePawn(int square, char promotionPiece);

private:
    void parseFen(const std::string& fen);
    int minimax(int depth, int alpha, int beta, bool isMaximizing);
    void generatePawnMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateRookMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateKnightMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateBishopMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateQueenMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateKingMoves(int square, std::vector<std::pair<int, int>>& moves);
    bool isKingInCheck(bool checkWhiteKing) const;
    bool isPawnAttackingKing(int kingSquare, bool checkWhiteKing) const; // Add this method declaration

    int evaluateKingSafety(bool isWhite) const;
    int evaluateCenterControl(bool isWhite) const;
    int evaluatePawnStructure(bool isWhite) const;
    Bitboard bitboard;
    bool isWhiteTurn;
    int enPassantTarget; // Add this member variable
    uint8_t castlingRights; // Add this member variable (bitmask for castling rights)
    struct MoveHistory {
        std::pair<int, int> move;
        uint64_t movedPiece;
        uint64_t capturedPiece;
        int originalPosition;
        int targetPosition;
        uint8_t castlingRights; // Add this member variable
        int enPassantTarget; // Add this member variable
        bool wasPromotion; // Add this member variable
        uint64_t originalPiece; // Add this member variable
    };
    std::stack<MoveHistory> moveHistory;
};

#endif // ENGINE_H