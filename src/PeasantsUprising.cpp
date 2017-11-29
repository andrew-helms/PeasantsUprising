#include "./PeasantsUprising.h"

//x -> horizontal, y -> vertical
//board[] -> vertical, board[][] -> horizontal

/* TODO:
1) Fix issue with intially trying to move a Queen or Bishop
2) Add Card UI
*/

int main() {
	bool humansTurn = true;
	bool cardsPlayed[2][3] = { {false, false, false}, {false, false, false} };
	bool cardsUnlocked[2][3] = { { false, false, false }, { false, false, false } };
	Point captureLoc= Point(-1, -1);
	string input;
	Point* moves;
	int empoweredLoc;

	std::cout << "Welcome to Peasant's Uprising!\n\nHumans start.";

	createBoard();
	printBoard();

	//Loops until user exits
	while (input != "exit") {
		//Loops to find if a card has been unlocked
		for (int i = 0; i < 3; i++) {
			//If the player has cards
			if (cardsUnlocked[!humansTurn][i]) {
				//Prompts for type of turn
				std::cout << "\n1. Normal turn\n2. Play Card\n\nChoice - ";
				std::cin >> input;
				if (toLower(input) == "exit") return 0;

				//Loops until the player enters a valid response
				while (input != "1" && input != "2") {
					std::cout << "\nThat was not an option. Let's try that again.\n\nChoice - ";
					std::cin >> input;
					if (toLower(input) == "exit") return 0;
				}

				//If the player wants to play a card
				if (input == "2") {
					players[!humansTurn].printCards();

					std::cout << "\n\nCard - ";
					std::cin >> input;
					if (toLower(input) == "exit") return 0;
				}

				break;
			}
		}


		std::cout << "\nPiece - ";
		std::cin >> input;
		input = toLower(input);
		if (toLower(input) == "exit") return 0;

		Point pieceLoc = boardSpotToPoint(input);
		Piece* piece = findPiece(pieceLoc, humansTurn);
		int movesTracker = 0;
		Piece* capturePiece;
		Piece* temp;
		Point newLoc;

		//Loops until the user enters a valid board space
		while (pieceLoc.x == -1 || pieceLoc.y == -1 || piece == NULL) {
			std::cout << "\nThat's not a valid piece. Let's try that again.\n\nPiece - ";
			std::cin >> input;
			if (toLower(input) == "exit") return 0;
			pieceLoc = boardSpotToPoint(input);
			piece = findPiece(pieceLoc, humansTurn);
		}

		//If the empowered card is being played
		if (empowered) {
			temp = piece; //Holds the original piece
			empoweredLoc = empowerCard(piece); //Empowers the piece
			moves = pieces[!temp->isHuman][empoweredLoc]->determineMoveSet(pieces); //Gets the possible moves

			if (moves[0].equals(captureLoc)) { //If there are no available moves
				std::cout << "\nThat piece cannot move. Let's try that again.\n";
				continue;
			} else {
				std::cout << "\nThese are the available spots for the " << temp->name << ":";
				previewBoard(moves);
			}
		} else { //A normal turn
			moves = piece->determineMoveSet(pieces); //Gets the possible moves

			if (moves[0].equals(captureLoc)) { //If there are no available moves
				//Works for HK and HRs -> not HQ or HBs
				//Works for OK and ORs -> not OQ or OBs
				std::cout << "\nThat piece cannot move. Let's try that again.\n\n";
				continue;
			} else {
				std::cout << "\nThese are the available spots for the " << piece->name << ":";
				previewBoard(moves);
			}
		}

		//Prompts to move this piece or to pick a different one
		std::cout << "\n1. Move this piece\n2. Pick new piece\n\nChoice - ";
		std::cin >> input;
		if (toLower(input) == "exit") return 0;

		//Loops until a valid choice has been entered
		while (input != "1" && input != "2") {
			std::cout << "\nThat was not an option. Let's try that again.\n\nChoice - ";
			std::cin >> input;
			if (toLower(input) == "exit") return 0;
		}
		
		//Pick a different piece -> go back to the beginning of the loop
		if (input == "2") continue;

		//Gathers information on the spot to move to
		std::cout << "\nSpot - ";
		std::cin >> input;
		if (toLower(input) == "exit") return 0;
		newLoc = boardSpotToPoint(input);

		//Loops until a valid move has been found
		while (!moves[movesTracker].equals(newLoc)) {
			movesTracker++;

			//If at the end of the move set array (the entered space isn't a move)
			if (moves[movesTracker].equals(captureLoc)) {
				std::cout << "\nThat piece cannot move there. Let's try that again.\n\Spot - ";
				std::cin >> input;
				if (toLower(input) == "exit") return 0;
				newLoc = boardSpotToPoint(input);
				movesTracker = 0;
			}
		}

		//Finds the theoretical piece to capture
		capturePiece = findPiece(newLoc, !humansTurn);

		//If there is a piece to be captured
		if (capturePiece != NULL) {
			movePiece(capturePiece, captureLoc);
			players[!humansTurn].setScore(capturePiece->getScore());

			//If a king has been captured
			if (capturePiece->bit == king) {
				movePiece(piece, newLoc);
				printBoard();
				std::cout << "\nCongratulations! " << (humansTurn ? "Humans" : "Orcz") << "have won the game.";
				return 0;
			}

			//Unlocks cards
			if (players[!humansTurn].getScore() >= 30) { //Revive threshold
				if (!cardsPlayed[!humansTurn][2]) { //Revive not played
					cardsUnlocked[!humansTurn][2] = true;
					std::cout << "\nYay! You have unlocked a card.\n";
					players[!humansTurn].printCards();
				}
			} else if (players[!humansTurn].getScore() >= 20) { //Switch threshold
				if (!cardsPlayed[!humansTurn][1]) { //Switch not played
					cardsUnlocked[!humansTurn][1] = true;
					std::cout << "\nYay! You have unlocked a card.\n";
					players[!humansTurn].printCards();
				}
			} else if (players[!humansTurn].getScore() >= 10) { //Empower threshold
				if (!cardsPlayed[!humansTurn][0]) { //Empower not played
					cardsUnlocked[!humansTurn][0] = true;
					std::cout << "\nYay! You have unlocked a card.\n";
					players[!humansTurn].printCards();
				}
			}
		}

		//Moves the piece
		movePiece(piece, newLoc);

		//If the empowered card is finished
		if (empowered) {
			empowered = false; //Turns empower off
			temp->move(pieces, newLoc.x, newLoc.y); //Updates the original piece
			pieces[!humansTurn][empoweredLoc] = temp; //Resets the original piece
		}

		//CLEAR SCREEN HERE

		//Switches to the next player
		humansTurn = !humansTurn;

		std::cout << (humansTurn ? "\nHumans " : "\nOrcz ") << "turn";
		printBoard();
	}

	return 1;
}

