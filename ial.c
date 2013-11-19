/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            ial.c
 * Author:              Marcel Fiala
 *                      Ivan Straka
 * Encoding:            UTF-8
 *
 * Description:         Implementation for search of substring in string.
 *
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "errors.h"

/*
 * Examines substring for repeating patterns and creates table accordingly.
 * Values from table are used as "fallback" for search index, after first match
 * is corrupted.
 */
int* build_table (char* substring)
{
    unsigned int subtring_lenght = strlen(substring);
    unsigned int index = 2, match = 0;
    int *table = malloc(subtring_lenght * sizeof(int));

    if (table == NULL)
    {
        return NULL;
    }

    table[0] = -1; table[1] = 0;

    while(index < subtring_lenght)
    {
        if(substring[index-1] == substring[match]) /* match in substring */
        {
            table[index++] = ++match;
        }
        else if(match > 0)
        {
            match = table[match]; /* end of match */
        }
        else
        {
            table[index++] = 0;  /* na match */
        }
    }

    return table;
}

/*
 * Searches for substring. If substring is found, its index within string is
 * stored in variable found. In any other case, found is set to -1.
 */
int search_substring(char* substring, char* string, int *found)
{
    int* table = build_table(substring); /* creation of table - look above */

    if (table == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }


    unsigned int index = 0, match = 0;
    unsigned int substring_lenght = (strlen(substring));
    unsigned int string_lenght = (strlen(string));

    while (index+match < string_lenght) /* while there is chance of finding */
    {
        if(substring[index] == string[match+index]) /* while matching */
        {
            if (index == substring_lenght-1)
            {
               free(table);
               *found = match; /* complete match */
               return EXIT_SUCCESS;
            }
            else
            {
                ++index; /* part match */
            }
        }
        else /*
              * match corrupted -> look into table for anothed index (where to
              * start another search)
              */
        {
            match += index - table[index];
            index = (table[index] > -1)? table[index]:0 ;
        }
    }

    free(table);
    *found = -1; /* no match found */
    return EXIT_SUCCESS;
}

/*** End of file ial.c ***/
