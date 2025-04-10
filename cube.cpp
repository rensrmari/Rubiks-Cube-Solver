#include "cube.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <set>
#include <cctype>
#include <algorithm>
using namespace std;

const set<char> Cube::VALID_MOVES = { 'U', 'L', 'F', 'R', 'B', 'D', 'x', 'y' };
const map<char, string> Cube::COLOR_STRINGS = { // for hint message, annotation of step
	{ 'W', "White" },
	{ 'G', "Green" },
	{ 'R', "Red" },
	{ 'B', "Blue" },
	{ 'O', "Orange" },
	{ 'Y', "Yellow" },
};
const map<int, char> Cube::FACE_COLORS = {
	{ Faces::TOP, 'W' },
	{ Faces::LEFT, 'G' },
	{ Faces::FRONT, 'R' },
	{ Faces::RIGHT, 'B' },
	{ Faces::BACK, 'O' },
	{ Faces::BOTTOM, 'Y' }
};

bool Cube::checkMoves(const string& moves) {
	istringstream iss(moves);
	char ch;
	bool prevMove = false;
	bool prevApostrophe = false;

	while (iss >> ch) {
		if (Cube::VALID_MOVES.count(ch) == 1) {                                 // Characters in the set are valid
			prevMove = true;
		} else {
			if (ch != ' ') {                                                    // Spaces are valid
				if (isdigit(ch) && prevMove || isdigit(ch) && prevApostrophe) { // If there was a valid move or apostrophe before, check if number is 2
					int testNum = ch - '0';

					if (testNum == 2) {
						prevMove = false;
						prevApostrophe = false;
					} else {
						return false;
					}
				} else if (ch == '\'' && prevMove) {                            // Apostrophes are valid if there was a previous move
					prevMove = false;
					prevApostrophe = true;
				} else {
					return false;
				}
			}
		}

		if (iss.fail()) { 
			return false;
		}
	}

	return true;
}

int Cube::countMoves(const string& moves) {
	if (!checkMoves(moves)) {
		return -1;
	}

	istringstream iss(moves);
	int numMoves = 0;
	char ch;

	while (iss >> ch) {
		if (ch != '\'' && ch != '2') { // Counting letters
			numMoves++;
		}

		if (ch == '2') { // Double the count if there is a double move
			numMoves++;
		}
	}

	return numMoves;
}

Cube::Cube() {
	name = "";
	scramble = "";
	moves = "";
	totalMoves = 0;
	createSolved();
}

Cube::Cube(const string& name, const string& scramble, const string& moves, int totalMoves) {
	this->name = name;
	this->scramble = scramble;
	this->moves = moves;
	this->totalMoves = totalMoves;
	createSolved();
	doMoves(scramble, false);
	doMoves(moves, false);
}

string Cube::getName() const { return name; }

void Cube::setName(const string& name) { this->name = name; }

void Cube::setScramble(const string& scramble) { this->scramble = scramble; }

string Cube::getScramble() const { return scramble; }

string Cube::getMoves() const { return moves; }

string Cube::getCurrentMoves() const {
	string res = "";
	stack<string> temp = currentMoves;
	vector<string> reversed;

	while (temp.size() > 0) {
		reversed.push_back(temp.top());
		temp.pop();
	}

	for (string move : reversed) {
		res += move;
	}

	return res;
}

int Cube::getTotalMoves() const { return totalMoves; }

void Cube::createSolved() {
	char color;

	for (int i = 0; i < NUM_FACES; i++) { // For each face, create 3x3 array of its color
		color = FACE_COLORS.at(i);

		for (int j = 0; j < SIZE; j++) {
			for (int k = 0; k < SIZE; k++) {
				stickers[i][j][k] = color;
			}
		}
	}
}

