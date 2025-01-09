#include <SFML/Graphics.hpp>
#include "board.h"
#include "piece.h"

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

    // std::string initialFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    // board.initializeBoard();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    board.handleEvent(event, window);
                }
            }
        }

        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}