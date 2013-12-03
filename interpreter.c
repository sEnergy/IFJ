/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            Interpreter.c
 * Author:              Lubo¹ Vaníèek
 * Encoding:            UTF-8
 *
 * Description:         Source file of Interpreter.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#include "errors.h"
#include "token_list.h"
#include "token_id.h"
#include "syntax_analyzer.h"
#include "ial.h"
#include "interpreter.h"
#include "functions.h"
#include "token_list.h"

/*
 * GLOBAL VARIABLE: to free all changeable_tokens at once
 */
changeable_tokenPtr first_changeable_token = NULL;

//DEBUGGING - 0 OFF, 1 SMALL INFO, 2 LARGE INFO
int DEBUGGING = 0;

/****************************************************
 *              FUNCTION HASHTABLE
 *      remake of hashtable for variables
 ****************************************************/

/*
 * Creates an empty function_hashtable and returns a pointer to it.
 * If the allocation fails, it returns NULL instead.
*/
function_hashtablePtr* function_hashtable_init (void)
{
    function_hashtablePtr* returned_pointer;
    /* allocation for pointer to number(size_of_hashtable) pointers */
    returned_pointer = (function_hashtablePtr*) malloc (SIZE_OF_HASHTABLE * sizeof(function_hashtablePtr));

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
 *  If it's not there - insert, if it is - redefinition error
 */

int function_hashtable_insert (function_hashtablePtr* function_hashtable, TokenPtr function_token,  BUFFER_STRUCT buffer)
{
    if (DEBUGGING) printf("HT-insert\n");

    int error = 0;
    char* string = &(buffer->data[function_token->content]);
    if (DEBUGGING == 2) printf ("nazev vkladane funkce: \"%s\"\n", string);
    function_hashtablePtr searched_item = function_hashtable_search (function_hashtable, string);
    /* Haven't found, have to insert */
    if (DEBUGGING == 2) printf("HT-insert1\n");
    if (searched_item == NULL)
    {
        if (strcmp(string, "boolval") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "intval") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "strval") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "doubleval") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "strlen") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "get_string") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "put_string") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "sort_string") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "get_substring") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else if (strcmp(string, "find_string") == 0)
        {
            error = IFJ_ERR_UNDEF_REDEF;
        }
        else
        {
            if (DEBUGGING == 2) printf("HT-insert2\n");

            unsigned int index = hash_function (string);
            searched_item = malloc (sizeof(struct function_hash_table));
            searched_item->name = malloc(sizeof(strlen(string)) + 1);

            if (DEBUGGING == 2) printf("HT-insert2a\n");
            /* Failed allocation*/
            if (searched_item->name == NULL)
            {
                return IFJ_ERR_INTERNAL;
            }

            int i = 0;
            if (DEBUGGING == 2) printf("HT-insert2b\n");
            while (string[i] != '\0')
            {
                searched_item->name[i] = string[i];
                ++i;
            }
            if (DEBUGGING == 2) printf("HT-insert2c\n");
            searched_item->name[i] = '\0';
            searched_item->function_token = function_token;
            if (DEBUGGING == 2) printf("HT-insert2d\n");

            function_hashtablePtr collision_item = function_hashtable[index];
            searched_item->next = collision_item;
            function_hashtable[index] = searched_item;
            if (DEBUGGING == 2) printf("HT-insert2e\n");
        }
    }
    /* Found - ERROR - Function redefinition */
    else
    {
        if (DEBUGGING == 2) printf("HT-insert3\n");
        error = IFJ_ERR_UNDEF_REDEF;
    }

    if (DEBUGGING) printf("HT-insert -end\n");

    return error;
}

/*
 * Searches for an item in function_hashtable, if the item is found it returns
 * pointer to the item, otherwise it returns NULL.
 */
