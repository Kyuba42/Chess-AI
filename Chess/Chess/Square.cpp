#include <cstddef>

#include "Square.h"

Square::Square()
{
	occupyingPiece = nullptr;
}

Square::Square(const Square& square)
{

	occupyingPiece = square.occupyingPiece;
	
}

Square::~Square()
{

}

bool Square::occupiedState()
{
	return occupyingPiece == nullptr ? false : true;
}

bool Square::occupySquare(std::shared_ptr<Piece> piece)
{
	if(occupiedState()) return false;
	else occupyingPiece = piece;

	return true;
}

std::shared_ptr<Piece> Square::removeOccupyingPiece()
{
	std::shared_ptr<Piece> removed = occupyingPiece;

	occupyingPiece = nullptr;

	return removed;
}

std::shared_ptr<Piece> Square::getOccupyingPiece()
{
	return occupyingPiece;
}

bool Square::hasOccupyingPiece()
{
	return occupyingPiece != nullptr;
}

Square Square::hardCopy()
{
	Square copy;
	copy.occupyingPiece = nullptr;

	if (this->occupyingPiece != nullptr) 
	{
		copy.occupyingPiece = this->occupyingPiece;
	}

	return copy;
}