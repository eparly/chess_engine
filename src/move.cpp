#include "move.h"

Move::Move(sf::Vector2i start, sf::Vector2i end, MoveType type)
    : start(start), end(end), type(type) {}

sf::Vector2i Move::getStart() const {
    return start;
}

sf::Vector2i Move::getEnd() const {
    return end;
}

MoveType Move::getType() const {
    return type;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << "Move from (" << move.getStart().x << ", " << move.getStart().y << ") to ("
       << move.getEnd().x << ", " << move.getEnd().y << ") of type ";

    switch (move.getType()) {
        case MoveType::Normal: os << "Normal"; break;
        case MoveType::Capture: os << "Capture"; break;
        case MoveType::Castling: os << "Castling"; break;
        case MoveType::EnPassant: os << "En Passant"; break;
        case MoveType::Promotion: os << "Promotion"; break;
    }

    return os;
}

std::string moveTypeToString(MoveType type) {
    switch (type) {
        case MoveType::Normal: return "Normal";
        case MoveType::Capture: return "Capture";
        case MoveType::Castling: return "Castling";
        case MoveType::EnPassant: return "En Passant";
        case MoveType::Promotion: return "Promotion";
        default: return "Unknown";
    }
}