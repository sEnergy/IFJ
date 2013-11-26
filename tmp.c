//jebnut ma ide z tohto
(void*) PSA()
{
	int table[14][14] = 
	{
	  // false - 0     < - 1      > - 2      = - 3        succes - 4
	  
	  //  *   /   +   -   .   <   >  <=  >=  === !==  (   )   $
		{ 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // *
		{ 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // /
		{ 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // +
		{ 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // -
		{ 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // .
		{ 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // <
		{ 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // >
		{ 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // <=
		{ 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}; // >=
		{ 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 1 , 2 , 2}; // ===
		{ 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 1 , 2 , 2}; // !==
		{ 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 2}; // (
		{ 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 2 , 2}; // )
		{ 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 4}; // $
	}
	if empty return false;
	*TokenPtr first = malloc(sizeof(struck Token));
	*TokenPtr last = malloc(sizeof(struck Token));
	first->id = IFJ_T_MOD;
	first->content = 0;
	first->LPtr = NULL;
	first->RPtr = NULL;
	first->id = IFJ_T_MOD;
	last->content = 0;
	last->LPtr = NULL;
	last->RPtr = NULL;
	TL_Insert_Last(list,last);
	TL_Insert_First(list,first);
	while (list->first != list->last )
	{
		if (list->first->RPtr == list->last->LPtr) 
		{
			TL_ActiveNext; // simulacia E -> i
			continue;
		}
		swith (table[TokenList->active->id][TokenList->active->LPtr->id])
		{
			case 2:
				if work_out(stack,list) != 0 return IFJ_ERR_SYNTAX;
				break;
			case 1:
				if shift(stack,list) != 0 return IFJ_ERR_SYNTAX;
				break;
			case 0:
				return IFJ_ERR_SYNTAX;
				break;
			case 4:
				return 0;
				break;
		}
		
		
