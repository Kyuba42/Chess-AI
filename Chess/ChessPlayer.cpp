#include "ChessPlayer.h"
#include "Chess\GameStatus.h"
#include "Chess\Gameplay.h"
#include "Chess\Board.h"
#include "Chess\Piece.h"

#include "EvaluationTables.h"
#include <iostream>
#include <chrono>

using namespace std;

void ChessPlayer::setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay)
{
	*playerBlack = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::BLACK);
	//(*playerBlack)->setAI();

	*playerWhite = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::WHITE);
	(*playerWhite)->setAI();
}

ChessPlayer::ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour)
{
	m_colour = colour;
	m_pBoard = pBoard;
	m_pGameStatus = pGameStatus;
	m_pGamePlay = pGamePlay;
	m_bAI = false;

	m_colour_copy = m_colour;
	gamePhase = GamePhase::OPEN;
}

unsigned int ChessPlayer::getAllFriendlyLivePieces(vecPieces& vpieces)
{
	vpieces.clear();

	PieceInPostion pip;

	unsigned int count = 0;
	for (int i = m_pBoard->MIN_ROW_INDEX; i < m_pBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = m_pBoard->MIN_COL_INDEX; j < m_pBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = m_pBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

unsigned int ChessPlayer::getAllEnemyLivePieces(vecPieces& vpieces)
{
	vpieces.clear();

	PieceInPostion pip;

	unsigned int count = 0;
	for (int i = m_pBoard->MIN_ROW_INDEX; i < m_pBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = m_pBoard->MIN_COL_INDEX; j < m_pBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = m_pBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() == m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

unsigned int ChessPlayer::getAllFriendlyCustomBoard(vecPieces& vpieces, Board* currentBoard)
{
	vpieces.clear();

	PieceInPostion pip;

	unsigned int count = 0;
	for (int i = currentBoard->MIN_ROW_INDEX; i < currentBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = currentBoard->MIN_COL_INDEX; j < currentBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = currentBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

unsigned int ChessPlayer::getAllEnemyCustomBoard(vecPieces& vpieces, Board* currentBoard)
{
	vpieces.clear();

	PieceInPostion pip;

	unsigned int count = 0;
	for (int i = currentBoard->MIN_ROW_INDEX; i < currentBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = currentBoard->MIN_COL_INDEX; j < currentBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = currentBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() == m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

vector<std::shared_ptr<Move>> ChessPlayer::getValidMovesForPiece(PieceInPostion pip)
{
	return Gameplay::getValidMoves(m_pGameStatus, m_pBoard, pip.piece, pip.row, pip.col);
}

// chooseAIMove
// in this method - for an AI chess player - choose a move to make. This is called once per play. 
bool ChessPlayer::chooseAIMove(std::shared_ptr<Move>* moveToMake, bool inCheck)
{
	m_colour = m_colour_copy;
	auto start = std::chrono::steady_clock::now();
	vecPieces fPieces;
	vecPieces ePieces;

	getAllFriendlyLivePieces(fPieces);
	getAllEnemyLivePieces(ePieces);


	if (inCheck)
	{
		vector<std::shared_ptr<Move>>	moves;
		for (unsigned int i = 0; i < fPieces.size(); i++)
		{
			if (getValidMovesForPiece(fPieces[i]).size() > 0)
			{
				if (moves.size() == 0)
				{
					moves = getValidMovesForPiece(fPieces[i]);
				}
				else
				{
					vector<std::shared_ptr<Move>> temp = getValidMovesForPiece(fPieces[i]);
					moves.insert(moves.end(), temp.begin(), temp.end());
				}
			}
		}
		*moveToMake = moves[0];
		return true;
	}

	std::shared_ptr<Move> tempMove;
	PieceColor* invertCol = InvertColour(&m_colour);
	tempMove = MiniMaxRoot(m_pBoard, fPieces, ePieces, *invertCol, ALPHA, BETA, 0);
	*moveToMake = tempMove;

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

	return true;
}

//tempMove = negamaxRoot(ALPHA, BETA, 5, m_pBoard, m_colour, fPieces);

float ChessPlayer::Evaluation(Board* currentBoard, PieceColor colour) // PieceInPostion currentPiece,
{
	float EvalScore;
	EvalScore = MaterialCount(currentBoard);

	if (colour != m_colour)
		EvalScore = EvalScore * -1;
	//If it can be taken by another piece
	//If it will check the other king
	//Can take piece of more value

	return EvalScore;
}

float ChessPlayer::MaterialCount(Board* currentBoard) // something in here is breaking shit somehow
{

	float fMaterial = 0;
	float eMaterial = 0;
	float totalMaterial = 0;

	vecPieces friendly;
	vecPieces enemy;
	PieceColor friendlyColour = m_colour;

	getAllFriendlyCustomBoard(friendly, currentBoard);
	getAllEnemyCustomBoard(enemy, currentBoard);

	float		fMoveTotal = 0;
	float    eMoveTotal = 0;

	float		fPosTotal = 0;
	float		ePosTotal = 0;

	float		fPosTotalEnd = 0; //used during mid 
	float		ePosTotalEnd = 0;

	float		fPawns = 0;
	float		ePawns = 0;

	float		fKnights = 0;
	float		eKnights = 0;

	float		fBishops = 0;
	float		eBishops = 0;

	float		fRooks = 0;
	float		eRooks = 0;

	float		fQueens = 0;
	float		eQueens = 0;

	float		fKing = 0;
	float		eKing = 0;

	float fKingMoveScore = 0;
	float eKingMoveScore = 0;

	EvaluationTables	tables;

	//search different Eval tables for positioning depending on gamephase

	if (gamePhase == GamePhase::OPEN)
	{
		for (unsigned int i = 0; i < friendly.size(); i++)
		{
			fMoveTotal += getValidMovesForPiece(friendly[i]).size();
			switch (friendly[i].piece->getType())
			{
			case PieceType::PAWN:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhitePawnTableOpen[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackPawnTableOpen[friendly[i].row][friendly[i].col];
				fPawns++;
				break;
			case PieceType::KNIGHT:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteKnightTableOpen[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackKnightTableOpen[friendly[i].row][friendly[i].col];
				fKnights++;
				break;
			case PieceType::BISHOP:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteBishopTableOpen[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackBishopTableOpen[friendly[i].row][friendly[i].col];
				fBishops++;
				break;
			case PieceType::ROOK:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteRookTableOpen[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackRookTableOpen[friendly[i].row][friendly[i].col];
				fRooks++;
				break;
			case PieceType::QUEEN:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteQueenTableOpen[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackQueenTableOpen[friendly[i].row][friendly[i].col];
				fQueens++;
				break;
			case PieceType::KING:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteKingTableOpen[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackKingTableOpen[friendly[i].row][friendly[i].col];
				fKing++;
				break;
			}
		}

		for (unsigned int i = 0; i < enemy.size(); i++)
		{
			eMoveTotal += getValidMovesForPiece(enemy[i]).size();
			switch (enemy[i].piece->getType())
			{
			case PieceType::PAWN:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhitePawnTableOpen[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackPawnTableOpen[enemy[i].row][enemy[i].col];
				ePawns++;
				break;
			case PieceType::KNIGHT:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteKnightTableOpen[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackKnightTableOpen[enemy[i].row][enemy[i].col];
				eKnights++;
				break;
			case PieceType::BISHOP:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteBishopTableOpen[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackBishopTableOpen[enemy[i].row][enemy[i].col];
				eBishops++;
				break;
			case PieceType::ROOK:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteRookTableOpen[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackRookTableOpen[enemy[i].row][enemy[i].col];
				eRooks++;
				break;
			case PieceType::QUEEN:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteQueenTableOpen[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackQueenTableOpen[enemy[i].row][enemy[i].col];
				eQueens++;
				break;
			case PieceType::KING:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteKingTableOpen[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackKingTableOpen[enemy[i].row][enemy[i].col];
				eKingMoveScore == getValidMovesForPiece(enemy[i]).size();
				eKing++;
				break;
			}
		}
	}
	else if (gamePhase == GamePhase::END)
	{
		for (unsigned int i = 0; i < friendly.size(); i++)
		{
			fMoveTotal += getValidMovesForPiece(friendly[i]).size();
			switch (friendly[i].piece->getType())
			{
			case PieceType::PAWN:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhitePawnTableEnd[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackPawnTableEnd[friendly[i].row][friendly[i].col];
				fPawns++;
				break;
			case PieceType::KNIGHT:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteKnightTableEnd[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackKnightTableEnd[friendly[i].row][friendly[i].col];
				fKnights++;
				break;
			case PieceType::BISHOP:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteBishopTableEnd[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackBishopTableEnd[friendly[i].row][friendly[i].col];
				fBishops++;
				break;
			case PieceType::ROOK:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteRookTableEnd[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackRookTableEnd[friendly[i].row][friendly[i].col];
				fRooks++;
				break;
			case PieceType::QUEEN:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteQueenTableEnd[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.BlackQueenTableEnd[friendly[i].row][friendly[i].col];
				fQueens++;
				break;
			case PieceType::KING:
				if (friendlyColour == PieceColor::WHITE)
					fPosTotal += tables.WhiteKingTableEnd[friendly[i].row][friendly[i].col];
				else
					fPosTotal += tables.WhiteKingTableEnd[friendly[i].row][friendly[i].col];

				fKingMoveScore == getValidMovesForPiece(friendly[i]).size();
				fKing++;
				break;
			}
		}

		for (unsigned int i = 0; i < enemy.size(); i++)
		{
			eMoveTotal += getValidMovesForPiece(enemy[i]).size();
			switch (enemy[i].piece->getType())
			{
			case PieceType::PAWN:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhitePawnTableEnd[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackPawnTableEnd[enemy[i].row][enemy[i].col];
				ePawns++;
				break;
			case PieceType::KNIGHT:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteKnightTableEnd[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackKnightTableEnd[enemy[i].row][enemy[i].col];
				eKnights++;
				break;
			case PieceType::BISHOP:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteBishopTableEnd[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackBishopTableEnd[enemy[i].row][enemy[i].col];
				eBishops++;
				break;
			case PieceType::ROOK:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteRookTableEnd[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackRookTableEnd[enemy[i].row][enemy[i].col];
				eRooks++;
				break;
			case PieceType::QUEEN:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteQueenTableEnd[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackQueenTableEnd[enemy[i].row][enemy[i].col];
				eQueens++;
				break;
			case PieceType::KING:
				if (friendlyColour != PieceColor::WHITE)
					ePosTotal += tables.WhiteKingTableEnd[enemy[i].row][enemy[i].col];
				else
					ePosTotal += tables.BlackKingTableEnd[enemy[i].row][enemy[i].col];
				eKingMoveScore == getValidMovesForPiece(enemy[i]).size();
				eKing++;
				break;
			}
		}
	}
	else if (gamePhase == GamePhase::MID)
	{
		for (unsigned int i = 0; i < friendly.size(); i++)
		{
			fMoveTotal += getValidMovesForPiece(friendly[i]).size();
			switch (friendly[i].piece->getType())
			{
			case PieceType::PAWN:
				if (friendlyColour == PieceColor::WHITE)
				{
					fPosTotal += tables.WhitePawnTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.WhitePawnTableEnd[friendly[i].row][friendly[i].col];
				}
				else
				{
					fPosTotal += tables.BlackPawnTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.BlackPawnTableEnd[friendly[i].row][friendly[i].col];
				}
				fPawns++;
				break;
			case PieceType::KNIGHT:
				if (friendlyColour == PieceColor::WHITE)
				{
					fPosTotal += tables.WhiteKnightTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.WhiteKnightTableEnd[friendly[i].row][friendly[i].col];
				}
				else
				{
					fPosTotal += tables.BlackKnightTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.BlackKnightTableEnd[friendly[i].row][friendly[i].col];
				}
				fKnights++;
				break;
			case PieceType::BISHOP:
				if (friendlyColour == PieceColor::WHITE)
				{
					fPosTotal += tables.WhiteBishopTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.WhiteBishopTableEnd[friendly[i].row][friendly[i].col];
				}
				else
				{
					fPosTotal += tables.BlackBishopTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.BlackBishopTableEnd[friendly[i].row][friendly[i].col];
				}
				fBishops++;
				break;
			case PieceType::ROOK:
				if (friendlyColour == PieceColor::WHITE)
				{
					fPosTotal += tables.WhiteRookTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.WhiteRookTableEnd[friendly[i].row][friendly[i].col];
				}
				else
				{
					fPosTotal += tables.BlackRookTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.BlackRookTableEnd[friendly[i].row][friendly[i].col];
				}
				fRooks++;
				break;
			case PieceType::QUEEN:
				if (friendlyColour == PieceColor::WHITE)
				{
					fPosTotal += tables.WhiteQueenTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.WhiteQueenTableEnd[friendly[i].row][friendly[i].col];
				}
				else
				{
					fPosTotal += tables.BlackQueenTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.BlackQueenTableEnd[friendly[i].row][friendly[i].col];
				}

				fQueens++;
				break;
			case PieceType::KING:
				if (friendlyColour == PieceColor::WHITE)
				{
					fPosTotal += tables.WhiteKingTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.WhiteKingTableEnd[friendly[i].row][friendly[i].col];
				}
				else
				{
					fPosTotal += tables.BlackKingTableOpen[friendly[i].row][friendly[i].col];
					fPosTotalEnd += tables.BlackKingTableEnd[friendly[i].row][friendly[i].col];
				}


				fKingMoveScore == getValidMovesForPiece(friendly[i]).size();
				fKing++;
				break;
			}
		}

		for (unsigned int i = 0; i < enemy.size(); i++)
		{
			eMoveTotal += getValidMovesForPiece(enemy[i]).size();
			switch (enemy[i].piece->getType())
			{
			case PieceType::PAWN:
				if (friendlyColour != PieceColor::WHITE)
				{
					ePosTotal += tables.WhitePawnTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd += tables.WhitePawnTableEnd[enemy[i].row][enemy[i].col];
				}
				else
				{
					ePosTotal += tables.BlackPawnTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd += tables.BlackPawnTableEnd[enemy[i].row][enemy[i].col];
				}
				ePawns++;
				break;
			case PieceType::KNIGHT:
				if (friendlyColour != PieceColor::WHITE)
				{
					ePosTotal += tables.WhiteKnightTableOpen[enemy[i].row][enemy[i].col];
					ePosTotal = tables.WhiteKnightTableEnd[enemy[i].row][enemy[i].col];
				}
				else
				{
					ePosTotal = tables.BlackKnightTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd = tables.BlackKnightTableEnd[enemy[i].row][enemy[i].col];
				}
				eKnights++;
				break;
			case PieceType::BISHOP:
				if (friendlyColour != PieceColor::WHITE)
				{
					ePosTotal += tables.WhiteBishopTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd += tables.WhiteBishopTableEnd[enemy[i].row][enemy[i].col];
				}
				else
				{
					ePosTotal += tables.BlackBishopTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd += tables.BlackBishopTableEnd[enemy[i].row][enemy[i].col];
				}
				eBishops++;
				break;
			case PieceType::ROOK:
				if (friendlyColour != PieceColor::WHITE)
				{
					ePosTotal = tables.WhiteRookTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd = tables.WhiteRookTableEnd[enemy[i].row][enemy[i].col];
				}
				else
				{
					ePosTotal = tables.BlackRookTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd = tables.BlackRookTableEnd[enemy[i].row][enemy[i].col];
				}
				eRooks++;
				break;
			case PieceType::QUEEN:
				if (friendlyColour != PieceColor::WHITE)
				{
					ePosTotal += tables.WhiteQueenTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd += tables.WhiteQueenTableEnd[enemy[i].row][enemy[i].col];
				}
				else
				{
					ePosTotal += tables.BlackQueenTableOpen[enemy[i].row][enemy[i].col];
					ePosTotal += tables.BlackQueenTableEnd[enemy[i].row][enemy[i].col];
				}
				eQueens++;
				break;
			case PieceType::KING:
				if (friendlyColour != PieceColor::WHITE)
				{
					ePosTotal = tables.WhiteKingTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd = tables.WhiteKingTableEnd[enemy[i].row][enemy[i].col];
				}
				else
				{
					ePosTotal = tables.BlackKingTableOpen[enemy[i].row][enemy[i].col];
					ePosTotalEnd = tables.BlackKingTableEnd[enemy[i].row][enemy[i].col];
				}
				eKing++;
				break;
			}
		}
	}

	fPawns = fPawns * (int)PieceWeighting::PAWN;
	fKnights = fKnights * (int)PieceWeighting::KNIGHT;
	fBishops = fBishops * (int)PieceWeighting::BISHOP;
	fRooks = fRooks * (int)PieceWeighting::ROOK;
	fQueens = fQueens * (int)PieceWeighting::QUEEN;
	fKing = fKing * (int)PieceWeighting::KING;

	ePawns = ePawns * (int)PieceWeighting::PAWN;
	eKnights = eKnights * (int)PieceWeighting::KNIGHT;
	eBishops = eBishops * (int)PieceWeighting::BISHOP;
	eRooks = eRooks * (int)PieceWeighting::ROOK;
	eQueens = eQueens * (int)PieceWeighting::QUEEN;
	eKing = eKing * (int)PieceWeighting::KING;

	fMaterial = (fKing + fPawns + fKnights + fBishops + fRooks + fQueens);		 //Add up total weighting of pieces + the total amount of moves that can be made (friendly pieces)
	eMaterial = (eKing + ePawns + eKnights + eBishops + eRooks + eQueens);		//Add up total weighting of pieces + the total amount of moves that can be made (enemy pieces)

	//Opening and Closing implementation, Numbers feel extremely arbitrary but unsure how to rescale

	//  interpolate score in the middlegame
	/*if (gamePhase == GamePhase::MID)
	{
		totalMaterial = (fPosTotal * (500+fMaterial) + fPosTotalEnd * (5900 - (500+fMaterial))) / 5900;
		totalMaterial -= (ePosTotal * (500 + eMaterial) + ePosTotalEnd * (5900 - (500 + eMaterial))) / 5900;
	}
	else if (gamePhase == GamePhase::OPEN ||gamePhase == GamePhase::END ) totalMaterial = ((fMaterial + fMoveTotal + fPosTotal) / (eMaterial + eMoveTotal + ePosTotal));*/

	totalMaterial = ((fMaterial + fMoveTotal + fPosTotal) / (eMaterial + eMoveTotal + ePosTotal)); // should get unit count, number of moves and evaluation table moves and divide them by the other teams total

	return totalMaterial;
}
//- 0.5(D - D' + B-B' + I - I')
//D, B, I = Doubled, Blocked and Isolated pawns


PieceColor* ChessPlayer::InvertColour(PieceColor* colToInvert)
{
	if (*colToInvert == PieceColor::WHITE)
		*colToInvert = PieceColor::BLACK;
	else if (*colToInvert == PieceColor::BLACK)
		*colToInvert = PieceColor::WHITE;

	return colToInvert;
}

std::shared_ptr<Move> ChessPlayer::MiniMaxRoot(Board* pBoard, vecPieces& fPieces, vecPieces& ePieces, PieceColor pColour, float alpha, float beta, int currentDepth)
{
	std::shared_ptr<Move>					bestMove{};
	Board* testBoard{};
	vector<std::shared_ptr<Move>>	moves;
	float												minMaxEvaluation;
	float												bestEvaluation;

	for (unsigned int i = 0; i < fPieces.size(); i++)
	{
		if (getValidMovesForPiece(fPieces[i]).size() > 0)
		{
			if (moves.size() == 0)
			{
				moves = getValidMovesForPiece(fPieces[i]);
			}
			else
			{
				vector<std::shared_ptr<Move>> temp = getValidMovesForPiece(fPieces[i]);
				moves.insert(moves.end(), temp.begin(), temp.end());
			}
		}
	}

	minMaxEvaluation = -99999;

	for (std::shared_ptr<Move> move : moves)
	{
		testBoard = pBoard->hardCopy();
		Gameplay::move(m_pGameStatus, testBoard, move);
		float evaluation;

		PieceColor* invertCol = InvertColour(&pColour);
		evaluation = MiniMax(testBoard, fPieces, ePieces, *invertCol, alpha, beta, currentDepth + 1);
		//bestEvaluation = min(minMaxEvaluation, evaluation);
		testBoard = pBoard;

		if (evaluation > minMaxEvaluation)
		{
			minMaxEvaluation = evaluation;
			bestMove = move;
			cout << "Current Best Eval " << minMaxEvaluation << endl;
		}

		if (alpha <= evaluation)
			alpha = evaluation;
		if (beta <= alpha)
			break;
	}

	return bestMove;
}


float ChessPlayer::MiniMax(Board* pBoard, vecPieces& fPieces, vecPieces& ePieces, PieceColor pColour, float alpha, float beta, int currentDepth)
{
	Board* testBoard{};
	float minMaxEvaluation;

	if (pColour != m_colour)
	{
		minMaxEvaluation = -99999;
	}
	else
	{
		minMaxEvaluation = 99999;
	}

	if (currentDepth == MAX_DEPTH)
	{
		return Evaluation(pBoard, pColour);
	}

	vector<std::shared_ptr<Move>> moves;
	if (pColour == m_colour)
	{
		for (unsigned int i = 0; i < fPieces.size(); i++)
		{
			if (getValidMovesForPiece(fPieces[i]).size() > 0)
			{
				if (moves.size() == 0)
				{
					moves = getValidMovesForPiece(fPieces[i]);
				}
				else
				{
					vector<std::shared_ptr<Move>> temp = getValidMovesForPiece(fPieces[i]);
					moves.insert(moves.end(), temp.begin(), temp.end());
				}
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < ePieces.size(); i++)
		{
			if (getValidMovesForPiece(ePieces[i]).size() > 0)
			{
				if (moves.size() == 0)
				{
					moves = getValidMovesForPiece(ePieces[i]);
				}
				else
				{
					vector<std::shared_ptr<Move>> temp = getValidMovesForPiece(ePieces[i]);
					moves.insert(moves.end(), temp.begin(), temp.end());
				}
			}
		}
	}

	if (moves.size() == 0)
	{
		return -99999;
	}

	for (std::shared_ptr<Move> move : moves)
	{
		float evaluation;
		testBoard = pBoard->hardCopy();
		Gameplay::move(m_pGameStatus, testBoard, move);

		if (pColour != m_colour)
		{
			PieceColor* invertCol = InvertColour(&pColour);
			evaluation = MiniMax(testBoard, fPieces, ePieces, *invertCol, alpha, beta, currentDepth + 1);
			testBoard = pBoard;

			if (evaluation > minMaxEvaluation)
				minMaxEvaluation = evaluation;

			alpha = max(alpha, minMaxEvaluation);
			if (beta <= alpha)
				break;

		}
		else
		{
			PieceColor* invertCol = InvertColour(&pColour);
			evaluation = MiniMax(testBoard, fPieces, ePieces,*invertCol, alpha, beta, currentDepth + 1);
			testBoard = pBoard;

			if (evaluation < minMaxEvaluation)
				minMaxEvaluation = evaluation;

			beta = min(beta, minMaxEvaluation);
			if (beta <= alpha)
				break;
		}
	}
	return minMaxEvaluation;
}

//NEGAMAX ATTEMPT

std::shared_ptr<Move>	 ChessPlayer::negamaxRoot(int alpha, int beta, int depth, Board* pBoard, PieceColor pColour, vecPieces pieces)
{
	std::shared_ptr<Move>					bestMove{};
	std::shared_ptr<Move>					bestMoveSoFar{};
	Board copy{};
	vector<std::shared_ptr<Move>>	moves;
	float												minMaxEvaluation;
	float												bestEvaluation;
	// init old alpha
	int old_alpha = alpha;

	for (unsigned int i = 0; i < pieces.size(); i++)
	{
		if (getValidMovesForPiece(pieces[i]).size() > 0)
		{
			if (moves.size() == 0)
			{
				moves = getValidMovesForPiece(pieces[i]);
			}
			else
			{
				vector<std::shared_ptr<Move>> temp = getValidMovesForPiece(pieces[i]);
				moves.insert(moves.end(), temp.begin(), temp.end());
			}
		}
	}

	minMaxEvaluation = -99999;

	for (std::shared_ptr<Move> move : moves)
	{
		copy = *pBoard->hardCopy();
		Gameplay::move(m_pGameStatus, &copy, move);
		float evaluation;

		PieceColor* invertCol = InvertColour(&pColour);
		evaluation = -negamax(-beta, -alpha, depth - 1, &copy, pColour);
		copy = *pBoard;

		if (evaluation > minMaxEvaluation)
		{
			minMaxEvaluation = evaluation;
			bestMove = move;
			cout << "Current Best Eval " << minMaxEvaluation << endl;
		}

		if (alpha <= evaluation)
		{
			alpha = evaluation;
			bestMoveSoFar = move;
		}
		if (beta <= alpha)
			break;
	}

	// associate best move with best score
	if (old_alpha != alpha)
		bestMove = bestMoveSoFar;


	return bestMove;
}

// negamax serach with alpha-beta pruning
float ChessPlayer::negamax(int alpha, int beta, int depth, Board* pBoard, PieceColor pColour)
{
	std::shared_ptr<Move>					bestMove{};
	// escape condition
	if (depth == 0)
		return Evaluation(pBoard, pColour);

	// init old alpha
	int old_alpha = alpha;

	// init best move so far
	std::shared_ptr<Move>					bestMoveSoFar{};

	vector<std::shared_ptr<Move>> moves;
	vecPieces fPieces;

	for (unsigned int i = 0; i < fPieces.size(); i++)
	{
		if (getValidMovesForPiece(fPieces[i]).size() > 0)
		{
			if (moves.size() == 0)
			{
				moves = getValidMovesForPiece(fPieces[i]);
			}
			else
			{
				vector<std::shared_ptr<Move>> temp = getValidMovesForPiece(fPieces[i]);
				moves.insert(moves.end(), temp.begin(), temp.end());
			}
		}
	}

	int score;
	// loop over move list
	for (std::shared_ptr<Move> move : moves)
	{
		// preserve board position
		Board* copy = pBoard->hardCopy();
		Gameplay::move(m_pGameStatus, copy, move);
		// recursive negamax call
		score = -negamax(-beta, -alpha, depth - 1, copy, pColour);

		// take move back
		copy = pBoard->hardCopy();

		// fail hard beta cutoff
		if (score >= beta)
		{
			return beta;
		}

		// found a better move
		if (score > alpha)
		{
			// increase lower bound
			alpha = score;
			bestMoveSoFar = move;
		}
	}

	// associate best move with best score
	if (old_alpha != alpha)
		bestMove = bestMoveSoFar;

	return score;
}