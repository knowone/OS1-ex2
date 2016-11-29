#pragma once
/**
* File: ex2.h
Double Soduku solver
====================
Written by: Omer Schwartz - id: 201234002, login: omersc and
Matan Bachar -  id: 203167333, login: matanba
*/

#ifndef EX2_EX2_H
#define EX2_EX2_H

#endif //EX2_EX2_H

//============================================================================//
//                             Include Section
//============================================================================//
#include <Windows.h>
#include <process.h>
#include <iostream>
#include <stdbool.h>
#include <time.h>

//============================================================================//
//                             Define Section
//============================================================================//

#define N 3             //The size of root (Complexity is an exponential N^3)
#define DIGITS N*N      //How many digits are allowed or the size of each row
						//in the board
#define FAILED false
#define SUCCESS true
#define POSSIBLE 1
#define IMPOSSIBLE 0
#define FINISHED_BOARD false
#define MAX_CELL_INPUT DIGITS*DIGITS+1
#define USER_INPUT_LEN 3 
#define DO_NOT true
#define ME_NOW false

/******************************************************************************
* Solution enum for startChild - each solution is a different implementation
* of a Sudoku solving algorithm.
*******************************************************************************/
typedef enum { SOLUTION_A, SOLUTION_B } SOLUTION;

/******************************************************************************
* Defining a struct to represent the content of a cell, made up of content and the
* markup table to keep the available cells in, and the priority indicator
*****************************************************************************/
typedef struct {
	int _content;
	int _priority;
	int _markupTable[DIGITS];
} Cell;

/*****************************************************************************
* Defining a board that represents an array of arrays of Cell struct to
* represent the Sudoku board.
*****************************************************************************/
typedef struct {
	Cell _board[DIGITS][DIGITS];
} Board;


//============================================================================//
//                           Function Declaration
//============================================================================//

/**************************************************************************
* isAllowed checks if no Sudoku rule is violated when inserting the
* value of @param digit into the cell at (currRow, currCol) in the
* current Sudoku board.
*
* @param board the Board struct containing a legal DIGITSxDIGITS size board
* @param currRow the target cell's row position (0..DIGITS-1)
* @param currCol the target cell's column position (0..DIGITS-1)
* The position values are not checked and may throw an error
* @param digit the value to be checked if allowd in position (currRow, currCol).
*
* @return true if digit is an allowed move, or false if digit will violate
* a Sudoku rule.
* *************************************************************************/
bool isAllowed(Board* board, int currRow, int currCol, int digit);


/**************************************************************************
* printBoard - simply prints the Sudoku game board.
* @param board the Board struct to print.
*
* *************************************************************************/
void printBoard(Board* board);


/**************************************************************************
* copyBoard returns a copy of the given board.
* Copy-by-value each of the struct's fields and returns the copied struct.
*
* @param b a Board struct to by copied by value.
* @return a copy of the original Board struct.
* *************************************************************************/
Board copyBoard(Board* b);


/***************************************************************************
* getBoardFromUser receives a valid Sudoku board details and creates a valid Board with the
* relevant fields.
* User inputs a row, a column and a cell value, three numbers at a time for
* each cell. Once done the user inputs to stdin -1 to indicate the suer was
* finished.
* Function returns a Board struct after it had calculated the markup for the
* input cells and the relevnat other cells and zeroed all other cells.
* @return Board struct with user input.
****************************************************************************/
Board getBoardFromUser();


/*****************************************************************************
* updateMarkup calculates the markup for the cells associated with the cell at position
* (row, col) with the value of digit.
* Markup is the possible values a valid sudoku cell can have. If digit is
* put to the cell, the associated row, column and square cannot contain the
* same digit, and thus removed from the markup table.
* @param board the currently built sudoku board
* @param row the digit's row position.
* @param col the digit's column position
* @return true if the update returned a valid board, or false if the update
* caused the board to become invalid - meaning a cell has no digit and no
* options to insert a digit to it.
*****************************************************************************/
bool updateMarkup(Board* board, int row, int col, int digit);


/*****************************************************************************
* findHighestPriority searches the board for a cell with the highest priority and update the values
* of @param row and @param col (using pointer dereference) to the current
* position of that cell.
* A priority is considered high when the cell has the LEAST possible options
* to accept a digit, thus is more likely that option is the correct option.
* The priority counter counts the possibilities a cell can have, meaning
* Lower is Better.
*
* @param board the board pointer to search it's cells.
* @param row the position reference that will be changed to the highest
* priority cell's row position.
* @param col the position reference that will be changed to the highest
* priority cell's column position.
*
* @return true if a high priority cell was found.
*  false if no cell with priority was found. Returning false means there are
*  no options left to position a digit in the board, which in turn means the
*  board is either complete or is wrong.
*  In our program, if false is returned the board is (necessarily) solved.
*****************************************************************************/
bool findHighestPriority(Board* board, int* row, int* col);


/******************************************************************************
* SolveA uses a backtracking algorithm to iterate over all possible solutions
* to the Sudoku board and stops when any solution was reached.
* The board is filled with the correct solution and the boolean true value is
* returned to indicate a solution was found.
* Otherwise, the board stays untouched and a false value is returned.
*
* @param board a partial or empty Board Struct to find its solution
* @param row the starting row position - used for the recursion. Must begin with 0.
* @param col the starting column position, same as row. Must begin with 0.
*
* @return true if solution was found. False otherwise.
*
*****************************************************************************/
bool solveA(Board *board, int row, int col);


/*****************************************************************************
* SolveB uses a markup table to track the current possibilities for each cell
* on the board, and each iteration chooses to solve the cell with the least
* amount of options, and also skips the unavailable options for a cell thus
* saving time and operations to perform.
* This is an improved version of SolveA.
*
* @param board a partial or empty Board Struct to find its solution
* @param row the starting row position - used for the recursion. Must begin with 0.
* @param col the starting column position, same as row. Must begin with 0.
*
* @return true if solution was found. False otherwise.
*****************************************************************************/
bool solveB(Board *b, int row, int col);


/*****************************************************************************
* startChild starts an algorithm solution
*
* @param board a partial or empty Board Struct to find its solution
* @param solution choose between the 2 possible solutions:
*  SOLUTION_A - backtracking algorithm
*  SOLUTION_B - markup-improved backtracking algorithm
*
* @return true if algorithm chosen returned true. False otherwise.
*****************************************************************************/
bool startChild(Board* b, SOLUTION s);

char** getBoardAsStringFromUser();

Board convertToBoardFromCharArr(char** arr);