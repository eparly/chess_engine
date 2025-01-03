#include "board.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

Piece::Piece(const sf::Texture& texture, int x, int y, int squareSize){
    sprite.setTexture(texture);
    float scale = 0.8f;
    sprite.setScale(scale, scale);
    sprite.setPosition(x + squareSize / 2 - sprite.getGlobalBounds().width / 2,
                       y + squareSize / 2 - sprite.getGlobalBounds().height / 2);
}

void Piece::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Piece::setPosition(sf::Vector2f position) {
    sprite.setPosition(position.x, position.y);
}

sf::Vector2f Piece::getPosition() {
    return sprite.getPosition();
}

sf::FloatRect Piece::getBounds() {
    return sprite.getGlobalBounds();
}

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

void Board::addPiece(const sf::Texture& texture, int x, int y) {
    pieces.emplace_back(texture, x * squareSize, y * squareSize, squareSize);
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
                if (isValidMove(*selectedPiece, targetPos)) {
                    selectedPiece->setPosition(targetPos);
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
            break;
        }
    }
}

bool Board::isValidMove(const Piece& piece, sf::Vector2f position) {
    int x = position.x / squareSize;
    int y = position.y / squareSize;
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}
