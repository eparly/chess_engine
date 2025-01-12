#include "board.h"
#include "piece.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cctype>

Board::Board(int squareSize, sf::Color lightColor, sf::Color darkColor)
    : squareSize(squareSize), lightColor(lightColor), darkColor(darkColor) {
    initializeBoard();
}

void Board::initializeBoard() {
    whiteRookTexture.loadFromFile("images/white-rook.png");
    whiteKnightTexture.loadFromFile("images/white-knight.png");
    whiteBishopTexture.loadFromFile("images/white-bishop.png");
    whiteQueenTexture.loadFromFile("images/white-queen.png");
    whiteKingTexture.loadFromFile("images/white-king.png");
    whitePawnTexture.loadFromFile("images/white-pawn.png");
    blackRookTexture.loadFromFile("images/black-rook.png");
    blackKnightTexture.loadFromFile("images/black-knight.png");
    blackBishopTexture.loadFromFile("images/black-bishop.png");
    blackQueenTexture.loadFromFile("images/black-queen.png");
    blackKingTexture.loadFromFile("images/black-king.png");
    blackPawnTexture.loadFromFile("images/black-pawn.png");

    // Initialize the board with pieces using a FEN string
    std::string initialFEN = "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w KQkq - 0 1";
    parseFen(initialFEN);
    std::vector<std::string> moves = generateLegalMoves();
    legalMoves = std::unordered_set<std::string>(moves.begin(), moves.end());
}

void Board::parseFen(const std::string &fen) {
    std::istringstream fenStream(fen);
    std::string boardPart;
    fenStream >> boardPart;

    int row = 7;
    int col = 0;

    for (char c : boardPart) {
        if (c == '/') {
            row--;
            col = 0;
        } else if (isdigit(c)) {
            col += c - '0';
        } else {
            PieceType type;
            PieceColour colour;
            sf::Texture* texture = nullptr;

            switch (tolower(c)) {
                case 'p': type = PieceType::Pawn; texture = isupper(c) ? &whitePawnTexture : &blackPawnTexture; break;
                case 'r': type = PieceType::Rook; texture = isupper(c) ? &whiteRookTexture : &blackRookTexture; break;
                case 'n': type = PieceType::Knight; texture = isupper(c) ? &whiteKnightTexture : &blackKnightTexture; break;
                case 'b': type = PieceType::Bishop; texture = isupper(c) ? &whiteBishopTexture : &blackBishopTexture; break;
                case 'q': type = PieceType::Queen; texture = isupper(c) ? &whiteQueenTexture : &blackQueenTexture; break;
                case 'k': type = PieceType::King; texture = isupper(c) ? &whiteKingTexture : &blackKingTexture; break;
                default: continue;
            }

            colour = isupper(c) ? PieceColour::White : PieceColour::Black;
            addPiece(*texture, col, row, colour, type);
            col++;
        }
    }
}

std::vector<std::string> Board::generateLegalMoves() {
    std::vector<std::string> moves;
    for (Piece& piece : pieces) {
        if ((isWhiteTurn && piece.getColour() == PieceColour::White) 
            || (!isWhiteTurn && piece.getColour() == PieceColour::Black)) {
            std::vector<std::string> pieceMoves;
            switch (piece.getType()) {
                case PieceType::Pawn: pieceMoves = generatePawnMoves(piece); break;
                case PieceType::Rook: pieceMoves = generateRookMoves(piece); break;
                case PieceType::Knight: pieceMoves = generateKnightMoves(piece); break;
                case PieceType::Bishop: pieceMoves = generateBishopMoves(piece); break;
                case PieceType::Queen: pieceMoves = generateQueenMoves(piece); break;
                case PieceType::King: pieceMoves = generateKingMoves(piece); break;
            }
            for (const std::string& move : pieceMoves) {
                sf::Vector2i from(move[0] - 'a', move[1] - '1');
                sf::Vector2i to(move[2] - 'a', move[3] - '1');
                // std::cout << "Checking move: " << move << std::endl;
                if (!simulateMoveAndCheck(piece, to)) {
                    // std::cout << "Adding move: " << move << std::endl;
                    moves.push_back(move);
                }
            }
        }
        
    }
    //print out all legal moves
    // for(auto& move : moves){
    //     std::cout << move << std::endl;
    // }
    return moves;
}

bool Board::isKingInCheck(PieceColour colour) {
    sf::Vector2i kingPos;
    for (const Piece& piece : pieces) {
        if(piece.getType() == PieceType::King && piece.getColour() == colour){
            kingPos = sf::Vector2i(piece.getBoardPosition().x / squareSize, piece.getBoardPosition().y / squareSize);
            break;
        }
    }
    bool isKingAttacked = isPositionAttacked(kingPos.x, kingPos.y, colour);
    if (isKingAttacked) {
        std::cout << "King is in check" << std::endl;
    }
    return isKingAttacked;
}

