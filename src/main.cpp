
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>


////////////// Qt-related includes
#include <QApplication>
#include <QWidget>
#include <QObject>

#include <QGridLayout>

#include <QPushButton>

#include <QAbstractItemModel>
#include <QStringListModel>
#include <QAbstractTableModel>

#include <QListView>
#include <QTableView>

#include <qheaderview.h>
////////////// 
#include "QGameStateModel.h"
#include "GameAI_Thread.h"

#define OK   0
#define ERROR -1
using namespace std;

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
	GameAI_Thread gameAI_Thread(gameStateModel, Player::PLAYER_2);

	QObject::connect(&gameAI_Thread, &QThread::started, &gameStateModel, &QGameStateModel::doWork);
	QObject::connect(&gameStateModel, &QGameStateModel::workDone, &gameAI_Thread, &QThread::quit);

	//*********************
	//*** Setup View ******
	//*********************
	/// construct the view, set the model and show
	QTableView *tableView = new QTableView;
	tableView->setModel(&gameStateModel);
	tableView->show();
	tableView->setWindowTitle("Connect-Four");
	tableView->resize(460, 280);
	tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	tableView->show();

	return app.exec();	// start event loop of QApplication to notify all its QObjects about events
}