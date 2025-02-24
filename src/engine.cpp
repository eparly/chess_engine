#include "engine.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm> // Add this header
#include "bitboard.h"
#include "piece_tables.h"

Engine::Engine() : isWhiteTurn(true) {}

void Engine::setBoardState(const std::string &fen) {
    parseFen(fen);
}

std::string Engine::getBestMove() {
    auto bestMove = searchBestMove(1);
    int start = bestMove.first;
    int end = bestMove.second;

    char startFile = 'a' + (start % 8);
    char startRank = '1' + (start / 8);
    char endFile = 'a' + (end % 8);
    char endRank = '1' + (end / 8);

    std::ostringstream oss;
    oss << startFile << startRank << endFile << endRank;
    return oss.str();
}

void Engine::parseFen(const std::string& fen) {
    std::istringstream iss(fen);
    std::string boardPart, turnPart, castlingPart, enPassantPart;
    iss >> boardPart >> turnPart >> castlingPart >> enPassantPart;

    int row = 7, col = 0;
    for(char c : boardPart) {
        if (c == '/') {
            row--;
            col = 0;
        } else if (isdigit(c)) {
            col += c - '0';
        } else {
            uint64_t piece;
            bool isWhite = isupper(c);
            switch (tolower(c)) {
                case 'p': piece = 1; break; // Pawn
                case 'r': piece = 2; break; // Rook
                case 'n': piece = 3; break; // Knight
                case 'b': piece = 4; break; // Bishop
                case 'q': piece = 5; break; // Queen
                case 'k': piece = 6; break; // King
                default: piece = 0; break; // Default case
            }
            if (isWhite) {
                piece |= 0x8; // Set the white piece bit
            }
            bitboard.setPiece(row * 8 + col, piece);
            col++;
        }
    }
    isWhiteTurn = (turnPart == "w");

    // Parse castling rights
    castlingRights = 0;
    if (castlingPart.find('K') != std::string::npos){
        castlingRights |= 0x1;
    }
    if (castlingPart.find('Q') != std::string::npos) castlingRights |= 0x2;
    if (castlingPart.find('k') != std::string::npos) castlingRights |= 0x4;
    if (castlingPart.find('q') != std::string::npos) castlingRights |= 0x8;
    std::cout << "Castling rights: " << static_cast<unsigned int>(castlingRights) << std::endl;

    // Parse en passant target square
    if (enPassantPart == "-") {
        enPassantTarget = -1;
    } else {
        int file = enPassantPart[0] - 'a';
        int rank = enPassantPart[1] - '1';
        enPassantTarget = rank * 8 + file;
    }
}

