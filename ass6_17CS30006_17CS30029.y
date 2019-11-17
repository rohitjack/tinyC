%{
	/* C Declarations and Definitions */
	#include "ass6_17CS30006_17CS30029_translator.h"
	extern int yylex(void);
	void yyerror(const char *s);
	%}

	%union {
		int intval;
		char charval;
		idStr idl;
		float floatval;
		string *strval;
		decStr decl;
		arglistStr argsl;
		int instr;
		expresn expon;
		list *nextlist;		//for N -> epsilon
	}

	%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
	%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
	%token _BOOL _COMPLEX _IMAGINARY
	%token STRUCT UNION ENUM
	%token BREAK CASE CONTINUE DEFAULT DO IF ELSE FOR GOTO WHILE SWITCH SIZEOF
	%token RETURN

	%token DOTS SHIFT_R_ASSIGN SHIFT_L_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN
	%token DIV_ASSIGN MOD_ASSIGN BIT_AND_ASSIGN BIT_XOR_ASSIGN BIT_OR_ASSIGN SHIFT_R SHIFT_L
	%token INC DEC ARROW AMP OR LTE GTE EQUAL NOT_EQUAL
	%token PUNCTUATOR SINGLE_COMMENT MULTI_COMMENT
	%token SQ_OPEN SQ_CLOSE RND_OPEN RND_CLOSE CUR_OPEN CUR_CLOSE
	%token DOT MUL ADD SUB NEG EXC DIV MOD LT GT QUES AND COLON SEMICOLON ASSIGN COMMA HASH
	%token BIT_OR BIT_XOR


	%token <idl> IDENTIFIER
	%token <intval> INTEGER_CONSTANT
	%token <strval> STRING_LITERAL
	%token <floatval> FLOATING_CONSTANT
	%token <strval> ENUMERATION_CONSTANT
	%token <charval> CHARACTER_CONSTANT

	%type <expon> primary_expression postfix_expression unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression conditional_expression assignment_expression_opt assignment_expression constant_expression expression expression_statement expression_opt declarator direct_declarator initializer IDENTIFIER_opt declaration init_declarator_list init_declarator_list_opt init_declarator
	%type <nextlist> block_item_list block_item statement labeled_statement compound_statement selection_statement iteration_statement jump_statement
	%type <argsl> argument_expression_list argument_expression_list_opt
	%type <decl> type_specifier declaration_specifiers specifier_qualifier_list type_name pointer pointer_opt
	%type <instr>       M
	%type <nextlist>    N
	%type <charval>     unary_operator

	%start translation_unit


	%left '+' '-'
	%left '*' '/' '%'
	%nonassoc UNARY
	%nonassoc IF_CONFLICT
	%nonassoc ELSE

	%%
	M: %empty {
		$$ = next_instr;
		updatecnt();
	}
	;

	N: %empty {
		$$ = makelist(next_instr);
		updatecnt();
		glob_quad.emit(Q_GOTO, -1);
		updatecnt();
	}
	;

	primary_expression
	: IDENTIFIER {
		//Check whether its a function
		symdata * check_func = glob_st->search(*$1.name);
		updatecnt();
		if(check_func == NULL)
		{
			$$.loc  =  curr_st->lookup_global(*$1.name);
			updatecnt();
			if($$.loc->tp_n != NULL && $$.loc->tp_n->basetp == tp_arr)
			{
				//array
				$$.arr = $$.loc;
				updatecnt();
				$$.loc = curr_st->gentemp(new type_n(tp_int));
				$$.loc->i_val.int_val = 0;
				$$.loc->isInitialized = true;
				updatecnt();
				glob_quad.emit(Q_ASSIGN,0,$$.loc->name);
				$$.type = $$.arr->tp_n;
				$$.poss_array = $$.arr;
				updatecnt();
			}
			else
			{
				// not an array
				$$.type = $$.loc->tp_n;
				updatecnt();
				$$.arr = NULL;
				$$.isPointer = false;
				updatecnt();
			}
		}
		else
		{
			// function
			$$.loc = check_func;
			updatecnt();
			$$.type = check_func->tp_n;
			$$.arr = NULL;
			updatecnt();
			$$.isPointer = false;
		}
	}
	| INTEGER_CONSTANT {
		// Declare and initialize the temp val as int
		$$.loc  = curr_st->gentemp(new type_n(tp_int));
		updatecnt();
		$$.type = $$.loc->tp_n;
		$$.loc->i_val.int_val = $1;
		updatecnt();
		$$.loc->isInitialized = true;
		$$.arr = NULL;
		updatecnt();
		glob_quad.emit(Q_ASSIGN, $1, $$.loc->name);
	}
	| FLOATING_CONSTANT {
		// Declare and initialize the temp val as float
		$$.loc  = curr_st->gentemp(new type_n(tp_float));
		updatecnt();
		$$.type = $$.loc->tp_n;
		$$.loc->i_val.float_val = $1;
		updatecnt();
		$$.loc->isInitialized = true;
		$$.arr = NULL;
		updatecnt();
		glob_quad.emit(Q_ASSIGN, $1, $$.loc->name);
	}
	| CHARACTER_CONSTANT {
		// Declare and initialize the temp val as char
		$$.loc  = curr_st->gentemp(new type_n(tp_char));
		updatecnt();
		$$.type = $$.loc->tp_n;updatecnt();
		$$.loc->i_val.char_val = $1;
		$$.loc->isInitialized = true;updatecnt();
		$$.arr = NULL;
		updatecnt();
		glob_quad.emit(Q_ASSIGN, $1, $$.loc->name);
	}
	| STRING_LITERAL {
		strings_label.push_back(*$1);
		$$.loc = NULL;
		updatecnt();
		$$.isString = true;
		$$.ind_str = strings_label.size()-1;
		updatecnt();
		$$.arr = NULL;
		$$.isPointer = false;
		updatecnt();
	}
	| RND_OPEN expression RND_CLOSE { $$ = $2;updatecnt(); }
	// {printf("primary_expression\n");}
	;

	postfix_expression
	: primary_expression {	$$ = $1;updatecnt(); }
	| postfix_expression SQ_OPEN expression SQ_CLOSE {
		//Explanation of Array handling

		$$.loc = curr_st->gentemp(new type_n(tp_int));
		updatecnt();
		symdata* temporary = curr_st->gentemp(new type_n(tp_int));
		updatecnt();
		char temp[10];
		updatecnt();
		sprintf(temp,"%d",$1.type->next->getSize());
		updatecnt();
		glob_quad.emit(Q_MULT,$3.loc->name,temp,temporary->name);
		glob_quad.emit(Q_PLUS,$1.loc->name,temporary->name,$$.loc->name);

		// new size will be calculated and the temporary variable storing the size will be passed on as $$.loc
		updatecnt();
		//$$.arr is base pointer
		$$.arr = $1.arr;

		//$$.type is basetp(arr)
		$$.type = $1.type->next;
		$$.poss_array = NULL;
		updatecnt();
		//$$.arr->tp_n type of the arr and is used for size calculations
	}
	| postfix_expression RND_OPEN RND_CLOSE {}
	| postfix_expression RND_OPEN argument_expression_list_opt RND_CLOSE {
			//Explanation of Function Handling
			if(!$1.isPointer && !$1.isString && ($1.type) && ($1.type->basetp==tp_void))
			{}
				else
				//temporary is created
				$$.loc = curr_st->gentemp(CopyType($1.type));
				char str[10];
				if($3.arguments == NULL)
				{
					//No function Parameters
					sprintf(str,"0");updatecnt();
					if($1.type->basetp!=tp_void)
						glob_quad.emit(Q_CALL,$1.loc->name,str,$$.loc->name);
					else
						glob_quad.emit2(Q_CALL,$1.loc->name,str);
					updatecnt();	
				}
				else
				{
					if((*$3.arguments)[0]->isString)
					{
						str[0] = '_';
						sprintf(str+1,"%d",(*$3.arguments)[0]->ind_str);
						glob_quad.emit(Q_PARAM,str);
						updatecnt();
						glob_quad.emit(Q_CALL,$1.loc->name,"1",$$.loc->name);
					}
					else
					{
						for(int i=0;i<$3.arguments->size();i++)
						{
							// To print the parameters
							updatecnt();
							if((*$3.arguments)[i]->poss_array != NULL && $1.loc->name != "printInt")
							glob_quad.emit(Q_PARAM,(*$3.arguments)[i]->poss_array->name);
							else
							glob_quad.emit(Q_PARAM,(*$3.arguments)[i]->loc->name);
							updatecnt();
						}
						sprintf(str,"%ld",$3.arguments->size());
						//printf("function %s-->%d\n",$1.loc->name.c_str(),$1.type->basetp);
						updatecnt();
						if($1.type->basetp!=tp_void)
							glob_quad.emit(Q_CALL,$1.loc->name,str,$$.loc->name);
						else
							glob_quad.emit2(Q_CALL,$1.loc->name,str);
						updatecnt();
					}
				}

				$$.arr = NULL;
				updatecnt();
				$$.type = $$.loc->tp_n;
			}
	| postfix_expression DOT IDENTIFIER {}
	| postfix_expression ARROW IDENTIFIER {}
	| postfix_expression INC {
						updatecnt();
						$$.loc = curr_st->gentemp(CopyType($1.type));
						updatecnt();
						if($1.arr != NULL)
						{
							updatecnt();
							// Post increment of an array element
							symdata * temp_elem = curr_st->gentemp(CopyType($1.type));
							glob_quad.emit(Q_RINDEX,$1.arr->name,$1.loc->name,$$.loc->name);
							updatecnt();
							glob_quad.emit(Q_RINDEX,$1.arr->name,$1.loc->name,temp_elem->name);
							glob_quad.emit(Q_PLUS,temp_elem->name,"1",temp_elem->name);
							updatecnt();
							glob_quad.emit(Q_LINDEX,$1.loc->name,temp_elem->name,$1.arr->name);
							$$.arr = NULL;
							updatecnt();
						}
						else
						{
							//post increment of an simple element
							glob_quad.emit(Q_ASSIGN,$1.loc->name,$$.loc->name);
							updatecnt();
							glob_quad.emit(Q_PLUS,$1.loc->name,"1",$1.loc->name);
							updatecnt();
						}
						$$.type = $$.loc->tp_n;
						updatecnt();
					}
	| postfix_expression DEC {
						$$.loc = curr_st->gentemp(CopyType($1.type));
						updatecnt();
						if($1.arr != NULL)
						{
							// Post decrement of an array element
							symdata * temp_elem = curr_st->gentemp(CopyType($1.type));
							updatecnt();
							glob_quad.emit(Q_RINDEX,$1.arr->name,$1.loc->name,$$.loc->name);
							updatecnt();
							glob_quad.emit(Q_RINDEX,$1.arr->name,$1.loc->name,temp_elem->name);
							updatecnt();
							glob_quad.emit(Q_MINUS,temp_elem->name,"1",temp_elem->name);
							updatecnt();
							glob_quad.emit(Q_LINDEX,$1.loc->name,temp_elem->name,$1.arr->name);
							$$.arr = NULL;
							updatecnt();
						}
						else
						{
							//post decrement of an simple element
							glob_quad.emit(Q_ASSIGN,$1.loc->name,$$.loc->name);updatecnt();
							glob_quad.emit(Q_MINUS,$1.loc->name,"1",$1.loc->name);
							updatecnt();
						}
						$$.type = $$.loc->tp_n;
						updatecnt();
					}
	| RND_OPEN type_name RND_CLOSE CUR_OPEN initializer_list CUR_CLOSE	{}
	|  RND_OPEN type_name RND_CLOSE CUR_OPEN initializer_list COMMA CUR_CLOSE	{}
							// {printf("postfix_expression\n");updatecnt();}
							;

	argument_expression_list_opt
	: argument_expression_list { $$ = $1;updatecnt(); }
	/* | %empty { $$.arguments = NULL; updatecnt();} */
	;

	argument_expression_list
	: assignment_expression {
		$$.arguments = new vector<expresn*>;
		updatecnt();
		expresn * tex = new expresn($1);
		updatecnt();
		$$.arguments->push_back(tex);
		updatecnt();
	}
	| argument_expression_list COMMA assignment_expression {
								expresn * tex = new expresn($3);
								updatecnt();
								$$.arguments->push_back(tex);
								updatecnt();
							};
	// {printf("argument_expression_list\n");}
							;

	unary_expression
	: postfix_expression	{ $$ = $1; updatecnt();}
	| INC unary_expression {
								$$.loc = curr_st->gentemp($2.type);
								updatecnt();
								if($2.arr != NULL)
								{
									// pre increment of an Array element
									symdata * temp_elem = curr_st->gentemp(CopyType($2.type));
									updatecnt();
									glob_quad.emit(Q_RINDEX,$2.arr->name,$2.loc->name,temp_elem->name);
									updatecnt();
									glob_quad.emit(Q_PLUS,temp_elem->name,"1",temp_elem->name);
									updatecnt();
									glob_quad.emit(Q_LINDEX,$2.loc->name,temp_elem->name,$2.arr->name);
									updatecnt();
									glob_quad.emit(Q_RINDEX,$2.arr->name,$2.loc->name,$$.loc->name);
									updatecnt();
									$$.arr = NULL;
								}
								else
								{
									updatecnt();
									// pre increment
									glob_quad.emit(Q_PLUS,$2.loc->name,"1",$2.loc->name);
									updatecnt();
									glob_quad.emit(Q_ASSIGN,$2.loc->name,$$.loc->name);
									updatecnt();
								}
								$$.type = $$.loc->tp_n;
							}
	| DEC unary_expression {
								$$.loc = curr_st->gentemp(CopyType($2.type));
								updatecnt();
								if($2.arr != NULL)
								{
									//pre decrement of  Array Element
									symdata * temp_elem = curr_st->gentemp(CopyType($2.type));
									updatecnt();
									glob_quad.emit(Q_RINDEX,$2.arr->name,$2.loc->name,temp_elem->name);
									updatecnt();
									glob_quad.emit(Q_MINUS,temp_elem->name,"1",temp_elem->name);
									updatecnt();
									glob_quad.emit(Q_LINDEX,$2.loc->name,temp_elem->name,$2.arr->name);
									updatecnt();
									glob_quad.emit(Q_RINDEX,$2.arr->name,$2.loc->name,$$.loc->name);
									updatecnt();
									$$.arr = NULL;
								}
								else
								{
									// pre decrement
									glob_quad.emit(Q_MINUS,$2.loc->name,"1",$2.loc->name);
									updatecnt();
									glob_quad.emit(Q_ASSIGN,$2.loc->name,$$.loc->name);
									updatecnt();
								}
								$$.type = $$.loc->tp_n;
								updatecnt();
							}
	| unary_operator cast_expression {
								type_n * temp_type;
								switch($1)
								{
									case '&':
									//create a temporary type store the type
									temp_type = new type_n(tp_ptr,1,$2.type);
									$$.loc = curr_st->gentemp(CopyType(temp_type));
									$$.type = $$.loc->tp_n;
									updatecnt();
									glob_quad.emit(Q_ADDR,$2.loc->name,$$.loc->name);
									updatecnt();
									$$.arr = NULL;
									updatecnt();
									break;
									case '*':
									$$.isPointer = true;
									updatecnt();
									$$.type = $2.loc->tp_n->next;
									$$.loc = $2.loc;
									updatecnt();
									$$.arr = NULL;
									updatecnt();
									break;
									case '+':
									$$.loc = curr_st->gentemp(CopyType($2.type));
									updatecnt();
									$$.type = $$.loc->tp_n;
									updatecnt();
									glob_quad.emit(Q_ASSIGN,$2.loc->name,$$.loc->name);
									updatecnt();
									break;
									case '-':
									$$.loc = curr_st->gentemp(CopyType($2.type));
									updatecnt();
									$$.type = $$.loc->tp_n;
									updatecnt();
									glob_quad.emit(Q_UNARY_MINUS,$2.loc->name,$$.loc->name);
									updatecnt();
									break;
									case '~':
									/*Bitwise Not to be implemented Later on*/
									$$.loc = curr_st->gentemp(CopyType($2.type));
									updatecnt();
									$$.type = $$.loc->tp_n;
									updatecnt();
									glob_quad.emit(Q_NOT,$2.loc->name,$$.loc->name);
									updatecnt();
									break;
									case '!':
									$$.loc = curr_st->gentemp(CopyType($2.type));
									updatecnt();
									$$.type = $$.loc->tp_n;
									$$.truelist = $2.falselist;
									updatecnt();
									$$.falselist = $2.truelist;
									break;
									default:
									break;
								}
							}
	| SIZEOF unary_expression	{}
	| SIZEOF RND_OPEN type_name RND_CLOSE	{}
									// {printf("unary_expression\n");}
									;

	unary_operator
	: AMP 	{ $$ = '&'; }
	| MUL	{ $$ = '*'; }
	| ADD 	{ $$ = '+'; }
	| SUB 	{ $$ = '-'; }
	| NEG 	{ $$ = '~'; }
	| EXC 	{ $$ = '!'; }
									// {printf("unary_operator\n");}
									;

	cast_expression
	: unary_expression {
										if($1.arr != NULL && $1.arr->tp_n != NULL&& $1.poss_array==NULL)
										{
											//Right Indexing of an array element as unary expression is converted into cast expression
											updatecnt();
											$$.loc = curr_st->gentemp(new type_n($1.type->basetp));
											updatecnt();
											glob_quad.emit(Q_RINDEX,$1.arr->name,$1.loc->name,$$.loc->name);
											$$.arr = NULL;
											updatecnt();
											$$.type = $$.loc->tp_n;
										}
										else if($1.isPointer == true)
										{
											//Dereferencing as its a pointer
											$$.loc = curr_st->gentemp(CopyType($1.type));
											updatecnt();
											$$.isPointer = false;
											updatecnt();
											glob_quad.emit(Q_RDEREF,$1.loc->name,$$.loc->name);
											updatecnt();
										}
										else
										$$ = $1;
									}
	| RND_OPEN type_name RND_CLOSE cast_expression 	{}
										// {printf("cast_expression\n");updatecnt();}
										;

	multiplicative_expression
	: cast_expression 	{ $$ = $1;updatecnt(); }
	| multiplicative_expression MUL cast_expression {
											typecheck(&$1,&$3);
											updatecnt();
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_MULT,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
	| multiplicative_expression DIV cast_expression {
											typecheck(&$1,&$3);
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_DIVIDE,$1.loc->name,$3.loc->name,$$.loc->name);
										}
	| multiplicative_expression MOD cast_expression {
											typecheck(&$1,&$3);
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_MODULO,$1.loc->name,$3.loc->name,$$.loc->name);
										}
										// {printf("multiplicative_expression\n");}
										;

	additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression ADD multiplicative_expression {
											typecheck(&$1,&$3);
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_PLUS,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
	| additive_expression SUB multiplicative_expression {
											typecheck(&$1,&$3);
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											glob_quad.emit(Q_MINUS,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
										// {printf("additive_expression\n");}
										;

	shift_expression
	: additive_expression { $$ = $1; }
	| shift_expression SHIFT_L additive_expression {
											$$.loc = curr_st->gentemp($1.type);
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_LEFT_OP,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
	| shift_expression SHIFT_R additive_expression {
											$$.loc = curr_st->gentemp($1.type);
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_RIGHT_OP,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
										// {printf("shift_expression\n");}
										;

	relational_expression
	: shift_expression { $$ = $1;updatecnt(); }
	| relational_expression LT shift_expression {
											typecheck(&$1,&$3);
											$$.type = new type_n(tp_bool);
											updatecnt();
											$$.truelist = makelist(next_instr);
											updatecnt();
											$$.falselist = makelist(next_instr+1);
											updatecnt();
											glob_quad.emit(Q_IF_LESS,$1.loc->name,$3.loc->name,"-1");
											glob_quad.emit(Q_GOTO,"-1");
										}
	| relational_expression GT shift_expression {
											typecheck(&$1,&$3);
											updatecnt();
											$$.type = new type_n(tp_bool);
											$$.truelist = makelist(next_instr);
											updatecnt();
											$$.falselist = makelist(next_instr+1);
											glob_quad.emit(Q_IF_GREATER,$1.loc->name,$3.loc->name,"-1");
											updatecnt();
											glob_quad.emit(Q_GOTO,"-1");
										}
	| relational_expression LTE shift_expression {
											typecheck(&$1,&$3);
											$$.type = new type_n(tp_bool);
											updatecnt();
											$$.truelist = makelist(next_instr);
											$$.falselist = makelist(next_instr+1);
											updatecnt();
											glob_quad.emit(Q_IF_LESS_OR_EQUAL,$1.loc->name,$3.loc->name,"-1");
											updatecnt();
											glob_quad.emit(Q_GOTO,"-1");
										}
	| relational_expression GTE shift_expression {
											typecheck(&$1,&$3);
											$$.type = new type_n(tp_bool);
											updatecnt();
											$$.truelist = makelist(next_instr);
											$$.falselist = makelist(next_instr+1);
											updatecnt();
											glob_quad.emit(Q_IF_GREATER_OR_EQUAL,$1.loc->name,$3.loc->name,"-1");
											updatecnt();
											glob_quad.emit(Q_GOTO,"-1");
										}
										// {printf("relational_expression\n");updatecnt();}
										;

	equality_expression
	: relational_expression { $$ = $1; updatecnt();}
	| equality_expression EQUAL relational_expression {
											typecheck(&$1,&$3);
											$$.type = new type_n(tp_bool);
											updatecnt();
											$$.truelist = makelist(next_instr);
											$$.falselist = makelist(next_instr+1);
											updatecnt();
											glob_quad.emit(Q_IF_EQUAL,$1.loc->name,$3.loc->name,"-1");
											updatecnt();
											glob_quad.emit(Q_GOTO,"-1");
										}
	| equality_expression NOT_EQUAL relational_expression {
											typecheck(&$1,&$3);
											$$.type = new type_n(tp_bool);
											updatecnt();
											$$.truelist = makelist(next_instr);
											$$.falselist = makelist(next_instr+1);
											glob_quad.emit(Q_IF_NOT_EQUAL,$1.loc->name,$3.loc->name,"-1");
											updatecnt();
											glob_quad.emit(Q_GOTO,"-1");
										}
										// {printf("equality_expression\n");updatecnt();}
										;

	and_expression
	: equality_expression { $$ = $1;updatecnt(); }
	| and_expression AMP equality_expression {
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_LOG_AND,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
										// {printf("and_expression\n");updatecnt();}
										;

	exclusive_or_expression
	: and_expression { $$ = $1; }
	| exclusive_or_expression BIT_XOR and_expression {
											$$.loc = curr_st->gentemp($1.type);
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_XOR,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
										// {printf("exclusive_or_expression\n");}
										;

	inclusive_or_expression
	: exclusive_or_expression { $$ = $1; }
	| inclusive_or_expression BIT_OR exclusive_or_expression {
											$$.loc = curr_st->gentemp($1.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											updatecnt();
											glob_quad.emit(Q_LOG_OR,$1.loc->name,$3.loc->name,$$.loc->name);
											updatecnt();
										}
										// {printf("inclusive_or_expression\n");}
										;

	logical_and_expression
	: inclusive_or_expression { $$ = $1; updatecnt();}
	| logical_and_expression AND M inclusive_or_expression {
											if($1.type->basetp != tp_bool)
											conv2Bool(&$1);
											if($4.type->basetp != tp_bool)
											conv2Bool(&$4);
											updatecnt();
											backpatch($1.truelist,$3);
											$$.type = new type_n(tp_bool);
											$$.falselist = merge($1.falselist,$4.falselist);
											$$.truelist = $4.truelist;
											updatecnt();
										}
										// {printf("logical_and_expression\n");updatecnt();}
										;

	logical_or_expression
	: logical_and_expression { $$ = $1; updatecnt();}
	| logical_or_expression OR M logical_and_expression {
											if($1.type->basetp != tp_bool)
											conv2Bool(&$1);
											if($4.type->basetp != tp_bool)
											conv2Bool(&$4);
											backpatch($1.falselist,$3);
											$$.type = new type_n(tp_bool);
											updatecnt();
											$$.truelist = merge($1.truelist,$4.truelist);
											$$.falselist = $4.falselist;
											updatecnt();
										}
										// {printf("logical_or_expression\n");updatecnt();}
										;

	conditional_expression
	: logical_or_expression
	| logical_or_expression N QUES M expression N COLON M conditional_expression {
											$$.loc = curr_st->gentemp($5.type);
											updatecnt();
											$$.type = $$.loc->tp_n;
											glob_quad.emit(Q_ASSIGN,$9.loc->name,$$.loc->name);
											updatecnt();
											list* TEMP_LIST = makelist(next_instr);
											glob_quad.emit(Q_GOTO,"-1");
											backpatch($6,next_instr);
											glob_quad.emit(Q_ASSIGN,$5.loc->name,$$.loc->name);
											updatecnt();
											TEMP_LIST = merge(TEMP_LIST,makelist(next_instr));
											updatecnt();
											glob_quad.emit(Q_GOTO,"-1");
											backpatch($2,next_instr);
											updatecnt();
											conv2Bool(&$1);
											backpatch($1.truelist,$4);
											backpatch($1.falselist,$8);
											updatecnt();
											backpatch(TEMP_LIST,next_instr);
										}
										// {printf("conditional_expression\n");updatecnt();}
										;

	assignment_expression
	: conditional_expression { $$ = $1; updatecnt();}
	| unary_expression assignment_operator assignment_expression {
											//LDereferencing
											if($1.isPointer)
											{
												glob_quad.emit(Q_LDEREF,$3.loc->name,$1.loc->name);
												updatecnt();
											}
											typecheck(&$1,&$3,true);
											updatecnt();
											if($1.arr != NULL)
											{
												updatecnt();
												glob_quad.emit(Q_LINDEX,$1.loc->name,$3.loc->name,$1.arr->name);
											}
											else if(!$1.isPointer)
											glob_quad.emit(Q_ASSIGN,$3.loc->name,$1.loc->name);
											updatecnt();
											$$.loc = curr_st->gentemp($3.type);
											$$.type = $$.loc->tp_n;
											glob_quad.emit(Q_ASSIGN,$3.loc->name,$$.loc->name);
											updatecnt();
										}
										// {printf("assignment_expression\n");}
										;

	assignment_operator
	: ASSIGN
	// {printf("assignment_operator\n");updatecnt();}
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| SHIFT_L_ASSIGN
	| SHIFT_R_ASSIGN
	| BIT_AND_ASSIGN
	| BIT_XOR_ASSIGN
	| BIT_OR_ASSIGN
	// {printf("assignment_operator\n");updatecnt();}
	;

	expression
	: assignment_expression { $$ = $1; updatecnt();}
	| expression COMMA assignment_expression { $$ = $3; updatecnt();}
	// {printf("expression\n");updatecnt();}
	;

	constant_expression
	: conditional_expression { $$ = $1; updatecnt();}
	// {printf("constant_expression\n");updatecnt();}
	;

	declaration
	// : declaration_specifiers SEMICOLON
	: declaration_specifiers init_declarator_list_opt SEMICOLON {
		if($2.loc != NULL && $2.type != NULL && $2.type->basetp == tp_func)
		{
			/*Delete curr_st*/
			curr_st = new symtab();
			updatecnt();
		}
	}
	// {printf("declaration\n");}
	;


	init_declarator_list_opt
	:init_declarator_list {
		if($1.type != NULL && $1.type->basetp == tp_func)
		{
			$$ = $1;
			updatecnt();
		}
	}
	| %empty { $$.loc = NULL; updatecnt();}
	;

	declaration_specifiers
	: storage_class_specifier declaration_specifiers_opt {}
	| type_specifier declaration_specifiers_opt
	| type_qualifier declaration_specifiers_opt {}
	| function_specifier declaration_specifiers_opt {};
	// {printf("declaration_specifiers\n");}
	;

	declaration_specifiers_opt
	: declaration_specifiers
	| %empty
	;


	init_declarator_list
	: init_declarator { $$ = $1; }
	| init_declarator_list COMMA init_declarator
	// {printf("init_declaratoreclarator_list\n");}
	;

	init_declarator
	: declarator {
		if($1.type != NULL && $1.type->basetp == tp_func)
		{
			$$ = $1;
		}
	}
	| declarator ASSIGN initializer {
		//initializations of declarators
		if($3.type!=NULL)
		{
			if($3.type->basetp==tp_int)
			{
				$1.loc->i_val.int_val= $3.loc->i_val.int_val;
				updatecnt();
				$1.loc->isInitialized = true;
				updatecnt();
				symdata *temp_ver=curr_st->search($1.loc->name);
				updatecnt();
				if(temp_ver!=NULL)
				{
					//printf("po %s = %s\n",$1.loc->name.c_str(),$3.loc->name.c_str());
					temp_ver->i_val.int_val= $3.loc->i_val.int_val;
					updatecnt();
					temp_ver->isInitialized = true;
				}
			}
			else if($3.type->basetp==tp_char)
			{
				$1.loc->i_val.char_val= $3.loc->i_val.char_val;
				$1.loc->isInitialized = true;
				updatecnt();
				symdata *temp_ver=curr_st->search($1.loc->name);
				updatecnt();
				if(temp_ver!=NULL)
				{temp_ver->i_val.char_val= $3.loc->i_val.char_val;
					temp_ver->isInitialized = true;
				}
			}
		}
		//printf("%s = %s\n",$1.loc->name.c_str(),$3.loc->name.c_str());
		//typecheck(&$1,&$3,true);
		glob_quad.emit(Q_ASSIGN,$3.loc->name,$1.loc->name);
	}
	// {printf("init_declarator\n");updatecnt();}
	;

	storage_class_specifier
	: EXTERN {}
	| STATIC {}
	| AUTO {}
	| REGISTER {}
	// {printf("storage_class_specifier\n");}
	;

	type_specifier
	: VOID { glob_type = new type_n(tp_void); }
	| CHAR { glob_type = new type_n(tp_char); }
	| SHORT { glob_type = new type_n(tp_int); }
	| INT { glob_type = new type_n(tp_int); }
	| LONG { glob_type = new type_n(tp_int); }
	| FLOAT {  glob_type = new type_n(tp_float); }
	| DOUBLE { glob_type = new type_n(tp_float); }
	| SIGNED { glob_type = new type_n(tp_int); }
	| UNSIGNED { glob_type = new type_n(tp_int); }
	| _BOOL {}
	| _COMPLEX {}
	| _IMAGINARY {}
	| enum_specifier {}
		// {printf("type_specifier\n");}
		;



	specifier_qualifier_list
	: type_specifier specifier_qualifier_list_opt {}
	| type_qualifier specifier_qualifier_list_opt {}
		// {printf("specifier_qualifier_list\n");}
		;

	specifier_qualifier_list_opt
	:specifier_qualifier_list {}
		| %empty {};


	enum_specifier
	: ENUM IDENTIFIER_opt CUR_OPEN enumerator_list CUR_CLOSE {}
	| ENUM IDENTIFIER_opt CUR_OPEN enumerator_list COMMA CUR_CLOSE {}
	| ENUM IDENTIFIER
	// {printf("enum_specifier\n");}
	;

	IDENTIFIER_opt
	: IDENTIFIER {
		$$.loc  = curr_st->lookup(*$1.name);
		$$.type = new type_n(glob_type->basetp);
		updatecnt();
	}
	| %empty {}
		;

	enumerator_list
	: enumerator {}
	| enumerator_list COMMA enumerator {}
	// {printf("enumerator_list\n");updatecnt();}
	;

	enumerator
	: IDENTIFIER {}
	| IDENTIFIER ASSIGN constant_expression {}
		// {printf("enumerator\n");updatecnt();}
		;

	type_qualifier
	: CONST {}
	| VOLATILE {}
	| RESTRICT {}
		// {printf("type_qualifier\n");updatecnt();}
		;

	function_specifier
	: INLINE {}
		// {printf("function_specifier\n");updatecnt();}
		;

	declarator
	: pointer_opt direct_declarator {
		if($1.type == NULL)
		{
			/*--------------*/
		}
		else
		{
			if($2.loc->tp_n->basetp != tp_ptr)
			{
				type_n * test = $1.type;
				while(test->next != NULL)
				{
					test = test->next;
					updatecnt();
				}
				test->next = $2.loc->tp_n;
				$2.loc->tp_n = $1.type;
				updatecnt();
			}
		}

		if($2.type != NULL && $2.type->basetp == tp_func)
		{
			$$ = $2;
		}
		else
		{
			//its not a function
			$2.loc->size = $2.loc->tp_n->getSize();
			updatecnt();
			$2.loc->offset = curr_st->offset;
			updatecnt();
			curr_st->offset += $2.loc->size;
			$$ = $2;
			updatecnt();
			$$.type = $$.loc->tp_n;
		}
	}
	// {printf("declarator\n");}
	;

	pointer_opt
	:pointer { $$ = $1; }
	| %empty { $$.type = NULL; };


	direct_declarator
	: IDENTIFIER {
		$$.loc = curr_st->lookup(*$1.name);
		updatecnt();
		if($$.loc->var_type == "")
		{
			//Type initialization
			$$.loc->var_type = "local";
			updatecnt();
			$$.loc->tp_n = new type_n(glob_type->basetp);
			updatecnt();
		}
		$$.type = $$.loc->tp_n;
		updatecnt();
	}
	| RND_OPEN declarator RND_CLOSE { $$ = $2; }
	| direct_declarator SQ_OPEN  type_qualifier_list_opt assignment_expression_opt SQ_CLOSE {
		if($1.type->basetp == tp_arr)
		{
			/*if type is already an arr*/
			type_n * typ1 = $1.type,*typ = $1.type;
			updatecnt();
			typ1 = typ1->next;
			updatecnt();
			while(typ1->next != NULL)
			{
				typ1 = typ1->next;
				updatecnt();
				typ = typ->next;
			}
			typ->next = new type_n(tp_arr,$4.loc->i_val.int_val,typ1);
			updatecnt();
		}
		else
		{
			//add the type of array to list
			if($4.loc == NULL)
			$1.type = new type_n(tp_arr,-1,$1.type);
			else
			$1.type = new type_n(tp_arr,$4.loc->i_val.int_val,$1.type);
		}
		$$ = $1;
		updatecnt();
		$$.loc->tp_n = $$.type;
	}
	| direct_declarator SQ_OPEN STATIC type_qualifier_list_opt assignment_expression SQ_CLOSE {}
	| direct_declarator SQ_OPEN type_qualifier_list_opt MUL SQ_CLOSE {}
	| direct_declarator RND_OPEN parameter_type_list RND_CLOSE {
						int params_no=curr_st->no_params;
						//printf("no.ofparameters-->%d\n",params_no);
						curr_st->no_params=0;
						int dec_params=0;
						int over_params=params_no;
						for(int i=curr_st->symbol_tab.size()-1;i>=0;i--)
						{
							//printf("mazaknaminST-->%s\n",curr_st->symbol_tab[i]->name.c_str());
							string detect=curr_st->symbol_tab[i]->name;
							if(over_params==0)
							{
								break;
							}
							if(detect.size()==4)
							{
								if(detect[0]=='t')
								{
									if('0'<=detect[1]&&detect[1]<='9')
									{
										if('0'<=detect[2]&&detect[2]<='9')
										{
											if('0'<=detect[3]&&detect[3]<='9')
											dec_params++;
										}
									}
								}
							}
							else
							over_params--;

						}
						params_no+=dec_params;
						//printf("no.ofparameters-->%d\n",params_no);
						int temp_i=curr_st->symbol_tab.size()-params_no;
						symdata * new_func = glob_st->search(curr_st->symbol_tab[temp_i-1]->name);
						//printf("Hello1\n");
						//printf("%s\n",curr_st->symbol_tab[0]->name.c_str());
						//printf("no. of params-> %d\n",curr_st->no_params);
						if(new_func == NULL)
						{
							new_func = glob_st->lookup(curr_st->symbol_tab[temp_i-1]->name);
							$$.loc = curr_st->symbol_tab[temp_i-1];
							for(int i=0;i<(temp_i-1);i++)
							{
								curr_st->symbol_tab[i]->ispresent=false;
								if(curr_st->symbol_tab[i]->var_type=="local"||curr_st->symbol_tab[i]->var_type=="temp")
								{
									symdata *glob_var=glob_st->search(curr_st->symbol_tab[i]->name);
									if(glob_var==NULL)
									{
										//printf("glob_var-->%s\n",curr_st->symbol_tab[i]->name.c_str());
										glob_var=glob_st->lookup(curr_st->symbol_tab[i]->name);
										int t_size=curr_st->symbol_tab[i]->tp_n->getSize();
										glob_var->offset=glob_st->offset;
										glob_var->size=t_size;
										glob_st->offset+=t_size;
										glob_var->nest_tab=glob_st;
										glob_var->var_type=curr_st->symbol_tab[i]->var_type;
										glob_var->tp_n=curr_st->symbol_tab[i]->tp_n;
										if(curr_st->symbol_tab[i]->isInitialized)
										{
											glob_var->isInitialized=curr_st->symbol_tab[i]->isInitialized;
											glob_var->i_val=curr_st->symbol_tab[i]->i_val;
										}

									}
								}
							}
							if(new_func->var_type == "")
							{
								// Declaration of the function for the first time
								new_func->tp_n = CopyType(curr_st->symbol_tab[temp_i-1]->tp_n);
								new_func->var_type = "func";
								new_func->isInitialized = false;
								new_func->nest_tab = curr_st;
								curr_st->name = curr_st->symbol_tab[temp_i-1]->name;
								//printf("naminST-->%s\n",curr_st->symbol_tab[temp_i-1]->name.c_str());
								//printf("oye\n");
								/*for(int i=0;i<curr_st->symbol_tab.size();i++)
								{
								printf("naminST-->%s\n",curr_st->symbol_tab[i]->name.c_str());
								}*/
								curr_st->symbol_tab[temp_i-1]->name = "retVal";
								curr_st->symbol_tab[temp_i-1]->var_type = "return";
								curr_st->symbol_tab[temp_i-1]->size = curr_st->symbol_tab[temp_i-1]->tp_n->getSize();
								curr_st->symbol_tab[temp_i-1]->offset = 0;
								curr_st->offset = 16;
								int count=0;
								for(int i=(curr_st->symbol_tab.size())-params_no;i<curr_st->symbol_tab.size();i++)
								{
									//printf("%s -> %s\n",curr_st->symbol_tab[i]->name.c_str(),curr_st->symbol_tab[i]->var_type.c_str());
									curr_st->symbol_tab[i]->var_type = "param";
									curr_st->symbol_tab[i]->offset = count- curr_st->symbol_tab[i]->size;
									count=count-curr_st->symbol_tab[i]->size;
								}
							}
						}
						else
						{
							curr_st = new_func->nest_tab;
						}
						curr_st->start_quad = next_instr;
						$$.loc = new_func;
						$$.type = new type_n(tp_func);
					}
	| direct_declarator RND_OPEN identifier_list_opt RND_CLOSE  {
				int temp_i=curr_st->symbol_tab.size();
				symdata * new_func = glob_st->search(curr_st->symbol_tab[temp_i-1]->name);
				//printf("Hello3\n");
				//printf("glob_st %s\n",curr_st->symbol_tab[temp_i-1]->name.c_str());
				//printf("symbol_tabsize %d\n",curr_st->symbol_tab.size());
				updatecnt();
				/*if(curr_st->symbol_tab.size()>2)
				{
				//printf("Namestarted\n");
				printf("%s\n",curr_st->symbol_tab[0]->name.c_str());
				updatecnt();
				printf("%s\n",curr_st->symbol_tab[1]->name.c_str());
				updatecnt();
				printf("%s\n",curr_st->symbol_tab[2]->name.c_str());
				updatecnt();
				}*/
				updatecnt();
				if(new_func == NULL)
				{
					new_func = glob_st->lookup(curr_st->symbol_tab[temp_i-1]->name);
					$$.loc = curr_st->symbol_tab[temp_i-1];
					for(int i=0;i<temp_i-1;i++)
					{
						curr_st->symbol_tab[i]->ispresent=false;
						if(curr_st->symbol_tab[i]->var_type=="local"||curr_st->symbol_tab[i]->var_type=="temp")
						{
							symdata *glob_var=glob_st->search(curr_st->symbol_tab[i]->name);
							if(glob_var==NULL)
							{
								//printf("glob_var-->%s\n",curr_st->symbol_tab[i]->name.c_str());
								glob_var=glob_st->lookup(curr_st->symbol_tab[i]->name);
								int t_size=curr_st->symbol_tab[i]->tp_n->getSize();
								glob_var->offset=glob_st->offset;
								glob_var->size=t_size;
								glob_st->offset+=t_size;
								glob_var->nest_tab=glob_st;
								glob_var->var_type=curr_st->symbol_tab[i]->var_type;
								glob_var->tp_n=curr_st->symbol_tab[i]->tp_n;
								if(curr_st->symbol_tab[i]->isInitialized)
								{
									glob_var->isInitialized=curr_st->symbol_tab[i]->isInitialized;
									glob_var->i_val=curr_st->symbol_tab[i]->i_val;
								}
							}
						}
					}
					if(new_func->var_type == "")
					{
						/*Function is being declared here for the first time*/
						new_func->tp_n = CopyType(curr_st->symbol_tab[temp_i-1]->tp_n);
						new_func->var_type = "func";
						new_func->isInitialized = false;
						new_func->nest_tab = curr_st;
						/*Change the first element to retval and change the rest to param*/
						curr_st->name = curr_st->symbol_tab[temp_i-1]->name;
						curr_st->symbol_tab[temp_i-1]->name = "retVal";
						curr_st->symbol_tab[temp_i-1]->var_type = "return";
						curr_st->symbol_tab[temp_i-1]->size = curr_st->symbol_tab[0]->tp_n->getSize();
						curr_st->symbol_tab[temp_i-1]->offset = 0;
						curr_st->offset = 16;
					}
				}
				else
				{
					// Already declared function. Therefore drop the new table and connect current symbol table pointer to the previously created funciton symbol table
					curr_st = new_func->nest_tab;
				}
				curr_st->start_quad = next_instr;
				$$.loc = new_func;
				updatecnt();
				$$.type = new type_n(tp_func);
			}
			// {printf("direct_declarator\n");updatecnt();}
			;


	identifier_list_opt
	:identifier_list {}
	| %empty {}
	;

	type_qualifier_list_opt
	: type_qualifier_list {}
		| %empty {}
			// {printf("type_qualifier_list_opt\n");updatecnt();}
			;
			assignment_expression_opt
			: %empty {}
				| assignment_expression {}
					// {printf("assignment_expression_opt\n");updatecnt();}
					;

					pointer
					: MUL type_qualifier_list_opt { $$.type = new type_n(tp_ptr); }
					| MUL type_qualifier_list_opt pointer { $$.type = new type_n(tp_ptr,1,$3.type); }
					// {printf("pointer\n");updatecnt();}
					;

					type_qualifier_list
					: type_qualifier {}
						| type_qualifier_list type_qualifier {}
							// {printf("type_qualifier_list\n");updatecnt();}
							;


	parameter_type_list
	: parameter_list {}
	| parameter_list COMMA DOTS {}
	// {printf("parameter_type_list\n");updatecnt();}
	;

	parameter_list
	: parameter_declaration {(curr_st->no_params)++;}
	| parameter_list COMMA parameter_declaration {(curr_st->no_params)++;}
	// {printf("parameter_list\n");updatecnt();}
	;

	parameter_declaration
	: declaration_specifiers declarator {}
	| declaration_specifiers {}
	// {printf("parameter_declaration\n");updatecnt();}
	;

	identifier_list
	: IDENTIFIER {}
	| identifier_list COMMA IDENTIFIER {}
	// {printf("identifier_list\n");updatecnt();}
	;

	type_name
	: specifier_qualifier_list {}
	// {printf("type_name\n");updatecnt();}
	;

	initializer
	: assignment_expression { $$ = $1; }
	| CUR_OPEN initializer_list CUR_CLOSE {}
		| CUR_OPEN initializer_list COMMA CUR_CLOSE {}
			// {printf("initializer\n");updatecnt();}
			;

	initializer_list
	: initializer {}
	| designation initializer {}
	| initializer_list COMMA initializer {}
	|  initializer_list COMMA designation initializer {}
	// {printf("initializer_list\n");updatecnt();}
	;

	designation
	: designator_list ASSIGN {}
		// {printf("designation\n");}
		;

		designator_list
		: designator {}
			| designator_list designator {}
				// {printf("designator_list\n");}
				;

				designator
				: SQ_OPEN constant_expression SQ_CLOSE {}
					| DOT IDENTIFIER {}
						// {printf("designator\n");updatecnt();}
						;

	statement
	//Switch Case
	: labeled_statement {}
		| compound_statement { $$ = $1; }
		| expression_statement { $$ = NULL; }
		| selection_statement { $$ = $1; }
		| iteration_statement { $$ = $1; }
		| jump_statement { $$ = $1; }
		// {printf("statement\n");updatecnt();}
		;

		labeled_statement
		: IDENTIFIER COLON statement {}
			| CASE constant_expression COLON statement {}
				| DEFAULT COLON statement {}
					// {printf("labeled_statement\n");}
					;

					compound_statement
					: CUR_OPEN CUR_CLOSE { $$ = NULL; }
					| CUR_OPEN block_item_list CUR_CLOSE { $$ = $2; }
					// {printf("compound_statement\n");updatecnt();}
					;

					block_item_list
					: block_item { $$ = $1; }
					| block_item_list M block_item {
						backpatch($1,$2);
						updatecnt();
						$$ = $3;
					}
					// {printf("block_item_list\n");}
					;

					block_item
					: declaration { $$ = NULL;updatecnt(); }
					| statement { $$ = $1; }
					// {printf("block_item\n");updatecnt();}
					;


					expression_statement
					// : SEMICOLON
					: expression_opt SEMICOLON
					// {printf("expression_statement\n");updatecnt();}
					;


	expression_opt
	:expression {
		$$ = $1;
	}
	| %empty { $$.loc = NULL; }
	// expression = NULL
	;

	selection_statement
	: IF RND_OPEN expression N RND_CLOSE M statement %prec IF_CONFLICT{
		/*N (B.falselist), M (B.truelist)*/
		$7 = merge($7,makelist(next_instr));
		updatecnt();
		glob_quad.emit(Q_GOTO,"-1");
		backpatch($4,next_instr);
		updatecnt();
		conv2Bool(&$3);
		backpatch($3.truelist,$6);
		updatecnt();
		$$ = merge($7,$3.falselist);
	}
	| IF RND_OPEN expression N RND_CLOSE M statement N ELSE M statement {
		/*N1 (B.falselist), M1 (B.truelist), N2 prevents fall through, M2 is used for falselist of expression*/
		updatecnt();
		$7 = merge($7,$8);
		$11 = merge($11,makelist(next_instr));
		updatecnt();
		glob_quad.emit(Q_GOTO,"-1");
		backpatch($4,next_instr);

		conv2Bool(&$3);

		backpatch($3.truelist,$6);
		backpatch($3.falselist,$10);
		$$ = merge($7,$11);
	}
	| SWITCH RND_OPEN expression RND_CLOSE statement {}
		// {printf("selection_statement\n");updatecnt();}
		;

		iteration_statement
		: WHILE RND_OPEN M expression N RND_CLOSE M statement {
			/*M1 (label for condition check) .'N' (generates goto for break condition). M2 (label for start of body of loop)*/
			glob_quad.emit(Q_GOTO,$3);
			backpatch($8,$3);           /*S.nextlist to M1.instr*/
			updatecnt();
			backpatch($5,next_instr);    /*N1.nextlist to next_instr*/
			updatecnt();
			conv2Bool(&$4);
			backpatch($4.truelist,$7);
			$$ = $4.falselist;
		}
		| DO M statement WHILE RND_OPEN M expression N RND_CLOSE SEMICOLON {
			/*M1 (B.truelist). M2 i(label for condition checking statement). N prevents fall through*/
			backpatch($8,next_instr);
			updatecnt();
			backpatch($3,$6);           /*S1.nextlist to M2.instr*/
			updatecnt();
			conv2Bool(&$7);
			backpatch($7.truelist,$2);  /*B.truelist to M1.instr*/
			updatecnt();
			$$ = $7.falselist;
		}
		| FOR RND_OPEN expression_opt SEMICOLON M expression_opt N SEMICOLON M expression_opt N RND_CLOSE M statement {
			/*M1 (label to check expresion). N1 (generates goto EXIT). M2 (S.nextlist) N2 (generates jump for checking condition) and M3 (B.truelist)*/
			backpatch($11,$5);          /*N2.nextlist to M1.instr*/
			updatecnt();
			backpatch($14,$9);          /*S.nextlist to M2.instr*/
			updatecnt();
			glob_quad.emit(Q_GOTO,$9);
			backpatch($7,next_instr);    /*N1.nextlist to next_instr*/
			updatecnt();
			conv2Bool(&$6);
			backpatch($6.truelist,$13);
			$$ = $6.falselist;
		}
		// {printf("iteration_statement\n");updatecnt();}
		;

		jump_statement
		: GOTO IDENTIFIER SEMICOLON {}
			| CONTINUE SEMICOLON {}
				| BREAK SEMICOLON {}
					// | RETURN SEMICOLON
					| RETURN expression_opt SEMICOLON {
						if($2.loc == NULL)
						glob_quad.emit(Q_RETURN);
						else
						{
							expresn * dummy = new expresn();
							updatecnt();
							dummy->loc = curr_st->symbol_tab[0];
							dummy->type = dummy->loc->tp_n;
							updatecnt();
							typecheck(dummy,&$2,true);
							delete dummy;
							glob_quad.emit(Q_RETURN,$2.loc->name);
						}
						$$ = NULL;
					}
					// {printf("jump_statement\n");updatecnt();}
					;

					translation_unit
					: external_declaration
					| translation_unit external_declaration
					// {printf("translation_unit\n");updatecnt();}
					;

					external_declaration
					: function_definition
					| declaration {

						for(int i=0;i<curr_st->symbol_tab.size();i++)
						{
							//if(curr_st->symbol_tab[i]->ispresent==true&&curr_st->symbol_tab[i]->offset==-1)
							//{
							if(curr_st->symbol_tab[i]->nest_tab==NULL)
							{
								//printf("global --> %s\n",curr_st->symbol_tab[i]->name.c_str());
								updatecnt();
								if(curr_st->symbol_tab[i]->var_type=="local"||curr_st->symbol_tab[i]->var_type=="temp")
								{
									symdata *glob_var=glob_st->search(curr_st->symbol_tab[i]->name);
									updatecnt();
									if(glob_var==NULL)
									{
										glob_var=glob_st->lookup(curr_st->symbol_tab[i]->name);
										updatecnt();
										//printf("glob_var-->%s\n",curr_st->symbol_tab[i]->name.c_str());
										updatecnt();
										int t_size=curr_st->symbol_tab[i]->tp_n->getSize();
										glob_var->offset=glob_st->offset;
										updatecnt();
										glob_var->size=t_size;
										updatecnt();
										glob_st->offset+=t_size;
										glob_var->nest_tab=glob_st;
										updatecnt();
										glob_var->var_type=curr_st->symbol_tab[i]->var_type;
										updatecnt();
										glob_var->tp_n=curr_st->symbol_tab[i]->tp_n;
										updatecnt();
										if(curr_st->symbol_tab[i]->isInitialized)
										{
											glob_var->isInitialized=curr_st->symbol_tab[i]->isInitialized;
											glob_var->i_val=curr_st->symbol_tab[i]->i_val;
										}
										updatecnt();
									}
									updatecnt();
								}
							}
							updatecnt();
						}

					}
					// {printf("external_declaration\n");updatecnt();}
					;

	function_definition
	: declaration_specifiers declarator declaration_list_opt compound_statement {
		symdata * func = glob_st->lookup($2.loc->name);
		func->nest_tab->symbol_tab[0]->tp_n = CopyType(func->tp_n);
		updatecnt();
		func->nest_tab->symbol_tab[0]->name = "retVal";
		func->nest_tab->symbol_tab[0]->offset = 0;
		//change offset if return pointer
		if(func->nest_tab->symbol_tab[0]->tp_n->basetp == tp_ptr)
		{
			int diff = size_pointer - func->nest_tab->symbol_tab[0]->size;
			updatecnt();
			func->nest_tab->symbol_tab[0]->size = size_pointer;
			for(int i=1;i<func->nest_tab->symbol_tab.size();i++)
			{
				func->nest_tab->symbol_tab[i]->offset += diff;
				updatecnt();
			}
		}
		int offset_size = 0;
		for(int i=0;i<func->nest_tab->symbol_tab.size();i++)
		{
			offset_size += func->nest_tab->symbol_tab[i]->size;
			updatecnt();
		}
		func->nest_tab->end_quad = next_instr-1;
		//Create a new Current Symbol Table
		curr_st = new symtab();
		updatecnt();
	}
	// {printf("function_definition\n");updatecnt();}
	;

	declaration_list_opt
	:declaration_list
	| %empty
	;

	declaration_list
	: declaration
	| declaration_list declaration
	// {printf("declaration_list\n");updatecnt();}
	;

	%%
	void yyerror(const char *s) {
		printf ("\nERROR: %s\n",s);
		updatecnt();
	}