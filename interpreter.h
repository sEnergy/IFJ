/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            Interpreter.h
 * Author:              Lubo� Van��ek
 * Encoding:            UTF-8
 *
 * Description:         Header file of Interpreter.
 *
*******************************************************************************/

#ifndef IFJ_INTERPRETER_INCLUDED
#define IFJ_INTERPRETER_INCLUDED

#include "ial.h"
#include "token_list.h"

/* changable token for interpreter */
typedef struct changeable_token 
{
    unsigned int id;
    char* data;
    struct changeable_token* next;			//next for deallocation
    struct changeable_token* next_params;	//next for parametrs in functions
    struct changeable_token* next_stack;	//next for stack (return in functions)
} *changeable_tokenPtr;

/* function hashtable */
typedef struct function_hash_table
{
    char* name;
    TokenPtr function_token;
    struct function_hash_table* next;
}* function_hashtablePtr;

function_hashtablePtr* function_hashtable_init (void);
int function_hashtable_insert (function_hashtablePtr* function_hashtable, TokenPtr function_token,  BUFFER_STRUCT buffer);
function_hashtablePtr function_hashtable_search (function_hashtablePtr* function_hashtable, char* string);
void function_hashtable_free (function_hashtablePtr* function_hashtable);

/* tokens that can be changed */
changeable_tokenPtr changeable_token_Init (void);
int changeable_token_update(changeable_tokenPtr change_token, char * new_data);
int changeable_token_Insert (changeable_tokenPtr change_token, TokenPtr token, BUFFER_STRUCT buffer);
void changeable_token_Destroy (void);

/* core function */
int interpreter (BUFFER_STRUCT buffer, TokenPtr token, function_hashtablePtr* function_hashtable);

/* support functions */
int is_true (changeable_tokenPtr change_token);
void print_ASS (TokenPtr token, int number);
int check_params (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, int number_of_params);

/* leaf functions */
int call_leaf_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable);

int var_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable);
int number_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer);
int basic_operator_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable);
int concatenate_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable);
int boolean_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable);
int functions (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable, function_hashtablePtr* function_hashtable);


/* root functions */
int call_root_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable);

int assignment (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable);
int if_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable);
int while_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer, function_hashtablePtr* function_hashtable);

#endif