/* This function takes a string and converts it to an all lowercase string. The string can contain any character.
@param string: The string to convert
@return string: The lowercase string
*/
string toLower(string s1) {
	for (int i = 0; i < s1.length(); i++) {
		if (s1[i] >= 65 && s1[i] <= 90) s1[i] = tolower(s1[i]);
	}

	return s1;
}

/* This function converts a space on the board to the corresponding index in the board array. If the space isn't on the board, -1 is returned.
@param string: The space to convert.
@return int: The corresponding index in the board array. The first digit corresponds the first array index, the second digit corresponds to the second.
An example of a return is 67 which means the spot is the 7th index in the 6th indexed array in board
*/
Point boardSpotToPoint(string s1) {
	Point spot;
	s1 = toLower(s1);

	if (s1.length() != 2) return Point(-1, -1); //If the input is not 2 characters

	//Evaluates alphaphabetic characters
	if (s1[0] >= 97 && s1[0] <= 104) {
		spot.x = (int)(s1[0]) - 97;
	} else {
		spot.x = -1;
	}

	//Evaluates numeric characters
	if (s1[1] >= 49 && s1[1] <= 56) {
		spot.y = (int)(s1[1]) - 49;
	} else {
		spot.y = -1;
	}

	return spot;
}

/* This function returns if a spot on the board is occupied
*/
bool spotOccupied(Point spot) {
	return (board[spot.y][spot.x][0] != ' ');
}