std::string Engine::generateFen() const {
    std::ostringstream oss;
    for (int row = 7; row >= 0; --row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            int square = row * 8 + col;
            uint64_t piece = bitboard.getPiece(square);
            if (piece == 0) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    oss << emptyCount;
                    emptyCount = 0;
                }
                char pieceChar;
                switch (piece & 0x7) { // Mask out the white piece bit
                    case 1: pieceChar = 'p'; break; // Pawn
                    case 2: pieceChar = 'r'; break; // Rook
                    case 3: pieceChar = 'n'; break; // Knight
                    case 4: pieceChar = 'b'; break; // Bishop
                    case 5: pieceChar = 'q'; break; // Queen
                    case 6: pieceChar = 'k'; break; // King
                    default: pieceChar = ' '; break; // Default case
                }
                if (piece & 0x8) { // Check the white piece bit
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

    // Add castling rights
    std::string castlingPart;
    std::cout << "Castling rights parsing: " << static_cast<unsigned int>(castlingRights) << std::endl;

    if (castlingRights & 0x1) castlingPart += 'K';
    if (castlingRights & 0x2) castlingPart += 'Q';
    if (castlingRights & 0x4) castlingPart += 'k';
    if (castlingRights & 0x8) castlingPart += 'q';
    if (castlingPart.empty()) castlingPart = "-";
    oss << ' ' << castlingPart;

    // Add en passant target square
    if (enPassantTarget == -1) {
        oss << " -";
    } else {
        char file = 'a' + (enPassantTarget % 8);
        char rank = '1' + (enPassantTarget / 8);
        oss << ' ' << file << rank;
    }

    return oss.str();
}

bool Engine::isKingInCheck(bool checkWhiteKing) const {
    int kingSquare = -1;
    for (int square = 0; square < 64; ++square) {
        uint64_t piece = bitboard.getPiece(square);
        if ((piece & 0x7) == 6 && ((piece & 0x8) == (checkWhiteKing ? 0x8 : 0))) {
            kingSquare = square;
            break;
        }
    }
    // if (kingSquare == -1) {
    //     std::cout << "checkWhiteKing: " << checkWhiteKing << std::endl;
    //     std::cout << "King not found" << std::endl;
    //     return false; // King not found, should not happen
    // }

    // Check for attacks from all directions
    static const int directions[] = {8, -8, 1, -1, 9, 7, -9, -7};
    for (int direction : directions) {
        int currentSquare = kingSquare;
        // std::cout << "King square: " << kingSquare << std::endl;
        while (true) {
            currentSquare += direction;
            if (currentSquare < 0 || currentSquare >= 64 || 
                (direction == 1 && currentSquare % 8 == 0) || 
                (direction == -1 && currentSquare % 8 == 7) || 
                (direction == 9 && currentSquare % 8 == 0) || 
                (direction == 7 && currentSquare % 8 == 7) || 
                (direction == -9 && currentSquare % 8 == 7) || 
                (direction == -7 && currentSquare % 8 == 0)) {
                // std::cout << "Direction: " << direction << std::endl;
                // std::cout << "Current square: " << currentSquare << std::endl;
                break;
            }
            uint64_t piece = bitboard.getPiece(currentSquare);
            if (piece != 0) {
                // std::cout << "Piece found: " << piece << std::endl;
                // std::cout << "Piece type: " << (piece & 0x7) << std::endl;
                // std::cout << "Piece colour: " << (piece & 0x8) << std::endl;
                // std::cout << "Location: " << currentSquare << std::endl;
                // std::cout << "Check white king: " << (checkWhiteKing ? 0x8 : 0) << std::endl;
                if ((piece & 0x8) != (checkWhiteKing ? 0x8 : 0)) {
                    // std::cout << "Piece colour: " << (piece & 0x8) << std::endl;
                    // std::cout << "Check white king: " << (checkWhiteKing ? 0x8 : 0) << std::endl;
                    int pieceType = piece & 0x7;
                    if ((pieceType == 2 || pieceType == 5) && (direction == 8 || direction == -8 || direction == 1 || direction == -1)) { // Rook or Queen (vertical/horizontal)
                        return true;
                    }
                    if ((pieceType == 4 || pieceType == 5) && (direction == 9 || direction == 7 || direction == -9 || direction == -7)) { // Bishop or Queen (diagonal)
                        return true;
                    }
                    if (pieceType == 6 && abs(currentSquare - kingSquare) <= direction){
                        return true;
                    }
                }
                break;
            }
        }
    }

    // Check for knight attacks
    static const int knightMoves[] = {15, 17, -15, -17, 10, -10, 6, -6};
    for (int move : knightMoves) {
        int targetSquare = kingSquare + move;
        if (targetSquare < 0 || targetSquare >= 64) {
            continue;
        }
        int rowDiff = abs((kingSquare / 8) - (targetSquare / 8));
        int colDiff = abs((kingSquare % 8) - (targetSquare % 8));
        if ((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2)) {
            uint64_t piece = bitboard.getPiece(targetSquare);
            if ((piece & 0x7) == 3 && ((piece & 0x8) != (checkWhiteKing ? 0x8 : 0))) {
                return true;
            }
        }
    }

    // Check for pawn attacks
    std::cout << "Checking for pawn attacks" << std::endl;
    if (isPawnAttackingKing(kingSquare, checkWhiteKing)) {
        std::cout << "Pawn attacking king" << std::endl;
        return true;
    }

    return false;
}

bool Engine::isPawnAttackingKing(int kingSquare, bool checkWhiteKing) const {
    int pawnDirection = checkWhiteKing ? 8 : -8;
    int pawnLeft = kingSquare + pawnDirection - 1;
    int pawnRight = kingSquare + pawnDirection + 1;
    std::cout << "King square: " << kingSquare << std::endl;
    std::cout << "Pawn left: " << pawnLeft << std::endl;
    std::cout << "Pawn right: " << pawnRight << std::endl;
    if (pawnLeft >= 0 && pawnLeft < 64 && (pawnLeft % 8) != 7) {
        uint64_t piece = bitboard.getPiece(pawnLeft);
        if ((piece & 0x7) == 1 && ((piece & 0x8) != (checkWhiteKing ? 0x8 : 0))) {
            std::cout << "Pawn left: " << piece << std::endl;
            return true;
        }
    }
    if (pawnRight >= 0 && pawnRight < 64 && (pawnRight % 8) != 0) {
        uint64_t piece = bitboard.getPiece(pawnRight);
        if ((piece & 0x7) == 1 && ((piece & 0x8) != (checkWhiteKing ? 0x8 : 0))) {
            std::cout << "Pawn right: " << piece << std::endl;
            return true;
        }
    }
    return false;
}

std::vector<std::pair<int, int>> Engine::generateLegalMoves(bool isSearch) {
    std::vector<std::pair<int, int>> moves;
    for (int square = 0; square < 64; ++square) {
        uint64_t piece = bitboard.getPiece(square);
        if (piece == 0) {
            continue;
        }
        bool isWhite = piece & 0x8;
        if (isWhite != isWhiteTurn) {
            continue;
        }
        switch (piece & 0x7) {
            case 1: // Pawn
                generatePawnMoves(square, moves);
                break;
            case 2: // Rook
                generateRookMoves(square, moves);
                break;
            case 3: // Knight
                generateKnightMoves(square, moves);
                break;
            case 4: // Bishop
                generateBishopMoves(square, moves);
                break;
            case 5: // Queen
                generateQueenMoves(square, moves);
                break;
            case 6: // King
                generateKingMoves(square, moves);
                break;
        }
    }

    // Filter out moves that leave the king in check
    std::vector<std::pair<int, int>> legalMoves;
    for (const auto& move : moves) {
        std::cout << "Move: " << move.first << " " << move.second << std::endl;
        applyMove(move, true);
        if (!isKingInCheck(!isWhiteTurn)) { // Check if the current player's king is in check
            legalMoves.push_back(move);
        }
        undoMove();
    }

    return legalMoves;
}

void Engine::generatePawnMoves(int square, std::vector<std::pair<int, int>>& moves) {
    int direction = isWhiteTurn ? 8 : -8;
    int startRow = isWhiteTurn ? 1 : 6;
    int enPassantRow = isWhiteTurn ? 4 : 3;

    // Single move forward
    int singleMove = square + direction;
    if (bitboard.getPiece(singleMove) == 0) {
        moves.push_back({square, singleMove});
        // Double move forward from starting position
        if ((square / 8) == startRow) {
            int doubleMove = square + 2 * direction;
            if (bitboard.getPiece(doubleMove) == 0) {
                moves.push_back({square, doubleMove});
            }
        }
    }

    // Captures
    int captureLeft = singleMove - 1;
    int captureRight = singleMove + 1;
    if (captureLeft >= 0 && (captureLeft % 8) != 7) {
        uint64_t piece = bitboard.getPiece(captureLeft);
        if (piece != 0 && ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0))) {
            moves.push_back({square, captureLeft});
        }
    }
    if (captureRight < 64 && (captureRight % 8) != 0) {
        uint64_t piece = bitboard.getPiece(captureRight);
        if (piece != 0 && ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0))) {
            moves.push_back({square, captureRight});
        }
    }

    // En passant
    if (enPassantTarget != -1 && (square / 8) == enPassantRow) {
        if (enPassantTarget == captureLeft || enPassantTarget == captureRight) {
            moves.push_back({square, enPassantTarget});
        }
    }
}

