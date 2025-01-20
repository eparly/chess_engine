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

private:
    std::array<uint64_t, 64> board;
};

#endif // BITBOARD_H