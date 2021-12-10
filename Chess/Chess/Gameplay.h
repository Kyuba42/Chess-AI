#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*
Gameplay - a helper class for applying and querying the rules of gameplay
// for example: move() moves pieces and set various game states

*/

#include <vector>
#include <memory>
#include "Board.h"
#include "Move.h"
#include "Piece.h"
#include "PieceColor.h"
#include "MoveType.h"
#include "GameStatus.h"

class Gameplay
{
	public:
		static bool move(GameStatus* status, Board* board, std::shared_ptr<Move> move);
		static std::vector<std::shared_ptr<Move>> getPossibleMoves(GameStatus* status, Board* board, std::shared_ptr<Piece> piece, int fromRow, int fromCol);
		static std::vector<std::shared_ptr<Move>> getValidMoves(GameStatus* status, Board* board, std::shared_ptr<Piece> piece, int fromRow, int fromCol);
		static bool isCheckState(GameStatus* status, Board* board, PieceColor color);
		static bool isCheckMateState(GameStatus* status, Board* board, PieceColor color);
		static int allValidMoveCount(GameStatus* status, Board* board, PieceColor color);
		static bool pawnPromotion(Board* board, int row, int col, PieceType promoteTo);
};

#endif