bool Board::isPositionAttacked(int col, int row, PieceColour colour) {
    // std::cout << "Checking for checks" << std::endl;
    // std::cout << "Position: " << col << ", " << row << std::endl;
    // std::cout << "=============" << std::endl;
    for (const Piece& piece : pieces) {
        if(piece.getColour() != colour) {
            std::vector<std::string> moves;
            switch (piece.getType()) {
                case PieceType::Pawn: moves = generatePawnMoves(piece); break;
                case PieceType::Rook: moves = generateRookMoves(piece); break;
                case PieceType::Knight: moves = generateKnightMoves(piece); break;
                case PieceType::Bishop: moves = generateBishopMoves(piece); break;
                case PieceType::Queen: moves = generateQueenMoves(piece); break;
                case PieceType::King: {
                    int x = piece.getBoardPosition().x / squareSize;
                    int y = piece.getBoardPosition().y / squareSize;
                    // Directly check the king's attacking positions
                    // To avoid infinite loop while checking castling rights
                    std::vector<sf::Vector2i> kingMoves = {
                        {x, y + 1},
                        {x, y - 1},
                        {x + 1, y},
                        {x - 1, y},
                        {x + 1, y + 1},
                        {x + 1, y - 1},
                        {x - 1, y + 1},
                        {x - 1, y - 1}
                    };
                    for (const auto& move : kingMoves) {
                        if (move.x == col && move.y == row) {
                            return true;
                        }
                    }
                    break;
                }
            }
            for (const std::string & move : moves) {
                sf::Vector2i to(move[2] - 'a', move[3] - '1');
                if (to.x == col && to.y == row) {
                    // std::cout << "Position " << col << ", " << row << " is attacked by " << piece.getTypeAsString() << "at " << piece.getBoardPosition().x << "," << piece.getBoardPosition().y << std::endl;
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::simulateMoveAndCheck(Piece& piece, sf::Vector2i targetPos) {
    // std::cout << "Simulating move" << std::endl;
    // std::cout << "Piece type: " << piece.getTypeAsString() << std::endl;
    // std::cout << "Target position: " << targetPos.x << ", " << targetPos.y << std::endl;
    
    sf::Vector2i originalPos = sf::Vector2i(piece.getBoardPosition().x / squareSize, piece.getBoardPosition().y / squareSize);
    Piece* capturedPiece = nullptr;
    bool capturedPieceWasRemoved = false;
    // std::cout << "Original position: " << originalPos.x << ", " << originalPos.y << std::endl;
    for(Piece& p : pieces) {
        if (p.getBoardPosition().x / squareSize == targetPos.x && p.getBoardPosition().y / squareSize == targetPos.y) {
            // std::cout << "Capturing piece" << p.getTypeAsString() << std::endl;
            capturedPiece = &p;
            break;
        }
    }
    piece.setPosition(targetPos * squareSize);
    if(capturedPiece) {
        capturedPiece->setPosition(sf::Vector2i(-1000, -1000));
        capturedPieceWasRemoved = true;
        // pieces.erase(std::remove(pieces.begin(), pieces.end(), *capturedPiece), pieces.end());
    }

    bool inCheck = isKingInCheck(piece.getColour());
    piece.setPosition(originalPos * squareSize);
    if(capturedPiece && capturedPieceWasRemoved) {
        capturedPiece->setPosition(targetPos * squareSize);
    }
    return inCheck;
}

std::vector<std::string> Board::generatePawnMoves(const Piece& piece) {
    std::vector<std::string> moves;
    int direction = (piece.getColour() == PieceColour::White) ? 1 : -1;
    int startRow = (piece.getColour() == PieceColour::White) ? 1 : 6;
    sf::Vector2i piecePosition = piece.getBoardPosition();
    int row = piecePosition.y / squareSize;
    int col = piecePosition.x / squareSize;

    // Move forward one square
    if (isValidPosition(col, row + direction) && isEmpty(col, row + direction)) {
        std::string move = moveToString(piecePosition / squareSize, sf::Vector2i(col, row + direction));
        moves.push_back(move);
    }

    // Move forward two squares from starting position
    if (row == startRow && isEmpty(col, row + direction) && isEmpty(col, row + 2 * direction)) {
        moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col, row + 2 * direction)));
    }

    // Capture diagonally left
    if (isValidPosition(col - 1, row + direction) && isEnemyPiece(col - 1, row + direction, piece.getColour())) {
        std::cout << "Capture left" << std::endl;
        std::cout << moveToString(piecePosition / squareSize, sf::Vector2i(col - 1, row + direction)) << std::endl;
        moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col - 1, row + direction)));
    }

    // Capture diagonally right
    if (isValidPosition(col + 1, row + direction) && isEnemyPiece(col + 1, row + direction, piece.getColour())) {
        std::cout << "Capture right" << std::endl;
        moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col + 1, row + direction)));
    }

    // En passant capture
    if (isValidPosition(col + 1, row + direction) && sf::Vector2i(col + 1, row + direction) == enPassantTarget) {
        std::cout << "En passant capture right" << std::endl;
        std::cout << "col: " << col << " row: " << row << std::endl;
        std::cout << "enPassantTarget: " << enPassantTarget.x << ", " << enPassantTarget.y << std::endl;
        std::cout << moveToString(sf::Vector2i(col, row), sf::Vector2i(col + 1, row + direction)) << std::endl;
        moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(col + 1, row + direction)));
    }
    if (isValidPosition(col - 1, row + direction) && sf::Vector2i(col - 1, row + direction) == enPassantTarget) {
        std::cout << "En passant capture left" << std::endl;
        std::cout << moveToString(sf::Vector2i(col, row), sf::Vector2i(col - 1, row + direction)) << std::endl;
        moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(col - 1, row + direction)));
    }
    // std::cout << "Pawn moves" << std::endl;
    // for(auto move : moves){
    //     std::cout << move << std::endl;
    // }
    return moves;
}

