/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            token_id.h
 * Authors:             Marcel Fiala
 *                      Attila Večerek
 * Encoding:            UTF-8
 *
 * Description:         Token identificators
 *
*******************************************************************************/

#ifndef IFJ_TOKEN_ID_INCLUDED
#define IFJ_TOKEN_ID_INCLUDED

enum token_identificators
{
    // operators & "special" characters
 
    IFJ_T_ASSIGN = 1, // '='        1
    IFJ_T_MUL, // '*'
    IFJ_T_DIV, // '/'
    IFJ_T_PLUS, // '+'
    IFJ_T_MIN, // '-'                5
    IFJ_T_CONC, // '.'
    IFJ_T_LESS, // '<'
    IFJ_T_GREATER, // '>'
    IFJ_T_LESS_EQUAL, // '<='
    IFJ_T_GREATER_EQUAL, // '>='    10
    IFJ_T_SUPER_EQUAL, // '==='
    IFJ_T_NOT_SUPER_EQUAL, // '!=='
    IFJ_T_LB, // '('
    IFJ_T_RB,  // ')'
    IFJ_T_MOD, // '$' for PSA         15
    IFJ_T_EQUAL, // '=='
    IFJ_T_NOT_EQUAL, // '!='
    IFJ_T_LSB, // '['
    IFJ_T_RSB, // ']' 
    IFJ_T_LCB, // '{'                20
    IFJ_T_RCB, // '}'
    IFJ_T_SEP, // ','
    IFJ_T_KEYWORD, //
    IFJ_T_ID,
    IFJ_T_VARIALBE,    //                25

    IFJ_T_INT,
    IFJ_T_DOUBLE,
    IFJ_T_STRING,

    // assing & comparison


    IFJ_T_SEMICOLON,
    IFJ_T_EOF //                    30
};

#endif

/*** End of file token_id.h ***/
