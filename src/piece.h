#ifndef PIECE_H
#define PIECE_H

#include <SFML/Graphics.hpp>
#include <vector>
enum class PieceType
{
    King,
    Queen,
    Bishop,
    Knight,
    Rook,
    Pawn,
    None
};

enum class PieceColour
{
    White,
    Black,
    None
};


class Piece {
public:
    Piece(const sf::Texture& texture, int x, int y, int squareSize, PieceColour colour, PieceType type);
    void draw(sf::RenderWindow& window);    
    sf::Vector2f getPosition();
    sf::FloatRect getBounds();
    void setPosition(sf::Vector2f position);
    PieceColour getColour();
    PieceType getType() const;
    std::string getTypeAsString() const;

private:
    sf::Sprite sprite;
    PieceColour colour;
    PieceType type;
};


#endif // PIECE_H