std::vector<std::string> Board::generateRookMoves(const Piece& piece) {
    std::vector<std::string> moves;
    sf::Vector2i piecePosition = piece.getBoardPosition();
    int row = piecePosition.y / squareSize;
    int col = piecePosition.x / squareSize;

    // Move up
    for (int r = row + 1; r < 8; ++r) {
        // std::cout << "Checking for piece at: " << col << ", " << r << std::endl;
        if (isEmpty(col, r)) {
            // std::cout << "Empty square" << std::endl;
            moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col, r)));
        }
        else if (isEnemyPiece(col, r, piece.getColour())) {
            // std::cout << "Enemy piece" << std::endl;
            moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col, r)));
            break;
        }
        else {
            // std::cout << "Friendly piece" << std::endl;
            break;
        }
    }
    // Move down
    for (int r = row - 1; r >= 0; --r) {
        if (isEmpty(col, r)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(col, r)));
        } else if (isEnemyPiece(col, r, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(col, r)));
            break;
        } else {
            break;
        }
    }

    // Move right
    for (int c = col + 1; c < 8; ++c) {
        if (isEmpty(c, row)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, row)));
        } else if (isEnemyPiece(c, row, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, row)));
            break;
        } else {
            break;
        }
    }

    // Move left
    for (int c = col - 1; c >= 0; --c) {
        if (isEmpty(c, row)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, row)));
        } else if (isEnemyPiece(c, row, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, row)));
            break;
        } else {
            break;
        }
    }
        // Implement rook movement rules
    return moves;
}

std::vector<std::string> Board::generateKnightMoves(const Piece& piece) {
    std::vector<std::string> moves;
    int row = piece.getBoardPosition().y / squareSize;
    int col = piece.getBoardPosition().x / squareSize;

    std::vector<sf::Vector2i> knightMoves = {
        {col + 1, row + 2}, {col + 1, row - 2}, {col - 1, row + 2}, {col - 1, row - 2}, {col + 2, row + 1}, {col + 2, row - 1}, {col - 2, row + 1}, {col - 2, row - 1}};

    for (const auto& move: knightMoves) {
        if (isValidPosition(move.x, move.y) && (isEmpty(move.x, move.y) || isEnemyPiece(move.x, move.y, piece.getColour()))) {
            moves.push_back(moveToString(piece.getBoardPosition() / squareSize, move));
        }
    }
    // Implement knight movement rules
    return moves;
}

std::vector<std::string> Board::generateBishopMoves(const Piece& piece) {
    std::vector<std::string> moves;
    int row = piece.getBoardPosition().y / squareSize;
    int col = piece.getBoardPosition().x / squareSize;

    // Move diagonally up-right
    for (int r = row + 1, c = col + 1; r < 8 && c < 8; ++r, ++c) {
        if (isEmpty(c, r)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
        } else if (isEnemyPiece(c, r, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
            break;
        } else {
            break;
        }
    }

    // Move diagonally up-left
    for (int r = row + 1, c = col - 1; r < 8 && c >= 0; ++r, --c) {
        if (isEmpty(c, r)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
        } else if (isEnemyPiece(c, r, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
            break;
        } else {
            break;
        }
    }

    // Move diagonally down-right
    for (int r = row - 1, c = col + 1; r >= 0 && c < 8; --r, ++c) {
        if (isEmpty(c, r)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
        } else if (isEnemyPiece(c, r, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
            break;
        } else {
            break;
        }
    }

    // Move diagonally down-left
    for (int r = row - 1, c = col - 1; r >= 0 && c >= 0; --r, --c) {
        if (isEmpty(c, r)) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
        } else if (isEnemyPiece(c, r, piece.getColour())) {
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(c, r)));
            break;
        } else {
            break;
        }
    }
    // Implement bishop movement rules
    return moves;
}

std::vector<std::string> Board::generateQueenMoves(const Piece& piece) {
    std::vector<std::string> moves;

    // Combine rook and bishop moves
    std::vector<std::string> rookMoves = generateRookMoves(piece);
    std::vector<std::string> bishopMoves = generateBishopMoves(piece);

    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

    // for(auto move : moves){
    //     std::cout << move << std::endl;
    // }
    return moves;
}

