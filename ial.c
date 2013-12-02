/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            ial.c
 * Author:              Marcel Fiala
 *                      Luboš Vaníček
 *                      Ivan Straka
 * Encoding:            UTF-8
 *
 * Description:         Implementation for search of substring in string.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "ial.h"

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
int search_substring(char* substring, char* string, int* found)
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

/**********************************************
 *                    HASHTABLE
 **********************************************/

/*
 * Creates an index into the hashtable.
 */
 
unsigned int hash_function(const char *string) 
{
    unsigned int result = 0;
    while (*string)
    {
        result = 31 * result + *string++;    
    }
    return (result % SIZE_OF_HASHTABLE);
}

/*
 * Creates an empty hashtable and returns a pointer to it.
 * If the allocation fails, it returns NULL instead.
*/
hashtable_item** hashtable_init (void)
{
    hashtable_item** returned_pointer;
    /* allocation for pointer to number(size_of_hashtable) pointers */
    returned_pointer = (hashtable_item **) malloc (SIZE_OF_HASHTABLE * sizeof(hashtable_item*));
  
    if (returned_pointer == NULL)
    {
        return NULL;
    }
    
    for (unsigned int i = 0; i < SIZE_OF_HASHTABLE; ++i)
    {
        returned_pointer[i] = NULL;
    }
  
    return returned_pointer;
}

/*
 * Frees the whole table. 
 */ 
void hashtable_free (hashtable_item** hashtable)
{
    clear_hashtable (hashtable);
    free(hashtable);
    hashtable = NULL;
}

/*
 * Inserts an item into the table, if the table already contains the item 
 * with the same string, this function changes the value and/or type.
 * If an error occures, function returns non-zero.
 */
int insert_item_hashtable (hashtable_item** hashtable, char* string, int type, char* value)
{ 
    hashtable_item* searched_item = search_hashtable (hashtable, string);
    /* Haven't found, have to insert */
    if (searched_item == NULL)
    {
        /* 
         * @collision_item - the item, that is already in the table on 
         * the same @index as the @searched item
         */
        searched_item = create_item_hashtable (string, type, value);
        
        /* Failed allocation*/
        if (searched_item == NULL)
        {
            return 1;
        }
        unsigned int index = hash_function (string);
        hashtable_item* collision_item = hashtable[index];
        searched_item->next = collision_item;
        hashtable[index] = searched_item;
    }
    /* Found - actualize data */
    else
    {
        searched_item->type = type;
        searched_item->value = value;
    }
    return 0;
}

/*
 * Creates a new item and returns a pointer to it.
 * When problem with allocation happens it returns NULL.
 */
hashtable_item* create_item_hashtable (char* string, int type, char* value)
{
    /* Allocation */
    hashtable_item* pointer_to_item = (hashtable_item*) malloc (sizeof(hashtable_item));
    
    if (pointer_to_item == NULL)
    {
        return NULL;
    }
    
    /* Adding data */
    pointer_to_item->string = string;
    pointer_to_item->type = type;
    pointer_to_item->value = value;
    pointer_to_item->next = NULL;
    
    return pointer_to_item;
}

/*
 * Searches for an item in hashtable, if the item is found it returns
 * pointer to the item, otherwise it returns NULL.
 */
hashtable_item* search_hashtable (hashtable_item** hashtable, char* string)
{
    /* using the hash function to get an index to the hashtable */
    unsigned int index = hash_function (string);
    
    hashtable_item* searched_item = hashtable[index];
    int found = false;
    
    while (searched_item != NULL)
    {
        if (!strcmp(hashtable[index]->string, string))
        {
            found = true;
            break; 
        }
        else
        {
            searched_item = searched_item->next;
        }
    }
    
    if (found)
    {
        return searched_item;
    }
    else
    {
        return NULL;
    }
}

/*
 * Cleares the whole table, it will be the same as after initialization
 */
void clear_hashtable (hashtable_item** hashtable)
{
    hashtable_item* item;
    for (unsigned int i = 0; i < SIZE_OF_HASHTABLE; ++i)
    {
        while ((item = hashtable[i]) != NULL)
        {
            hashtable[i] = item->next;
            free(item);
            item = NULL;
        }
    }
}

/*
* Copies a hashtable and returns pointer to the created one.
* If an error happens, it returns NULL instead.
* The items in copied hashtable are in reversed order (better effectivity),
* it doesn't chance its purpose.
*/ 
hashtable_item** copy_hashtable (hashtable_item** hashtable)
{
    hashtable_item* item;
    hashtable_item* created_item;   
    hashtable_item** created_hashtable = hashtable_init();
    
    /* Allocation error */
    if (created_hashtable == NULL)
    {
        return NULL;
    }
    for (unsigned int i = 0; i < SIZE_OF_HASHTABLE; ++i)
    {
        item = hashtable[i];
        while (item != NULL)
        {
            created_item = create_item_hashtable (item->string, item->type, item->value);
            
            /* Allocation error */
            if (created_item == NULL)
            {
                hashtable_free(created_hashtable);
                return NULL;
            }
            /* Adding new items as first ones -> reversed order */
            created_item->next = created_hashtable[i];
            created_hashtable[i] = created_item;          
            item = item->next;
        }
    }
    return created_hashtable;
}


/**********************************************************************
*                            MERGE SORT                               *
***********************************************************************/

// merge sort   - divide and conquer algorithm
//              - sorting chars in string (ordinal value - lowest to highest)
// string_array - string to sort
// output       - temporary output array
// first        - starting index
// last         - ending index
// output overwrites string_array

void merge_sort(char string_array[], char output[], int first, int last)
{
    if(first == last) return;
    int middle = (first + last) / 2;
    merge_sort(string_array, output, first, middle);
    merge_sort(string_array, output, middle + 1, last);
    merge(string_array, output, first, last);

    for(int i = first; i<=last;i++)
    {
        string_array[i]=output[i];
    }
}

// merge        - merging divided lists
//              - lists will be sorted
// string_array - string to sort
// output       - temporary output array
// first        - starting index
// last         - ending index

void merge(char string_array[], char output[], int first, int last)
{
    int middle = (first + last) / 2;
    int first_index = first;
    int last_index = middle + 1;
    int out_index = first;

    while(first_index <= middle && last_index <= last)
    {
        if(string_array[first_index] >= string_array[last_index])
        {
            output[out_index] = string_array[last_index++];
        }
        else
        {
            output[out_index] = string_array[first_index++];
        }
        out_index++;
    }

    while(last_index <= last)
    {
        output[out_index] = string_array[last_index++];
        out_index++;
    }

    while(first_index <= middle)
    {
        output[out_index] = string_array[first_index++];
        out_index++;
    }
}

/*** End of file ial.c ***/
