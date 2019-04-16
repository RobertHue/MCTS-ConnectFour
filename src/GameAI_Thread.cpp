#include "GameAI_Thread.h"

GameAI_Thread::GameAI_Thread(QGameStateModel& gsm, Player p)
	:
	m_gameAI(p),
	m_gameStateModel(gsm)
{
	QThread::start();	// launches the thread
}


GameAI_Thread::~GameAI_Thread()
{
	QThread::wait();	// waits for the thread to end
}

void GameAI_Thread::run() {
	/*
	m_cvAITurn.wait(&m_mutex);	// wait until notified by GameStateModel

	bool isValidMove;
	const GameState &gs = m_gameStateModel.getGameState();
	int col = m_gameAI.findNextMove(gs);
	//isValidMove = gs.insertTokenIntoColumn(col);
	QModelIndex idx = m_gameStateModel.index(0, col);
	isValidMove = m_gameStateModel.setData(idx, Qt::EditRole);
	m_gameStateModel.checkWinner();	// has someone won?
	*/
	QThread::exec();	// run the event loop
}

QWaitCondition & GameAI_Thread::getCVTurnBLocked()
{
	return m_cvTurnBlocked;
}

