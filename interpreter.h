/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            Interpreter.h
 * Author:              Lubo¹ Vaníèek
 * Encoding:            UTF-8
 *
 * Description:         Implementation of funtions for list of tokens.
 *
*******************************************************************************/

#ifndef IFJ_INTERPRETER_INCLUDED
#define IFJ_INTERPRETER_INCLUDED

#include "ial.h"

/* changable token for interpreter */
typedef struct changeable_token 
{
    unsigned int id;
    char* data;
    struct changeable_token* next;
} *changeable_tokenPtr;

changeable_tokenPtr changeable_token_Init (void);
int changeable_token_update(changeable_tokenPtr change_token, char * new_data);
int changeable_token_Insert (changeable_tokenPtr change_token, TokenPtr token, BUFFER_STRUCT buffer);
void changeable_token_Destroy (void);

/* core function */
int interpreter (BUFFER_STRUCT buffer, TokenPtr token);

int call_leaf_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable);

int var_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable);
int number_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer);
int basic_operator_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable);
int concatenate_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable);

int call_root_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer);

int assignment (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer);

#endif