void Engine::generateRookMoves(int square, std::vector<std::pair<int, int>>& moves) {
    static const int directions[] = {8, -8, 1, -1}; // Up, Down, Right, Left
    for (int direction : directions) {
        int currentSquare = square;
        while (true) {
            currentSquare += direction;
            if (currentSquare < 0 || currentSquare >= 64 || 
                (direction == 1 && currentSquare % 8 == 0) || 
                (direction == -1 && currentSquare % 8 == 7)) {
                break;
            }
            uint64_t piece = bitboard.getPiece(currentSquare);
            if (piece == 0) {
                moves.push_back({square, currentSquare});
            } else {
                if ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0)) {
                    moves.push_back({square, currentSquare});
                }
                break;
            }
        }
    }
}

void Engine::generateKnightMoves(int square, std::vector<std::pair<int, int>>& moves) {
    static const int knightMoves[] = {15, 17, -15, -17, 10, -10, 6, -6};
    for (int move : knightMoves) {
        int targetSquare = square + move;
        if (targetSquare < 0 || targetSquare >= 64) {
            continue;
        }
        int rowDiff = abs((square / 8) - (targetSquare / 8));
        int colDiff = abs((square % 8) - (targetSquare % 8));
        if (rowDiff == 2 && colDiff == 1 || rowDiff == 1 && colDiff == 2) {
            uint64_t piece = bitboard.getPiece(targetSquare);
            if (piece == 0 || (piece & 0x8) != (isWhiteTurn ? 0x8 : 0)) {
                moves.push_back({square, targetSquare});
            }
        }
    }
}

