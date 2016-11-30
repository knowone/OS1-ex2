
/**
* File: ex2.c
Double Soduku solver
====================
Written by: Omer Schwartz - id: 201234002, login: omersc and
Matan Bachar -  id: 203167333, login: matanba

This program receives data from the user, as sets of 3 integers representing the Soduku board
he wants to solve.
The first integer represents the row number of the cell he wants to fill, the second integer
represents the column and the third integer is the content of that certain cell.
To finish the input the user will be required to write -1.

The program will create two processes that will solve the Soduku board in two different ways.
The first process will solve the board as we solved it last year, in a simple backtracking recursion.
The second child will solve the board while using a priority (markup) table that updates and chooses
for each iteration the highest priority cell.

As a result, the program will print the solved Soduko board 2 times, each process will print his solution,
and then each process will print it's runtime (in seconds) to get to the solution.
*/


//============================================================================//
//                             Include Section
//============================================================================//

#include "ex2.h"

//============================================================================//
//                           Function Implementation
//============================================================================//

/****************************************************************************
* Checks if the value of digit is allowed in the current square of the
* associated cell positioned in (currRow, currCol).
* Returns true if no cell in the associated square is equal to digit.
* False, otherwise.
*
* *************************************************************************/
bool isAllowSqr(Board* board, int currRow, int currCol, int digit) {

	bool found = false;
	int i, j, startRowPos, startColPos;
	startRowPos = (currRow / N)*N;            //get the first cell of the relevant square
	startColPos = (currCol / N)*N;            //if N=3, then we get either 0*3=0, 1*3=3, or 2*3=6
											  //which are the starting position of the square (row&col)

	for (i = startRowPos; i < startRowPos + N; i++) {
		for (j = startColPos; j < startColPos + N; j++) {
			if (board->_board[i][j]._content == digit) {     //check all cells in the square
				return found;                               //return when found
			}
		}
	}
	return !found;                                          //return not found (isAllowed=true)
}


/***************************************************************************
* Check if the value of digit is allowed in the current column and row of
* the cell, positioned in (currRow, currCol).
* Returns true if no cell in the associated row or column is equal to digit.
* False if otherwise.
* *************************************************************************/
bool isAllowRowCol(Board* board, int currRow, int currCol, int digit) {
	bool found = false;
	int i;
	for (i = 0; i < DIGITS; i++) {         //go across the row and column to look for digit
		if (board->_board[currRow][i]._content == digit || board->_board[i][currCol]._content == digit) {
			return found;
		}
	}
	return !(found);
}


bool isAllowed(Board* board, int currRow, int currCol, int digit) {
	//true if didn't find in the square and didn't find in the row and column
	return isAllowSqr(board, currRow, currCol, digit) && isAllowRowCol(board, currRow, currCol, digit);
}



void printBoard(Board* board) {
	int i, j;
	for (i = 0; i < DIGITS; ++i) {
		for (j = 0; j < DIGITS; ++j) {
			//print line
			printf("%d ", board->_board[i][j]._content);
		}                               //separate the lines with newline
		printf("\n");
	}
	putchar('\n');                      //flush the buffer, end with newline
}


Board copyBoard(Board* b) {
	Board newBoard;                //create empty struct
	int i, j, k;
	for (i = 0; i < DIGITS; ++i) {
		for (j = 0; j < DIGITS; ++j) {
			//copy the cell's content and priority
			newBoard._board[i][j]._content = b->_board[i][j]._content;
			newBoard._board[i][j]._priority = b->_board[i][j]._priority;
			for (k = 0; k < DIGITS; ++k) {
				//copy each cell's markup table
				newBoard._board[i][j]._markupTable[k] = b->_board[i][j]._markupTable[k];
			}

		}
	}
	return newBoard;
}

char* getBoardAsStringFromUser() {
	int size = 2;
	int pos = 0;
	char* output = (char*)malloc(sizeof(char*)*size);
	char c;
	printf("Input your data:\n");
	while ((c = getchar()) != '-') {
		output[pos] = c;
		pos++;
		size++;
		output = (char*)realloc(output, sizeof(char)*size);
	}
	if (c == '-') {
		output[pos] = c;
		size += 2;
		output = (char*)realloc(output, sizeof(char)*size);
		output[pos + 1] = getchar(); //should be 1 but no worries
		output[pos + 2] = '\0';		//null terminated string
		getchar();
	}

	return output;

}