std::vector<std::string> Board::generateKingMoves(const Piece& piece) {
    std::vector<std::string> moves;
    int row = piece.getBoardPosition().y / squareSize;
    int col = piece.getBoardPosition().x / squareSize;

    std::vector<sf::Vector2i> kingMoves = {
        {col, row + 1}, {col, row - 1}, {col + 1, row}, {col - 1, row},
        {col + 1, row + 1}, {col + 1, row - 1}, {col - 1, row + 1}, {col - 1, row - 1}
    };

    for (const auto& move : kingMoves) {
        if (isValidPosition(move.x, move.y) && (isEmpty(move.x, move.y) || isEnemyPiece(move.x, move.y, piece.getColour()))) {
            moves.push_back(moveToString(sf::Vector2i(col, row), move));
        }
    }
    //castling
    // std::cout << "Checking for castling" << std::endl;
    if(piece.getColour() == PieceColour::White) {
        // std::cout << "Checking kingside castling white" << std::endl;
        if(canCastleKingside(PieceColour::White)){
            // std::cout << "White can castle kingside" << std::endl;
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(1, 0)));
        }
        // std::cout << "Checking queenside castling white" << std::endl;
        if(canCastleQueenside(PieceColour::White)){
            // std::cout << "White can castle queenside" << std::endl;
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(5, 0)));
        }
    } else {
        // std::cout << "Checking kingside castling black" << std::endl;
        if(canCastleKingside(PieceColour::Black)){
            // std::cout << "Black can castle kingside" << std::endl;
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(1, 7)));
        }
        // std::cout << "Checking queenside castling black" << std::endl;
        if(canCastleQueenside(PieceColour::Black)){
            // std::cout << "Black can castle queenside" << std::endl;
            moves.push_back(moveToString(sf::Vector2i(col, row), sf::Vector2i(5, 7)));
        }
    }
    return moves;
}

bool Board::canCastleKingside(PieceColour colour) {
    if (colour == PieceColour::White) {
        // std::cout << "Checking white" << std::endl;
        // std::cout << "White king moved: " << whiteKingMoved << std::endl;
        // std::cout << "White kingside rook moved: " << whiteKingsideRookMoved << std::endl;
        if(whiteKingMoved || whiteKingsideRookMoved){
            // std::cout << "White king or kingside rook has moved" << std::endl;
            return false;
        }
        // std::cout << "Checking if squares are empty" << std::endl;
        // std::cout << "Square 1: " << isEmpty(1, 0) << std::endl;
        // std::cout << "Square 2: " << isEmpty(2, 0) << std::endl;
        if(!isEmpty(1, 0) || !isEmpty(2, 0)) return false;
        // std::cout << "Checking if squares are attacked" << std::endl;
        // std::cout << "Square 1: " << isPositionAttacked(1, 0, PieceColour::White) << std::endl;
        // std::cout << "Square 2: " << isPositionAttacked(2, 0, PieceColour::White) << std::endl;
        if (isPositionAttacked(1, 0, PieceColour::White) || isPositionAttacked(2, 0, PieceColour::White)) return false;
    } else {
        if(blackKingMoved || blackKingsideRookMoved) return false;
        if(!isEmpty(1, 7) || !isEmpty(2, 7)) return false;
        if (isPositionAttacked(1, 7, PieceColour::Black) || isPositionAttacked(2, 7, PieceColour::Black)) return false;
    }
    return true;
}

bool Board::canCastleQueenside(PieceColour colour) {
    if (colour == PieceColour::White) {
        // std::cout << "Checking white" << std::endl;
        // std::cout << "White king moved: " << whiteKingMoved << std::endl;
        // std::cout << "White queenside rook moved: " << whiteQueensideRookMoved << std::endl;
        if (whiteKingMoved || whiteQueensideRookMoved) return false;

        // std::cout << "Checking if squares are empty" << std::endl;
        // std::cout << "Square 1: " << isEmpty(4, 0) << std::endl;
        // std::cout << "Square 2: " << isEmpty(5, 0) << std::endl;
        // std::cout << "Square 3: " << isEmpty(6, 0) << std::endl;
        if (!isEmpty(4, 0) || !isEmpty(5, 0) || !isEmpty(6, 0)) return false;
        // std::cout << "Checking if squares are attacked" << std::endl;
        // std::cout << "Square 1: " << isPositionAttacked(4, 0, PieceColour::White) << std::endl;
        // std::cout << "Square 2: " << isPositionAttacked(3, 0, PieceColour::Black) << std::endl;
        // std::cout << "Square 3: " << isPositionAttacked(2, 0, PieceColour::Black) << std::endl;
        if (isPositionAttacked(4, 0, PieceColour::White) || isPositionAttacked(5, 0, PieceColour::White) || isPositionAttacked(6, 0, PieceColour::White)) return false;
    } else {
        if (blackKingMoved || blackQueensideRookMoved) return false;
        if (!isEmpty(4, 7) || !isEmpty(5, 7) || !isEmpty(6, 7)) return false;
        if (isPositionAttacked(4, 7, PieceColour::Black) || isPositionAttacked(5, 7, PieceColour::Black) || isPositionAttacked(6, 7, PieceColour::Black)) return false;
    }
    return true;
}

