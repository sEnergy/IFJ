/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            Interpreter.c
 * Author:              Lubo¹ Vaníèek
 * Encoding:            UTF-8
 *
 * Description:         Implementation of funtions for list of tokens.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "token_list.h"
#include "token_id.h"
#include "syntax_analyzer.h"
#include "ial.h"
#include "interpreter.h"
#include "functions.h"

changeable_tokenPtr changeable_token_Init (void)
{
    changeable_tokenPtr New_token = malloc(sizeof(struct changeable_token));
    if (New_token == NULL)
    {
        return NULL;
    }
    return New_token;
}

/*
 * function changes data in @change_token, reallocates spaces if needed
 */
int changeable_token_update(changeable_tokenPtr change_token, char * new_data)
{ 
    if(strlen(change_token->data) < strlen(new_data)) 
    { 
        free(change_token->data); 
        change_token->data = malloc (sizeof(strlen(new_data))+1); 
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
    return 0;
}

int changeable_token_Insert (changeable_tokenPtr change_token, TokenPtr token, BUFFER_STRUCT buffer)
{
    change_token->id = token->id;
    if ((change_token->data = malloc(sizeof(strlen(&(buffer->data[token->content]))) + 1)) == NULL)
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
    
    return 0;
}

void changeable_token_Destroy (changeable_tokenPtr change_token)
{
    free(change_token->data);
    free(change_token);
}

/*
 * Chooses which function should be used, depends on token_id
 * This functions can only be in the root area
 */
int call_leaf_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable)
{
    /* error should be always changed */
    int error = IFJ_ERR_INTERNAL;
    
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
            /* need hashtable and token for search */
            error = var_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_MUL:             // '*'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_DIV:             // '/'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_PLUS:            // '+'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_MIN:             // '-'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_CONC:            // '.'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_LESS:            // '<'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_GREATER:         // '>'
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_LESS_EQUAL:      // '<='
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_GREATER_EQUAL:   // '>='
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_SUPER_EQUAL:     // '==='
            error = operator_function (token, change_token, buffer);
            break;
        case IFJ_T_NOT_SUPER_EQUAL: // '!=='
            error = operator_function (token, change_token, buffer);
            break;
                
    /** TO DO
     *  when calling function, i shouldn't forget on creating new hashtable, when returning then free */
    }
    printf("error = %d token_id = %d \n", error, token->id);    
    
    return error;
}


/*
 * Function that handles operators
 */
int operator_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer)
{
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
    
    /**
     *  lot of work to do
     **/
}

/* 
 * Function that handles numbers as terminals. 
 */
int number_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer)
{
    int error = changeable_token_Insert (change_token, token, buffer);
    return error;
}

/* 
 * Searches for the variable in hashtable. Retruns 0 if ok, otherwise returns nonzero value. 
 */
int var_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable)
{
    int error = 0;
    hashtable_item* item = search_hashtable (hashtable, &(buffer->data[token->content]));
    if (item == NULL)
    {
        return IFJ_ERR_UNDECLARED_VARIABLE;
    }
    /* if the data are found, get them from the hashtable */
    change_token->id = item->type;
    change_token->data = item->value;
    
    return error;
}

/*
 * function that handles '='
 */ 
int assignment (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer)
{
    int error = 0;
    changeable_tokenPtr change_token;
    if ((change_token = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    
    error = call_leaf_function (token->RPtr, change_token, buffer, hashtable);
    
    /* first check, if it even should try to insert, then it checks if the allocations in insert was fine */
    if ((!error) && (insert_item_hashtable (hashtable, &(buffer->data[token->content]), change_token->id, change_token->data)))
    {
        error = IFJ_ERR_INTERNAL;
    }
    free(change_token);
    return error;
}

/*
 * Chooses which function should be used, depends on token_id
 * This functions can only be in the leaf area
 */
int call_root_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer)
{
    /* error should be always changed */
    int error = IFJ_ERR_INTERNAL;
    
    switch (token->id)
    {
        case IFJ_T_ASSIGN:
            error = assignment (token, hashtable, buffer);
            break;
        case IFJ_T_EOF:
            error = 0;
            break;
    }
    return error;
}

/*
 * The core function that calls everything else
 */
int interpreter (BUFFER_STRUCT buffer, TokenPtr token) 
{
    int error = 0;
    hashtable_item** hashtable = hashtable_init();
    
    while(token != NULL)
    {
      
        if ((error = call_root_function (token, hashtable, buffer)) != 0)
        {
            printf("error = %d\n", error);
            hashtable_free(hashtable);
            return error;
        }
        token = token->next;
    }
    printf("error = %d\n", error);
    hashtable_free(hashtable);
    return error;
}
