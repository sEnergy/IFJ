/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            main.c
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Main source file of team project for course IFJ
 *
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "syntax_analyzer.h"

int main (int argc, char *argv[])
{
    int code = 0;

    if (argc == 2)
    {
        printf("\nNow starting Syntax analyzer...\n");
        code = syntax_analyzer(argv[1]);
    }
    else
    {
        code = IFJ_ERR_INTERNAL;
    }

    return code;
}

/*** End of file main.c ***/