void Board::performCastling(Piece& king, sf::Vector2i targetPos) {
    if (king.getColour() == PieceColour::White) {
        if (targetPos == sf::Vector2i(1, 0)) {
            // Kingside castling
            for (Piece& piece : pieces) {
                if (piece.getType() == PieceType::Rook && 
                    piece.getColour() == PieceColour::White //&&
                    // piece.getBoardPosition() == sf::Vector2i(0 * squareSize, 0)
                ) {
                    // std::cout << "Performing castling" << std::endl;
                    // std::cout << "Rook position: " << piece.getBoardPosition().x << ", " << piece.getBoardPosition().y << std::endl;
                    if(piece.getBoardPosition() / squareSize == sf::Vector2i(0 * squareSize, 0)){
                        sf::Vector2i newPos = sf::Vector2i(2 * squareSize, 0);
                        piece.setPosition(newPos);
                        break;
                    }
                    // sf::Vector2i newPos = sf::Vector2i(2 * squareSize, 0);
                    // piece.setPosition(newPos);
                    // break;
                }
            }
        }
        else if (targetPos == sf::Vector2i(5, 0))
        {
            // Queenside castling
            for (Piece& piece : pieces) {
                if (piece.getType() == PieceType::Rook && piece.getColour() == PieceColour::White && piece.getBoardPosition() / squareSize == sf::Vector2i(7, 0)) {
                    sf::Vector2i newPos = sf::Vector2i(4 * squareSize, 0);
                    piece.setPosition(newPos);
                    break;
                }
            }
        }
        whiteKingMoved = true;
    } else {
        if (targetPos == sf::Vector2i(1, 7)) {
            // Kingside castling
            for (Piece& piece : pieces) {
                if (piece.getType() == PieceType::Rook && piece.getColour() == PieceColour::Black && piece.getBoardPosition() / squareSize == sf::Vector2i(0, 7)) {
                    sf::Vector2i newPos = sf::Vector2i(2 * squareSize, 7 * squareSize);
                    piece.setPosition(newPos);
                    break;
                }
            }
        } else if (targetPos == sf::Vector2i(5, 7)) {
            // Queenside castling
            for (Piece& piece : pieces) {
                if (piece.getType() == PieceType::Rook && piece.getColour() == PieceColour::Black && piece.getBoardPosition() / squareSize == sf::Vector2i(7, 7)) {
                    sf::Vector2i newPos = sf::Vector2i(4 * squareSize, 7 * squareSize);
                    piece.setPosition(newPos);
                    break;
                }
            }
        }
        blackKingMoved = true;
    }
}

bool Board::isValidPosition(int col, int row) {
    return col >= 0 && col < 8 && row >= 0 && row < 8;
}

bool Board::isEmpty(int col, int row) {
    for (const Piece& piece : pieces) {
        if (piece.getBoardPosition().x / squareSize == col && piece.getBoardPosition().y / squareSize == row) {
            return false;
        }
    }
    return true;
}

bool Board::isEnemyPiece(int col, int row, PieceColour colour) {
    // std::cout << "Checking for enemy piece" << std::endl;
    for (const Piece& piece : pieces) {
        if (piece.getBoardPosition().x / squareSize == col && piece.getBoardPosition().y / squareSize == row && piece.getColour() != colour) {
            // std::cout << "Enemy piece found" << std::endl;
            return true;
        }
    }
    return false;
}

std::string Board::moveToString(sf::Vector2i from, sf::Vector2i to) {
    // std::cout << "From: " << from.x << ", " << from.y << std::endl;
    // std::cout << "To: " << to.x << ", " << to.y << std::endl;
    char colFrom = 'a' + from.x;
    char rowFrom = '1' + from.y;
    char colTo = 'a' + to.x;
    char rowTo = '1' + to.y;
    // std::cout << "Move: " << colFrom << rowFrom << colTo << rowTo << std::endl;
    return std::string() + colFrom + rowFrom + colTo + rowTo;
}

bool Board::isLegalMove(const std::string &move) {
    bool isLegal = legalMoves.find(move) != legalMoves.end();
    // std::cout << "Is legal move: " << isLegal << std::endl;
    return isLegal;
}

void Board::draw(sf::RenderWindow& window) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
            square.setPosition(i * squareSize, j * squareSize);
            square.setFillColor((i + j) % 2 == 0 ? lightColor : darkColor);
            window.draw(square);
        }
    }

    if(highlightSquare){
        sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
        square.setPosition(highlightedX * squareSize, highlightedY * squareSize);
        square.setFillColor(sf::Color(0, 255, 0, 128));
        window.draw(square);
    }

    if(gameOver) {
        sf::RectangleShape highlight(sf::Vector2f(squareSize, squareSize));
        highlight.setPosition(checkmateKingPosition.x * squareSize, checkmateKingPosition.y * squareSize);
        highlight.setFillColor(sf::Color(255, 0, 0, 128));
        window.draw(highlight);
    }

    for (auto& piece : pieces) {
        piece.draw(window);
    }
}

void Board::addPiece(const sf::Texture& texture, int x, int y, PieceColour colour, PieceType type) {
    sf::Vector2i position(x*100, y*100);
    pieces.emplace_back(texture, position.x, position.y, squareSize, colour, type);
}

sf::Vector2i Board::snapToSquare(const sf::Vector2f& position) {
    int snappedX = std::floor(position.x / squareSize) * squareSize;
    int snappedY = std::floor(position.y / squareSize) * squareSize;
    return sf::Vector2i(snappedX, snappedY);
}

