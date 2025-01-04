#include "board.h"
#include "piece.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

Board::Board(int squareSize, sf::Color lightColor, sf::Color darkColor)
    : squareSize(squareSize), lightColor(lightColor), darkColor(darkColor) {}

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
    sf::Vector2f position = snapToSquare(sf::Vector2f(x*squareSize, y*squareSize));
    pieces.emplace_back(texture, position.x, position.y, squareSize, colour, type);
}

sf::Vector2f Board::snapToSquare(const sf::Vector2f& position) {
    float snappedX = std::floor(position.x / squareSize) * squareSize;
    float snappedY = std::floor(position.y / squareSize) * squareSize;
    return sf::Vector2f(snappedX, snappedY);
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
                sf::Vector2f targetPos = snapToSquare(mousePos);
                if (isValidMove(*selectedPiece, targetPos) && isWhiteTurn == (selectedPiece->getColour() == PieceColour::White)) {
                    // Check if there is a piece at the target position
                    selectedPiece->setPosition(targetPos);
                    std::cout << "Moving piece" << selectedPiece->getTypeAsString() << std::endl;
                    std::cout << "Target position: " << targetPos.x << ", " << targetPos.y << std::endl;
                    std::cout << "Original position: " << originalPosition.x << ", " << originalPosition.y << std::endl;
                    std::cout << "Selected piece position: " << selectedPiece->getPosition().x << ", " << selectedPiece->getPosition().y << std::endl;
                    bool isPieceAtTarget = false;
                    for (auto& piece : pieces) {
                        if (piece.getPosition() == targetPos && selectedPiece != &piece) {
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

bool Board::isValidMove(Piece& piece, sf::Vector2f position) {
    int x = position.x / squareSize;
    int y = position.y / squareSize;
    //check if the  spot is occupied by a piece of the same colour
    for(auto& p : pieces){
        sf::Vector2f piecePosition = snapToSquare(p.getPosition());
        if(piecePosition== position && p.getColour() == piece.getColour()){
            return false;
        }
    }
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}


void Board::endTurn() {
    isWhiteTurn = !isWhiteTurn;
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