#include <SFML/Graphics.hpp>
#include "board.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chessboard");

    int squareSize = 100;
    sf::Color lightColor(222, 184, 135);
    sf::Color darkColor(139, 69, 19);

    Board board(squareSize, lightColor, darkColor);

    sf::Texture whiteKingTexture, whiteQueenTexture, whiteRookTexture, whiteBishopTexture, whiteKnightTexture, whitePawnTexture;
    sf::Texture blackKingTexture, blackQueenTexture, blackRookTexture, blackBishopTexture, blackKnightTexture, blackPawnTexture;

    if (!whiteKingTexture.loadFromFile("images/white-king.png") ||
        !whiteQueenTexture.loadFromFile("images/white-queen.png") ||
        !whiteRookTexture.loadFromFile("images/white-rook.png") ||
        !whiteBishopTexture.loadFromFile("images/white-bishop.png") ||
        !whiteKnightTexture.loadFromFile("images/white-knight.png") ||
        !whitePawnTexture.loadFromFile("images/white-pawn.png") ||
        !blackKingTexture.loadFromFile("images/black-king.png") ||
        !blackQueenTexture.loadFromFile("images/black-queen.png") ||
        !blackRookTexture.loadFromFile("images/black-rook.png") ||
        !blackBishopTexture.loadFromFile("images/black-bishop.png") ||
        !blackKnightTexture.loadFromFile("images/black-knight.png") ||
        !blackPawnTexture.loadFromFile("images/black-pawn.png")) {
        return -1;
    }

    // Add white pieces
    board.addPiece(whiteRookTexture, 0, 0);
    board.addPiece(whiteKnightTexture, 1, 0);
    board.addPiece(whiteBishopTexture, 2, 0);
    board.addPiece(whiteQueenTexture, 3, 0);
    board.addPiece(whiteKingTexture, 4, 0);
    board.addPiece(whiteBishopTexture, 5, 0);
    board.addPiece(whiteKnightTexture, 6, 0);
    board.addPiece(whiteRookTexture, 7, 0);
    for (int i = 0; i < 8; ++i) {
        board.addPiece(whitePawnTexture, i, 1);
    }

    // Add black pieces
    board.addPiece(blackRookTexture, 0, 7);
    board.addPiece(blackKnightTexture, 1, 7);
    board.addPiece(blackBishopTexture, 2, 7);
    board.addPiece(blackQueenTexture, 3, 7);
    board.addPiece(blackKingTexture, 4, 7);
    board.addPiece(blackBishopTexture, 5, 7);
    board.addPiece(blackKnightTexture, 6, 7);
    board.addPiece(blackRookTexture, 7, 7);
    for (int i = 0; i < 8; ++i) {
        board.addPiece(blackPawnTexture, i, 6);
    }
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}