#include "piece.h"

#include <SFML/Graphics.hpp>
#include <vector>

Piece::Piece(const sf::Texture& texture, int x, int y, int squareSize, PieceColour colour, PieceType type)
    : colour(colour), type(type), x(x), y(y) {
    sprite.setTexture(texture);
    float scale = 0.8f;
    sprite.setScale(scale, scale);
    sprite.setPosition(x + squareSize / 2 - sprite.getGlobalBounds().width / 2,
                       y + squareSize / 2 - sprite.getGlobalBounds().height / 2);
}

void Piece::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Piece::setPosition(sf::Vector2i position) {
    sprite.setPosition(position.x, position.y);
    x = position.x;
    y = position.y;
}

sf::Vector2f Piece::getPosition() {
    return sprite.getPosition();
}

sf::FloatRect Piece::getBounds() {
    return sprite.getGlobalBounds();
}

PieceColour Piece::getColour() {
    return colour;
}

PieceType Piece::getType() const {
    return type;
}

sf::Vector2i Piece::getBoardPosition() {
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