void Cube::displayState(bool printCommands) const {
	const int INDIV_WIDTH = 2;
	const int FACE_SPACING = 5;
	const int FACE_WIDTH = INDIV_WIDTH * SIZE + (SIZE - 1);
	
	const int LEFT_PADDING = 3;
	const int MIDDLE_PADDING = LEFT_PADDING + FACE_WIDTH + FACE_SPACING;
	const int RIGHT_PADDING = 8;
	const int RIGHT_PADDING_FROM_MIDDLE = (FACE_WIDTH * 2) + (FACE_SPACING * 2) + RIGHT_PADDING;
	const int RIGHT_PADDING_FROM_LEFT = RIGHT_PADDING_FROM_MIDDLE + (FACE_WIDTH * 2) + FACE_SPACING + LEFT_PADDING;

	const int COMMAND_LENGTH = 25;
	int count = 1;

	// Print the top segments of the cube.
	cout << endl << endl << printSpacing(MIDDLE_PADDING) << "TOP" << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE + (FACE_WIDTH - 3), count, printCommands) << endl
	     << printSpacing(MIDDLE_PADDING) << printFaceSegment(Faces::TOP, INDIV_WIDTH, 0) << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE, count, printCommands) << endl
		 << printSpacing(MIDDLE_PADDING) << printFaceSegment(Faces::TOP, INDIV_WIDTH, 1) << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE, count, printCommands) << endl
		 << printSpacing(MIDDLE_PADDING) << printFaceSegment(Faces::TOP, INDIV_WIDTH, 2) << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE, count, printCommands) << endl
		 << printCommandWithSpacing(RIGHT_PADDING_FROM_LEFT, count, printCommands) << endl;

	// Print the left, front, right, and back segments of the cube.
	cout << printSpacing(LEFT_PADDING) << "LEFT"
		 << printSpacing(FACE_WIDTH - 4 + FACE_SPACING) << "FRONT"
		 << printSpacing(FACE_WIDTH - 5 + FACE_SPACING) << "RIGHT"
		 << printSpacing(FACE_WIDTH - 5 + FACE_SPACING) << "BACK" << printCommandWithSpacing(RIGHT_PADDING + (FACE_WIDTH - 4), count, printCommands) << endl;

	cout << printSpacing(LEFT_PADDING) << printFaceSegment(Faces::LEFT, INDIV_WIDTH, 0)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::FRONT, INDIV_WIDTH, 0)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::RIGHT, INDIV_WIDTH, 0)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::BACK, INDIV_WIDTH, 0) << printCommandWithSpacing(RIGHT_PADDING, count, printCommands) << endl;

	cout << printSpacing(LEFT_PADDING) << printFaceSegment(Faces::LEFT, INDIV_WIDTH, 1)
	 	 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::FRONT, INDIV_WIDTH, 1)
	 	 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::RIGHT, INDIV_WIDTH, 1)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::BACK, INDIV_WIDTH, 1) << printCommandWithSpacing(RIGHT_PADDING, count, printCommands) << endl;

	cout << printSpacing(LEFT_PADDING) << printFaceSegment(Faces::LEFT, INDIV_WIDTH, 2)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::FRONT, INDIV_WIDTH, 2)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::RIGHT, INDIV_WIDTH, 2)
		 << printSpacing(FACE_SPACING) << printFaceSegment(Faces::BACK, INDIV_WIDTH, 2) << printCommandWithSpacing(RIGHT_PADDING, count, printCommands) << endl
		 << printCommandWithSpacing(RIGHT_PADDING_FROM_LEFT, count, printCommands) << endl;

	// Print the bottom segments of the cube.
	cout << printSpacing(MIDDLE_PADDING) << "BOTTOM" << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE + (FACE_WIDTH - 6), count, printCommands) << endl
		 << printSpacing(MIDDLE_PADDING) << printFaceSegment(Faces::BOTTOM, INDIV_WIDTH, 0) << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE, count, printCommands) << endl
		 << printSpacing(MIDDLE_PADDING) << printFaceSegment(Faces::BOTTOM, INDIV_WIDTH, 1) << printCommandWithSpacing(RIGHT_PADDING_FROM_MIDDLE, count, printCommands) << endl
		 << printSpacing(MIDDLE_PADDING) << printFaceSegment(Faces::BOTTOM, INDIV_WIDTH, 2) << endl << endl;
		
	// Print cube data.
	string scrambleString = tokenizeMoves(scramble);
	cout << "Original scramble: " << (scrambleString.length() > 0 ? scrambleString : "None") << endl;
	cout << "Total moves: " << totalMoves << endl;
	cout << "Current moves: ";
	printCurrentMoves();
	cout << endl;
}

