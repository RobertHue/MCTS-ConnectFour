#pragma once

#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QFont>
#include <QBrush>

#include <QAbstractTableModel>
#include <QStringList>
#include <QVariant>

#include <mutex>
#include <condition_variable>

#include "GameState.h"
/**
	@brief	functions as a adapter for the GameState
	GameState's m_gameData is processessed to be available for Qt-Views

	*/
class QGameStateModel : public QAbstractTableModel {
	Q_OBJECT

public:
	QGameStateModel(QObject *parent = 0);
	QGameStateModel(Player playerYou, int ncols, int nrows, QObject *parent = 0);

public:	/// implement pure virtual from QAbstractTableModel  
	// READ-only
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	// Returns the number of columns for the children of the given parent.
	// @note In most subclasses, the number of columns is independent of the parent.
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	// used to supply item data to views and delegates.
	// view will call this function columnCount() * rowCount times (for each cell) asking for the actual data in model's internal data structures.
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	// provides views with information to show in their headers (?)
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

	bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
	bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;



	/////////////////////////////////////
	// return of const game state because its protected by the mutex here in this adapter (no changes should happen to it)
	const GameState& getGameState() const;
	
	bool checkWinner();

public slots:
	void insertTokenIntoColumn(int col);
	
signals:
	void playerTurnDone(Player turnPlayer);

private:
	Player m_playerYou;
	Player m_playerOpp;
	GameState m_gameState;
	bool m_endOfGame;
};

////////////////////////////////////////////////

#include <QDialog>
#include <QLabel>
#include <QFont>
#include <QVBoxLayout>

class WinnerDialog : public QDialog {
	Q_OBJECT

public:
	WinnerDialog(const QString& text, QWidget * parent = 0)
		: QDialog(parent)
	{
		QVBoxLayout *mainLayout = new QVBoxLayout;
		this->setModal(true);
		this->setWindowTitle("Connect-Four Who won?");
		this->resize(260, 180);
		this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
		this->setLayout(mainLayout);


		QFont sansFont("Helvetica [Cronyx]", 18); 
		QFont f("Arial", 10, QFont::Bold);
		QLabel *label = new QLabel();
		label->setStyleSheet(
			"font: large 28px; font-weight: bold; text-align: center; color: rgb(200,100,150)"
		);
		label->setText(text);
		mainLayout->addWidget(label);
	}
};