bool Board::isWithinBounds(const sf::Vector2f& position) {
    return position.x >= 0 && position.x < 8 * squareSize &&
           position.y >= 0 && position.y < 8 * squareSize;
}

void Board::handleEvent(sf::Event& event, sf::RenderWindow& window) {
    highlightedX = event.mouseButton.x / squareSize;
    highlightedY = event.mouseButton.y / squareSize;

    if(highlightedX >= 0 && highlightedX < 8 && highlightedY >= 0 && highlightedY < 8){
        highlightSquare = true;
    }
    else {
        highlightSquare = false;
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

            if (selectedPiece == nullptr) {
                // Select a piece if clicked on it
                selectPiece(mousePos);
            } else {
                // Move the selected piece to the new position
                sf::Vector2i targetPos = snapToSquare(mousePos);
                sf::Vector2i originalPos = sf::Vector2i(selectedPiece->getBoardPosition().x / squareSize, selectedPiece->getBoardPosition().y / squareSize);

                std::string move = moveToString(sf::Vector2i(selectedPiece->getBoardPosition().x / squareSize, selectedPiece->getBoardPosition().y / squareSize), targetPos / squareSize);
                std::cout << "Move: " << move << std::endl;
                if (isLegalMove(move) && isWhiteTurn == (selectedPiece->getColour() == PieceColour::White)) {
                    std::cout << "targetPos: " << targetPos.x << ", " << targetPos.y << std::endl;
                    if (selectedPiece->getType() == PieceType::King && 
                            (targetPos / squareSize == sf::Vector2i(1, 0) && canCastleKingside(PieceColour::White) || 
                            targetPos / squareSize == sf::Vector2i(5, 0) && canCastleQueenside(PieceColour::White) || 
                            targetPos / squareSize == sf::Vector2i(1, 7) && canCastleKingside(PieceColour::Black) || 
                            targetPos / squareSize == sf::Vector2i(5, 7) && canCastleQueenside(PieceColour::Black)
                            )
                        ){
                        std::cout << "Performing castling" << std::endl;
                        performCastling(*selectedPiece, targetPos / squareSize);
                    } else {
                        std::cout << "checking en passant" << std::endl;
                        std::cout << "targetPos: " << targetPos.x / squareSize << ", " << targetPos.y / squareSize << std::endl;
                        std::cout << "enPassantTarget: " << enPassantTarget.x << ", " << enPassantTarget.y << std::endl;
                        if(selectedPiece->getType() == PieceType::Pawn && targetPos / squareSize == enPassantTarget) {
                            std::cout << "En passant found" << std::endl;
                            int captureRow = isWhiteTurn ? targetPos.y / squareSize - 1 : targetPos.y / squareSize + 1;
                            std::cout << "Capture row: " << captureRow << std::endl;
                            std::cout << "move: " << move << std::endl;
                            std::cout << "Selected piece position 1: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;
                            for (auto it = pieces.begin(); it != pieces.end(); ++it) {
                                if(it->getBoardPosition().x / squareSize == targetPos.x / squareSize && it->getPosition().y / squareSize == captureRow) {
                                    std::cout << "Capturing piece at: " << it->getPosition().x << ", " << it->getPosition().y << std::endl;
                                    std::cout << "Selected piece position 3: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;

                                    // Store the index of the selected piece
                                    int selectedIndex = std::distance(pieces.begin(), std::find(pieces.begin(), pieces.end(), *selectedPiece));
                                    std::cout << "Selected piece index: " << selectedIndex << std::endl;
                                    std::cout << "size1: " << pieces.size() << std::endl;
                                    pieces.erase(it);
                                    std::cout << "size2: " << pieces.size() << std::endl;


                                    // Restore the selected piece pointer
                                    //This is super janky but for some reason it works
                                    //Do not touch as long as everything is working!!
                                    if(selectedPiece->getColour() == PieceColour::White) {
                                        selectedPiece = &pieces[selectedIndex-1];
                                    }
                                    else {
                                        selectedPiece = &pieces[selectedIndex];
                                    }
                                    std::cout << "Selected piece position 4: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;

                                    break;
                                }
                            }
                            std::cout << "Selected piece position 2: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;

                            std::cout << "Piece captured" << std::endl;
                            selectedPiece->setPosition(targetPos);
                        }
                    }
                    // Check if there is a piece at the target position
                    selectedPiece->setPosition(targetPos);

                    //handle pawn promotion
                    std::cout << "Checking pawn promotion" << std::endl;
                    std::cout << "Selected piece type: " << selectedPiece->getTypeAsString() << std::endl;
                    std::cout << "Target position: " << targetPos.x << ", " << targetPos.y << std::endl;
                    if(selectedPiece->getType() == PieceType::Pawn && (targetPos.y / squareSize == 0 || targetPos.y / squareSize == 7)) {
                        std::cout << "Promoting pawn" << std::endl;
                        PieceType promotionType = showPromotionWindow(window, selectedPiece->getColour());
                        selectedPiece->setType(promotionType);
                    }

                    // Set en passant target
                    std::cout << "Checking en passant" << std::endl;
                    std::cout << abs(targetPos.y / squareSize - originalPosition.y / squareSize) << std::endl;
                    if (selectedPiece->getType() == PieceType::Pawn && abs(targetPos.y / squareSize - originalPosition.y / squareSize) == 2)
                    {
                        std::cout << "Setting en passant target" << std::endl;
                        std::cout << "Target position: " << targetPos.x << ", " << targetPos.y << std::endl;
                        enPassantTarget = sf::Vector2i(targetPos.x / squareSize, (targetPos.y / squareSize + originalPosition.y / squareSize) / 2);
                    }
                    else
                    {
                        enPassantTarget = sf::Vector2i(-500, -500);
                    }
                    std::cout << "Moving piece" << selectedPiece->getTypeAsString() << std::endl;
                    std::cout << "Target position: " << targetPos.x << ", " << targetPos.y << std::endl;
                    std::cout << "Original position: " << originalPosition.x << ", " << originalPosition.y << std::endl;
                    std::cout << "Selected piece position: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;
                    bool isPieceAtTarget = false;
                    for (auto& piece : pieces) {
                        if (piece.getBoardPosition() == targetPos && selectedPiece != &piece) {
                            std::cout << "Piece at target position: " << piece.getTypeAsString() << std::endl;
                            isPieceAtTarget = true;
                            if (piece.getColour() != selectedPiece->getColour()) {
                                capturePiece(piece);
                            }
                            break;
                        }
                    }

                    //update castling rights
                    if (selectedPiece->getType() == PieceType::King) {
                        if (selectedPiece->getColour() == PieceColour::White) {
                            whiteKingMoved = true;
                        } else {
                            blackKingMoved = true;
                        }
                    } else if (selectedPiece->getType() == PieceType::Rook) {
                        if (selectedPiece->getColour() == PieceColour::White) {
                            if (originalPosition / squareSize == sf::Vector2i(7, 0)) {
                                whiteQueensideRookMoved = true;
                            } else if (originalPosition / squareSize == sf::Vector2i(0, 0)) {
                                whiteKingsideRookMoved = true;
                            }
                        } else {
                            // std::cout << "Black rook moved" << std::endl;
                            // std::cout << "Original position: " << originalPosition.x << ", " << originalPosition.y << std::endl;
                            if (originalPosition / squareSize == sf::Vector2i(7, 7)) {
                                blackQueensideRookMoved = true;
                            } else if (originalPosition / squareSize == sf::Vector2i(0, 7)) {
                                blackKingsideRookMoved = true;
                            }
                        }
                    }
                    if (isCheckmate(isWhiteTurn ? PieceColour::Black : PieceColour::White)) {
                        gameOver = true;
                        gameOverMessage = (isWhiteTurn ? "White" : "Black") + std::string(" wins by checkmate!");
                        // Store the position of the king in checkmate
                        for (Piece& piece : pieces) {
                            if (piece.getType() == PieceType::King && piece.getColour() == (isWhiteTurn ? PieceColour::Black : PieceColour::White)) {
                                checkmateKingPosition = sf::Vector2i(piece.getPosition().x / squareSize, piece.getPosition().y / squareSize);
                                break;
                            }
                        }
                        std::cout << gameOverMessage << std::endl;
                    }
                    endTurn();
                }
                // Unselect the piece
                selectedPiece = nullptr;
            }
        }
    }
}

