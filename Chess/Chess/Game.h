#ifndef GAME_H
#define GAME_H


/*
Game - this is the game class - a game container - has the players, the board, can check various states. 
relies on the various objects (such as gameplay) to provide results
*/

#include <stack>
#include "Piece.h"
#include "Gameplay.h"
#include "Board.h"
#include "GameStatus.h"
#include "Move.h"

class ChessPlayer;

class Game
{
	private:
		std::stack<std::shared_ptr<Move>> log;
		Gameplay* gameplay;
		Board* board;
		GameStatus* status;
		int turn;
		ChessPlayer* m_PlayerBlack;
		ChessPlayer* m_PlayerWhite;
		std::vector<std::shared_ptr<Piece>> m_vecPieces;

	public:
		Game();
		~Game();

		void setInitialPieces(PieceColor color);
		std::stack<std::shared_ptr<Move>> getAllLog();
		Board* getBoard();
		GameStatus* getGameStatus();
		std::vector<std::shared_ptr<Move>> getValidMoves(int fromRow, int fromCol);
		bool move(int fromRow, int fromCol, int toRow, int toCol);
		PieceColor getPieceColor(int fromRow, int fromCol);
		std::shared_ptr<Piece> getPiece(int fromRow, int fromCol);
		bool isSquareOccupied(int fromRow, int fromCol);
		bool inCheckState();
		bool inCheckMateState();
		int getTurn();
		void nextTurn();
		PieceColor getTurnColor();
		bool promote(int row, int col, PieceType type);

		Board* getCopyOfBoard();
		Board* replaceBoard(Board* board);

		ChessPlayer* getBlackPlayer() { return m_PlayerBlack;}
		ChessPlayer* getWhitePlayer() { return m_PlayerWhite; }
		
};

#endif
