/*	To Do
*	Token not done, so function returns void
*	Function expect:
*	 - FILE *pFile; - pointer to file
* 	 - int c; - read character
*/ 

void s_first_variable (void)
{
	c = fgetchar(pFile);
	if (isalpha(c) || c == '_') //s_first_variable
	{
		//ulozit do tokenu
		while (isalpha(c) || isdigit(c) || c == '_') //s_variable
		{
				//ulozit do tokenu
		}
		ungetc(c, pFile); //get the symbol back
		//return token
	}
	else
	{
		//error
	}
}
