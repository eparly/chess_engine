#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

class Piece {
public:
    Piece(const sf::Texture& texture, int x, int y, int squareSize);
    void draw(sf::RenderWindow& window);

private:
    sf::Sprite sprite;
};

class Board {
public:
    Board(int squareSize, sf::Color lightColor, sf::Color darkColor);
    void draw(sf::RenderWindow& window);
    void addPiece(const sf::Texture& texture, int x, int y);

private:
    int squareSize;
    sf::Color lightColor;
    sf::Color darkColor;
    std::vector<Piece> pieces;
};

#endif // BOARD_H