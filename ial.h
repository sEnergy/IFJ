/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            ial.h
 * Author:              Marcel Fiala
 *                         Luboš Vaníček
 *                      Ivan Straka
 * Encoding:            UTF-8
 *
 * Description:         Header file for search of substring in string.
 *
*******************************************************************************/

#ifndef IFJ_IAL_INCLUDED
#define IFJ_IAL_INCLUDED

/*
 * the bigger is the size, the less collisions we will possibly have
 */
#define SIZE_OF_HASHTABLE 1000


/*
 * Deffinitions of used structures
 */ 

typedef struct htable_item 
{
    int type;
    struct htable_item* next;
    char* value;
    char* string;
} hashtable_item;

/*
 * Function prototypes
 */
unsigned int hash_function(const char *string);
hashtable_item** hashtable_init (void);
int insert_item_hashtable (hashtable_item** hashtable, char* string, int type, char* value);
hashtable_item* create_item_hashtable (char* string, int type, char* value);
hashtable_item* search_hashtable (hashtable_item** hashtable, char* string);
void hashtable_free (hashtable_item** hashtable);
hashtable_item* search_hashtable (hashtable_item** hashtable, char* string);
void clear_hashtable (hashtable_item** hashtable);
hashtable_item** copy_hashtable (hashtable_item** hashtable);

int* build_table (char* substring);
int search_substring(char* string, char* substring, int* found);

char *shell_sort(char array[], int size);

#endif

/*** End of file ial.h ***/
