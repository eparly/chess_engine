#ifndef MOVE_H
#define MOVE_H

#include <SFML/Graphics.hpp>

enum class MoveType {
    Normal,
    Capture,
    Castling,
    EnPassant,
    Promotion
};

std::string moveTypeToString(MoveType type);

class Move {
public:
    Move(sf::Vector2i start, sf::Vector2i end, MoveType type = MoveType::Normal);

    sf::Vector2i getStart() const;
    sf::Vector2i getEnd() const;
    MoveType getType() const;

    bool operator==(const Move& other) const {
        return start == other.start && end == other.end && type == other.type;
    }

    friend std::ostream& operator<<(std::ostream& os, const Move& move);


private:
    sf::Vector2i start;
    sf::Vector2i end;
    MoveType type;
};

namespace std {
    template <>
    struct hash<Move> {
        std::size_t operator()(const Move& move) const {
            return std::hash<int>()(move.getStart().x) ^ std::hash<int>()(move.getStart().y) ^
                   std::hash<int>()(move.getEnd().x) ^ std::hash<int>()(move.getEnd().y) ^
                   std::hash<int>()(static_cast<int>(move.getType()));
        }
    };
}

#endif // MOVE_H