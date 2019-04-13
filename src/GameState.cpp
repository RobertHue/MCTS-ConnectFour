
#include "GameState.h"

int GameState::getNumOfFreeFields() const {
    return this->numOfFreeFields;
}

Position GameState::getPositionOfLastPlacedToken() const {
    return this->positionOfLastPlacedToken;
}

std::vector<std::vector<int>> GameState::getGameData() const {
    return gameData;
}

void GameState::setTurnPlayer(Player turnPlayer) {
    this->turnPlayer = turnPlayer;
}

Player GameState::getTurnPlayer() const {
    return this->turnPlayer;
}

Player GameState::getOtherPlayer() const {
    return this->otherPlayer;
}

void GameState::nextTurn() {
    if (turnPlayer == Player::PLAYER_1) {
        turnPlayer = Player::PLAYER_2;
        otherPlayer = Player::PLAYER_1;
    } else {
        turnPlayer = Player::PLAYER_1;
        otherPlayer = Player::PLAYER_2;
    }
}

std::vector<int> GameState::getPossibleMoves() const
{
	// stores the cols which can be made as possible moves
	std::vector<int> possibleMoves;

	int row, col;
	for (col = 0; col < MAX_X; ++col) {
		for (row = MAX_Y - 1; row >= 0; --row) {

			// check whether insertion cell is still free
			if (gameData[col][row] == FREE_FIELD) {
				possibleMoves.push_back(col);
				break;	// break out of rows loop
			}
		}
	}

	return possibleMoves;
}

/*
Allgemein:
        gibt den Spieler (der zuvor ein Token platziert hat) zurck, der kurz davor ist, eine 4er-Reihe zu vervollstndigen.
        gibt den Spieler (der zuvor ein Token platziert hat) zurck, der eine 3er-Reihe besitzt, die durch Platzieren eines Tokens vervollstndigt werden kann

Falls es keine solche 3er-Reihe gibt, wird NONE zurckgegeben

Beispiele:
        "X,X,X,O" => NONE;  "X,X,X,_" oder "X,_,X,X" oder "O,X,X,_,X" => Player X
        "_,_,X,_,X,_,X,_,X,X"
 */
Player GameState::isAboutToWin() {
    bool closeFreeField = false;
    int x_placed = positionOfLastPlacedToken.x;
    int y_placed = positionOfLastPlacedToken.y;

    // wer hat dieses Token berhaupt platziert
    Player tokenPlacedByPlayer, otherPlayer;
    if (gameData[x_placed][y_placed] == static_cast<int>(Player::PLAYER_1)) {
        tokenPlacedByPlayer = Player::PLAYER_1;
        otherPlayer = Player::PLAYER_2;
    } else {
        tokenPlacedByPlayer = Player::PLAYER_2;
        otherPlayer = Player::PLAYER_1;
    }

    // immer mitz�hlen wieviel tokens in einer Reihe:
    int num_of_tokens_in_row = 0;


    int offsets[][2] = {
        { -3, 0},
        { -2, 0},
        { -1, 0},
        { 0, 0},
        { 1, 0},
        { 2, 0},
        { 3, 0}, // horizontal
        { 0, -3},
        { 0, -2},
        { 0, -1},
        { 0, 0},
        { 0, 1},
        { 0, 2},
        { 0, 3}, // vertical
        { -3, -3},
        { -2, -2},
        { -1, -1},
        { 0, 0},
        { 1, 1},
        { 2, 2},
        { 3, 3}, // diagonal1
        { -3, 3},
        { -2, 2},
        { -1, 1},
        { 0, 0},
        { 1, -1},
        { 2, -2},
        { 3, -3} // diagonal2
    };

    // berprfe vom platzierten Spielstein aus alle 8 Richtungen, und zhle die Anzahl der Spielsteine in einer Reihe
    for (size_t i = 0; i < ((sizeof (offsets) / sizeof (int)) / 2); ++i) {
        // neue Zeile in der offset_Matrix => neue berprfung der Anzahl der Token in einer Reihe
        if ((i % 7) == 0) {
            num_of_tokens_in_row = 0;
            closeFreeField = false;
        }
        int x_check = x_placed + offsets[i][0];
        int y_check = y_placed + offsets[i][1];

        // Plausi-Check (Grenzwerte des GamePanels erreicht):
        if (x_check < 0 || x_check >= MAX_X) continue;
        if (y_check < 0 || y_check >= MAX_Y) continue;

        if (this->gameData[x_check][y_check] == static_cast<int>(tokenPlacedByPlayer)) {
            ++num_of_tokens_in_row;
        } else if (this->gameData[x_check][y_check] == FREE_FIELD) {
            // falls zum zweiten mal ein freies Feld kam, setzte die zahl zur�ck
            if (closeFreeField == true) {
                num_of_tokens_in_row = 0;
                continue;
            }
            closeFreeField = true;
        } else if (this->gameData[x_check][y_check] == static_cast<int>(otherPlayer)) { // auf Position befindet sich ein gegnerischer Spielstein (Token)
            num_of_tokens_in_row = 0;
            closeFreeField = false;
            continue;
        }

        // hat der turnPlayer eine 3er-Reihe (wird nur �berpr�ft falls ein FREE_FIELD vorkam!)
        if (closeFreeField == true && num_of_tokens_in_row >= 3)
            return turnPlayer;
    }


    return Player::NONE; // der turnPlayer hat keine 3er-Reihe mit offenem Feld
}

