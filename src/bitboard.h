#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <array>

class Bitboard {
public:
    Bitboard();
    void setPiece(int square, uint64_t piece);
    uint64_t getPiece(int square) const;
    void clearSquare(int square);
    void movePiece(int fromSquare, int toSquare);
    bool isWhitePiece(int square) const {
        // Implement the logic to determine if the piece on the given square is white
        // This is a placeholder implementation
        return (getPiece(square) & 0x8) != 0;
    }

private:
    std::array<uint64_t, 64> board;
};

#endif // BITBOARD_H