void Engine::generateBishopMoves(int square, std::vector<std::pair<int, int>>& moves) {
    static const int directions[] = {9, 7, -9, -7}; // Diagonal directions
    for (int direction : directions) {
        int currentSquare = square;
        while (true) {
            currentSquare += direction;
            if (currentSquare < 0 || currentSquare >= 64 || 
                (direction == 9 && currentSquare % 8 == 0) || 
                (direction == 7 && currentSquare % 8 == 7) || 
                (direction == -9 && currentSquare % 8 == 7) || 
                (direction == -7 && currentSquare % 8 == 0)) {
                break;
            }
            uint64_t piece = bitboard.getPiece(currentSquare);
            if (piece == 0) {
                moves.push_back({square, currentSquare});
            } else {
                if ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0)) {
                    moves.push_back({square, currentSquare});
                }
                break;
            }
        }
    }
}

void Engine::generateQueenMoves(int square, std::vector<std::pair<int, int>>& moves) {
    static const int directions[] = {8, -8, 1, -1, 9, 7, -9, -7}; // All directions (rook + bishop)
    for (int direction : directions) {
        int currentSquare = square;
        while (true) {
            currentSquare += direction;
            if (currentSquare < 0 || currentSquare >= 64 || 
                (direction == 1 && currentSquare % 8 == 0) || 
                (direction == -1 && currentSquare % 8 == 7) || 
                (direction == 9 && currentSquare % 8 == 0) || 
                (direction == 7 && currentSquare % 8 == 7) || 
                (direction == -9 && currentSquare % 8 == 7) || 
                (direction == -7 && currentSquare % 8 == 0)) {
                break;
            }
            uint64_t piece = bitboard.getPiece(currentSquare);
            if (piece == 0) {
                moves.push_back({square, currentSquare});
            } else {
                if ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0)) {
                    moves.push_back({square, currentSquare});
                }
                break;
            }
        }
    }
}

