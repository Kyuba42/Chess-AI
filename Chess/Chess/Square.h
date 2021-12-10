#ifndef SQUARE_H
#define SQUARE_H

// square - potentially contains a piece

#include <memory>
#include "Piece.h"

class Square
{
	private:
		std::shared_ptr<Piece> occupyingPiece = nullptr;
	public:
		Square();
		Square(const Square& square);

		~Square();

		bool occupiedState();
		bool occupySquare(std::shared_ptr<Piece> piece);
		std::shared_ptr<Piece> removeOccupyingPiece();
		std::shared_ptr<Piece> getOccupyingPiece();
		bool hasOccupyingPiece();
		Square hardCopy();
};

#endif