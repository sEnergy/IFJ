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

/* All functions return error codes to analyzer */

// Changes id and data to bool
int boolval(changeable_tokenPtr token);

// Changes id and data to integer
int intval(changeable_tokenPtr token);

// Changes id and data to double

int doubleval(changeable_tokenPtr token);

// Changes id and data to string
int strval(changeable_tokenPtr token);

// Changes id, data to int - In data will be length of string
int str_len(changeable_tokenPtr token);

// Changes id to string. In data will be new string from stdin.
int get_string(changeable_tokenPtr token);

// Changes id and data to int. Prints strings to stdin.
int put_string(int* arg_number, hashtable_item** hashtable, changeable_tokenPtr token);

// In data will be sorted string.
int sort_string(changeable_tokenPtr token);

// Creates new string from part of source string
int get_substring (changeable_tokenPtr token);

// Searches for substring in string
int find_string (changeable_tokenPtr token);

#endif

/*** End of file functions.h ***/
