
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>

#include "GameState.h"
#include "GameAI.h"

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QObject>


#define OK   0
#define ERROR -1
using namespace std;

const int MAX_X = 7;
const int MAX_Y = 5;
int main(int argc, char* argv[]) {
	int col_err;
	GameState gp(MAX_X, MAX_Y);
	GameState::drawGameStateOnConsole(
		gp.getGameData(), 
		gp.getMAX_X(), 
		gp.getMAX_Y()
	);
	/*
	QApplication app(argc, argv);

	QPixmap pixmap_cross("../../resources/cross.jpg");
	QPixmap pixmap_circle("../../resources/circle.jpg");
	QIcon iconBackCross(pixmap_cross);
	QIcon iconBackCircle(pixmap_circle);

	QPushButton *buttons[MAX_X][MAX_Y];
	QGridLayout *layout = new QGridLayout();
	for (int i = 0; i < gp.getMAX_X(); ++i) {
		for (int j = 0; j < gp.getMAX_Y(); ++j) {

			buttons[i][j] = new QPushButton(
				QString::fromStdString(
					std::to_string(i).append(" , ").append(std::to_string(j))
			));
			buttons[i][j]->setIcon(iconBackCross); 
			buttons[i][j]->setIconSize(QSize(65, 65));
			layout->addWidget(buttons[i][j], i, j);
		}
	}
	QModelIndex indexA = model->index(0, 0, QModelIndex());
	QModelIndex indexB = model->index(1, 1, QModelIndex());
	QModelIndex indexC = model->index(2, 1, QModelIndex());


	QWidget *widget = new QWidget();
	widget->setLayout(layout);
	ostringstream windowTitle;
	windowTitle << "Connect 4 ( "
		<< std::to_string(gp.getMAX_X())
		<< " x "
		<< std::to_string(gp.getMAX_Y())
		<< " )";
	widget->setWindowTitle(QString(windowTitle.str().c_str()));
	widget->show();
	*/

    //*************************
    //*** Spieler-Auwahl ******
    //*************************
    Player playerYou, playerAI;
	char input_char;
    do {
        cout << "Choose Player (xX / oO): ";
        //cin.getline(&input_char, 256);
		//cin >> input_char;
		input_char = getchar();  //'x'; // 
        cout << "Input successful!\n";

        switch (input_char) {
            case 'x': case 'X':
                playerYou = PLAYER_2;
                playerAI = PLAYER_1;
                break;
            case 'o': case 'O':
                playerYou = PLAYER_1;
                playerAI = PLAYER_2;
                break;
            default:
                cout << "Wrong input! Please choose again!";
        }
    } while (
		input_char != 'x' && 
		input_char != 'X' && 
		input_char != 'o' && 
		input_char != 'O'
	);



    //***************************
    //*** Player-Move-Choice ****
    //***************************
    gp.setTurnPlayer(playerAI); // set Player that needs to begin here
    GameAI gKI(playerAI);


    while (1) {
        if (gp.getTurnPlayer() == playerYou) {
            int col;
            do {
                cout << "Please choose COLUMN: ";
                cin >> col;

                col_err = gp.insertTokenIntoColumn(col);
                if (col_err == ERROR) {
                    cout << "Column is already full! Please chose again: \n";
                }
            } while (col < 0 || col >= gp.getMAX_X() || col_err);
        }
        else {
            int col = gKI.calculateNextTurn(gp);
            col_err = gp.insertTokenIntoColumn(col);
        }
        GameState::drawGameStateOnConsole(
			gp.getGameData(), 
			gp.getMAX_X(), 
			gp.getMAX_Y()
		);


        Player hasWon = gp.hasSomeoneWon();
        if (hasWon == playerYou) {
            cout << "Congratulations!!! You have won!!! :)\n";
            break;
        } else if (hasWon == playerAI) {
            cout << "Unfortunately the KI has won... :(\n";
            break;
        }

        if (gp.getNumOfFreeFields() <= 0) {
            cout << "Game is over. You did well!."
					"No more cells are free on the Game!\n";
            break;
        }
		//system("PAUSE");
    }
	system("PAUSE");
    return 0;
}