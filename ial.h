/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            ial.h
 * Author:              Marcel Fiala
 *                      Ivan Straka
 * Encoding:            UTF-8
 *
 * Description:         Header file for search of substring in string.
 *
*******************************************************************************/

#ifndef IFJ_IAL_INCLUDED
#define IFJ_IAL_INCLUDED

int* build_table (char* substring);
int search_substring(char* substring, char* string, int *found);

#endif

/*** End of file ial.h ***/
