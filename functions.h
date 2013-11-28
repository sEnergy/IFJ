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





#endif

				/*** LONG TIME AGO IN A GALAXY FAR, FAR AWAY ... ***/
						/*** THIS IFJ13 SHIT WAS BORN ***/