void Board::selectPiece(const sf::Vector2f& mousePos) {
    for(auto& piece : pieces) {
        if(piece.getBounds().contains(mousePos)) {
            selectedPiece = &piece;
            originalPosition = piece.getBoardPosition();
            std::cout << "Selecting piece of type: " << selectedPiece->getTypeAsString() << std::endl;
            std::cout << "Selected piece position: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;
            break;
        }
    }
}

bool Board::isValidMove(Piece& piece, sf::Vector2i position) {
    int x = position.x / squareSize;
    int y = position.y / squareSize;
    //check if the  spot is occupied by a piece of the same colour
    for(auto& p : pieces){
        sf::Vector2i piecePosition = p.getBoardPosition();
        // std::cout << "Piece position: " << piecePosition.x << ", " << piecePosition.y << std::endl;
        // std::cout << "Position: " << position.x << ", " << position.y << std::endl;
        if(position == piecePosition && p.getColour() == piece.getColour()){
            std::cout << "Invalid move: spot is occupied by a piece of the same colour" << std::endl;
            return false;
        }
    }
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}


void Board::endTurn() {
    std::cout << "Ending turn" << std::endl;
    std::cout << "==========" << std::endl;
    isWhiteTurn = !isWhiteTurn;
    std::vector<std::string> moves = generateLegalMoves();
    legalMoves = std::unordered_set<std::string>(moves.begin(), moves.end());
    // for(auto& move : legalMoves){
    //     std::cout << move << std::endl;
    // }
}

