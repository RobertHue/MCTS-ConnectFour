//#include <iostream>
//#include <string>
//#include <sstream>
//#include <cstdio>

////////////// Qt-related includes
#include <QApplication>
#include <QTableView>
#include <QHeaderView>
//////////////
#include "QGameStateModel.h"
#include "QGameAI_Thread.h"

const int MAX_X = 7;
const int MAX_Y = 5;

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	//**********************
	//*** Setup Model ******
	//**********************
	QGameStateModel gameStateModel(Player::PLAYER_1, MAX_X, MAX_Y);

	//********************
	//*** Setup AI *******
	//********************
	QGameAI_Thread gameAI_Thread(gameStateModel, Player::PLAYER_2);

	// setup a connection between signal playerturndone(you) and doturn (ai),
	// so that the ai can start its turn when you are done (emit playerTurnDone)
	// The connection is automatically disconnected if either the sender or the receiver becomes deleted.
	QObject::connect(
		&gameStateModel,		// object which emits the signal - the sender
		&QGameStateModel::playerTurnDone, // pointer to a member function - the signal
		&gameAI_Thread,			// hook it up to the listener - the receiver
		&QGameAI_Thread::doTurn	// pointer to a member function - the slot
	);

	// ai responds with the move he made in col for the model to update
	QObject::connect(
		&gameAI_Thread,							// object which emits the signal - the sender
		&QGameAI_Thread::aiMoveChosen,			// pointer to a member function - the signal
		&gameStateModel,							// hook it up to the listener - the receiver
		&QGameStateModel::insertTokenIntoColumn		// pointer to a member function - the slot
	);

	//gameStateModel.dumpObjectInfo();
	//gameAI_Thread.dumpObjectInfo();

	//*********************
	//*** Setup View ******
	//*********************
	/// construct the view, set the model and show
	QTableView *tableView = new QTableView;
	tableView->setModel(&gameStateModel);
	tableView->setWindowTitle("Connect-Four");
	tableView->resize(460, 280);
	tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	tableView->show();

	return app.exec();	// start event loop of QApplication to notify all its QObjects about events
}