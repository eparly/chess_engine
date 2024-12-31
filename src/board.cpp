#include "board.h"

Piece::Piece(const sf::Texture& texture, int x, int y, int squareSize) {
    sprite.setTexture(texture);
    float scale = 0.8f;
    sprite.setScale(scale, scale);
    sprite.setPosition(x + squareSize / 2 - sprite.getGlobalBounds().width / 2,
                       y + squareSize / 2 - sprite.getGlobalBounds().height / 2);
}

void Piece::draw(sf::RenderWindow& window) {
    window.draw(sprite);
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

    for (auto& piece : pieces) {
        piece.draw(window);
    }


    for(auto& piece : pieces) {
        piece.draw(window);
    }
}

void Board::addPiece(const sf::Texture& texture, int x, int y) {
    pieces.emplace_back(texture, x * squareSize, y * squareSize, squareSize);
}