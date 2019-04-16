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

	// return of const game state because its protected by the mutex here in this adapter (no changes should happen to it)
	const GameState& getGameState() const;
	//std::condition_variable& getCVBusyGame() const;
	//std::unique_lock<std::mutex>& getMutexLockGame() const;

	bool checkWinner();

public slots:
	void doWork();

signals:
	void workDone();


private: 

	// instance of std::mutex to protect
	// is mutable so it can still be changed in a constant member
	//mutable std::mutex m_mutex;

	//mutable std::mutex m_cv;
	//mutable std::unique_lock<std::mutex> m_mutexLock;
	//mutable std::condition_variable m_cvBusyGame;
	Player m_playerYou;
	Player m_playerOpp;
	GameState m_gameState;
};
