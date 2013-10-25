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

#ifndef IFJ_ERRORS_INCLUDED
#define IFJ_ERRORS_INCLUDED

#define IFJ_ERR_LEXICAL 1 // error during lexical analysis
#define IFJ_ERR_SYNTAX 2 // error during syntax analysis
#define IFJ_ERR_UNDEF_REDEF 3 // undefined function / function redefinition
#define IFJ_ERR_MISSING_PARAMETER 4
#define IFJ_ERR_UNDECLARED_VARIABLE 5

#define IFJ_ERR_ZERO_DIVISION 10
#define IFJ_ERR_RETYPE 11 // for function doubleval()
#define IFJ_ERR_TYPE_COMPATIBILITY 12
#define IFJ_ERR_OTHER_RUNTIME 13

#define IFJ_ERR_INTERNAL 99 // malloc error etc.

#endif

/*** End of file errors.h ***/
