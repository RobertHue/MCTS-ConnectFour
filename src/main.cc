
#include <iostream>

#include "GamePanel.h"
#include "GameKI.h"

#define OK   0
#define ERROR -1
using namespace std;

int main() {
    int col_err;
    GamePanel gp(8, 6);
    GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

    //*************************
    //*** Spieler-Auwahl ******
    //*************************
    Player playerYou, playerKI;
    char input_char;
    do {
        cout << "Bitte Spieler waehlen (xX / oO): ";
        while ((input_char = getchar()) == '\n');
        // doesn't eat the char NL from stdin 

        switch (input_char) {
            case 'x': case 'X':
                playerYou = PLAYER_2;
                playerKI = PLAYER_1;
                break;
            case 'o': case 'O':
                playerYou = PLAYER_1;
                playerKI = PLAYER_2;
                break;
            default:
                cout << "falsche eingabe bitte nochmal eingeben!";
        }
    } while (input_char != 'x' && input_char != 'X' && input_char != 'o' && input_char != 'O');



    //*************************
    //*** Spieler-Z�gewahl ****
    //*************************
    gp.setTurnPlayer(playerKI); // Spieler der beginnen soll setzen!

    GameKI gKI(playerKI);


    while (1) {
        if (gp.getTurnPlayer() == playerYou) {
            int col;
            do {
                cout << "Bitte COLUMN waehlen: ";
                cin >> col;

                col_err = gp.insertTokenIntoColumn(col);
                if (col_err == ERROR) {
                    cout << "Column ist bereits voll! Bitte nochmal Column eingeben: " << endl;
                }
            } while (col < 0 || col >= gp.getMAX_X() || col_err);

        }
        else {
            int col = gKI.calculateNextTurn(gp);
            col_err = gp.insertTokenIntoColumn(col);
        }
        GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());



        Player hasWon = gp.hasSomeoneWon();
        if (hasWon == playerYou) {
            cout << "Herzlichen Glueckwunsch du hast gewonnen!!!" << endl;
            break;
        } else if (hasWon == playerKI) {
            cout << "Leider hat die KI gewonnen..." << endl;
            break;
        }

        if (gp.getNumOfFreeFields() <= 0) {
            cout << "Spiel ist zuende. Da keine Felder mehr frei!" << endl;
            break;
        }
        // system("PAUSE");
    }


    system("PAUSE");
    return 0;
}
