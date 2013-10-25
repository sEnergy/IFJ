/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            syntax_analyzer.c
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Source file of Syntax analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#include <stdio.h>  // pak můžu smazat
#include <stdlib.h>

#include "syntax_analyzer.h"
#include "lex_analyzer.h"
#include "token_id.h"
#include "errors.h"

int syntax_analyzer (char* input_filename)
{
    int code = 0;

    printf("Syntax analyzer started...\n");
    printf("Input filename is: %s\n", input_filename);

    char *token_data = malloc(IFJ_SYNTAX_DEF_BUFF_SIZE*sizeof(char));
    int token_id = -1;

    FILE* input = fopen(input_filename, "r");
    if (input == NULL) return IFJ_ERR_INTERNAL;

    int token_number = 1;

    printf("\nProcessed by lexical analyzer:\n\n");

    while ((code = lex_analyzer(input, &token_id, token_data)) == 0)
    {
        printf("Token number: %i\nToken ID: %i\nToken data: %s\n\n", token_number++,
        token_id, token_data);

        if (token_id == IFJ_T_EOF) break;
    }

    fclose(input);
    free(token_data);
    return code;
}

/*** End of file syntax_analyzer.c ***/
