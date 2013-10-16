/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            errors.h
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Definition of error codes.
 *
*******************************************************************************/

#define ERR_LEXICAL 1 // error during lexical analysis
#define ERR_SYNTAX 2 // error during syntax analysis
#define ERR_UNDEF_REDEF 3 // undefined function / function redefinition
#define ERR_MISSING_PARAMETER 4
#define ERR_UNDECLARED_VARIABLE 5

#define ERR_ZERO_DIVISION 10
#define ERR_RETYPE 11 // for function doubleval()
#define ERR_TYPE_COMPATIBILITY 12
#define ERR_OTHER_RUNTIME 13

#define ERR_INTERNAL 99 // malloc error etc.

/*** End of file errors.h ***/
