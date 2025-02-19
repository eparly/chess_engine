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
    Piece();
    Piece(PieceType type, PieceColour colour, int x, int y);
    Piece(const sf::Texture &texture, int x, int y, int squareSize, PieceColour colour, PieceType type);
    void draw(sf::RenderWindow& window);    
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    void setPosition(sf::Vector2i position);
    PieceColour getColour() const;
    PieceType getType() const;
    std::string getTypeAsString() const;
    sf::Vector2i getBoardPosition() const;

    void setType(PieceType newType);

    bool operator==(const Piece &other) const;

private:
    sf::Sprite sprite;
    PieceColour colour;
    PieceType type;
    int x;
    int y;

    static sf::Texture whiteRookTexture, whiteKnightTexture, whiteBishopTexture, whiteQueenTexture, whiteKingTexture, whitePawnTexture;
    static sf::Texture blackRookTexture, blackKnightTexture, blackBishopTexture, blackQueenTexture, blackKingTexture, blackPawnTexture;

    static void loadTextures();
};

#endif // PIECE_H