string Cube::printFaceSegment(int side, int width, int row) const {
	ostringstream oss;
	
	for (int i = 0; i < SIZE; i++) {
		// Assign the right background color
		char elem = stickers[side][row][i];
		string color;

		switch (elem) {
			case 'W':
				color = WHITE_BG;
				break;
			case 'G':
				color = GREEN_BG;
				break;
			case 'R':
				color = RED_BG;
				break;
			case 'B':
				color = BLUE_BG;
				break;
			case 'O':
				color = ORANGE_BG;
				break;
			case 'Y':
				color = YELLOW_BG;
				break;
		}
	
		oss << color << BLACK_FG << elem << printSpacing(width - 1) << RESET << (i < SIZE - 1 ? " " : ""); // Decrement (additional width)
	}

	return oss.str();
}

string Cube::printSpacing(int spacing) const {
	ostringstream oss;
	oss << left << setw(spacing) << " ";

	return oss.str();
}

string Cube::printCommandWithSpacing(int spacing, int& counter, bool print) const {
	string res = "";

	if (print) {
		res = printSpacing(spacing) + printCommandSegment(counter);
		counter++;
	} 

	return res;
}

string Cube::printCommandSegment(int line) const {
	switch (line) {
		case 1:
			return "-------------------------------------------------------------------------------";
		case 2:
			return "| COMMANDS                                                                    |";
		case 3:
			return "| --------                                                                    |";
		case 4:
			return "|  U: Clockwise turn on the top side.          Note: You can append a \"'\"     |";
		case 5:
			return "|  L: Clockwise turn on the left side.               to a letter for          |";
		case 6:
			return "|  F: Clockwise turn on the front side.              counterclockwise moves,  |";
		case 7:
			return "|  R: Clockwise turn on the right side.              or \"2\" for two moves.    |";
		case 8: 
			return "|  B: Clockwise turn on the back side.                                        |";
		case 9:
			return "|  D: Clockwise turn on the bottom side.                                      |";
		case 10:
			return "|  x: Clockwise rotation on the x-axis.                       HINT     SOLVE  |";
		case 11:
			return "|  y: Clockwise rotation on the y-axis.            UNDO       SAVE      EXIT  |";
		case 12:
			return "-------------------------------------------------------------------------------";
		default:
			return "";
	}

	return "";
}

void Cube::printCurrentMoves() const {
	stack<string> temp = currentMoves;
	vector<string> reversed;
	bool moves = false;

	while (temp.size() > 0) {
		reversed.push_back(temp.top());
		temp.pop();
		moves = true;
	}

	if (moves) {
		for (string move : reversed) {
			cout << move << " ";
		}
	} else {
		cout << "None";
	}
}

string Cube::doMoves(const string& moves, bool update) {
	string res = "";

	if (checkMoves(moves)) {
		// Process the moves if valid.
		res = tokenizeMoves(moves);
		istringstream iss(res);
		string move;

		while (iss >> move) {
			char letter = move[0];
			bool prime = false;
			bool twice = false;

			for (int i = 0; i < move.length(); i++) { // Check for move modifiers
				if (move[i] == '\'') {
					prime = true;
				}

				if (move[i] == '2') {
					twice = true;
				}
			}

			if (!prime && !twice) { // No modifier
				processMove(letter);
			} else if (twice) { // Turning a face twice in either direction ends up the same
				processMove(letter);
				processMove(letter);
			} else { // Prime moves are counterclockwise, which is the same as three times clockwise
				processMove(letter);
				processMove(letter);
				processMove(letter);
			}

			// Add the move to the current moves.
			if (update) {
				currentMoves.push(move);
			}
		}

		// Update the counter.
		if (update) {
			totalMoves += countMoves(res);
		}
	}

	return res;
}

string Cube::tokenizeMoves(const string& moves) const {
	string res = "";
	istringstream iss(moves);
	char ch;

	while (iss >> ch) {
		char next = iss.peek();
		
		// Check for numbers and apostrophes (ex: U, U2, U', and U'2 are all valid moves).
		if (next == '2') {                // The number will always be the last in a move
			res += string(1, ch) + "2 ";
			iss.get();
		} else if (next == '\'') {        // Apostrophes will always follow the letter
			res += string(1, ch);
		} else {                          // Otherwise, the next letter will be another letter or EOF
			res += string(1, ch) + " ";
		}
	}

	return res;
}

