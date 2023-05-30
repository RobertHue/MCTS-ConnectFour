#include "QGameStateModel.h"
#include "QWinnerDialog.h"
//////////////////////////////////////////////////////////////////////


QGameStateModel::QGameStateModel(QObject *parent)
    : QAbstractTableModel(parent), m_gameState(7, 5),
      m_playerYou(Player::PLAYER_1),
      m_playerOpp(m_playerYou == Player::PLAYER_1 ? Player::PLAYER_2
                                                  : Player::PLAYER_1),
      m_endOfGame(false)
{
    m_gameState.setTurnPlayer(Player::PLAYER_1);
}

QGameStateModel::QGameStateModel(Player playerYou,
                                 int ncols,
                                 int nrows,
                                 QObject *parent)
    : QAbstractTableModel(parent), m_gameState(ncols, nrows),
      m_playerYou(playerYou),
      m_playerOpp(m_playerYou == Player::PLAYER_1 ? Player::PLAYER_2
                                                  : Player::PLAYER_1),
      m_endOfGame(false)
{
    m_gameState.setTurnPlayer(playerYou);
}

// READ-only
int QGameStateModel::rowCount(const QModelIndex &parent) const
{
    // Returns the number of columns for the children of the given parent.
    // In most subclasses, the number of columns is independent of the parent.
    if (parent.isValid())
        return 0;
    return m_gameState.getMAX_Y();
}

// Returns the number of columns for the children of the given parent.
// @note In most subclasses, the number of columns is independent of the parent.

int QGameStateModel::columnCount(const QModelIndex &parent) const
{
    // Returns the number of columns for the children of the given parent.
    // In most subclasses, the number of columns is independent of the parent.
    if (parent.isValid())
        return 0;
    return m_gameState.getMAX_X();
}

// used to supply item data to views and delegates.
// view will call this function columnCount() * rowCount times (for each cell) asking for the actual data in model's internal data structures.

QVariant QGameStateModel::data(const QModelIndex &index, int role) const
{

    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    if (role == Qt::FontRole)
    {
        QFont sansFont("Helvetica [Cronyx]", 18);
        return sansFont;
    }

    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= m_gameState.getMAX_Y() || index.row() < 0 ||
        index.column() >= m_gameState.getMAX_X() || index.column() < 0)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {

        int column = index.column();
        int row = index.row();

        int data = m_gameState.getGameDataAt(column, row);

        if (data == static_cast<int>(Player::PLAYER_1))
        {
            return QString("X");
        }
        else if (data == static_cast<int>(Player::PLAYER_2))
        {
            return QString("O");
        }
        else
        {
            return QString(" ");
        }
    }

    return QVariant();
}

// provides views with information to show in their headers (?)

QVariant QGameStateModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{


    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags QGameStateModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool QGameStateModel::setData(const QModelIndex &index,
                              const QVariant &value,
                              int role)
{
    if (!index.isValid())
    {
        return false;
    }

    if (index.row() >= m_gameState.getMAX_Y() || index.row() < 0 ||
        index.column() >= m_gameState.getMAX_X() || index.column() < 0)
    {
        return false;
    }
    if (role == Qt::EditRole && !m_endOfGame)
    {


        if (m_gameState.getTurnPlayer() == Player::PLAYER_1)
        { // player 1 has control over the gui todo maybe some pattern that ensure it

            m_gameState.insertTokenIntoColumn(index.column());
            checkWinner();
            emit playerTurnDone(m_gameState.getTurnPlayer());
            return true;
        }
    }
    return false;
}

bool QGameStateModel::insertRows(int position,
                                 int rows,
                                 const QModelIndex &index)
{
    return false;
}

bool QGameStateModel::removeRows(int position,
                                 int rows,
                                 const QModelIndex &index)
{
    return false;
}

bool QGameStateModel::insertColumns(int position,
                                    int columns,
                                    const QModelIndex &parent)
{
    return false;
}

bool QGameStateModel::removeColumns(int position,
                                    int columns,
                                    const QModelIndex &parent)
{
    return false;
}


// return of const game state because its protected by the mutex here in this adapter (no changes should happen to it)

const GameState &QGameStateModel::getGameState() const
{
    return m_gameState;
}

void QGameStateModel::insertTokenIntoColumn(int col)
{
    bool isValidMove;
    isValidMove = m_gameState.insertTokenIntoColumn(col);
    Position pos = m_gameState.getPositionOfLastPlacedToken();
    QModelIndex topLeftIdx = this->index(pos.x, pos.y);
    QModelIndex bottomRightIdx = this->index(pos.x + 1, pos.y + 1);
    //Try to force the view(s) to redraw at certain cell:
    emit QAbstractItemModel::dataChanged(topLeftIdx, bottomRightIdx);
    checkWinner(); // has someone won?
}

bool QGameStateModel::checkWinner()
{

    // check whether the game has ended & print out reason for end
    Player hasWon = m_gameState.hasSomeoneWon();
    if (hasWon == m_playerYou)
    {
        std::string s = "Congratulations!!! You have won!!! :)\n";
        std::cout << s;
        m_endOfGame = true;
        QWinnerDialog *w = new QWinnerDialog(QString(s.c_str()));
        w->show();
        return true;
    }
    else if (hasWon == m_playerOpp)
    {
        std::string s = "Unfortunately the KI has won... :(\n";
        std::cout << s;
        m_endOfGame = true;
        QWinnerDialog *w = new QWinnerDialog(QString(s.c_str()));
        w->show();
        return true;
    }
    if (m_gameState.getNumOfFreeFields() <= 0)
    {
        std::string s = "Game is over. You did well!."
                        "No more cells are free on the Game!\n";
        std::cout << s;
        m_endOfGame = true;
        QWinnerDialog *w = new QWinnerDialog(QString(s.c_str()));
        w->show();
        return true;
    }
    return false;
}
