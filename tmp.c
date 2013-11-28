//jebnut ma ide z tohto
int work(Stack_t* stack, TokenList* list)
{
    List_itemPtr proceeded = list->active->LPtr;
    int i = 1;
    while(proceeded != NULL and proceeded->content != S_top(stack))
    {
    	i++;
    	proceeded = proceeded->LPtr;
    	if (i>=6)
    	{
    		return IFJ_ERR_LEXICAL
    	}
    }
    proceeded -> list->active
    if (proceeded->content->id == IFJ_T_RB)
    {    
    	if (i!=5)
    	{
    		return IFJ_ERR_LEXICAL;
    	}
    	List_itemPtr r_op = proceeded->LPtr;
    	List_itemPtr operator = r_op->LPtr;
    	List_itemPtr l_op = r_op->Lptr;
    	List_itemPtr l_br = r_op->LPtr;
    	if (is_terminal(r_op->content->id) && l_br->content->id == IFJ_T_LB
    		&& is_terminal(l_op->content->id) && is_operator(operator->content->id))
    	{
    		operator->content->LPtr = l_op->content;
    		operator->content->RPtr = p_op->content;
    		operator->LPtr = l_br->LPtr;
    		operator->RPtr = proceeded->RPtr;
    		proceeded->RPtr->LPtr = operator;
    		l_br->LPtr->RPtr = operator;
    		free(l_op);
    		free(r_op);
    		free(proceeded);
    		free(l_br);
    	}
    	else
    	{
    		return IFJ_ERR_LEXICAL;
    	}
    }
    else if (is_terminal(proceeded->content->id))
    {
    	if (i!=3)
    	{
    		return IFJ_ERR_LEXICAL;
    	}
    	List_itemPtr operator = proceeded->LPtr;
    	List_itemPtr l_op = r_op->Lptr;
    	if (is_terminal(r_op->content->id) && 
    		is_operator(operator->content->id))
    	{
    		operator->content->LPtr = l_op->content;
    		operator->content->RPtr = proceeded->content;
    		operator->LPtr = l_op->LPtr;
    		operator->RPtr = proceeded->RPtr;
    		proceeded->RPtr->LPtr = operator;
    		l_op->LPtr->RPtr = operator;
    		free(r_op);
    		free(proceeded);
    	}
    	else
    	{
    		return IFJ_ERR_LEXICAL;
    	}
    }
    else
    {
    	return IFJ_ERR_LEXICAL;
    }
}
    	
    			
    
TokenPtr PSA(TokenList* list)
{
    struct Stack_t stack;
    TokenPtr first;
    TokenPtr last;
    TokenPtr stack_top;
    
    S_init(&stack);
    int table[14][14] = 
    {
      // false - 0     < - 1      > - 2      = - 3        succes - 4
      
// input  *   /   +   -   .   <   >  <=  >=  === !==  (   )   $  
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // *
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // /
        { 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // +
        { 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // -
        { 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // .
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // <
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // >
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // <=
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // >=
        { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 1 , 2 , 2}, // ===
        { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 1 , 2 , 2}, // !==
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 3 , 0}, // (
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 2 , 2}, // )
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 0 , 4}, // $
    }
    if((start = new_token()) == NULL)
    {
    	free(first);
    	free(last);
    	return IFJ_ERR_INTERNAL;
    }
    if((end = new_token()) == NULL)
    {
    	free(first);
    	free(last);
    	free(end);
    	return IFJ_ERR_INTERNAL;
    }
    start->id = IFJ_T_MOD;
    end->id = IFJ_T_MOD;
    
    TL_Insert_Last(list,last);
    TL_Insert_First(list,first);
    S_push(&stack,start);
    TL_ActiveReset(list);
    TL_ActiveNext;
    
    
    while (!(S_top(&stack) != start && TL_GetID(list) != end))
    {
    	input = TL_GetID(list);
        if (is_terminal(input->id)) 
        {
            TL_ActiveNext; // simulacia E -> i
            continue;
        }
    	stack_top = S_top(&stack);
        switch (stack_top->id-2][list->active->id-2])
        {
            
            case 4:
                break;
            case 3:
    			TL_ActiveNext;
    			break;
            case 2:
                work(list,&stack);
                break;
            case 1:
                S_push(&stack);
                TL_ActiveNext;
                break;
            case 0:
                return IFJ_ERR_SYNTAX;
                break;
        }
    }
}
        
        