void Cube::processMove(char letter) {
	switch (letter) {
		case 'U':
			turnU();
			break;
		case 'L':
			turnL();
			break;
		case 'F':
			turnF();
			break;
		case 'R':
			turnR();
			break;
		case 'B':
			turnB();
			break;
		case 'D':
			turnD();
			break;
		case 'x':
			rotX();
			break;
		case 'y':
			rotY();
			break;
	}
}

void Cube::placeRow(int face, vector<char> colors, int row) {
	for (int i = 0; i < SIZE; i++) {
		stickers[face][row][i] = colors[i];
	}
}

void Cube::placeCol(int face, vector<char> colors, int col) {
	for (int i = 0; i < SIZE; i++) {
		stickers[face][i][col] = colors[i];
	}
}

vector<char> Cube::getRow(int face, int row) const {
	vector<char> res;

	for (int i = 0; i < SIZE; i++) {
		res.push_back(stickers[face][row][i]);
	}
	
	return res;
}

vector<char> Cube::getCol(int face, int col) const {
	vector<char> res;

	for (int i = 0; i < SIZE; i++) {
		res.push_back(stickers[face][i][col]);
	}
	
	return res;
}

void Cube::rotateFace(int face) {
	// Get original color values.
	vector<char> topRow = getRow(face, 0);
	vector<char> rightCol = getCol(face, 2);
	vector<char> bottomRow = getRow(face, 2);
	vector<char> leftCol = getCol(face, 0);

	// Reverse the order of the right and left segments to properly translate them to bottom and top respectively.
	reverse(rightCol.begin(), rightCol.end());
	reverse(leftCol.begin(), leftCol.end());

	// Replace each segment of the face.
	placeCol(face, topRow, 2);    // Top to right
	placeRow(face, rightCol, 2);  // Right to bottom
	placeCol(face, bottomRow, 0); // Bottom to left
	placeRow(face, leftCol, 0);   // Left to top
}

void Cube::turnU() {
	// Rotate the top face.
	rotateFace(Faces::TOP);

	// Switch sides of the top face.
	vector<char> frontSide = getRow(Faces::FRONT, 0);
	vector<char> rightSide = getRow(Faces::RIGHT, 0);
	vector<char> backSide = getRow(Faces::BACK, 0);
	vector<char> leftSide = getRow(Faces::LEFT, 0);

	placeRow(Faces::FRONT, rightSide, 0);
	placeRow(Faces::RIGHT, backSide, 0);
	placeRow(Faces::BACK, leftSide, 0);
	placeRow(Faces::LEFT, frontSide, 0);
}

void Cube::turnL() {
	// Rotate the left face.
	rotateFace(Faces::LEFT);

	// Switch sides of the left face.
	vector<char> topSide = getCol(Faces::TOP, 0);
	vector<char> frontSide = getCol(Faces::FRONT, 0);
	vector<char> bottomSide = getCol(Faces::BOTTOM, 0);
	vector<char> backSide = getCol(Faces::BACK, 2);

	// Reverse back segment so it can be properly translated to the top.
	// Reverse bottom segment so it can be properly translated to the back.
	reverse(backSide.begin(), backSide.end());
	reverse(bottomSide.begin(), bottomSide.end());

	placeCol(Faces::TOP, backSide, 0);
	placeCol(Faces::FRONT, topSide, 0);
	placeCol(Faces::BOTTOM, frontSide, 0);
	placeCol(Faces::BACK, bottomSide, 2);
}

void Cube::turnF() {
	// Rotate the front face.
	rotateFace(Faces::FRONT);

	// Switch sides of the front face.
	vector<char> topSide = getRow(Faces::TOP, 2);
	vector<char> rightSide = getCol(Faces::RIGHT, 0);
	vector<char> bottomSide = getRow(Faces::BOTTOM, 0);
	vector<char> leftSide = getCol(Faces::LEFT, 2);

	// Reverse left segment so it can be properly translated to the top.
	// Reverse right segment so it can be properly translated to the bottom.
	reverse(leftSide.begin(), leftSide.end());
	reverse(rightSide.begin(), rightSide.end());

	placeRow(Faces::TOP, leftSide, 2);
	placeCol(Faces::RIGHT, topSide, 0);
	placeRow(Faces::BOTTOM, rightSide, 0);
	placeCol(Faces::LEFT, bottomSide, 2);
}