void Board::capturePiece(Piece& piece) {
    // auto it = std::find_if(pieces.begin(), pieces.end(), [&piece](const Piece& p) {
    //     return &p == &piece;
    // });

    // if (it != pieces.end()) {
    //     pieces.erase(it);
    // }

    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if(it->getPosition() == piece.getPosition() && it->getColour() == piece.getColour()) {
            std::cout << "Capturing piece" << it->getTypeAsString() << std::endl;
            std::cout << "Selected piece position 3: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;

            pieces.erase(it);
            std::cout << "Selected piece position 4: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;

            break;
        }
    }

}

bool Board::isCheckmate(PieceColour colour) {
    std::cout << "Checking for checkmate" << std::endl;
    std::cout << "colour: " << (colour==PieceColour::White? "white" : "black") << std::endl;
    if(!isKingInCheck(colour)) return false;
    std::cout << "the King is in check" << std::endl;
    std::vector<std::string> moves = generateLegalMoves();

    for (const std::string& move : moves) {
        sf::Vector2i from(move[0] - 'a', move[1] - '1');
        sf::Vector2i to(move[2] - 'a', move[3] - '1');
        std::cout << "Checking Move: " << move << std::endl;
        Piece* piece = nullptr;
        for (Piece& p : pieces) {
            if (p.getBoardPosition() / squareSize == from && p.getColour() == colour) {
                piece = &p;
                break;
            }
        }


        // Simulate the move and check if it removes the check
        if (piece) {
            // Temporarily move the piece
            sf::Vector2i originalPos = sf::Vector2i(piece->getPosition().x / squareSize, piece->getPosition().y / squareSize);
            piece->setPosition(to*squareSize);

            // Check if the king is still in check
            bool stillInCheck = isKingInCheck(colour);

            // Restore the original position
            piece->setPosition(originalPos);

            if (!stillInCheck) {
                std::cout << "Found a move that removes the check: " << move << std::endl;
                return false; // There is a move that removes the check
            }
        }
    }

    return true;
}

PieceType showPromotionWindow(sf::RenderWindow& window, PieceColour colour) {
    std::cout << "Showing promotion window" << std::endl;
    sf::RenderWindow promotionWindow(sf::VideoMode(400, 100), "Pawn Promotion");
    // Load textures for promotion options
    sf::Texture queenTexture, rookTexture, bishopTexture, knightTexture;
    if (colour == PieceColour::White) {
        queenTexture.loadFromFile("images/white-queen.png");
        rookTexture.loadFromFile("images/white-rook.png");
        bishopTexture.loadFromFile("images/white-bishop.png");
        knightTexture.loadFromFile("images/white-knight.png");
    } else {
        queenTexture.loadFromFile("images/black-queen.png");
        rookTexture.loadFromFile("images/black-rook.png");
        bishopTexture.loadFromFile("images/black-bishop.png");
        knightTexture.loadFromFile("images/black-knight.png");
    }

    // Create sprites for promotion options
    sf::Sprite queenSprite(queenTexture);
    sf::Sprite rookSprite(rookTexture);
    sf::Sprite bishopSprite(bishopTexture);
    sf::Sprite knightSprite(knightTexture);

    // Set positions for the sprites
    queenSprite.setPosition(50, 25);
    rookSprite.setPosition(150, 25);
    bishopSprite.setPosition(250, 25);
    knightSprite.setPosition(350, 25);

    while (promotionWindow.isOpen()) {
        sf::Event event;
        while (promotionWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                promotionWindow.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                std::cout << "Mouse pressed" << std::endl;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(promotionWindow);
                    std::cout << "Mouse position: " << mousePos.x << ", " << mousePos.y << std::endl;
                    std::cout << "Queen bounds: " << queenSprite.getGlobalBounds().left << ", " << queenSprite.getGlobalBounds().top << ", " << queenSprite.getGlobalBounds().width << ", " << queenSprite.getGlobalBounds().height << std::endl;
                    std::cout << "Rook bounds: " << rookSprite.getGlobalBounds().left << ", " << rookSprite.getGlobalBounds().top << ", " << rookSprite.getGlobalBounds().width << ", " << rookSprite.getGlobalBounds().height << std::endl;
                    std::cout << "Bishop bounds: " << bishopSprite.getGlobalBounds().left << ", " << bishopSprite.getGlobalBounds().top << ", " << bishopSprite.getGlobalBounds().width << ", " << bishopSprite.getGlobalBounds().height << std::endl;
                    std::cout << "Knight bounds: " << knightSprite.getGlobalBounds().left << ", " << knightSprite.getGlobalBounds().top << ", " << knightSprite.getGlobalBounds().width << ", " << knightSprite.getGlobalBounds().height << std::endl;
                    if (queenSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        promotionWindow.close();
                        return PieceType::Queen;
                    } else if (rookSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        promotionWindow.close();
                        return PieceType::Rook;
                    } else if (bishopSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        promotionWindow.close();
                        return PieceType::Bishop;
                    } else if (knightSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        promotionWindow.close();
                        return PieceType::Knight;
                    }
                }
            }
        }

        promotionWindow.clear();
        promotionWindow.draw(queenSprite);
        promotionWindow.draw(rookSprite);
        promotionWindow.draw(bishopSprite);
        promotionWindow.draw(knightSprite);
        promotionWindow.display();
    }

    return PieceType::Queen; // Default promotion to Queen if window is closed
}