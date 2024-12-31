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
        PieceType type;
        PieceColour colour;

        int x, y;

        Piece(
            PieceType type = PieceType::None, 
            PieceColour colour = PieceColour::None, 
            int x = 0, 
            int y = 0
        ): type(type), colour(colour), x(x), y(y) {}
};