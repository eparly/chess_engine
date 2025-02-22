#include "engine.h"
#include <iostream>
#include <sstream>
#include "bitboard.h"
#include <vector>

Engine::Engine() : isWhiteTurn(true) {}

void Engine::setBoardState(const std::string &fen) {
    parseFen(fen);
}

std::string Engine::getBestMove() {
    auto bestMove = searchBestMove(3);
    std::ostringstream oss;
    oss << bestMove.first << "-" << bestMove.second;
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

bool Engine::isKingInCheck() const {
    int kingSquare = -1;
    for (int square = 0; square < 64; ++square) {
        uint64_t piece = bitboard.getPiece(square);
        if ((piece & 0x7) == 6 && ((piece & 0x8) == (isWhiteTurn ? 0x8 : 0))) {
            kingSquare = square;
            break;
        }
    }
    if (kingSquare == -1) {
        return false; // King not found, should not happen
    }

    // Check for attacks from all directions
    static const int directions[] = {8, -8, 1, -1, 9, 7, -9, -7};
    for (int direction : directions) {
        int currentSquare = kingSquare;
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
            if (piece != 0) {
                if ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0)) {
                    int pieceType = piece & 0x7;
                    if ((pieceType == 2 || pieceType == 5) || // Rook or Queen
                        (pieceType == 4 || pieceType == 5) || // Bishop or Queen
                        (pieceType == 6 && abs(direction) <= 1)) { // King
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
            if ((piece & 0x7) == 3 && ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0))) {
                return true;
            }
        }
    }

    // Check for pawn attacks
    int pawnDirection = isWhiteTurn ? -8 : 8;
    int pawnLeft = kingSquare + pawnDirection - 1;
    int pawnRight = kingSquare + pawnDirection + 1;
    if (pawnLeft >= 0 && pawnLeft < 64 && (pawnLeft % 8) != 7) {
        uint64_t piece = bitboard.getPiece(pawnLeft);
        if ((piece & 0x7) == 1 && ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0))) {
            return true;
        }
    }
    if (pawnRight >= 0 && pawnRight < 64 && (pawnRight % 8) != 0) {
        uint64_t piece = bitboard.getPiece(pawnRight);
        if ((piece & 0x7) == 1 && ((piece & 0x8) != (isWhiteTurn ? 0x8 : 0))) {
            return true;
        }
    }

    return false;
}

std::vector<std::pair<int, int>> Engine::generateLegalMoves() {
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
                std::cout << "Generating pawn moves for square " << square << std::endl;
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

    // Filter out moves that do not remove the check
    std::vector<std::pair<int, int>> legalMoves;
    for (const auto& move : moves) {
        applyMove(move);
        if (!isKingInCheck()) {
            legalMoves.push_back(move);
        }
        else{
            std::cout << "Move " << move.first << "-" << move.second << " puts king in check" << std::endl;
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
    // Implement a basic evaluation function using the Bitboard class
    int whiteCount = 0;
    int blackCount = 0;
    for (int square = 0; square < 64; ++square) {
        uint64_t piece = bitboard.getPiece(square);
        if (piece == 0) {
            continue;
        }
        int pieceValue;
        switch (piece & 0x7) { // Mask out the white piece bit
            case 1: pieceValue = 1; break; // Pawn
            case 2: pieceValue = 5; break; // Rook
            case 3: pieceValue = 3; break; // Knight
            case 4: pieceValue = 3; break; // Bishop
            case 5: pieceValue = 9; break; // Queen
            case 6: pieceValue = 1000; break; // King
            default: pieceValue = 0; break; // Default case
        }
        if (piece & 0x8) { // Check the white piece bit
            whiteCount += pieceValue;
        } else {
            blackCount += pieceValue;
        }
    }
    // Add your evaluation logic here
    return whiteCount - blackCount;
}

std::pair<int, int> Engine::searchBestMove(int depth) {
    // Implement a basic minimax search with alpha-beta pruning
    int bestScore = -10000;
    std::pair<int, int> bestMove = {0, 0}; // Initialize with a default move
    std::vector<std::pair<int, int>> legalMoves = generateLegalMoves();
    for (const auto& move : legalMoves) {
        applyMove(move);
        int score = -minimax(depth - 1, -10000, 10000, !isWhiteTurn);
        undoMove();
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}

int Engine::minimax(int depth, int alpha, int beta, bool isMaximizing) {
    if (depth == 0) {
        return evaluateBoard();
    }
    std::vector<std::pair<int, int>> legalMoves = generateLegalMoves();
    if (isMaximizing) {
        int maxEval = -10000;
        for (const auto& move : legalMoves) {
            applyMove(move);
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
            applyMove(move);
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

void Engine::applyMove(const std::pair<int, int>& move) {
    int start = move.first;
    int end = move.second;
    uint64_t movedPiece = bitboard.getPiece(start);
    uint64_t capturedPiece = bitboard.getPiece(end);

    moveHistory.push({move, movedPiece, capturedPiece, start, end, castlingRights}); // Save castling rights

    bitboard.setPiece(end, movedPiece);
    bitboard.clearSquare(start);

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

    // Handle en passant
    if ((movedPiece & 0x7) == 1 && end == enPassantTarget) { // Pawn
        int captureSquare = isWhiteTurn ? end - 8 : end + 8;
        bitboard.clearSquare(captureSquare);
    }

    // Update en passant target
    if ((movedPiece & 0x7) == 1 && abs(start - end) == 16) { // Pawn double move
        enPassantTarget = isWhiteTurn ? start + 8 : start - 8;
    } else {
        enPassantTarget = -1;
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
        return;
    }

    MoveHistory lastMove = moveHistory.top();
    moveHistory.pop();

    // Restore the moved piece to its original position
    bitboard.setPiece(lastMove.originalPosition, lastMove.movedPiece);

    // Restore the captured piece, if any
    if (lastMove.capturedPiece != 0) {
        bitboard.setPiece(lastMove.targetPosition, lastMove.capturedPiece);
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

    // Handle en passant
    if ((lastMove.movedPiece & 0x7) == 1 && lastMove.targetPosition == enPassantTarget) { // Pawn
        int captureSquare = isWhiteTurn ? lastMove.targetPosition + 8 : lastMove.targetPosition - 8;
        bitboard.setPiece(captureSquare, lastMove.capturedPiece);
    }

    // Restore castling rights
    castlingRights = lastMove.castlingRights;

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

void testFENConversion(const std::string& fen) {
    Engine engine;
    engine.setBoardState(fen);

        //generate legal moves
    std::vector<std::pair<int, int>> legalMoves = engine.generateLegalMoves();
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
    std::string fen; //  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    while (true) {
        std::cout << "Enter FEN: ";
        std::getline(std::cin, fen);
        testFENConversion(fen);
    }

    return 0;
}