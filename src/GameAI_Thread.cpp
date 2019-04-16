#include "GameAI_Thread.h"

GameAI_Thread::GameAI_Thread(QGameStateModel& gsm, Player p)
	:
	m_gameAI(p),
	m_gameStateModel(gsm)
{
	//QThread::start();	// launches the thread
}


GameAI_Thread::~GameAI_Thread()
{
	QThread::wait();	// waits for the thread to end
}

void GameAI_Thread::run() {
	// do the ai calculations needed to get the best col for it
	const GameState &gs = m_gameStateModel.getGameState();
	int col = m_gameAI.findNextMove(gs);

	// insert the token with the help of the gameStateModel reference
	emit aiMoveChosen(col);

	//QThread::exec();	// run the event loop
}

void GameAI_Thread::doTurn(Player turnPlayer) {
	if (turnPlayer == m_gameAI.getPlayer()) {
		this->start();
	}
}

