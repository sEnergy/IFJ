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




// THIS FUNCTION AND STRUCTURE 
// WILL BE ERASED AFTER LUBOS COMMITS HIS WORK
// IT'S JUST FOR GCC TO SHUT UP
typedef struct changeable_token 
{
    unsigned int id;
    char* data;
    unsigned long long int position; /* needed for casting keywords */
} *changeable_tokenPtr;
int changeable_token_update(changeable_tokenPtr token, char * new_data);


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
