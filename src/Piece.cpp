#include "piece.h"

#include <SFML/Graphics.hpp>
#include <vector>

// Define static texture members
sf::Texture Piece::whiteQueenTexture;
sf::Texture Piece::whiteRookTexture;
sf::Texture Piece::whiteBishopTexture;
sf::Texture Piece::whiteKnightTexture;
sf::Texture Piece::whitePawnTexture;
sf::Texture Piece::whiteKingTexture;

sf::Texture Piece::blackQueenTexture;
sf::Texture Piece::blackRookTexture;
sf::Texture Piece::blackBishopTexture;
sf::Texture Piece::blackKnightTexture;
sf::Texture Piece::blackPawnTexture;
sf::Texture Piece::blackKingTexture;

Piece::Piece() : type(PieceType::None), colour(PieceColour::None){}


Piece::Piece(PieceType type, PieceColour colour, int x, int y)
    : type(type), colour(colour) {
    // Load textures for promoted pieces
    loadTextures();

    // Set the texture based on the piece type and colour
    switch (type) {
        case PieceType::Queen:
            sprite.setTexture(colour == PieceColour::White ? whiteQueenTexture : blackQueenTexture);
            break;
        case PieceType::Rook:
            sprite.setTexture(colour == PieceColour::White ? whiteRookTexture : blackRookTexture);
            break;
        case PieceType::Bishop:
            sprite.setTexture(colour == PieceColour::White ? whiteBishopTexture : blackBishopTexture);
            break;
        case PieceType::Knight:
            sprite.setTexture(colour == PieceColour::White ? whiteKnightTexture : blackKnightTexture);
            break;
        case PieceType::Pawn:
            sprite.setTexture(colour == PieceColour::White ? whitePawnTexture : blackPawnTexture);
            break;
        case PieceType::King:
            sprite.setTexture(colour == PieceColour::White ? whiteKingTexture : blackKingTexture);
            break;
        default:
            break;
    }

    sprite.setPosition(x * 100, y * 100); // Adjust the position based on your board size
    sprite.setScale(1.0f, 1.0f); // Adjust the scale based on your texture size
}

Piece::Piece(const sf::Texture& texture, int x, int y, int squareSize, PieceColour colour, PieceType type)
    : colour(colour), type(type), x(x), y(y) {
    sprite.setTexture(texture);
    float scale = 0.8f;
    sprite.setScale(scale, scale);
    sprite.setPosition(x + squareSize / 2 - sprite.getGlobalBounds().width / 2,
                       y + squareSize / 2 - sprite.getGlobalBounds().height / 2);
    loadTextures();
}

void Piece::loadTextures() {
    static bool texturesLoaded = false;
    if(!texturesLoaded) {
        whiteRookTexture.loadFromFile("images/white-rook.png");
        whiteKnightTexture.loadFromFile("images/white-knight.png");
        whiteBishopTexture.loadFromFile("images/white-bishop.png");
        whiteQueenTexture.loadFromFile("images/white-queen.png");

        blackRookTexture.loadFromFile("images/black-rook.png");
        blackKnightTexture.loadFromFile("images/black-knight.png");
        blackBishopTexture.loadFromFile("images/black-bishop.png");
        blackQueenTexture.loadFromFile("images/black-queen.png");
        texturesLoaded = true;
    }
}

void Piece::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Piece::setPosition(sf::Vector2i position) {
    sprite.setPosition(position.x, position.y);
    x = position.x;
    y = position.y;
}

sf::Vector2f Piece::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Piece::getBounds() const {
    return sprite.getGlobalBounds();
}

PieceColour Piece::getColour() const {
    return colour;
}

PieceType Piece::getType() const {
    return type;
}

sf::Vector2i Piece::getBoardPosition() const {
    return sf::Vector2i(x, y);
}

std::string Piece::getTypeAsString() const {
    switch (getType()) {
        case PieceType::King: return "King";
        case PieceType::Queen: return "Queen";
        case PieceType::Rook: return "Rook";
        case PieceType::Bishop: return "Bishop";
        case PieceType::Knight: return "Knight";
        case PieceType::Pawn: return "Pawn";
        default: return "Unknown";
    }
}

void Piece::setType(PieceType newType) {
    type = newType;
    // Update the texture based on the new type
    switch (newType) {
        case PieceType::Queen:
            sprite.setTexture(colour == PieceColour::White ? whiteQueenTexture : blackQueenTexture);
            break;
        case PieceType::Rook:
            sprite.setTexture(colour == PieceColour::White ? whiteRookTexture : blackRookTexture);
            break;
        case PieceType::Bishop:
            sprite.setTexture(colour == PieceColour::White ? whiteBishopTexture : blackBishopTexture);
            break;
        case PieceType::Knight:
            sprite.setTexture(colour == PieceColour::White ? whiteKnightTexture : blackKnightTexture);
            break;
        default:
            break;
    }
    // Adjust the sprite scale to fit the new texture
    sprite.setScale(static_cast<float>(sprite.getGlobalBounds().width) / sprite.getTexture()->getSize().x, static_cast<float>(sprite.getGlobalBounds().height) / sprite.getTexture()->getSize().y);
}

bool Piece::operator==(const Piece& other) const {
    return sprite.getPosition() == other.getPosition() &&
        colour == other.getColour() &&
        type == other.getType();
}