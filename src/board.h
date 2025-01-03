#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

class Piece {
public:
    Piece(const sf::Texture& texture, int x, int y, int squareSize);
    void draw(sf::RenderWindow& window);    
    sf::Vector2f getPosition();
    sf::FloatRect getBounds();
    void setPosition(sf::Vector2f position);

private:
    sf::Sprite sprite;
};

class Board {
public:
    Board(int squareSize, sf::Color lightColor, sf::Color darkColor);
    void draw(sf::RenderWindow& window);
    void addPiece(const sf::Texture& texture, int x, int y);
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void selectPiece(const sf::Vector2f& mousePos);
    bool isValidMove(const Piece &piece, sf::Vector2f targetPos);

private:
    int squareSize;
    sf::Color lightColor;
    sf::Color darkColor;
    std::vector<sf::RectangleShape> squares;
    std::vector<Piece> pieces;

    Piece* selectedPiece = nullptr;
    sf::Vector2f originalPosition;
    bool isDragging = false;

    sf::Vector2f snapToSquare(const sf::Vector2f& position);
    bool isWithinBounds(const sf::Vector2f& position);

    bool highlightSquare = false;
    int highlightedX = -1;
    int highlightedY = -1;
};

#endif // BOARD_H