void Cube::turnR() {
	// Rotate the right face.
	rotateFace(Faces::RIGHT);

	// Switch sides of the right face.
	vector<char> topSide = getCol(Faces::TOP, 2);
	vector<char> frontSide = getCol(Faces::FRONT, 2);
	vector<char> bottomSide = getCol(Faces::BOTTOM, 2);
	vector<char> backSide = getCol(Faces::BACK, 0);

	// Reverse top segment so it can be properly translated to the back.
	// Reverse back segment so it can be properly translated to the bottom.
	reverse(topSide.begin(), topSide.end());
	reverse(backSide.begin(), backSide.end());

	placeCol(Faces::TOP, frontSide, 2);
	placeCol(Faces::FRONT, bottomSide, 2);
	placeCol(Faces::BOTTOM, backSide, 2);
	placeCol(Faces::BACK, topSide, 0);
}

void Cube::turnB() {
	// Rotate the back face.
	rotateFace(Faces::BACK);

	// Switch sides of the back face.
	vector<char> topSide = getRow(Faces::TOP, 0);
	vector<char> rightSide = getCol(Faces::RIGHT, 2);
	vector<char> bottomSide = getRow(Faces::BOTTOM, 2);
	vector<char> leftSide = getCol(Faces::LEFT, 0);

	// Reverse top segment so it can be properly translated to the left.
	// Reverse bottom segment so it can be properly translated to the right.
	reverse(topSide.begin(), topSide.end());
	reverse(bottomSide.begin(), bottomSide.end());

	placeRow(Faces::TOP, rightSide, 0);
	placeCol(Faces::RIGHT, bottomSide, 2);
	placeRow(Faces::BOTTOM, leftSide, 2);
	placeCol(Faces::LEFT, topSide, 0);
}

void Cube::turnD() {
	// Rotate the bottom face.
	rotateFace(Faces::BOTTOM);

	// Switch sides of the bottom face.
	vector<char> frontSide = getRow(Faces::FRONT, 2);
	vector<char> rightSide = getRow(Faces::RIGHT, 2);
	vector<char> backSide = getRow(Faces::BACK, 2);
	vector<char> leftSide = getRow(Faces::LEFT, 2);

	placeRow(Faces::FRONT, leftSide, 2);
	placeRow(Faces::RIGHT, frontSide, 2);
	placeRow(Faces::BACK, rightSide, 2);
	placeRow(Faces::LEFT, backSide, 2);
}

void Cube::rotX() {

}

void Cube::rotY() {

}

void Cube::undo() {
	if (currentMoves.size() > 0) {
		string undoneMove = currentMoves.top();
		currentMoves.pop();
		bool prime = false;
		bool twice = false;

		for (int i = 0; i < undoneMove.length(); i++) {
			if (undoneMove[i] == '\'') {
				prime = true;
			}

			if (undoneMove[i] == '2') {
				twice = true;
			}
		}

		char letter = undoneMove[0];
		if (!prime && !twice) { // A prime (counterclockwise) move is the opposite of a normal (clockwise) move
			doMoves(undoneMove + "'", false);
		} else if (twice) {
			// Remove the double modifier, leaving the rest of the move intact, and perform the opposite move.
			if (prime) {
				processMove(letter);
				undoneMove.erase(2);
			} else {
				doMoves(undoneMove + "'", false);
				undoneMove.erase(1);
			}

			currentMoves.push(undoneMove);
		} else { // A prime move is canceled out with a normal move
			processMove(letter);
		}

		totalMoves--;
		cout << "\nUndid " << undoneMove << ".\n";
	} else {
		cout << "\nNo current moves to undo.\n";
	}
}

void Cube::reset() {
	createSolved();
	scramble = "";
	moves = "";
	totalMoves = 0;
}