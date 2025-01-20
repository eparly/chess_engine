#include "bitboard.h"

Bitboard::Bitboard() {
    board.fill(0);
}

void Bitboard::setPiece(int square, uint64_t piece) {
    board[square] = piece;
}

uint64_t Bitboard::getPiece(int square) const {
    return board[square];
}

void Bitboard::clearSquare(int square) {
    board[square] = 0;
}

void Bitboard::movePiece(int fromSquare, int toSquare) {
    board[toSquare] = board[fromSquare];
    board[fromSquare] = 0;
}