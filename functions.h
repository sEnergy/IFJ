/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            functions.h
 * Author:              Matúš Turic
 * Encoding:            UTF-8
 *
 * Description:         Header file  of built-in functions for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#ifndef IFJ_FUNCTIONS_INCLUDED
#define IFJ_FUNCTIONS_INCLUDED

#include "syntax_analyzer.h"
#include "interpreter.h"


// Changes id and data to bool
// Returns error code for anylyzer
int boolval(changeable_tokenPtr token);

// Changes id and data to integer
// Returns error code for anylyzer
int intval(changeable_tokenPtr token);

// Changes id and data to double
// Returns error code for anylyzer
int doubleval(changeable_tokenPtr token);

// Changes id and data to string
// Returns error code for anylyzer
int strval(changeable_tokenPtr token);

// Changes id, data to int - In data will be length of string
// Returns error code for anylyzer
int str_len(changeable_tokenPtr token);

// Changes id to string. In data will be new string from stdin. 
// Returns error code for anylyzer
int get_string(changeable_tokenPtr token);

// Changes id and data to int. Prints strings to stdin.
// Returns error code for anylyzer
int put_string(int* arg_number, changeable_tokenPtr token);

// In data will be sorted string.
// Returns error code for anylyzer
int sort_string(changeable_tokenPtr token);



#endif

					/*** End of file functions.h ***/