Board convertToBoardFromCharArr(char* arr) {
	int i, j, k;
	Board board = {};                               //Create empty struct
	for (i = 0; i < DIGITS; ++i) {              //"Zero-out" the Board
		for (j = 0; j < DIGITS; ++j) {
			board._board[i][j]._content = 0;
			for (k = 0; k < DIGITS; ++k) {
				board._board[i][j]._markupTable[k] = POSSIBLE;  //make all cells possible
			}
			board._board[i][j]._priority = DIGITS;  //all cells have minimum priority
		}
	}
	//i = 1;
	int rowPos;
	int colPos;
	int digit;
	char* nextToken = NULL;
	char* seperators = " \n";
	char* token = strtok_s(arr, seperators, &nextToken);
	while (token != NULL && atoi(token) != -1) {
		rowPos = atoi(token);
		if ((token = strtok_s(NULL, seperators, &nextToken)) != NULL) {
			colPos = atoi(token);
			if ((token = strtok_s(NULL, seperators, &nextToken)) != NULL) {
				digit = atoi(token);

				board._board[rowPos][colPos]._content = digit;
				board._board[rowPos][colPos]._priority = 0;     //Cell is in use. Has no priority.
				updateMarkup(&board, rowPos, colPos, digit);    //Update all relevant cells affected by the cell's
				token = strtok_s(NULL, seperators, &nextToken);
			}
			else {
				//failed to find next token
			}
		}
		else {
			//failed to find next token
		}
	}
	if (token == NULL){
		//failed, go to end of array without -1
	}
	return board;
}

bool updateMarkup(Board* board, int row, int col, int digit) {
	bool result = SUCCEEDED;
	int i;
	/* **********************************************************************************************
	* All three blocks has the same logic:
	* if the cell is empty, we check if the desired
	* input (digit, that we want to put in, but still didn't)
	* will not violate the following cells (in the row, the column
	* and the square).
	* If it is not allowed, AND it was not previously marked impossible
	* then we mark that cell as impossible (in it's markup table).
	* If it is allowed, AND it was marked as impossible (due to old attempts)
	* then we mark that cell as possible.
	* When we take out a possibility we check if we got to a dead end
	* by checking if that (empty) cell's priority is 0, meaning
	* there are no more options for a cell content and this is indeed
	* a dead end.
	* ***********************************************************************************************/
	for (i = 0; i < DIGITS; i++) {
		if (board->_board[row][i]._content == 0) {
			//checking the cell's associated row cells
			if (!isAllowed(board, row, i, digit)) {
				if (board->_board[row][i]._markupTable[digit - 1] != IMPOSSIBLE) {
					board->_board[row][i]._markupTable[digit - 1] = IMPOSSIBLE;
					board->_board[row][i]._priority--;
					if (board->_board[row][i]._priority == 0) {
						result = FAILED;
					}
				}
			}
			else {
				if (board->_board[row][i]._markupTable[digit - 1] != POSSIBLE) {
					board->_board[row][i]._markupTable[digit - 1] = POSSIBLE;
					board->_board[row][i]._priority++;
				}
			}
		}
		//checking the cell's associated column cells
		if (board->_board[i][col]._content == 0) {
			if (!isAllowed(board, i, col, digit)) {
				if (board->_board[i][col]._markupTable[digit - 1] != IMPOSSIBLE) {
					board->_board[i][col]._markupTable[digit - 1] = IMPOSSIBLE;
					board->_board[i][col]._priority--;
					if (board->_board[i][col]._priority == 0) {
						result = FAILED;
					}
				}
			}
			else {
				if (board->_board[i][col]._markupTable[digit - 1] != POSSIBLE) {
					board->_board[i][col]._markupTable[digit - 1] = POSSIBLE;
					board->_board[i][col]._priority++;
				}
			}
		}
	}
	//same technique used in isAllowed to check the cell's associated square
	int j, startRowPos, startColPos;
	startRowPos = (row / N)*N;
	startColPos = (col / N)*N;

	for (i = startRowPos; i < startRowPos + N; i++) {
		for (j = startColPos; j < startColPos + N; j++) {
			if (board->_board[i][j]._content == 0) {
				if (!isAllowed(board, i, j, digit)) {
					if (board->_board[i][j]._markupTable[digit - 1] != IMPOSSIBLE) {
						board->_board[i][j]._markupTable[digit - 1] = IMPOSSIBLE;
						board->_board[i][j]._priority--;
						if (board->_board[i][j]._priority == 0) {
							result = FAILED;
						}
					}
				}
				else {
					if (board->_board[i][j]._markupTable[digit - 1] != POSSIBLE) {
						board->_board[i][j]._markupTable[digit - 1] = POSSIBLE;
						board->_board[i][j]._priority++;
					}
				}
			}
		}
	}
	return result;  //false if found a cell without options
}

