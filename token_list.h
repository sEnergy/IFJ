/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            token_list.h
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Header file of funtions for list of tokens.
 *
*******************************************************************************/

#ifndef IFJ_TOKEN_LIST_INCLUDED
#define IFJ_TOKEN_LIST_INCLUDED

#include <stdbool.h>

#include "lex_analyzer.h"

// token list item
typedef struct Token {
        unsigned int id; // token id - type of token
        char *content; // content of token
        struct Token *RPtr; // pointer to next token
        struct Token *LPtr; // pointer to previous token
} *TokenPtr;

// token list
typedef struct {
    TokenPtr first; // pointer to first token
    TokenPtr active; // pointer to active token
    TokenPtr last; // pointer to last token
} TokenList;

// initializes token list
void TL_Init (TokenList *List);

// deletes whole token list
void TL_Dispose (TokenList *List);

// inserts new item to the end of token list
int TL_Insert (TokenList *List, int token_id, BUFFER_STRUCT token_content);

// sets first item as active
void TL_ActiveReset (TokenList *List);

// shifts activity one token right, if possible
void TL_ActiveNext (TokenList *List);

// shifts activity one token left, if possible
void TL_ActivePrev (TokenList *List);

// sets *id to id of active token if possilbe; else sets *id to -1
void TL_GetID (TokenList *List, unsigned int *id);

/*
 * Sets *content to pointer to content of active token if possilbe; else sets
 * *content to NULL
 */
void TL_GetContent (TokenList *List, char** content);

// returns true if token list is empty; returns false otherwise
bool TL_IsEmpty (TokenList *List);

#endif

/*** End of file token_list.h ***/
