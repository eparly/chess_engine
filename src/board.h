#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "piece.h"
#include "move.h"
#include <unordered_set>

class Board {
public:
    Board(int squareSize, sf::Color lightColor, sf::Color darkColor);
    void draw(sf::RenderWindow& window);
    void addPiece(const sf::Texture& texture, int x, int y, PieceColour colour, PieceType type);
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void selectPiece(const sf::Vector2f& mousePos);
    bool isValidMove(Piece &piece, sf::Vector2i targetPos);
    std::vector<Move> generateLegalMoves();
    bool isLegalMove(const Move &move);
    void parseFen(const std::string &fen);
    void applyMove(const Move &move);


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

    std::unordered_set<Move> legalMoves;
    void initializeBoard();

    sf::Texture whiteRookTexture, whiteKnightTexture, whiteBishopTexture, whiteQueenTexture, whiteKingTexture, whitePawnTexture;
    sf::Texture blackRookTexture, blackKnightTexture, blackBishopTexture, blackQueenTexture, blackKingTexture, blackPawnTexture;


    std::vector<Move> generatePawnMoves(const Piece& piece);
    std::vector<Move> generateRookMoves(const Piece& piece);
    std::vector<Move> generateKnightMoves(const Piece& piece);
    std::vector<Move> generateBishopMoves(const Piece& piece);
    std::vector<Move> generateQueenMoves(const Piece& piece);
    std::vector<Move> generateKingMoves(const Piece& piece);

    bool isValidPosition(int col, int row);
    bool isEmpty(int col, int row);
    bool isEnemyPiece(int col, int row, PieceColour colour);
    // std::string moveToString(sf::Vector2i from, sf::Vector2i to); //unused  - remove

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

PieceType showPromotionWindow(sf::RenderWindow& window, PieceColour colour);

#endif // BOARD_H