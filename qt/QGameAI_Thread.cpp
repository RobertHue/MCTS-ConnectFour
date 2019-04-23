#include "QGameAI_Thread.h"

QGameAI_Thread::QGameAI_Thread(QGameStateModel& gsm, Player p)
	:
	m_gameAI(p),
	m_gameStateModel(gsm)
{
	//QThread::start();	// launches the thread
}


QGameAI_Thread::~QGameAI_Thread()
{
	QThread::wait();	// waits for the thread to end
}

void QGameAI_Thread::run() {
	// do the ai calculations needed to get the best col for it
	const GameState &gs = m_gameStateModel.getGameState();
	int col = m_gameAI.findNextMove(gs);

	// insert the token with the help of the gameStateModel reference
	emit aiMoveChosen(col);

	//QThread::exec();	// run the event loop
}

void QGameAI_Thread::doTurn(Player turnPlayer) {
	if (turnPlayer == m_gameAI.getPlayer()) {
		this->start();
	}
}

