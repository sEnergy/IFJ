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

int buffer_init(BUFFER_STRUCT buffer)
{
	buffer->data = (char*) malloc(IFJ_SYNTAX_DEF_BUFF_SIZE*sizeof(char));
	if (buffer->data == NULL)
		return -1;
	buffer->data[1] = '\0';
	buffer->max_length = IFJ_SYNTAX_DEF_BUFF_SIZE;
	buffer->position = 0;
	return 0;
}

int syntax_analyzer (char* input_filename)
{
    int code = 0;

    printf("Syntax analyzer started...\n");
    printf("Input filename is: %s\n", input_filename);

    BUFFER_STRUCT token_data = NULL;
    if ((token_data = (BUFFER_STRUCT) malloc(sizeof(struct buffer_struct))) == NULL)
	{
		return -1;
	}
	else
	{
		if (buffer_init(token_data) == -1)
			return -1;
	}
    int token_id = -1;

    FILE* input = fopen(input_filename, "r");
    if (input == NULL) return IFJ_ERR_INTERNAL;

    int token_number = 1;

    printf("\nProcessed by lexical analyzer:\n\n");

    while ((code = lex_analyzer(input, &token_id, token_data)) == 0)
    {
        printf("Token number: %i\nToken ID: %i\nToken data: %s\n\n", token_number++,
        token_id, token_data->data);
        token_data->data[0] = '\0';
        token_data->position = 0;

        if (token_id == IFJ_T_EOF) break;
    }
	printf("return code: %d\n",code);
    fclose(input);
    free(token_data);
    return code;
}

/*** End of file syntax_analyzer.c ***/
