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

	EvaluationTables	tables;

	for (unsigned int i = 0; i < friendly.size(); i++)
	{
		fMoveTotal += getValidMovesForPiece(friendly[i]).size();
		switch (friendly[i].piece->getType())
		{
		case PieceType::PAWN:
			if (friendlyColour == PieceColor::WHITE)
				fPosTotal = tables.WhitePawnTable[friendly[i].row][friendly[i].col];
			else
				fPosTotal = tables.BlackPawnTable[friendly[i].row][friendly[i].col];
			fPawns++;
			break;
		case PieceType::KNIGHT:
			if (friendlyColour == PieceColor::WHITE)
				fPosTotal = tables.WhiteKnightTable[friendly[i].row][friendly[i].col];
			else
				fPosTotal = tables.BlackKnightTable[friendly[i].row][friendly[i].col];
			fKnights++;
			break;
		case PieceType::BISHOP:
			if (friendlyColour == PieceColor::WHITE)
				fPosTotal = tables.WhiteBishopTable[friendly[i].row][friendly[i].col];
			else
				fPosTotal = tables.BlackBishopTable[friendly[i].row][friendly[i].col];
			fBishops++;
			break;
		case PieceType::ROOK:
			if (friendlyColour == PieceColor::WHITE)
				fPosTotal = tables.WhiteRookTable[friendly[i].row][friendly[i].col];
			else
				fPosTotal = tables.BlackRookTable[friendly[i].row][friendly[i].col];
			fRooks++;
			break;
		case PieceType::QUEEN:
			if (friendlyColour == PieceColor::WHITE)
				fPosTotal = tables.WhiteQueenTable[friendly[i].row][friendly[i].col];
			else
				fPosTotal = tables.BlackQueenTable[friendly[i].row][friendly[i].col];
			fQueens++;
			break;
		case PieceType::KING:
			if (friendlyColour == PieceColor::WHITE)
				fPosTotal = tables.WhiteKingTableMid[friendly[i].row][friendly[i].col];
			else
				fPosTotal = tables.WhiteKingTableMid[friendly[i].row][friendly[i].col];
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
				ePosTotal = tables.WhitePawnTable[enemy[i].row][enemy[i].col];
			else
				ePosTotal = tables.BlackPawnTable[enemy[i].row][enemy[i].col];
			ePawns++;
			break;
		case PieceType::KNIGHT:
			if (friendlyColour != PieceColor::WHITE)
				ePosTotal = tables.WhiteKnightTable[enemy[i].row][enemy[i].col];
			else
				ePosTotal = tables.BlackKnightTable[enemy[i].row][enemy[i].col];
			eKnights++;
			break;
		case PieceType::BISHOP:
			if (friendlyColour != PieceColor::WHITE)
				ePosTotal = tables.WhiteBishopTable[enemy[i].row][enemy[i].col];
			else
				ePosTotal = tables.BlackBishopTable[enemy[i].row][enemy[i].col];
			eBishops++;
			break;
		case PieceType::ROOK:
			if (friendlyColour != PieceColor::WHITE)
				ePosTotal = tables.WhiteRookTable[enemy[i].row][enemy[i].col];
			else
				ePosTotal = tables.BlackRookTable[enemy[i].row][enemy[i].col];
			eRooks++;
			break;
		case PieceType::QUEEN:
			if (friendlyColour != PieceColor::WHITE)
				ePosTotal = tables.WhiteQueenTable[enemy[i].row][enemy[i].col];
			else
				ePosTotal = tables.BlackQueenTable[enemy[i].row][enemy[i].col];
			eQueens++;
			break;
		case PieceType::KING:
			if (friendlyColour != PieceColor::WHITE)
				ePosTotal = tables.WhiteKingTableMid[enemy[i].row][enemy[i].col];
			else
				ePosTotal = tables.BlackKingTableMid[enemy[i].row][enemy[i].col];
			eKing++;
			break;
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

	fMaterial = (fKing + fPawns + fKnights + fBishops + fRooks + fQueens) + fMoveTotal + fPosTotal;		 //Add up total weighting of pieces + the total amount of moves that can be made (friendly pieces)
	eMaterial = (eKing + ePawns + eKnights + eBishops + eRooks + eQueens) + eMoveTotal  + ePosTotal;		//Add up total weighting of pieces + the total amount of moves that can be made (enemy pieces)

	cout << "fMaterial: " << fMaterial << "\n";
	cout << "eMaterial: " << eMaterial << "\n";

	totalMaterial = ((fMaterial + fMoveTotal + fPosTotal) / (eMaterial + eMoveTotal + ePosTotal));

	return totalMaterial;

	//- 0.5(D - D' + B-B' + I - I')
	//D, B, I = Doubled, Blocked and Isolated pawns
}

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

	/*if (pColour == m_colour)
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
	}*/


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
			evaluation = MiniMax(testBoard, fPieces, ePieces, *invertCol, alpha, beta, currentDepth + 1);
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