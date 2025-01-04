#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "piece.h"

class Board {
public:
    Board(int squareSize, sf::Color lightColor, sf::Color darkColor);
    void draw(sf::RenderWindow& window);
    void addPiece(const sf::Texture& texture, int x, int y, PieceColour colour, PieceType type);
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void selectPiece(const sf::Vector2f& mousePos);
    bool isValidMove(Piece &piece, sf::Vector2f targetPos);

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

    bool isWhiteTurn = true;
    void endTurn();

    void capturePiece(Piece &piece);
};

#endif // BOARD_H