/* This function finds the piece at the given spot.
@param Point: The point to look for
@return Piece*: A pointer to the piece found
*/
Piece* findPiece(Point spot, bool isHuman) {
	int i = !isHuman;

	for (int j = 0; j < 16; j++) {
		if (pieces[i][j]->getPos().x == spot.x && pieces[i][j]->getPos().y == spot.y) return pieces[i][j];
	}

	return NULL;
}

/* This function prints the board in its current state
*/
void printBoard() { 
	//Prints the scoreboard
	std::cout << "\n\n\tScore";
	std::cout << "\nHumans\t" << players[0].getScore();
	std::cout << "\nOrcz\t" << players[1].getScore();
	std::cout << "\n\n  +---------------------------------------+\n"; //outputs the first line of the board

																	  //Loops through the board
	for (int i = 0; i < 8; i++) {
		std::cout << (i + 1) << " |";
		for (int j = 0; j < 8; j++) {
			std::cout << " " << board[i][j][0] << board[i][j][1] << " |";
		}

		std::cout << ((i != 7) ? "\n  |----+----+----+----+----+----+----+----|\n" : "\n  +---------------------------------------+\n");
	}

	std::cout << "    A    B    C    D    E    F    G    H\n"; //Outputs the bottom of the board
}

/* This function prints a preview board which showcases all of the possible spots a piece can move.
@param Point*: The array of moves to preview
*/
void previewBoard(Point* move) {
	Point comp; //Comparison point (used to test whether piece is on spot)
	Point endPt = Point(-1, -1);

	std::cout << "\n  +---------------------------------------+\n"; //outputs the first line of the board

	//Loops through the board
	for (int i = 0; i < 8; i++) {
		std::cout << (i + 1) << " |";
		for (int j = 0; j < 8; j++) {
			int k = 0; //Tracks the position in move
			bool matched = false; //Tracks if the current board spot is a move
			comp.x = j;
			comp.y = i;

			//Loops through the available moves
			while (!move[k].equals(endPt)) {
				if (move[k].equals(comp)) { //Outputs a move
					if (board[i][j][0] == ' ') { //Empty spots are double spaced
						std::cout << "xxxx|";
					}
					else {
						std::cout << "x" << board[i][j][0] << board[i][j][1] << "x|";
					}

					matched = true;
				}

				k++;
			}

			if (!matched) std::cout << " " << board[i][j][0] << board[i][j][1] << " |"; //Outputs a normal space
		}

		std::cout << ((i != 7) ? "\n  |----+----+----+----+----+----+----+----|\n" : "\n  +---------------------------------------+\n");
	}

	std::cout << "    A    B    C    D    E    F    G    H\n"; //Outputs the bottom of the board
}

/* This function moves a piece to the given spot. It updates the piece's interal spot and the board. The space where the piece was originally is turned blank
@param Piece*: The pointer to the piece to move
@param Point: The spot to move the piece to
*/
void movePiece(Piece* piece, Point spot) {
	Point oldPos = piece->getPos(); //Tracks the old position
	
	//Moves the piece
	if (spot.x == -1) { //if the piece is being captured
		piece->move(pieces, -1, -1);
	} else {
		piece->move(pieces, spot.x, spot.y);
		board[spot.y][spot.x][0] = (piece->isHuman ? 'H' : 'O');
		board[spot.y][spot.x][1] = piece->symb;
	}

	//Updates the board
	board[oldPos.y][oldPos.x][0] = ' ';
	board[oldPos.y][oldPos.x][1] = ' ';
}

