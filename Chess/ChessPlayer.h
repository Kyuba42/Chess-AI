#pragma once
#include "Chess\PieceColor.h"
#include "Chess\PieceType.h"
#include <vector>
#include <memory>
#include "Chess/Move.h"

#define MAX_DEPTH 3
#define ALPHA   -99999999
#define BETA		99999999

using namespace std;

class Piece;
class Board;
class GameStatus;
class Gameplay;
class Move;

enum class PieceWeighting : int
{
	QUEEN = 90,
	ROOK =  50,
	KNIGHT = 30,
	BISHOP = 30,
	PAWN = 10,
	KING = 1000
};

struct MinMaxStruct
{
	float bestScore;
	Move bestMove;
	float currentScore;
	Move currentMove;
};

struct PieceInPostion
{
	std::shared_ptr<Piece> piece;
	int col;
	int row;
};

typedef vector<PieceInPostion> vecPieces;

class ChessPlayer
{
public:
	static void		setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay);
	ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour);

	void												setAI() { m_bAI = true; }
	bool												isAI() { return m_bAI; }
	unsigned int									getAllFriendlyLivePieces(vecPieces& vpieces);
	unsigned int									getAllEnemyLivePieces(vecPieces& vpieces);
	unsigned int									getAllFriendlyCustomBoard(vecPieces& vpieces, Board* currentBoard);
	unsigned int									getAllEnemyCustomBoard(vecPieces& vpieces, Board* currentBoard);
	vector<std::shared_ptr<Move>>	getValidMovesForPiece(PieceInPostion pip);
	bool												chooseAIMove(std::shared_ptr<Move>* moveToMake, bool inCheck);
	
protected:
	PieceColor									getColour() { return m_colour; }
	float												MiniMax(Board* pBoard, vecPieces& fPieces, vecPieces& ePieces, PieceColor colour, float alpha, float beta, int currentDepth);
	std::shared_ptr<Move>					MiniMaxRoot(Board* pBoard, vecPieces& fPieces, vecPieces& ePieces, PieceColor colour, float alpha, float beta, int currentDepth);


private:
	PieceColor			m_colour;
	PieceColor			m_colour_copy;
	Board*					m_pBoard;
	GameStatus*		m_pGameStatus;
	Gameplay*			m_pGamePlay;
	MinMaxStruct		minMax;
	float						Evaluation(Board* currentBoard, PieceColor colour); // PieceInPostion vPieces,
	float						MaterialCount(Board* currentBoard);
	PieceColor*			InvertColour(PieceColor* colToInvert);
	bool						m_bAI;

};

