#pragma once
#include <QThread>

#include "GameAI.h"
#include "QGameStateModel.h"

class QGameAI_Thread : public QThread
{
    Q_OBJECT

public:
    QGameAI_Thread(QGameStateModel &gsm, Player player = Player::PLAYER_1);
    ~QGameAI_Thread();

protected:
    // the starting point of the thread
    // After calling start(), the newly created thread calls this function.
    // The default implementation simply calls exec().
    void run() override;


public slots:
    void doTurn(Player turnPlayer);

signals:
    void aiMoveChosen(int);

private:
    // QImage currentImage;
    // QQueue<Transaction *> transactions;
    //QWaitCondition m_cvTurnBlocked;
    //QMutex m_mutex;
    //////////////////////
    GameAI m_gameAI;
    QGameStateModel &m_gameStateModel;
};
