#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "piece.h"
#include <unordered_set>

class Board {
public:
    Board(int squareSize, sf::Color lightColor, sf::Color darkColor);
    void draw(sf::RenderWindow& window);
    void addPiece(const sf::Texture& texture, int x, int y, PieceColour colour, PieceType type);
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void selectPiece(const sf::Vector2f& mousePos);
    bool isValidMove(Piece &piece, sf::Vector2i targetPos);
    std::vector<std::string> generateLegalMoves();
    bool isLegalMove(const std::string &move);

private:
    int squareSize;
    sf::Color lightColor;
    sf::Color darkColor;
    std::vector<sf::RectangleShape> squares;
    std::vector<Piece> pieces;

    Piece* selectedPiece = nullptr;
    sf::Vector2i originalPosition;
    bool isDragging = false;

    sf::Vector2i snapToSquare(const sf::Vector2f& position);
    bool isWithinBounds(const sf::Vector2f& position);

    bool highlightSquare = false;
    int highlightedX = -1;
    int highlightedY = -1;

    bool isWhiteTurn = true;
    void endTurn();

    void capturePiece(Piece &piece);

    std::unordered_set<std::string> legalMoves;
    void parseFen(const std::string &fen);
    void initializeBoard();

    sf::Texture whiteRookTexture, whiteKnightTexture, whiteBishopTexture, whiteQueenTexture, whiteKingTexture, whitePawnTexture;
    sf::Texture blackRookTexture, blackKnightTexture, blackBishopTexture, blackQueenTexture, blackKingTexture, blackPawnTexture;


    std::vector<std::string> generatePawnMoves(const Piece& piece);
    std::vector<std::string> generateRookMoves(const Piece& piece);
    std::vector<std::string> generateKnightMoves(const Piece& piece);
    std::vector<std::string> generateBishopMoves(const Piece& piece);
    std::vector<std::string> generateQueenMoves(const Piece& piece);
    std::vector<std::string> generateKingMoves(const Piece& piece);

    bool isValidPosition(int col, int row);
    bool isEmpty(int col, int row);
    bool isEnemyPiece(int col, int row, PieceColour colour);
    std::string moveToString(sf::Vector2i from, sf::Vector2i to);

    bool isKingInCheck(PieceColour colour);
    bool isPositionAttacked(int col, int row, PieceColour colour);
    bool simulateMoveAndCheck(Piece& piece, sf::Vector2i targetPos);

    //castling
    bool whiteKingMoved = false;
    bool whiteKingsideRookMoved = false;
    bool whiteQueensideRookMoved = false;

    bool blackKingMoved = false;
    bool blackKingsideRookMoved = false;
    bool blackQueensideRookMoved = false;

    //en passant
    sf::Vector2i enPassantTarget = sf::Vector2i(-500, -500);   

    bool canCastleKingside(PieceColour colour);
    bool canCastleQueenside(PieceColour colour);
    void performCastling(Piece& king, sf::Vector2i targetPos);

    bool isCheckmate(PieceColour colour);
    bool gameOver = false;
    std::string gameOverMessage;
    sf::Vector2i checkmateKingPosition;
};

#endif // BOARD_H