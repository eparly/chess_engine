#include "engine.h"
#include <iostream>
#include <sstream>

Engine::Engine() : board(64, sf::Color::White, sf::Color::Black), isWhiteTurn(true) {}


void Engine::setBoardState(const std::string &fen) {
    board.parseFen(fen);
    updateBitboard();
}

std::string Engine::getBestMove() {
    Move bestMove = searchBestMove(3);
    std::ostringstream oss;
    oss << bestMove;
    return oss.str();
}

void Engine::parseFen(const std::string& fen) {
    std::istringstream iss(fen);
    std::string boardPart, turnPart;
    iss >> boardPart >> turnPart;

    int row = 7, col = 0;
    for(char c : boardPart) {
        if (c == '/') {
            row--;
            col = 0;
        } else if (isdigit(c)) {
            col += c - '0';
        }
        else {
            PieceType type;
            PieceColour colour = isupper(c) ? PieceColour::White : PieceColour::Black;
            switch (tolower(c)) {
                case 'p': type = PieceType::Pawn; break;
                case 'r': type = PieceType::Rook; break;
                case 'n': type = PieceType::Knight; break;
                case 'b': type = PieceType::Bishop; break;
                case 'q': type = PieceType::Queen; break;
                case 'k': type = PieceType::King; break;
                default: type = PieceType::Pawn; break; // Default case
            }
            board[col][row] = Piece(type, colour, col, row);
            col++;
        }
    }
}

std::string Engine::generateFen() const {
    std::ostringstream oss;
    for (int row = 0; row >= 0; --row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            if (board[col][row].getType() == PieceType::None) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    oss << emptyCount;
                    emptyCount = 0;
                }
                char pieceChar;
                switch (board[col][row].getType()) {
                    case PieceType::Pawn: pieceChar = 'p'; break;
                    case PieceType::Rook: pieceChar = 'r'; break;
                    case PieceType::Knight: pieceChar = 'n'; break;
                    case PieceType::Bishop: pieceChar = 'b'; break;
                    case PieceType::Queen: pieceChar = 'q'; break;
                    case PieceType::King: pieceChar = 'k'; break;
                    default: pieceChar = ' '; break; // Default case
                }
                if (board[col][row].getColour() == PieceColour::White) {
                    pieceChar = toupper(pieceChar);
                }

                oss << pieceChar;
            }
        }
        if (emptyCount > 0) {
            oss << emptyCount;
        }
        if (row > 0) {
            oss << '/';
        }
    }
    oss << ' ' << (isWhiteTurn ? 'w' : 'b');
    return oss.str();
}

std::vector<Move> Engine::generateLegalMoves() {
    return board.generateLegalMoves();
}

int Engine::evaluateBoard() const {
    // Implement a basic evaluation function
    int score = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            switch (board[col][row].getType()) {
                case PieceType::Pawn: score += (board[col][row].getColour() == PieceColour::White) ? 1 : -1; break;
                case PieceType::Rook: score += (board[col][row].getColour() == PieceColour::White) ? 5 : -5; break;
                case PieceType::Knight: score += (board[col][row].getColour() == PieceColour::White) ? 3 : -3; break;
                case PieceType::Bishop: score += (board[col][row].getColour() == PieceColour::White) ? 3 : -3; break;
                case PieceType::Queen: score += (board[col][row].getColour() == PieceColour::White) ? 9 : -9; break;
                case PieceType::King: score += (board[col][row].getColour() == PieceColour::White) ? 100 : -100; break;
                default: break;
            }
        }
    }
    return score;
}

std::string Engine::searchBestMove(int depth) {
    // Implement a basic minimax search with alpha-beta pruning
    int bestScore = -10000;
    std::string bestMove;
    std::vector<std::string> legalMoves = generateLegalMoves();
    for (const std::string& move : legalMoves) {
        // Make the move
        // ...

        // Evaluate the move
        int score = -evaluateBoard();

        // Undo the move
        // ...

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}

void Engine::applyMove(const Move& move) {
    board.applyMove(move);
    updateBitboard();
}

void Engine::undoMove(const Move& move) {
    board.undoMove(move);
    updateBitboard();
}

void Engine::updateBitboard() {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece piece = board.getPiece(col, row);
            int square = row * 8 + col;
            bitboard.setPiece(square, pieceToBitboard(piece));
        }
    }
}

uint64_t Engine::pieceToBitboard(const Piece& piece) const {
    uint64_t bitboardPiece = 0;
    switch(piece.getType()) {
        case PieceType::Pawn: bitboardPiece = 1; break;
        case PieceType::Rook: bitboardPiece = 2; break;
        case PieceType::Knight: bitboardPiece = 3; break;
        case PieceType::Bishop: bitboardPiece = 4; break;
        case PieceType::Queen: bitboardPiece = 5; break;
        case PieceType::King: bitboardPiece = 6; break;
        default: break;
    }
    return bitboardPiece;
}


//testing fen generation and parsing

void testFENConversion(const std::string& fen) {
    Engine engine;
    engine.setBoardState(fen);
    std::string generatedFEN = engine.generateFen();

    std::cout << "Original FEN: " << fen << std::endl;
    std::cout << "Generated FEN: " << generatedFEN << std::endl;

    if (fen == generatedFEN) {
        std::cout << "Test passed!" << std::endl;
    } else {
        std::cout << "Test failed!" << std::endl;
    }
}

int main() {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    testFENConversion(fen);

    // Add more test cases as needed
    return 0;
}