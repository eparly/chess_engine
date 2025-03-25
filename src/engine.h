#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <vector>
#include <stack>
#include <utility>
#include <unordered_map>
#include "bitboard.h"

class Engine {
public:
    Engine();
    void setBoardState(const std::string &fen);
    std::string getBestMove();
    std::string generateFen() const;
    std::vector<std::pair<int, int>> generateLegalMoves(bool isSearch = false);
    int evaluateBoard(bool isWhite) const;
    std::pair<int, int> searchBestMove(int depth);
    void applyMove(const std::pair<int, int>& move, bool isSearch = false);
    void undoMove();
    const Bitboard& getBitboard() const;
    void promotePawn(int square, char promotionPiece);

private:
    void parseFen(const std::string& fen);
    int minimax(int depth, int alpha, int beta, bool isMaximizing);
    int negamax(int depth, int alpha, int beta, int color);
    void generatePawnMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateRookMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateKnightMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateBishopMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateQueenMoves(int square, std::vector<std::pair<int, int>>& moves);
    void generateKingMoves(int square, std::vector<std::pair<int, int>>& moves);
    bool isKingInCheck(bool checkWhiteKing) const;
    bool isPawnAttackingKing(int kingSquare, bool checkWhiteKing) const;

    int evaluateKingSafety(bool isWhite) const;
    int evaluateCenterControl(bool isWhite) const;
    int evaluatePawnStructure(bool isWhite) const;
    Bitboard bitboard;
    uint64_t hash; // Zobrist hash for the current board state
    bool isWhiteTurn;
    int enPassantTarget;
    uint8_t castlingRights;
    struct MoveHistory {
        std::pair<int, int> move;
        uint64_t movedPiece;
        uint64_t capturedPiece;
        int originalPosition;
        int targetPosition;
        uint8_t castlingRights;
        int enPassantTarget;
        bool wasPromotion;
        uint64_t originalPiece;
    };
    std::stack<MoveHistory> moveHistory;

    uint64_t zobristTable[12][64]; // Random numbers for pieces on squares (12 pieces: 6 per color)
    uint64_t zobristCastling[16]; // Random numbers for castling rights (16 combinations)
    uint64_t zobristEnPassant[8]; // Random numbers for en passant files (8 files)
    uint64_t zobristTurn;         // Random number for the side to move

    void initializeZobrist();     // Function to initialize Zobrist table
    void updateZobristHash(const std::pair<int, int>& move, bool isUndo = false);

    struct TranspositionEntry {
        std::pair<int, int> bestMove;
        int depth;
        int score;
        int flag;
        int age;
    };

    std::unordered_map<uint64_t, TranspositionEntry> transpositionTable;
};

#endif // ENGINE_H