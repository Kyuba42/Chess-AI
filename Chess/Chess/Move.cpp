#include "Move.h"


Move::Move(MoveType mType, int fRow, int fCol, int tRow, int tCol, std::shared_ptr<Piece> moved, std::shared_ptr<Piece> captured)
{
	type = mType;
	fromRow = fRow;
	fromCol = fCol;
	toRow = tRow;
	toCol = tCol;
	movePiece = moved;
	capturedPiece = captured;
}

Move::Move(const Move& move) = default;

std::pair<int, int> Move::getOriginPosition()
{
	return std::make_pair(fromRow, fromCol);
}

std::pair<int, int> Move::getDestinationPosition()
{
	return std::make_pair(toRow, toCol);
}

std::shared_ptr<Piece> Move::getMovedPiece()
{
	return movePiece;
}

std::shared_ptr<Piece> Move::getCapturedPiece()
{
	return capturedPiece;
}

MoveType Move::getType()
{
	return type;
}

bool Move::operator==(const Move& other)
{
	return (this->fromRow == other.fromRow) && (this->fromCol == other.fromCol) && (this->movePiece->getType() == other.movePiece->getType()) && (this->movePiece->getColor() == other.movePiece->getColor()) && (this->toRow == other.toRow) && (this->toCol == other.toCol) && (this->type == other.type) && (this->capturedPiece->getType() == other.capturedPiece->getType()) && (this->capturedPiece->getColor() == other.capturedPiece->getColor());
}
