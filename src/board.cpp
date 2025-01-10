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
                    std::cout << "Adding move: " << move << std::endl;
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
    for (const Piece& piece : pieces) {
        if(piece.getColour() != colour) {
            std::vector<std::string> moves;
            switch (piece.getType()) {
                case PieceType::Pawn: moves = generatePawnMoves(piece); break;
                case PieceType::Rook: moves = generateRookMoves(piece); break;
                case PieceType::Knight: moves = generateKnightMoves(piece); break;
                case PieceType::Bishop: moves = generateBishopMoves(piece); break;
                case PieceType::Queen: moves = generateQueenMoves(piece); break;
                case PieceType::King: moves = generateKingMoves(piece); break;
            }
            for (const std::string & move : moves) {
                sf::Vector2i to(move[2] - 'a', move[3] - '1');
                if (to.x == col && to.y == row) {
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
            std::cout << "Capturing piece" << p.getTypeAsString() << std::endl;
            capturedPiece = &p;
            break;
        }
    }
    piece.setPosition(targetPos * squareSize);
    if(capturedPiece) {
        capturedPiece->setPosition(sf::Vector2i(-100, -100));
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
        moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col - 1, row + direction)));
    }

    // Capture diagonally right
    if (isValidPosition(col + 1, row + direction) && isEnemyPiece(col + 1, row + direction, piece.getColour())) {
        moves.push_back(moveToString(piecePosition / squareSize, sf::Vector2i(col + 1, row + direction)));
    }
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
    // Implement king movement rules
    return moves;
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
    std::cout << "Is legal move: " << isLegal << std::endl;
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
                std::string move = moveToString(sf::Vector2i(selectedPiece->getBoardPosition().x / squareSize, selectedPiece->getBoardPosition().y / squareSize), targetPos / squareSize);
                // std::cout << "Move: " << move << std::endl;
                if (isLegalMove(move) && isWhiteTurn == (selectedPiece->getColour() == PieceColour::White)) {
                    
                    // Check if there is a piece at the target position
                    selectedPiece->setPosition(targetPos);
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
            originalPosition = piece.getPosition();
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
        std::cout << "Piece position: " << piecePosition.x << ", " << piecePosition.y << std::endl;
        std::cout << "Position: " << position.x << ", " << position.y << std::endl;
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
    for(auto& move : legalMoves){
        std::cout << move << std::endl;
    }
}

void Board::capturePiece(Piece& piece) {
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if(it->getPosition() == piece.getPosition() && it->getColour() == piece.getColour()) {
            std::cout << "Capturing piece" << it->getTypeAsString() << std::endl;
            pieces.erase(it);
            break;
        }
    }
}