/* This function fills the global board with the symbols for the initial piece positions
*/
void createBoard() {
	//Sets all values to a space the will be overwritten later
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			for (int k = 0; k < 2; k++)
				board[i][j][k] = ' ';

	for (int i = 0; i < 8; i++) {
		//Sets Human and Orc type
		board[0][i][0] = 'O';
		board[1][i][0] = 'O';
		board[6][i][0] = 'H';
		board[7][i][0] = 'H';

		//Sets pawn type
		board[1][i][1] = 'P';
		board[6][i][1] = 'P';
	}

	//Fills remaining Orc pieces
	board[0][0][1] = 'R';
	board[0][1][1] = 'N';
	board[0][2][1] = 'B';
	board[0][3][1] = 'Q';
	board[0][4][1] = 'K';
	board[0][5][1] = 'B';
	board[0][6][1] = 'N';
	board[0][7][1] = 'R';

	//FIlls remaining Human pieces
	board[7][0][1] = 'R';
	board[7][1][1] = 'N';
	board[7][2][1] = 'B';
	board[7][3][1] = 'Q';
	board[7][4][1] = 'K';
	board[7][5][1] = 'B';
	board[7][6][1] = 'N';
	board[7][7][1] = 'R';
}

/* This function handles the event in which the Switch card is played.
@param Piece*: The pointer to the first piece to switch
@param Piece*: The pointer to the second piece to switch
*/
void switchCard(Piece* piece1, Piece* piece2) {
	Point oldP1 = piece1->getPos();
	Point oldP2 = piece2->getPos();

	//Updates the pieces position
	piece1->move(pieces, oldP2.x, oldP2.y);
	piece2->move(pieces, oldP1.x, oldP1.y);

	//Updates the board
	board[oldP2.y][oldP2.x][1] = piece1->symb;
	board[oldP1.y][oldP1.x][1] = piece2->symb;
}

/* This function empowers a piece by making the pieces array point to a Queen. It does not update the global board array.
@param Piece*: Pointer to the piece to empower
@return int: The location of the empowered piece in pieces[].
*/
int empowerCard(Piece* piece) {
	empowered = true; //Sets empowered to true
	Point oldPos = piece->getPos(); //Tracks the old position
	Piece* oldPiece = piece;

	for (int i = 0; i < 16; i++) {
		if (pieces[!piece->isHuman][i]->getPos().x == oldPos.x && pieces[!piece->isHuman][i]->getPos().y == oldPos.y) {
			pieces[!piece->isHuman][i] = new Piece(queen, oldPos.x, oldPos.y, piece->isHuman); //updates pieces to point to a Queen instead

			oldPiece->move(pieces, -1, -1); //"kills" the piece
			return i;
		}
	}
}

/* This function brings the specified piece back to life in a spot in the first 2 rows.
@param Piece*: A pointer to the piece to bring back
*/
void reviveCard(Piece* res) {
	bool human = res->isHuman;
	Piece** pieces[2]; //All the pieces on the board
	pieces[0] = players[0].getPieces();
	pieces[1] = players[1].getPieces();
	string stringSpot;
	Point point; //Tracks the point of the piece
	bool first2Rows = false; //Tracks whether the spot is in the first two rows

	std::cout << "You may pick a spot in your first two rows.\nSpot - ";
	std::cin >> stringSpot;

	point = boardSpotToPoint(stringSpot);
	first2Rows = (human ? (point.y == 6 || point.y == 7) : (point.y == 0 || point.y == 1));

	//Loops while the user enters an available spaces
	while (spotOccupied(point) || !first2Rows) {
		std::cout << "\nSpot is unavailable. Let's try that again.\nSpot- ";
		std::cin >> stringSpot;

		point = boardSpotToPoint(stringSpot);
		first2Rows = (human ? (point.y == 6 || point.y == 7) : (point.y == 0 || point.y == 1));
	}

	//Loops through the players pieces
	for (int i = 0; i < 16; i++) {
		if (pieces[!res->isHuman][i]->bit == res->bit && pieces[!res->isHuman][i]->getPos().x == -1) { //If the piece matches and is captured
			pieces[!res->isHuman][i]->move(pieces, point.x, point.y); //Move the piece

			//update the board
			board[point.y][point.x][0] = (res->isHuman ? 'H' : 'O');
			board[point.y][point.x][1] = res->symb;
		}
	}
}