bool findHighestPriority(Board* board, int* row, int* col) {

	int minPriority = DIGITS + 1;        //set to the lowest priority (the lower the better)
	int i, j;
	for (i = 0; i < DIGITS; ++i) {
		for (j = 0; j < DIGITS; ++j) {
			if (board->_board[i][j]._content == 0) {     //if cell is empty

														 //if current cell has a lower priority than the previous cell
				if (board->_board[i][j]._priority < minPriority && board->_board[i][j]._priority > 0) {
					minPriority = board->_board[i][j]._priority;    //replace the minimum
					*row = i;                                       //update the newly-found cell's location
					*col = j;                                       //(row,col)
				}
				if (minPriority == 1) {                              //immediately stop if found an only-1-option cell
					return SUCCEEDED;
				}
			}
		}
	}
	if (minPriority == DIGITS + 1) {  //no more cells are left to fulfill - meaning the board is finished
		return FINISHED_BOARD;              //this is equals to false
	}
	else {
		return SUCCEEDED;
	}

}

bool solveA(Board *board, int row, int col) {
	/* Root condition - SUCCEEDEDfully ending the algorithm.
	* When all the cells of the board have been handled*/
	if (col == DIGITS && row == DIGITS - 1) {  //row is the last row in array, col the the one after the last
											   //indicating there are no more cells left
		return SUCCEEDED;
	}
	int i;
	bool result;

	/* If there is no value assigned to the cell: */
	if (row < DIGITS && col < DIGITS && board->_board[row][col]._content == 0) {

		/* Try for each digit all combinations from the next column  */
		for (i = 1; i <= DIGITS; i++) {
			if (isAllowed(board, row, col, i)) { //isAllowed checks for column, row and square assignment options
				board->_board[row][col]._content = i;   //assign the value if allowed
				result = solveA(board, row, col + 1);   //go to next column
				if (result) {                        //If SUCCEEDEDful then a solution was found
					return SUCCEEDED;
				}
			}
		}

		/* If all digits are not allowed or do not lead to solution,
		* zero the assigned cell and return FAILED                     */
		board->_board[row][col]._content = 0;
		return FAILED;
	}
	else {                                   //Non zero cell is reached
		if (col >= DIGITS) {
			return solveA(board, row + 1, 0);   //Go to next row if column is last column
		}
		return solveA(board, row, col + 1);     //otherwise go to next column
	}
}

bool solveB(Board* board, int row, int col) {

	bool result;
	int i;
	/* Root condition - no more cells to fulfill
	* If all cells have 0 priority and non empty content */
	if (!findHighestPriority(board, &row, &col)) {
		//updates to the new row and col and if returns false we finished the board
		return SUCCEEDED;
	}

	//Run through the high-priority cell's markup table (option list)
	for (i = 1; i <= DIGITS; ++i) {
		//ignore all impossible values
		if (board->_board[row][col]._markupTable[i - 1] == POSSIBLE) {
			//check if digit i is allowed with the other associated cells
			if (isAllowed(board, row, col, i)) {
				//if allowed, update the associated cells markup table (option list)
				if (updateMarkup(board, row, col, i)) {
					//set the cell's content
					board->_board[row][col]._content = i;
					//Try to fulfill the next high-priority cell.
					result = solveB(board, row, col);
					//If unSUCCEEDEDful reset what was done and try next markup option
					if (result) {
						//if SUCCEEDEDfull - a solution was found - return true
						return SUCCEEDED;
					}
					else {

						board->_board[row][col]._content = 0;
						updateMarkup(board, row, col, i);

					}
				}
			}
		}
	}
	//No solution path was found, return back
	return FAILED;
}

bool startChild(Board* b, SOLUTION s) {
	bool result;
	time_t start = time(NULL);      //Get the initial starting time
	switch (s) {
	case SOLUTION_A:            //First simple backtracking implementation
		result = solveA(b, 0, 0);
		break;
	case SOLUTION_B:            //Second markup backtracking implementation
		result = solveB(b, 0, 0);
		break;

	default:                   //case a wrong selection was made
		printf("No selection was made");
		result = false;
		break;
	}
	time_t end = time(NULL);        //Get the finishing time
	time_t lapse = end - start;     //Calculates the elapsed time
									//print to user the running time
	printf("Child %d took %f seconds to run\n", s + 1, (double)lapse);
	return result;
}

int main() {
	//int i;
	char* board = getBoardAsStringFromUser();  //Create a user with user input
	Board b = convertToBoardFromCharArr(board);
	printBoard(&b);
	
	int status_childA, status_childB;
	
	status_childA = _spawnl(_P_WAIT, "solveA.exe", "solveA.exe", board);
	status_childB = _spawnl(_P_WAIT, "solveB.exe", "solveB.exe", board);

	return 0;
}