void Engine::generateKingMoves(int square, std::vector<std::pair<int, int>>& moves) {
    static const int kingMoves[] = {8, -8, 1, -1, 9, 7, -9, -7}; // All possible king moves
    for (int move : kingMoves) {
        int targetSquare = square + move;
        if (targetSquare < 0 || targetSquare >= 64) {
            continue;
        }
        int rowDiff = abs((square / 8) - (targetSquare / 8));
        int colDiff = abs((square % 8) - (targetSquare % 8));
        if (rowDiff <= 1 && colDiff <= 1) {
            uint64_t piece = bitboard.getPiece(targetSquare);
            if (piece == 0 || (piece & 0x8) != (isWhiteTurn ? 0x8 : 0)) {
                moves.push_back({square, targetSquare});
            }
        }
    }

    // Castling moves
    if (isWhiteTurn) {
        if ((castlingRights & 0x1) && bitboard.getPiece(5) == 0 && bitboard.getPiece(6) == 0) { // White kingside
            moves.push_back({square, 6});
        }
        if ((castlingRights & 0x2) && bitboard.getPiece(1) == 0 && bitboard.getPiece(2) == 0 && bitboard.getPiece(3) == 0) { // White queenside
            moves.push_back({square, 2});
        }
    } else {
        if ((castlingRights & 0x4) && bitboard.getPiece(61) == 0 && bitboard.getPiece(62) == 0) { // Black kingside
            moves.push_back({square, 62});
        }
        if ((castlingRights & 0x8) && bitboard.getPiece(57) == 0 && bitboard.getPiece(58) == 0 && bitboard.getPiece(59) == 0) { // Black queenside
            moves.push_back({square, 58});
        }
    }
}

int Engine::evaluateBoard() const {

    int score = 0;
    for (int square = 0; square < 64; ++square) {
        uint64_t piece = bitboard.getPiece(square);
        if(piece == 0){
            continue;
        }

        bool isWhite = piece & 0x8;
        int pieceType = piece & 0x7;

        switch (pieceType) {
            case 1: //pawn
                score += isWhite ? PieceTables::pawnTable[square] : -PieceTables::pawnTable[63 - square];
                break;
            case 2: //rook
                score += isWhite ? PieceTables::rookTable[square] : -PieceTables::rookTable[63 - square];
                break;
            case 3: //knight
                score += isWhite ? PieceTables::knightTable[square] : -PieceTables::knightTable[63 - square];
                break;
            case 4: //bishop
                score += isWhite ? PieceTables::bishopTable[square] : -PieceTables::bishopTable[63 - square];
                break;
            case 5: //queen
                score += isWhite ? PieceTables::queenTable[square] : -PieceTables::queenTable[63 - square];
                break;
            case 6: //king
                score += isWhite ? PieceTables::kingTable[square] : -PieceTables::kingTable[63 - square];
                break;
        }
        int pieceValue = 0;
        switch (pieceType) {
            case 1: pieceValue = 100; break; // Pawn
            case 2: pieceValue = 500; break; // Rook
            case 3: pieceValue = 320; break; // Knight
            case 4: pieceValue = 330; break; // Bishop
            case 5: pieceValue = 900; break; // Queen
            case 6: pieceValue = 20000; break; // King
        }

        score += isWhite ? pieceValue : -pieceValue;
    }
    return score;
}