Player GameState::hasSomeoneWon() {
    int x_placed = positionOfLastPlacedToken.x;
    int y_placed = positionOfLastPlacedToken.y;

    // count how many tokens there are in each row:
    int num_of_tokens_p1_in_row = 0;
    int num_of_tokens_p2_in_row = 0;

    int offsets[][2] = {
        { -3, 0},
        { -2, 0},
        { -1, 0},
        { 0, 0},
        { 1, 0},
        { 2, 0},
        { 3, 0}, // horizontal
        { 0, -3},
        { 0, -2},
        { 0, -1},
        { 0, 0},
        { 0, 1},
        { 0, 2},
        { 0, 3}, // vertical
        { -3, -3},
        { -2, -2},
        { -1, -1},
        { 0, 0},
        { 1, 1},
        { 2, 2},
        { 3, 3}, // diagonal1
        { -3, 3},
        { -2, 2},
        { -1, 1},
        { 0, 0},
        { 1, -1},
        { 2, -2},
        { 3, -3} // diagonal2
    };
    // cout << sizeof(offsets) << endl;

	// check all 8 directions on the current placed token, and count the number of tokens in each of those directions
    for (size_t i = 0; i < ((sizeof (offsets) / sizeof (int)) / 2); ++i) {
		// new line inside the offset_Matrix => new check of number of tokens in each of the 8 directions
        if ((i % 7) == 0) {
            num_of_tokens_p1_in_row = 0;
            num_of_tokens_p2_in_row = 0;
        }
        int x_check = x_placed + offsets[i][0];
        int y_check = y_placed + offsets[i][1];

        // check whether bounds are reached in those directions:
        if (x_check < 0 || x_check >= MAX_X) continue;
        if (y_check < 0 || y_check >= MAX_Y) continue;


        if (this->gameData[x_check][y_check] == static_cast<int>(Player::PLAYER_1)) {
            num_of_tokens_p2_in_row = 0;
            ++num_of_tokens_p1_in_row;
        } else if (this->gameData[x_check][y_check] == static_cast<int>(Player::PLAYER_2)) {
            num_of_tokens_p1_in_row = 0;
            ++num_of_tokens_p2_in_row;
        } else if (this->gameData[x_check][y_check] == FREE_FIELD) {
            num_of_tokens_p1_in_row = 0;
            num_of_tokens_p2_in_row = 0;
        }

        // has someone won?
        if (num_of_tokens_p1_in_row >= 4) return Player::PLAYER_1;
        else if (num_of_tokens_p2_in_row >= 4) return Player::PLAYER_2;
    }

	// if number of free fields are 0,
	if (numOfFreeFields == 0) {
		// then there is a tie!
		return Player::DRAW;
	}

    return Player::NONE; // noone has won yet!
}

void GameState::drawGameStateOnConsole(std::vector<std::vector<int>> gameData, int MAX_X, int MAX_Y) {
    // system("cls");
	std::cout << "Connect-Four (" << MAX_Y << "x" << MAX_X << "):\n\n\n";
	std::cout << "  ";
    for (int x = 0; x < MAX_X; ++x) {
		std::cout << " " << x;
    }
	std::cout << "\n  ";


    for (int x = 0; x < MAX_X; ++x) {
		std::cout << "--";
    }
	std::cout << "-\n";


    for (int y = 0; y < MAX_Y; ++y) {
		std::cout << y << " ";
        for (int x = 0; x < MAX_X; ++x) {
			std::cout << "|";

            switch (gameData[x][y]) {
                case FREE_FIELD:
					std::cout << " "; // freier Platz
                    break;
                case Player::PLAYER_1:
					std::cout << "O";
                    break;
                case Player::PLAYER_2:
					std::cout << "X";
                    break;
                default:
					std::cout << "E"; // Error...
            }
        }
        // zwischenzeile fr Trennlinie
		std::cout << "|\n  ";
        for (int x = 0; x < MAX_X; ++x) {
			std::cout << "--";
        }
		std::cout << "-\n";
    }
	std::cout << "\n";
}

bool GameState::insertTokenIntoColumn(int column) {
    int row;
    for (row = MAX_Y - 1; row >= 0; --row) {

		// check whether insertion cell is still free
        if (gameData[column][row] == FREE_FIELD) {
            // insert token into the cell, which is free:
            gameData[column][row] = static_cast<int>(this->turnPlayer);

            // remember the position of the last placed token
            positionOfLastPlacedToken.x = column;
            positionOfLastPlacedToken.y = row;

			// decrease the number of left free fields on the GamePanel
            --numOfFreeFields;

            // swap TurnPlayer, in case of an valid move
            nextTurn();
            return VALID_MOVE;
        }
    }
    // token couldn't be placed at column. Please choose again!
	// Reason: Column is already full
    return NO_VALID_MOVE;
}

int GameState::getMAX_X() const {
    return MAX_X;
}

int GameState::getMAX_Y() const {
    return MAX_Y;
}
