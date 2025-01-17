#include "engine.h"
#include <iostream>
#include <sstream>

Engine::Engine() : isWhiteTurn(true) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            board[i][j] = Piece();
        }
    }
}

void Engine::setBoardState(const std::string &fen) {
    parseFen(fen);
}

std::string Engine::getBestMove() {
    return searchBestMove(3);
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

std::vector<std::string> Engine::generateLegalMoves() {
    std::vector<std::string> allMoves;
    // for (int row = 0; row < 8; ++row) {
    //     for (int col = 0; col < 8; ++col) {
    //         if ((isWhiteTurn && board[col][row].getColour() == PieceColour::White) || (!isWhiteTurn && board[col][row].getColour() == PieceColour::Black)) {
    //             std::vector<std::string> pieceMoves;
    //             switch (board[col][row].getType()) {
    //                 case PieceType::Pawn:
    //                     pieceMoves = generatePawnMoves(col, row);
    //                     break;
    //                 case PieceType::Rook:
    //                     pieceMoves = generateRookMoves(col, row);
    //                     break;
    //                 case PieceType::Knight:
    //                     pieceMoves = generateKnightMoves(col, row);
    //                     break;
    //                 case PieceType::Bishop:
    //                     pieceMoves = generateBishopMoves(col, row);
    //                     break;
    //                 case PieceType::Queen:
    //                     pieceMoves = generateQueenMoves(col, row);
    //                     break;
    //                 case PieceType::King:
    //                     pieceMoves = generateKingMoves(col, row);
    //                     break;
    //                 default:
    //                     break;
    //             }
    //             allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
    //         }
    //     }
    // }
    return allMoves;
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

// Implement move generation functions (generatePawnMoves, generateRookMoves, etc.) here

// Example implementation for pawn moves
std::vector<std::string> Engine::generatePawnMoves(int x, int y) {
    std::vector<std::string> moves;
    // int direction = (board[x][y].getColour() == PieceColour::White) ? 1 : -1;

    // // Move forward one square
    // if (isValidPosition(x, y + direction) && board[x][y + direction].getType() == PieceType::None) {
    //     moves.push_back(moveToString(sf::Vector2i(x, y), sf::Vector2i(x, y + direction)));

    //     // Move forward two squares from starting position
    //     if ((board[x][y].getColour() == PieceColour::White && y == 1) || (board[x][y].getColour() == PieceColour::Black && y == 6)) {
    //         if (isValidPosition(x, y + 2 * direction) && board[x][y + 2 * direction].getType() == PieceType::None) {
    //             moves.push_back(moveToString(sf::Vector2i(x, y), sf::Vector2i(x, y + 2 * direction)));
    //         }
    //     }
    // }

    // // Capture diagonally
    // if (isValidPosition(x + 1, y + direction) && board[x + 1][y + direction].getColour() != board[x][y].getColour() && board[x + 1][y + direction].getType() != PieceType::None) {
    //     moves.push_back(moveToString(sf::Vector2i(x, y), sf::Vector2i(x + 1, y + direction)));
    // }
    // if (isValidPosition(x - 1, y + direction) && board[x - 1][y + direction].getColour() != board[x][y].getColour() && board[x - 1][y + direction].getType() != PieceType::None) {
    //     moves.push_back(moveToString(sf::Vector2i(x, y), sf::Vector2i(x - 1, y + direction)));
    // }

    // // En passant capture
    // if (isValidPosition(x + 1, y + direction) && sf::Vector2i(x + 1, y + direction) == enPassantTarget) {
    //     moves.push_back(moveToString(sf::Vector2i(x, y), sf::Vector2i(x + 1, y + direction)));
    // }
    // if (isValidPosition(x - 1, y + direction) && sf::Vector2i(x - 1, y + direction) == enPassantTarget) {
    //     moves.push_back(moveToString(sf::Vector2i(x, y), sf::Vector2i(x - 1, y + direction)));
    // }

    return moves;
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