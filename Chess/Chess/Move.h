#ifndef MOVE_H
#define MOVE_H

// move - container for the move action

#include <utility>
#include <cstddef>

#include "Piece.h"
#include "MoveType.h"
#include <memory>

class Move
{
	private:
		int fromRow;
		int fromCol;
		std::shared_ptr<Piece> movePiece;
		int toRow;
		int toCol;
		std::shared_ptr<Piece> capturedPiece;
		MoveType type;
		
	public:
		Move(MoveType mType, int fRow, int fCol, int tRow, int tCol, std::shared_ptr<Piece> moved, std::shared_ptr<Piece> captured=nullptr);
		Move(const Move& move);
		Move() { }
		std::pair<int, int> getOriginPosition();
		std::pair<int, int> getDestinationPosition();
		std::shared_ptr<Piece> getMovedPiece();
		std::shared_ptr<Piece> getCapturedPiece();
		MoveType getType();
		bool operator==(const Move& other);
};

#endif