int Engine::minimax(int depth, int alpha, int beta, bool isMaximizing) {
    std::vector<std::pair<int, int>> legalMoves = generateLegalMoves(true);

    if (legalMoves.empty()) {
        // std::cout << "No legal moves found." << std::endl;
        if (isKingInCheck(isMaximizing)) {
            return isMaximizing ? -10000 : 10000; // Checkmate
        } else {
            return 0; // Stalemate
        }
    }

    if (depth == 0) {
        return evaluateBoard();
    }

    if (isMaximizing) {
        int maxEval = -10000;
        for (const auto& move : legalMoves) {
            applyMove(move, true);
            int eval = minimax(depth - 1, alpha, beta, false);
            undoMove();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = 10000;
        for (const auto& move : legalMoves) {
            applyMove(move, true);
            int eval = minimax(depth - 1, alpha, beta, true);
            undoMove();
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

std::pair<int, int> Engine::searchBestMove(int maxDepth) {
    //iterative deepening search with move ordering
    std::pair<int, int> bestMove = {0, 0};
    for (int depth = 1; depth <= maxDepth; ++depth) {
        int bestScore = -10000;
        std::vector<std::pair<int, int>> legalMoves = generateLegalMoves(true);
        std::sort(legalMoves.begin(), legalMoves.end(), [this](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return moveHeuristic(a) > moveHeuristic(b);
        });
        

        for (const auto& move : legalMoves) {
            applyMove(move, true);
            int score = minimax(depth - 1, -10000, 10000, isWhiteTurn); //negamax??
            undoMove();
            if (score > bestScore) {
                    bestScore = score;
                    bestMove = move;
            }
        }
        // std::cout << "Depth: " << depth << "Best move: " << bestMove.first << " " << bestMove.second << std::endl;
        // std::cout << "Depth: " << depth << " Best score: " << bestScore << std::endl;
    }
    return bestMove;

    // int bestScore = isWhiteTurn ? -10000 : 10000;
    // std::pair<int, int> bestMove = {0, 0}; // Initialize with a default move
    // std::vector<std::pair<int, int>> legalMoves = generateLegalMoves(true);
    // for (const auto& move : legalMoves) {
    //     std::cout << "Checking move: " << move.first << " " << move.second << std::endl;
    //     applyMove(move, true); // Pass true to indicate this is part of a search
    //     int score = minimax(depth - 1, -10000, 10000, isWhiteTurn);
    //     undoMove();
    //     std::cout << "Move score: " << score << std::endl;
    //     if (isWhiteTurn) {
    //         if (score > bestScore) {
    //             std::cout << "New best score white: " << score << std::endl;
    //             bestScore = score;
    //             bestMove = move;
    //         }
    //     } else {
    //         if (score < bestScore) {
    //             std::cout << "New best score black: " << score << std::endl;
    //             bestScore = score;
    //             bestMove = move;
    //         }
    //     }
    // }

    // // Check if the best score indicates checkmate
    // if ((isWhiteTurn && bestScore == 10000) || (!isWhiteTurn && bestScore == -10000)) {
    //     std::cout << "Checkmate is forced." << std::endl;
    // } else if (bestMove == std::pair<int, int>{0, 0}) {
    //     std::cout << "No valid moves found." << std::endl;
    // } else {
    //     std::cout << "Best score: " << bestScore << std::endl;
    // }

    // return bestMove;
}

int Engine::moveHeuristic(const std::pair<int, int>& move) {
    int start = move.first;
    int end = move.second;
    uint64_t movedPiece = bitboard.getPiece(start);
    uint64_t capturedPiece = bitboard.getPiece(end);

    int score = 0;
    //prioritize captures of high value pieces
    if(capturedPiece != 0) {
        score += 10 * (capturedPiece & 0x7);
    }

    //prioritize check and checkmate
    applyMove(move, true);
    if(isKingInCheck(!isWhiteTurn)) {
        std::cout << "Check found" << std::endl;
        score += 5;
        std::vector<std::pair<int, int>> opponentMoves = generateLegalMoves(true);
        if(opponentMoves.empty()) {
            std::cout << "Checkmate found" << std::endl;
            score += 10000; //checkmate
        }
    }
    undoMove();

    //promotions
    if ((movedPiece & 0x7) == 1 && (end / 8 == 0 || end / 8 == 7)) {
        score += 50;
    }
    return score;
}

void Engine::promotePawn(int square, char promotionPiece) {
    uint64_t piece;
    switch (tolower(promotionPiece)) {
        case 'q': piece = 5; break; // Queen
        case 'r': piece = 2; break; // Rook
        case 'b': piece = 4; break; // Bishop
        case 'n': piece = 3; break; // Knight
        default: piece = 5; break; // Default to Queen
    }
    if (isWhiteTurn) {
        piece |= 0x8; // Set the white piece bit
    }
    bitboard.setPiece(square, piece);
}

// void Engine::applyMove(const std::pair<int, int>& move) {
//     applyMove(move, false);
// }

void Engine::applyMove(const std::pair<int, int>& move, bool isSearch) {
    int start = move.first;
    int end = move.second;
    uint64_t movedPiece = bitboard.getPiece(start);
    uint64_t capturedPiece = bitboard.getPiece(end);
    bool wasPromotion = false;
    uint64_t originalPiece = movedPiece;

    // Handle en passant
    if ((movedPiece & 0x7) == 1 && end == enPassantTarget) { // Pawn
        int captureSquare = isWhiteTurn ? end - 8 : end + 8;
        capturedPiece = bitboard.getPiece(captureSquare);
        bitboard.clearSquare(captureSquare);
    }

    // Handle pawn promotion
    if ((movedPiece & 0x7) == 1 && (end / 8 == 0 || end / 8 == 7)) { // Pawn reaches the last rank
        wasPromotion = true;
        if (isSearch) {
            promotePawn(end, 'q'); // Automatically promote to queen during search
        } else {
            char promotionPiece;
            std::cout << "Promote pawn to (q/r/b/n): ";
            std::cin >> promotionPiece;
            promotePawn(end, promotionPiece);
        }
        movedPiece = bitboard.getPiece(end); // Update movedPiece to the promoted piece
    } else {
        bitboard.setPiece(end, movedPiece);
    }

    moveHistory.push({move, movedPiece, capturedPiece, start, end, castlingRights, enPassantTarget, wasPromotion, originalPiece}); // Save promotion info

    bitboard.clearSquare(start);

    // Update en passant target
    if ((movedPiece & 0x7) == 1 && abs(start - end) == 16) { // Pawn double move
        enPassantTarget = isWhiteTurn ? start + 8 : start - 8;
    } else {
        enPassantTarget = -1;
    }

    // Handle castling
    if ((movedPiece & 0x7) == 6) { // King
        if (abs(start - end) == 2) { // Castling move
            if (end == 62) { // White kingside
                bitboard.setPiece(61, bitboard.getPiece(63));
                bitboard.clearSquare(63);
            } else if (end == 58) { // White queenside
                bitboard.setPiece(59, bitboard.getPiece(56));
                bitboard.clearSquare(56);
            } else if (end == 6) { // Black kingside
                bitboard.setPiece(5, bitboard.getPiece(7));
                bitboard.clearSquare(7);
            } else if (end == 2) { // Black queenside
                bitboard.setPiece(3, bitboard.getPiece(0));
                bitboard.clearSquare(0);
            }
        }
    }

    // Update castling rights
    if ((movedPiece & 0x7) == 6) { // King
        if (isWhiteTurn) {
            castlingRights &= ~0x3; // Remove white castling rights
        } else {
            castlingRights &= ~0xC; // Remove black castling rights
        }
    } else if ((movedPiece & 0x7) == 2) { // Rook
        if (start == 0) castlingRights &= ~0x8; // Remove black queenside
        if (start == 7) castlingRights &= ~0x4; // Remove black kingside
        if (start == 56) castlingRights &= ~0x2; // Remove white queenside
        if (start == 63) castlingRights &= ~0x1; // Remove white kingside
    }

    isWhiteTurn = !isWhiteTurn;
}

void Engine::undoMove() {
    if (moveHistory.empty()) {
        std::cout << "No moves to undo." << std::endl;
        return;
    }

    MoveHistory lastMove = moveHistory.top();
    moveHistory.pop();

    // Restore the moved piece to its original position
    bitboard.setPiece(lastMove.originalPosition, lastMove.wasPromotion ? lastMove.originalPiece : lastMove.movedPiece);

    // Restore the captured piece, if any
    if (lastMove.capturedPiece != 0) {
        if ((lastMove.movedPiece & 0x7) == 1 && lastMove.targetPosition == lastMove.enPassantTarget) { // Pawn and en passant
            int captureSquare = isWhiteTurn ? lastMove.targetPosition + 8 : lastMove.targetPosition - 8;
            bitboard.setPiece(captureSquare, lastMove.capturedPiece);
            bitboard.clearSquare(lastMove.targetPosition);
        } else {
            bitboard.setPiece(lastMove.targetPosition, lastMove.capturedPiece);
        }
    } else {
        bitboard.clearSquare(lastMove.targetPosition);
    }

    // Handle castling
    if ((lastMove.movedPiece & 0x7) == 6) { // King
        if (abs(lastMove.originalPosition - lastMove.targetPosition) == 2) { // Castling move
            if (lastMove.targetPosition == 62) { // White kingside
                bitboard.setPiece(63, bitboard.getPiece(61));
                bitboard.clearSquare(61);
            } else if (lastMove.targetPosition == 58) { // White queenside
                bitboard.setPiece(56, bitboard.getPiece(59));
                bitboard.clearSquare(59);
            } else if (lastMove.targetPosition == 6) { // Black kingside
                bitboard.setPiece(7, bitboard.getPiece(5));
                bitboard.clearSquare(5);
            } else if (lastMove.targetPosition == 2) { // Black queenside
                bitboard.setPiece(0, bitboard.getPiece(3));
                bitboard.clearSquare(3);
            }
        }
    }

    // Restore castling rights
    castlingRights = lastMove.castlingRights;

    // Restore enPassantTarget
    enPassantTarget = lastMove.enPassantTarget;

    isWhiteTurn = !isWhiteTurn;
}

const Bitboard& Engine::getBitboard() const {
    return bitboard;
}

//testing fen generation and parsing

void printBitboard(const Bitboard& bitboard) {
    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; ++col) {
            int square = row * 8 + col;
            uint64_t piece = bitboard.getPiece(square);
            if (piece == 0) {
                std::cout << ".";
            } else {
                char pieceChar;
                switch (piece & 0x7) { // Mask out the white piece bit
                    case 1: pieceChar = 'p'; break; // Pawn
                    case 2: pieceChar = 'r'; break; // Rook
                    case 3: pieceChar = 'n'; break; // Knight
                    case 4: pieceChar = 'b'; break; // Bishop
                    case 5: pieceChar = 'q'; break; // Queen
                    case 6: pieceChar = 'k'; break; // King
                    default: pieceChar = ' '; break; // Default case
                }
                if (piece & 0x8) { // Check the white piece bit
                    pieceChar = toupper(pieceChar);
                }
                std::cout << pieceChar;
            }
        }
        std::cout << std::endl;
    }
}

void playGame() {
    Engine engine;
    std::string fen = "7k/8/5KP1/8/8/8/8/8 b - - 0 1";
    // std::string fen = "8/8/8/8/8/8/KRR5/7k w - - 0 1";
    engine.setBoardState(fen);

    while (true) {
        printBitboard(engine.getBitboard());
        std::cout << "Evaluation: " << engine.evaluateBoard() << std::endl;

        std::string bestMove = engine.getBestMove();
        std::cout << "Best move: " << bestMove << std::endl;

        std::string moveInput;
        std::cout << "Enter your move (e.g., e2e4): ";
        std::getline(std::cin, moveInput);

        if (moveInput == "quit") {
            break;
        }

        if (moveInput.length() != 4) {
            std::cout << "Invalid move format. Please enter a move in the format 'e2e4'." << std::endl;
            continue;
        }

        int startFile = moveInput[0] - 'a';
        int startRank = moveInput[1] - '1';
        int endFile = moveInput[2] - 'a';
        int endRank = moveInput[3] - '1';

        int startSquare = startRank * 8 + startFile;
        int endSquare = endRank * 8 + endFile;

        std::pair<int, int> move = {startSquare, endSquare};
        std::vector<std::pair<int, int>> legalMoves = engine.generateLegalMoves(false);

        if (std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end()) {
            engine.applyMove(move);
        } else {
            std::cout << "Illegal move. Please try again." << std::endl;
        }
    }
}

void testFENConversion(const std::string& fen) {
    Engine engine;
    engine.setBoardState(fen);

        //generate legal moves
    std::vector<std::pair<int, int>> legalMoves = engine.generateLegalMoves(false);
    std::cout << "Legal moves:" << std::endl;
    for (const auto& move : legalMoves) {
        std::cout << move.first << "-" << move.second << std::endl;
    }
    std::cout << "Number of moves: " << legalMoves.size() << std::endl;
    int eval = engine.evaluateBoard();
    std::cout << "Evaluation: " << eval << std::endl;

    std::cout << "Bitboard representation:" << std::endl;
    printBitboard(engine.getBitboard());
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
    playGame();
    return 0;
}