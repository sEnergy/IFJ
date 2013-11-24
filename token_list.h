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

#include "lex_analyzer.h"

typedef struct Token {
        unsigned int id; // token id - type of token
        char *content; // content of token
        struct Token *prev; // pointer to next token
        struct Token *next; // pointer to previous token
} *TokenPtr;

typedef struct {
    TokenPtr first; // pointer to first token
    TokenPtr active; // pointer to active token
    TokenPtr last; // pointer to last token
} TokenList;

void TL_Init (TokenList *List);
void TL_Dispose (TokenList *List);
int TL_Insert (TokenList *List, int token_id, BUFFER_STRUCT token_content);
void TL_ActiveReset (TokenList *List);
void TL_ActiveNext (TokenList *List);
void TL_ActivePrev (TokenList *List);
void TL_GetID (TokenList *List, unsigned int *id);
void TL_GetContent (TokenList *List, char** content);

#endif

/*** End of file token_list.h ***/