function_hashtablePtr function_hashtable_search (function_hashtablePtr* function_hashtable, char* string)
{
    /* using the hash function to get an index to the hashtable */
    unsigned int index = hash_function (string);

    function_hashtablePtr searched_item = function_hashtable[index];
    int found = false;

    while (searched_item != NULL)
    {
        if (!strcmp(function_hashtable[index]->name, string))
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
 * Cleares names in the whole table
 */
void function_hashtable_free (function_hashtablePtr* function_hashtable)
{
    function_hashtablePtr item;
    for (unsigned int i = 0; i < SIZE_OF_HASHTABLE; ++i)
    {
        while ((item = function_hashtable[i]) != NULL)
        {
            function_hashtable[i] = item->next;
            free(item->name);
            item->name = NULL;
            free(item);
            item = NULL;
        }
    }
    free(function_hashtable);
    function_hashtable = NULL;
}

/****************************************************
 *           CHANGEABLE TOKENS FUNCTIONS
 ****************************************************/

changeable_tokenPtr changeable_token_Init (void)
{
    if (DEBUGGING == 2) printf("token_init\n");

    changeable_tokenPtr New_token = malloc(sizeof(struct changeable_token));
    if (New_token == NULL)
    {
        return NULL;
    }
    New_token->next = first_changeable_token;
    New_token->data = NULL;
    first_changeable_token = New_token;

    if (DEBUGGING == 2) printf("token_init-end\n");
    return New_token;
}

/*
 * function changes data in @change_token, reallocates spaces if needed
 */
int changeable_token_update(changeable_tokenPtr change_token, char * new_data)
{
    if (DEBUGGING == 2) printf("token_update\n");
    if ((change_token->data == NULL) || strlen(change_token->data) < strlen(new_data))
    {
        if (change_token->data != NULL && (strlen(change_token->data) != 0))
        {
            free(change_token->data);
            change_token->data = NULL;
        }
        change_token->data = malloc (sizeof(char)* (strlen(new_data)+1));
        if(change_token->data == NULL)
        {
            return IFJ_ERR_INTERNAL;
        }
    }
    int i = 0;
    while (new_data[i] != '\0')
    {
        change_token->data[i] = new_data[i];
        ++i;
    }
    change_token->data[i] = '\0';

    if (DEBUGGING == 2) printf("token_update-end\n");
    return 0;
}

int changeable_token_Insert (changeable_tokenPtr change_token, TokenPtr token, BUFFER_STRUCT buffer)
{
    if (DEBUGGING == 2) printf("token_insert\n");
    change_token->id = token->id;
    if ((change_token->data = malloc(sizeof(char) * (strlen(&(buffer->data[token->content])) + 1))) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    int i = 0;
    while ((buffer->data[token->content + i]) != '\0')
    {
        change_token->data[i] = buffer->data[token->content + i];
        ++i;
    }
    change_token->data[i] = '\0';

    if (DEBUGGING == 2) printf("token_insert-end\n");
    return 0;
}

/*
 * Destroys all changeable tokens
 */
void changeable_token_Destroy (void)
{
    if (DEBUGGING == 2) printf("token_destroy\n");
    while (first_changeable_token != NULL)
    {
        changeable_tokenPtr destroyed_token = first_changeable_token;
        first_changeable_token = first_changeable_token->next;
        free(destroyed_token->data);
        destroyed_token->data = NULL;
        free(destroyed_token);
        destroyed_token = NULL;
    }
    if (DEBUGGING == 2) printf("token_destroy-end\n");
}

/****************************************************
 *              INTERPRETER FUNCTIONS
 ****************************************************/

/*
 * Chooses which function should be used, depends on token_id
 * This functions can only be in the root area
 */
int call_leaf_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("leaf: %d\n", token->id);

    /* error should be always changed */
    int error = IFJ_ERR_OTHER_RUNTIME;

    switch (token->id)
    {
        case IFJ_T_INT:             //10
            error = number_function (token, change_token, buffer);
            break;
        case IFJ_T_DOUBLE:          //10.0
            error = number_function (token, change_token, buffer);
            break;
        case IFJ_T_STRING:          //"10ahoj"
            error = number_function (token, change_token, buffer);
            break;
        case IFJ_T_VARIALBE:        //$ahoj
            error = var_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_MUL:             // '*'
            error = basic_operator_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_DIV:             // '/'
            error = basic_operator_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_PLUS:            // '+'
            error = basic_operator_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_MIN:             // '-'
            error = basic_operator_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_CONC:            // '.'
            error = concatenate_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_LESS:            // '<'
            error = boolean_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_GREATER:         // '>'
            error = boolean_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_LESS_EQUAL:      // '<='
            error = boolean_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_GREATER_EQUAL:   // '>='
            error = boolean_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_SUPER_EQUAL:     // '==='
            error = boolean_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_NOT_SUPER_EQUAL: // '!=='
            error = boolean_function (token, change_token, buffer, hashtable, function_hashtable);
            break;
        case IFJ_T_ID:              //function
            error = functions (token, change_token, buffer, hashtable, function_hashtable);
            break;
    }

    if (DEBUGGING) printf("leaf end: %d\n", token->id);
    return error;
}
/*
 * Function, that handles calling functions
 */
int functions (TokenPtr token, changeable_tokenPtr change_token,    
    BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("functions: %d\n", token->id);

    char* string = &(buffer->data[token->content]);

    if (DEBUGGING) printf("%s+++++++++++\n", string);

    int error = 0;
    /* BUILD IN FUNCTIONS */
    if (strcmp(string, "boolval") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 1);
        if (!error)
        {
            error = boolval(change_token);
        }
    }
    else if (strcmp(string, "intval") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 1);
        if (!error)
        {
            error = intval(change_token);
        }
    }
    else if (strcmp(string, "doubleval") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 1);
        if (!error)
        {
            error = doubleval(change_token);
        }
    }
    else if (strcmp(string, "strval") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 1);
        if (!error)
        {
            error = strval(change_token);
        }
    }
    else if (strcmp(string, "strlen") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 1);
        if (!error)
        {
            error = str_len(change_token);
        }
    }
    else if (strcmp(string, "get_string") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 0);
        if (!error)
        {
            error = get_string(change_token);
        }
    }
    else if (strcmp(string, "put_string") == 0)
    {
        int number_of_params = 0;
        error = check_params (token->condition, change_token, buffer, hashtable, -1);
        if (!error)
        {
            error = put_string(&number_of_params, change_token);
            if (!error)
            {
                int number_of_params_temp = number_of_params;
                int counter = 1;
                while ((number_of_params_temp /= 10) > 0)
                {
                    counter++;
                }
                char* new_data = malloc (sizeof(char) * (counter + 1));
                sprintf(new_data, "%d", number_of_params);
                change_token->id = IFJ_T_INT;
                error = changeable_token_update(change_token, new_data);
                free(new_data);
                new_data = NULL;
            }
        }
    }
    else if (strcmp(string, "sort_string") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 1);
        if (!error)
        {
            error = sort_string(change_token);
        }
    }
    else if (strcmp(string, "get_substring") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 3);
        if (!error)
        {
            error = get_substring(change_token);
        }
    }
    else if (strcmp(string, "find_string") == 0)
    {
        error = check_params (token->condition, change_token, buffer, hashtable, 2);
        if (!error)
        {
            error = find_string(change_token);
        }
    }
    /* NORMAL FUNCTIONS */
    else
    {
        if (DEBUGGING) printf("normal-functions : %d\n", token->id);
        
        //searching function in function hashtable
        function_hashtablePtr search = function_hashtable_search (function_hashtable, string);
        if (strcmp(search->name, string) == 0)
        {
            TokenPtr my_token = search->function_token->condition;
            int number_of_params = 0; //number of params in function
            while (my_token != NULL)
            {
                number_of_params++;
                my_token = my_token->next;
            }
            
            changeable_tokenPtr tmp_change_token = changeable_token_Init();
            if (change_token == NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            
            error = check_params (token->condition, tmp_change_token, buffer, hashtable, number_of_params);
            if (error)
            {
                return IFJ_ERR_MISSING_PARAMETER;
            }
            
            //creating new hashtable for inner function
            hashtable_item** new_hashtable = hashtable_init();
            if (hashtable == NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            
            my_token = search->function_token->condition;

            while (tmp_change_token != NULL)
            {
                if (insert_item_hashtable (new_hashtable, &(buffer->data[my_token->content]), 
                    tmp_change_token->id, tmp_change_token->data))
                {
                    hashtable_free(new_hashtable);
                    return IFJ_ERR_INTERNAL;
                }
                tmp_change_token = tmp_change_token->next_params;
                my_token = my_token->next;
            }
            
            my_token = search->function_token->LPtr;
            
            while (my_token != NULL && (my_token->id != IFJ_T_KEYWORD && 
                strcmp(&(buffer->data[my_token->content]), "return")))
            {
                if ((error = call_root_function (my_token, hashtable, buffer, function_hashtable)) != 0)
                {
                    hashtable_free(new_hashtable);
                    return error;
                }
                my_token = my_token->next;
            }
            
            if (my_token == NULL)
            {
                changeable_token_update(change_token, "null");
                change_token->id = IFJ_T_KEYWORD;
            }
            else //return
            {
                error = call_leaf_function(my_token->LPtr, change_token, buffer, new_hashtable, function_hashtable);
                change_token->id = IFJ_T_KEYWORD;
            }
            
            hashtable_free(new_hashtable);
        }
        else
        {
            return IFJ_ERR_UNDEF_REDEF;
        }
        if (DEBUGGING) printf("normal-functions-end : %d\n", token->id);
    }

    if (DEBUGGING) printf("functions end: %d\n", token->id);

    return error;
}

/*
 *  If number of params < 0, the number can be infinity
 */
int check_params (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, int number_of_params)
{
    if (DEBUGGING && token != NULL) printf("check_params: %d\n", token->id);
    else if (DEBUGGING) printf("check_params: nic\n");

    int error = 0;
    if (token == NULL && number_of_params == 0)
    {
        return error;
    }

    int first = 1; //if the changeable_token is the first one
    changeable_tokenPtr change_token_active = change_token;
    TokenPtr my_token = token; //so i dont destroy the original token

    while (!error && my_token != NULL && number_of_params != 0)
    {
        if (first)
        {
            switch (my_token->id)
            {
            case IFJ_T_INT:             //10
                error = number_function (my_token, change_token, buffer);
                break;
            case IFJ_T_DOUBLE:          //10.0
                error = number_function (my_token, change_token, buffer);
                break;
            case IFJ_T_STRING:          //"10ahoj"
                error = number_function (my_token, change_token, buffer);
                break;
            case IFJ_T_VARIALBE:        //$ahoj
                error = var_function (my_token, change_token, buffer, hashtable);
                break;
            default:
                error = IFJ_ERR_OTHER_RUNTIME;
                break;
            }
            change_token->next_params = NULL;
            first = 0;
        }
        else
        {
            changeable_tokenPtr change_token_new = changeable_token_Init();
            //allocation problem
            if (change_token_new == NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            change_token_new->next_params = NULL;
            change_token_active->next_params = change_token_new;
            switch (my_token->id)
            {
            case IFJ_T_INT:             //10
                error = number_function (my_token, change_token_new, buffer);
                break;
            case IFJ_T_DOUBLE:          //10.0
                error = number_function (my_token, change_token_new, buffer);
                break;
            case IFJ_T_STRING:          //"10ahoj"
                error = number_function (my_token, change_token_new, buffer);
                break;
            case IFJ_T_VARIALBE:        //$ahoj
                error = var_function (my_token, change_token_new, buffer, hashtable);
                break;
            default:
                error = IFJ_ERR_OTHER_RUNTIME;
                break;
            }
            change_token_active = change_token_new;
        }
        number_of_params--;
        my_token = my_token->next;
    }
    if (!error && number_of_params >= 0 && (number_of_params != 0 || my_token != NULL))
    {
        error = IFJ_ERR_MISSING_PARAMETER;
    }

    if (DEBUGGING) printf("check_params end: %d\n", token->id);

    return error;
}

int boolean_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("bool: %d\n", token->id);

    /* Allocating space for opperands */
    changeable_tokenPtr change_token_left;
    if ((change_token_left = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    changeable_tokenPtr change_token_right;
    if ((change_token_right = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    int error = call_leaf_function (token->LPtr, change_token_left, buffer, hashtable, function_hashtable);
    if (!error)
    {
        error = call_leaf_function (token->RPtr, change_token_right, buffer, hashtable, function_hashtable);
    }

    if (DEBUGGING == 2) printf("1bool: %d\n", token->id);

    if (!error)
    {
        int equal = 0;
        int less = 0;
        int greater = 0;
        int error = 0;
        //different token type
        if ((change_token_left->id != change_token_right->id))
        {
            error = IFJ_ERR_TYPE_COMPATIBILITY;
        }
        //same token type
        else
        {
            if (DEBUGGING == 2) printf("2bool: %d\n", token->id);

            if (change_token_left->id == IFJ_T_KEYWORD)
            {
                if (DEBUGGING == 2) printf("2abool: %d\n", token->id);
                // null
                if (strcmp(change_token_left->data, "null") == 0)
                {
                    if (strcmp(change_token_right->data, "null") == 0)
                    {
                        equal = 1;
                    }
                    else
                    {
                        error = IFJ_ERR_TYPE_COMPATIBILITY;
                    }
                }
                // boolean
                else if (strcmp(change_token_left->data, "true") == 0)
                {
                    if (DEBUGGING == 2) printf("2bbool: %d\n", token->id);

                    if (strcmp(change_token_right->data, "true") == 0)
                    {
                        equal = 1;
                    }
                    else if (strcmp(change_token_right->data, "false") == 0)
                    {
                        greater = 1;
                    }
                    else // different types
                    {
                        error = IFJ_ERR_TYPE_COMPATIBILITY;
                    }
                }
                else if (strcmp(change_token_left->data, "false") == 0)
                {
                    if (DEBUGGING == 2) printf("2cbool: %d\n", token->id);

                    if (strcmp(change_token_right->data, "false") == 0)
                    {
                        equal = 1;
                    }
                    else if (strcmp(change_token_right->data, "true") == 0)
                    {
                        less = 1;
                    }
                    else
                    {
                        error = IFJ_ERR_TYPE_COMPATIBILITY;
                    }
                }
                // different types
                else
                {
                    if (DEBUGGING == 2) printf("2dbool: %d\n", token->id);

                    error = IFJ_ERR_TYPE_COMPATIBILITY;
                }
            } // end of keyword
            //string
            else if (change_token_left->id == IFJ_T_STRING)
            {
                if (DEBUGGING == 2) printf("3bool: %d\n", token->id);

                int result = strcmp(change_token_left->data, change_token_right->data);
                if (result == 0)
                {
                    equal = 1;
                }
                else if (result > 0)
                {
                    greater = 1;
                }
                else
                {
                    less = 1;
                }
            }
            //int
            else if (change_token_left->id == IFJ_T_INT)
            {
                if (DEBUGGING == 2) printf("4bool: %d\n", token->id);

                int operand_1 = atoi(change_token_left->data);
                int operand_2 = atoi(change_token_right->data);
                if (operand_1 == operand_2)
                {
                    equal = 1;
                }
                else if (operand_1 > operand_2)
                {
                    greater = 1;
                }
                else
                {
                    less = 1;
                }
            }
            //double
            else
            {
                if (DEBUGGING == 2) printf("5bool: %d\n", token->id);

                double operand_1 = atof(change_token_left->data);
                double operand_2 = atof(change_token_right->data);
                if (operand_1 == operand_2)
                {
                    equal = 1;
                }
                else if (operand_1 > operand_2)
                {
                    greater = 1;
                }
                else
                {
                    less = 1;
                }
            }
        } //end of same token type

        //result will be always bool
        change_token->id = IFJ_T_KEYWORD;
        if (!error)
        {
            if (DEBUGGING == 2) printf("6bool: %d\n", token->id);

            switch (token->id)
            {
                case IFJ_T_LESS:            // '<'
                    if (less)
                    {
                        if (DEBUGGING == 2) printf("6abool: %d\n", token->id);
                        error = changeable_token_update(change_token, "true");
                        if (DEBUGGING == 2) printf("6a2bool: %d\n", token->id);
                    }
                    else
                    {
                        if (DEBUGGING == 2) printf("6bbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "false");
                        if (DEBUGGING == 2) printf("6b2bool: %d\n", token->id  );
                    }
                    break;
                case IFJ_T_GREATER:         // '>'
                    if (greater)
                    {
                        if (DEBUGGING == 2) printf("6cbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "true");
                        if (DEBUGGING == 2) printf("6c2bool: %d\n", token->id);
                    }
                    else
                    {
                        if (DEBUGGING == 2) printf("6dbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "false");
                        if (DEBUGGING == 2) printf("6d2bool: %d\n", token->id);
                    }
                    break;
                case IFJ_T_LESS_EQUAL:      // '<='
                    if (less || equal)
                    {
                        if (DEBUGGING == 2) printf("6ebool: %d\n", token->id);
                        error = changeable_token_update(change_token, "true");
                        if (DEBUGGING == 2) printf("6e2bool: %d\n", token->id);
                    }
                    else
                    {
                        if (DEBUGGING == 2) printf("6fbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "false");
                        if (DEBUGGING == 2) printf("6f2bool: %d\n", token->id);
                    }
                    break;
                case IFJ_T_GREATER_EQUAL:   // '>='
                    if (greater || equal)
                    {
                        if (DEBUGGING == 2) printf("6gbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "true");
                        if (DEBUGGING == 2) printf("6g2bool: %d\n", token->id);
                    }
                    else
                    {
                        if (DEBUGGING == 2) printf("6hbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "false");
                        if (DEBUGGING == 2) printf("6h2bool: %d\n", token->id);
                    }
                    break;
                case IFJ_T_SUPER_EQUAL:     // '==='
                    if (equal)
                    {
                        if (DEBUGGING == 2) printf("6ibool: %d\n", token->id);
                        error = changeable_token_update(change_token, "true");
                        if (DEBUGGING == 2) printf("6i2bool: %d\n", token->id);
                    }
                    else
                    {
                        if (DEBUGGING == 2) printf("6jbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "false");
                        if (DEBUGGING == 2) printf("6j2bool: %d\n", token->id);
                    }
                    break;
                case IFJ_T_NOT_SUPER_EQUAL: // '!=='
                    if (!equal)
                    {
                        if (DEBUGGING == 2) printf("6kbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "true");
                        if (DEBUGGING == 2) printf("6k2bool: %d\n", token->id);
                    }
                    else
                    {
                        if (DEBUGGING == 2) printf("6lbool: %d\n", token->id);
                        error = changeable_token_update(change_token, "false");
                        if (DEBUGGING == 2) printf("6l2bool: %d\n", token->id);
                    }
                    break;
            }
        }
        else if (error == IFJ_ERR_TYPE_COMPATIBILITY)
        {
            if (DEBUGGING == 2) printf("7bool: %d\n", token->id);

            switch (token->id)
            {
                case IFJ_T_SUPER_EQUAL:     // '==='
                    error = changeable_token_update(change_token, "false");
                    break;
                case IFJ_T_NOT_SUPER_EQUAL: // '!=='
                    error = changeable_token_update(change_token, "true");
                    break;
            }
        }
    }

    if (DEBUGGING) printf("bool end: %d\n", token->id);
    return error;
}

int concatenate_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("concatenate: %d\n", token->id);
    /* Allocating space for opperands */
    changeable_tokenPtr change_token_left;
    if ((change_token_left = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    changeable_tokenPtr change_token_right;
    if ((change_token_right = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    int error = call_leaf_function (token->LPtr, change_token_left, buffer, hashtable, function_hashtable);
    if (!error)
    {
        error = call_leaf_function (token->RPtr, change_token_right, buffer, hashtable, function_hashtable);
    }
    // get both operands to string if possible
    if (!error)
    {
        error = strval(change_token_left);
        if (!error)
        {
            error = strval(change_token_right);
        }
    }
    char* result_string;
    if (!error)
    {
        result_string = malloc(sizeof(char)* (strlen(change_token_left->data) + strlen(change_token_right->data) + 1));
        if (result_string == NULL)
        {
            return IFJ_ERR_INTERNAL;
        }
        //contatenation of both tokens into result_string
        strcat(result_string, change_token_left->data);
        strcat(result_string, change_token_right->data);
        // update data
        error = changeable_token_update(change_token, result_string);
        free (result_string);
        result_string = NULL;
    }
    return error;
}

/*
 * Function that handles "*", "+", "-", "/"
 */
int basic_operator_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("basic_operator: %d\n", token->id);

    /* Allocating space for opperands */
    changeable_tokenPtr change_token_left;
    if ((change_token_left = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    changeable_tokenPtr change_token_right;
    if ((change_token_right = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    int error = call_leaf_function (token->LPtr, change_token_left, buffer, hashtable, function_hashtable);
    if (!error)
    {
        error = call_leaf_function (token->RPtr, change_token_right, buffer, hashtable, function_hashtable);
    }
    if (!error)
    {
        // start of "*", "+", "-", "/"
        if (token->id == IFJ_T_MUL || token->id == IFJ_T_PLUS || token->id == IFJ_T_MIN || token->id == IFJ_T_DIV)
        {
            // good types
            if ((change_token_left->id == IFJ_T_INT || change_token_left->id == IFJ_T_DOUBLE) && (change_token_right->id == IFJ_T_INT || change_token_right->id == IFJ_T_DOUBLE))
            {
                /* @option int - true if result should be int, false if result should be double */
                int option_int = (change_token_left->id == IFJ_T_INT && change_token_right->id == IFJ_T_INT) && (!(token->id == IFJ_T_DIV));
                int operand_1_int;
                int operand_2_int;
                int result_int;
                double operand_1_double;
                double operand_2_double;
                double result_double;
                int length_result;
                char * result_string;

                // int and int
                if (option_int)
                {
                    operand_1_int = atoi(change_token_left->data);
                    operand_2_int = atoi(change_token_right->data);
                }
                // double and double, doble and int, int and double
                else
                {
                    operand_1_double = atof(change_token_left->data);
                    operand_2_double = atof(change_token_right->data);
                }
                int length_left = strlen(change_token_left->data);
                int length_right = strlen(change_token_right->data);
                // do the operation
                if (token->id == IFJ_T_MUL)        // "*"
                {
                    if (option_int)
                    {
                        result_int = operand_1_int * operand_2_int;
                    }
                    else
                    {
                        result_double = operand_1_double * operand_2_double;
                    }
                    length_result = length_left > length_right ? (2 * length_left) : (2 * length_right);
                }
                else if (token->id == IFJ_T_PLUS)  // "+"
                {
                    if (option_int)
                    {
                        result_int = operand_1_int + operand_2_int;
                    }
                    else
                    {
                        result_double = operand_1_double + operand_2_double;
                    }
                    length_result = length_left > length_right ? (length_left + 1) : (length_right + 1);
                }
                else if (token->id == IFJ_T_MIN)  // "-"
                {
                    if (option_int)
                    {
                        result_int = operand_1_int - operand_2_int;
                    }
                    else
                    {
                        result_double = operand_1_double - operand_2_double;
                    }
                    length_result = length_left > length_right ? (length_left) : (length_right);

                }
                else if (token->id == IFJ_T_DIV)  // "/"
                {
                    // first check zero division
                    if (operand_2_double == 0)
                    {
                        return IFJ_ERR_ZERO_DIVISION;
                    }
                    else
                    {
                        result_double = operand_1_double / operand_2_double;
                    }
                    length_result = length_left > length_right ? (length_left) : (length_right);

                }
                result_string = malloc(sizeof(char) * (length_result + 3));
                //allocation bug
                if (result_string == NULL)
                {
                    return IFJ_ERR_INTERNAL;
                }
                // result is int
                if (option_int)
                {
                    sprintf(result_string, "%d", result_int);
                    change_token->id = IFJ_T_INT;
                }
                // result is double
                else
                {
                    sprintf(result_string, "%g", result_double);
                    change_token->id = IFJ_T_DOUBLE;
                }
                // update data
                error = changeable_token_update(change_token, result_string);
                free (result_string);
                result_string = NULL;
            }
            // bad types
            else
            {
                error = IFJ_ERR_TYPE_COMPATIBILITY;
            }
        } // end of "*", "+", "-", "/"
    }

    if (DEBUGGING) printf("basic_operator_end: %d\n", token->id);
    return error;
}

/*
 * Function that handles numbers as terminals.
 */
int number_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer)
{
    if (DEBUGGING) printf("number_function: %d\n", token->id);

    int error = changeable_token_Insert (change_token, token, buffer);

    if (DEBUGGING) printf("number_function end: %d\n", token->id);
    return error;
}

/*
 * Searches for the variable in hashtable. Retruns 0 if ok, otherwise returns nonzero value.
 */
int var_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable)
{
    if (DEBUGGING) printf("var_function: %d\n", token->id);

    int error = 0;
    hashtable_item* item = search_hashtable (hashtable, &(buffer->data[token->content]));
    if (item == NULL)
    {
        return IFJ_ERR_UNDECLARED_VARIABLE;
    }

    /* if the data are found, get them from the hashtable */
    change_token->id = item->type;
    changeable_token_update(change_token, item->value);

    if (DEBUGGING) printf("var_function end: %d\n", token->id);
    return error;
}

/*
 * function that handles '='
 */
int assignment (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("assignment: %d\n", token->id);

    int error = 0;
    changeable_tokenPtr change_token_right;
    if ((change_token_right = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    changeable_tokenPtr change_token_left;
    if ((change_token_left = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    error = call_leaf_function (token->RPtr, change_token_right, buffer, hashtable, function_hashtable);
    if (!error)
    {
        error = changeable_token_Insert (change_token_left, token->LPtr, buffer);
    }
    /* first check, if it even should try to insert, then it checks if the allocations in insert was fine */
    if ((!error) && (insert_item_hashtable (hashtable, change_token_left->data, change_token_right->id, change_token_right->data)))
    {
        error = IFJ_ERR_INTERNAL;
    }
    if (DEBUGGING) printf("assignment end: %d\n", token->id);
    return error;
}

int while_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("while: %d\n", token->id);

    int error = 0;

    //create token for condition
    changeable_tokenPtr change_token;
    if ((change_token = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    while (!error)
    {
        //check condition
        error = call_leaf_function (token->condition, change_token, buffer, hashtable, function_hashtable);

        if (!error)
        {
            //retype condition to bool
            error = boolval (change_token);
            if (!error)
            {
                if(is_true (change_token))
                {
                    //check for empty statement
                    if (token->LPtr != NULL)
                    {
                        TokenPtr my_token = token->LPtr;
                        while(my_token != NULL)
                        {
                            if ((error = call_root_function (my_token, hashtable, buffer, function_hashtable)) != 0)
                            {
                                changeable_token_Destroy();
                                return error;
                            }
                            my_token = my_token->next;
                        }
                    }
                }
                else //false -> end of while
                {
                    break;
                }
            }
        }
    }

    if (DEBUGGING) printf("while end: %d\n", token->id);
    return error;
}

int if_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("if: %d\n", token->id);

    int error = 0;

    //create token for condition
    changeable_tokenPtr change_token;
    if ((change_token = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    //check condition
    error = call_leaf_function (token->condition, change_token, buffer, hashtable, function_hashtable);

    if (!error)
    {
        //retype condition to bool
        error = boolval (change_token);
        if (!error)
        {
            if(is_true (change_token))
            {
                //check for empty statement
                if (token->LPtr != NULL)
                {
                    TokenPtr my_token = token->LPtr;
                    while(my_token != NULL)
                    {
                        if ((error = call_root_function (my_token, hashtable, buffer, function_hashtable)) != 0)
                        {
                            changeable_token_Destroy();
                            return error;
                        }
                        my_token = my_token->next;
                    }
                }
            }
            else //false
            {
                if (token->RPtr != NULL)
                {
                    TokenPtr my_token = token->RPtr;
                    while(my_token != NULL)
                    {
                        if ((error = call_root_function (my_token, hashtable, buffer, function_hashtable)) != 0)
                        {
                            changeable_token_Destroy();
                            return error;
                        }
                        my_token = my_token->next;
                    }
                }
            }
        }
    }

    if (DEBUGGING) printf("if end: %d\n", token->id);
    return error;
}

/*
 * Function that checks, if the token is true(1) or false(0)
 */
int is_true (changeable_tokenPtr change_token)
{
    if (strcmp(change_token->data, "true") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * Chooses which function should be used, depends on token_id
 * This functions can only be in the leaf area
 */
int call_root_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable)
{
    if (DEBUGGING) printf("root: %d\n", token->id);

    /* error should be always changed */
    int error = IFJ_ERR_OTHER_RUNTIME;

    switch (token->id)
    {
        case IFJ_T_ASSIGN:
            error = assignment (token, hashtable, buffer, function_hashtable);
            break;
        case IFJ_T_EOF:
            error = 0;
            break;
        case IFJ_T_KEYWORD:
            if (strcmp(&(buffer->data[token->content]), "if") == 0)
            {
                error = if_function (token, hashtable, buffer, function_hashtable);
            }
            if (strcmp(&(buffer->data[token->content]), "while") == 0)
            {
                error = while_function (token, hashtable, buffer, function_hashtable);
            }
            /** PRIDAT RETURN */
            break;
    }

    if (DEBUGGING) printf("root end: %d\n", token->id);
    return error;
}

/*
 *  Print ASS
 */
void print_ASS (TokenPtr token, int number)
{
    TokenPtr print_token = token;
    while (print_token != NULL)
    {
        if (print_token->id == IFJ_T_EOF)
        {
        printf("%d: EOF\n", number);
        break;
        }
        if (print_token->LPtr == NULL && print_token->RPtr == NULL)
        {
            printf("%d: LEFT: NULL <- ROOT: %d -> RIGHT: NULL\n", number, print_token->id);
        }
        else if (print_token->LPtr != NULL && print_token->RPtr == NULL)
        {
            printf("%d: LEFT: %d <- ROOT: %d -> RIGHT: NULL\n", number, print_token->LPtr->id, print_token->id);
        }
        else if (print_token->LPtr == NULL && print_token->RPtr != NULL)
        {
            printf("%d: LEFT: NULL <- ROOT: %d -> RIGHT: %d\n", number, print_token->id, print_token->RPtr->id);
        }
        else
        {
            printf("%d: LEFT: %d <- ROOT: %d -> RIGHT: %d\n", number, print_token->LPtr->id, print_token->id, print_token->RPtr->id);
        }
        number++;
        print_ASS(print_token->condition, 100);
        print_ASS(print_token->LPtr, number);
        print_ASS(print_token->RPtr, number);
        number = 1;
        print_token = print_token->next;
    }
}

/****************************************************
 *                  INTERPRETER
 ****************************************************/

/*
 * The core function that calls everything else
 */
int interpreter (BUFFER_STRUCT buffer, TokenPtr token, function_hashtablePtr* function_hashtable)
{
    //print Ivan's Tree
    if (DEBUGGING)
    {
        printf("****************BAMBIHO STROM*******************\n");
        int number = 1;
        print_ASS (token, number);
        printf("\n**************LUBOSOVY PRUCHODY*****************\n");
    }
    int error = 0;

    TokenPtr my_token = token;

    hashtable_item** hashtable = hashtable_init();
    if (hashtable == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    while(my_token->id != IFJ_T_EOF && (my_token->id != IFJ_T_KEYWORD 
        && strcmp(&(buffer->data[my_token->content]), "return")))
    {
        if ((error = call_root_function (my_token, hashtable, buffer, function_hashtable)) != 0)
        {
            changeable_token_Destroy();
            return error;
        }
        my_token = my_token->next;
    }
    hashtable_free(hashtable);
    changeable_token_Destroy();

    return error;
}
