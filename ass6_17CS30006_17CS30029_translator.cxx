#include "ass6_17CS30006_17CS30029_translator.h"
#include "y.tab.h"

int size_int=4;
int size_float=8;
int size_pointer=8;
int size_char=1;
int size_bool=1;

type_n *glob_type;
int glob_width;
int next_instr;
int temp_count=0;
symtab *glob_st =new symtab();
symtab *curr_st = new symtab();
quad_arr glob_quad;
vector <string> vs;
vector <string> cs;
vector<string> strings_label;

type_n::type_n(types t,int sz,type_n *n)
{
  int cnt=0;
  basetp=t;
  cnt=cnt+1;
  size=sz;
  cnt=cnt+1;
  next=n;
  cnt=cnt+1;
}

int type_n::getSize()
{
  //return the size of the array by calling the recursive function
  //here we are not checking for null as if it will reach the final type it will enter the below conditions
  if(this!=NULL)
  {
    switch(this->basetp)
    {
      case tp_arr:
        return ((this->size)*(this->next->getSize()));
      case tp_void:
        return 0;
      case tp_int:
        return size_int;
      case tp_float:
        return size_float;
      case tp_bool:
        return size_bool;
      case tp_char:
        return size_char;
      case tp_ptr:
        return size_pointer;
    }
  }
  else
  {
    return 0;
  }
}

types type_n::getBasetp()
{
  if(this==NULL)
    return tp_void;
  else
    return this->basetp;
}

void type_n::printSize()
{
  int cnt=0;
  printf("%d\n",size);
  cnt=cnt+1;
}

void type_n::print()
{
  switch(basetp){
    case tp_void:
      printf("Void");
      break;
    case tp_int:
      printf("Int");
      break;
    case tp_char:
      printf("Char");
      break;
    case tp_float:
      printf("Float");
      break;
    case tp_bool:
      printf("Bool");
      break;
    case tp_ptr:
      printf("ptr(");
      if(this->next!=NULL)
        this->next->print();
      printf(")");
      break;
    case tp_arr:
      printf("array(%d,",size);
      if(this->next!=NULL)
        this->next->print();
      printf(")");
      break;
    case tp_func:
      printf("Function()");
      break;
    default:
      printf("TYPE NOT FOUND\n");
      exit(-1);
  }

}

array::array(string s,int sz,types t)
{
  int cnt=0;
  this->bsize=sz;
  cnt=cnt+1;
  this->base_arr=s;
  cnt=cnt+1;
  this->tp=t;
  cnt=cnt+1;
  this->dimension_size=1;
  cnt=cnt+1;
}

void array::addindex(int i)
{
  int cnt=0;
  this->dimension_size=this->dimension_size+1;
  cnt=cnt+1;
  this->dims.push_back(i);
  cnt=cnt+1;
}

void funct::print()
{
  int cnt=0;
  printf("Funct(");
  cnt=cnt+1;
  int i;
  cnt=cnt+1;
  i=0;
  cnt=cnt+1;
  for(i=0;i<typelist.size();i++)
  {
    if(i!=0)
        printf(",");
    printf("%d ",typelist[i]);
  }
  printf(")");
  cnt=cnt+1;
}

funct::funct(vector<types> tpls)
{
  int cnt=0;
  typelist=tpls;
  cnt=cnt+1;
}
symdata::symdata(string n)
{
  int cnt=0;
  name=n;
  cnt=cnt+1;
  size=0;
  cnt=cnt+1;
  tp_n=NULL;
  cnt=cnt+1;
  offset=-1;
  cnt=cnt+1;
  var_type="";
  cnt=cnt+1;
  isInitialized=false;
  cnt=cnt+1;
  isFunction=false;
  cnt=cnt+1;
  isArray=false;
  ispresent=true;
  cnt=cnt+1;
  arr=NULL;
  cnt=cnt+1;
  fun=NULL;
  cnt=cnt+1;
  nest_tab=NULL;
  cnt=cnt+1;
  isdone=false;
  isptrarr=false;
  isGlobal=false;
}

void symdata::createarray()
{
  int cnt=0;
  arr=new array(this->name,this->size,tp_arr);
  cnt=cnt+1;
}


symtab::symtab()
{
  int cnt=0;
  name="";
  cnt=cnt+1;
  offset=0;
  cnt=cnt+1;
  no_params=0;
}

symtab::~symtab()
{
  long int i;
  int cnt=0;
  i=0;
  cnt=cnt+1;
  for(i=0;i<symbol_tab.size();i=i+1)
  {
    type_n *p1=symbol_tab[i]->tp_n;
    cnt=cnt+1;
    type_n *p2;

    cnt=cnt+1;
    while(true)
    {
      if(p1 != NULL)
      {
        p2=p1;
        cnt=cnt+1;
        p1=p1->next;
        cnt=cnt+1;
        delete p2;
        cnt=cnt+1;
      }
      else
      {
        break;
      }
    }
    cnt=cnt+1;
  }
}

int symtab::findg(string n)
{
  int cnt=0;
  int i=0;
  for(i=0;i<vs.size();i=i+1)
  {
    if(vs[i]==n)
      return 1;
    cnt=cnt+1;
  }
  for(i=0;i<cs.size();i=i+1)
  {
    cnt++;
    if(cs[i]==n)
      return 2;
  }
  return 0;
}

type_n *CopyType(type_n *t)
{
  /*Duplicates the input type and returns the pointer to the newly created type*/
  int cnt=0;
  if(t == NULL) return t;
  type_n *res = new type_n(t->basetp);
  cnt=cnt+1;
  res->size = t->size;
  cnt=cnt+1;
  res->basetp = t->basetp;
  cnt=cnt+1;
  res->next = CopyType(t->next);
  cnt=cnt+1;
  return res;
}

symdata* symtab::lookup(string n)
{
  long int i=0;
  int cnt=0;
  for(i=0;i<symbol_tab.size();i=i+1)
  {
    if(symbol_tab[i]->name != n)
    {
      cnt=cnt+1;
    }
    else
    {
      cnt=cnt+1;
      return symbol_tab[i];
    }
  }
  symdata *temp_data=new symdata(n);
  cnt=cnt+1;
  temp_data->i_val.int_val=0;
  cnt=cnt+1;
  symbol_tab.push_back(temp_data);
  cnt=cnt+1;
  return symbol_tab[symbol_tab.size()-1];
}
void updatecnt()
{
  int counti=0;
  counti++;
}
symdata* symtab::lookup_global(string n)
{
  int i=0;
  int cnt=0;
  for(i=0;i<symbol_tab.size();i=i+1)
  {
    if(symbol_tab[i]->name != n)
    {
      cnt=cnt+1;
    }
    else
    {
      cnt=cnt+1;
      return symbol_tab[i];
    }
  }
  i=0;
  for(i=0;i<glob_st->symbol_tab.size();i=i+1)
  {
    if(glob_st->symbol_tab[i]->name != n)
    {
      cnt=cnt+1;
    }
    else
    {
      cnt=cnt+1;
      return symbol_tab[i];
    }
  }
  symdata *temp_o=new symdata(n);
  temp_o->i_val.int_val=0;
  symbol_tab.push_back(temp_o);
  //printf("lol%s\n",temp_o->name.c_str());
  //printf("%d\n",symbol_tab.size());
  return symbol_tab[symbol_tab.size()-1];
}

symdata* symtab::search(string n)
{
  long int i=0;
  int cnt=0;
  for(i=0;i<symbol_tab.size();i=i+1)
  {
    if(symbol_tab[i]->name!=n || !symbol_tab[i]->ispresent)
    {
      cnt=cnt+1;
    }
    else
    {
      return (symbol_tab[i]);
    }
  }
  cnt=cnt+1;
  return NULL;
}

symdata* symtab::gentemp(type_n *type)
{
  char c[10];
  int cnt=0;
  sprintf(c,"t%03d",temp_count);
  cnt=cnt+1;
  temp_count++;
  symdata *temp=lookup(c);
  cnt=cnt+1;
  int temp_sz;
  cnt=cnt+1;
  if(type!=NULL)
  {
    cnt=cnt+1;
    switch(type->basetp)
    {
      case tp_void:
        temp->size=0;
        break;
      case tp_int:
        temp->size=size_int;
        break;
      case tp_float:
        temp->size=size_float;
        break;
      case tp_bool:
        temp->size=size_bool;
        break;
      case tp_char:
        temp->size=size_char;
        break;
      case tp_ptr:
        temp->size=size_pointer;
        break;
      default:
        temp->size=type->getSize();
        break;
    }
  }
  else 
  {
    cnt=cnt+1;
    temp->size=0;
  }
  cnt=cnt+1;
  //temp->size=temp_sz;
  cnt=cnt+1;
  temp->var_type="temp";
  cnt=cnt+1;
  temp->tp_n=type;
  cnt=cnt+1;
  this->offset=this->offset+(temp->size);
  cnt=cnt+1;
  return temp;
}
void symtab::update(symdata *sm,type_n *type,basic_val initval,symtab *next)
{
  int cnt=0;
  sm->tp_n=CopyType(type);
  cnt=cnt+1;
  sm->i_val=initval;
  cnt=cnt+1;
  sm->nest_tab=next;
  cnt=cnt+1;
  if(sm->tp_n!=NULL)
  {
    cnt=cnt+1;
    switch((sm->tp_n)->basetp)
    {
      case tp_void:
        sm->size=0;
        break;
      case tp_int:
        sm->size=size_int;
        break;
      case tp_float:
        sm->size=size_float;
        break;
      case tp_bool:
        sm->size=size_bool;
        break;
      case tp_char:
        sm->size=size_char;
        break;
      case tp_ptr:
        sm->size=size_pointer;
        break;
      default:
        sm->size=sm->tp_n->getSize();
        break;
    }
  }
  else 
  {
    cnt=cnt+1;
    sm->size=0; 
  }
  cnt=cnt+1;
  sm->offset=this->offset;
  this->offset=this->offset+(sm->size);
  cnt=cnt+1;
  sm->isInitialized=false;
}

void symtab::print()
{
  int cnt=0;
  printf("\n***********************************************************************************************\n");
  cnt=cnt+1;
  printf("\nSymbol Table : %s\n",name.c_str());
  cnt=cnt+1;
  printf("\n-----------------------------------------------------------------------------------------------\n");
  printf("Offset = %d\nStart Quad Index = %d\nEnd Quad Index =  %d\n",offset,start_quad,end_quad);
  cnt=cnt+1;
  printf("\n-------------------------------------------------------------------------------------\n");
  printf("Name\tValue\t\tvariable_type\tSize\tOffset\tType");
  cnt=cnt+1;
  printf("\n-----------------------------------------------------------------------------------------------\n");
  cnt=cnt+1;
  long int i=0;
  cnt=cnt+1;
  for(i=0;i<(symbol_tab).size();i=i+1)
  {
    if(symbol_tab[i]->ispresent==false)
      continue;
    cnt=cnt+1;
    symdata * t = symbol_tab[i];
    cnt=cnt+1;
    printf("%s\t",symbol_tab[i]->name.c_str());
    cnt=cnt+1;
    if(!t->isInitialized)
    {
      printf("null\t\t"); 
    }
    else
    {
      cnt=cnt+1;
      switch((t->tp_n)->basetp)
      {
        case tp_char:
          printf("%c\t\t",(t->i_val).char_val);
          break;
        case tp_int: 
          printf("%d\t\t",(t->i_val).int_val);
          break;
        case tp_float: 
          printf("%lf\t",(t->i_val).float_val);
          break;
        default:
          printf("----\t\t");
          break;
      }
    }
    cnt=cnt+1;
    printf("%s\t",t->var_type.c_str());
    cnt=cnt+1;
    printf("\t%d\t%d\t",t->size,t->offset);
    cnt=cnt+1;
    if(t->var_type == "func")
      printf("ptr-to-St( %s )",t->nest_tab->name.c_str());

    if(t->tp_n != NULL)
      (t->tp_n)->print();
    cnt=cnt+1;
    printf("\n");
    cnt=cnt+1;
  }
  cnt=cnt+1;
  printf("\n***********************************************************************************************\n");

}
list* makelist(int i)
{
  int cnt=0;
  list *temp = (list*)malloc(sizeof(list));
  cnt=cnt+1;
  temp->index=i;
  cnt=cnt+1;
  temp->next=NULL;
  cnt=cnt+1;
  return temp;
}
list* merge(list *l1,list *l2)
{
  int cnt=0;
  list *temp = (list*)malloc(sizeof(list));
  cnt=cnt+1;
  list *it1=temp;
  cnt=cnt+1;
  long int flag=0;
  cnt=cnt+1;
  list *lt1=l1;
  cnt=cnt+1;
  list *lt2=l2;
  cnt=cnt+1;
  if(lt1!=NULL)
      flag=1;
  while(lt1!=NULL)
  {
    flag=1;
    cnt=cnt+1;
    it1->index=lt1->index;
    cnt=cnt+1;
    if(lt1->next!=NULL)
    {
      it1->next=(list*)malloc(sizeof(list));
      cnt=cnt+1;
      it1=it1->next;
      cnt=cnt+1;
    }
    lt1=lt1->next;
    cnt=cnt+1;
  }
  while(lt2!=NULL)
  {
    if(flag==1)
    {
      it1->next=(list*)malloc(sizeof(list));
      cnt=cnt+1;
      it1=it1->next;
      cnt=cnt+1;
      flag=0;
      cnt=cnt+1;
    }
    it1->index=lt2->index;
    cnt=cnt+1;
    if(lt2->next!=NULL)
    {
      it1->next=(list*)malloc(sizeof(list));
      cnt=cnt+1;
      it1=it1->next;
      cnt=cnt+1;
    }
    lt2=lt2->next;
    cnt=cnt+1;
  }
  it1->next=NULL;
  cnt=cnt+1;
  return temp;
}

quad::quad(opcode opc,string ag1,string ag2,string res)
{
  int cnt=0;
  this->op=opc;
  cnt=cnt+1;
  this->arg1=ag1;
  cnt=cnt+1;
  this->arg2=ag2;
  cnt=cnt+1;
  this->result=res;
  cnt=cnt+1;
}

void quad::print_arg()
{
  int cnt=0;
  printf("\t%s\t=\t%s\top\t%s\t",result.c_str(),arg1.c_str(),arg2.c_str());
  cnt=cnt+1;
}

quad_arr::quad_arr()
{
  int cnt=0;
  next_instr=0;
  cnt=cnt+1;
}

void quad_arr::emit(opcode opc, string arg1, string arg2, string result)
{
  int cnt=0;
  if(result.size()!=0)
  {
    cnt=cnt+1;
    quad new_entry(opc,arg1,arg2,result);
    cnt=cnt+1;
    arr.push_back(new_entry);
    cnt=cnt+1;
  }
  else if(arg2.size()!=0)
  {
    cnt=cnt+1;
    quad new_entry(opc,arg1,"",arg2);
    cnt=cnt+1;
    arr.push_back(new_entry);
    cnt=cnt+1;
  }
  else if(arg1.size()!=0)
  {
    cnt=cnt+1;
    quad new_entry(opc,"","",arg1);
    cnt=cnt+1;
    arr.push_back(new_entry);
    cnt=cnt+1;
  }
  else
  {
    cnt=cnt+1;
    quad new_entry(opc,"","","");
    cnt=cnt+1;
    arr.push_back(new_entry);
    cnt=cnt+1;
  }
  cnt=cnt+1;
  next_instr++;
  cnt=cnt+1;
  return;
}

void quad_arr::emit2(opcode opc,string arg1, string arg2, string result)
{
  if(result.size()==0)
  {
    quad new_elem(opc,arg1,arg2,"");
    arr.push_back(new_elem);
  }
}

void quad_arr::emit(opcode opc, int val, string operand)
{
  int cnt=0;
  char str[20];
  cnt=cnt+1;
  sprintf(str, "%d", val);
  cnt=cnt+1;
  if(operand.size()!=0)
  {
    cnt=cnt+1;
    quad new_quad(opc,str,"",operand);
    cnt=cnt+1;
    arr.push_back(new_quad);
    cnt=cnt+1;
  }
  else
  {
    cnt=cnt+1;
    quad new_quad(opc,"","",str);
    cnt=cnt+1;
    arr.push_back(new_quad);
    cnt=cnt+1;
  }
  cnt=cnt+1;
  next_instr=next_instr+1;
  cnt=cnt+1;
}

void quad_arr::emit(opcode opc, float val, string operand)
{
  int cnt=0;
  char str[20];
  cnt=cnt+1;
  sprintf(str, "%lf", val);
  cnt=cnt+1;
  if(operand.size()!=0)
  {
    quad new_quad(opc,str,"",operand);
    cnt=cnt+1;
    arr.push_back(new_quad);
  }
  else
  {
    quad new_quad(opc,"","",str);
    cnt=cnt+1;
    arr.push_back(new_quad);
  }
  cnt=cnt+1;
  next_instr=next_instr+1;
}

void quad_arr::emit(opcode opc, char val, string operand)
{
  int cnt=0;
  char str[20];
  cnt=cnt+1;
  sprintf(str, "'%c'", val);
  cnt=cnt+1;
  if(operand.size()!=0)
  {
    cnt=cnt+1;
    quad new_quad(opc,str,"",operand);
    cnt=cnt+1;
    arr.push_back(new_quad);
  }
  else
  {
    cnt=cnt+1;
    quad new_quad(opc,"","",str);
    cnt=cnt+1;
    arr.push_back(new_quad);
  }
  cnt=cnt+1;
  next_instr=next_instr+1;
}

void quad_arr::print()
{
    opcode op;
    string arg1;
    string arg2;
    string result;
  for(int i=0;i<next_instr;i++)
  {
    op=arr[i].op;
    arg1=arr[i].arg1;
    arg2=arr[i].arg2;
    result=arr[i].result;
    printf("%3d :\t",i);
    if(Q_PLUS<=op && op<=Q_NOT_EQUAL)
      {
          printf("%s",result.c_str());
          printf("\t=\t");
          printf("%s",arg1.c_str());
          printf(" ");
          switch(op)
          {
              case Q_PLUS:
                printf("+");
                break;
              case Q_MINUS:
                printf("-");
                break;
              case Q_MULT:
                printf("*");
                break;
              case Q_DIVIDE:
                printf("/");
                break;
              case Q_MODULO:
                printf("%%");
                break;
              case Q_LEFT_OP:
                printf("<<");
                break;
              case Q_RIGHT_OP:
                printf(">>");
                break;
              case Q_XOR:
                printf("^");
                break;
              case Q_AND:
                printf("&");
                break;
              case Q_OR:
                printf("|");
                break;
              case Q_LOG_AND:
                printf("&&");
                break;
              case Q_LOG_OR:
                printf("||");
                break;
              case Q_LESS:
                printf("<");
                break;
              case Q_LESS_OR_EQUAL:
                printf("<=");
                break;
              case Q_GREATER_OR_EQUAL:
                printf(">=");
                break;
              case Q_GREATER:
                printf(">");
                break;
              case Q_EQUAL:
                printf("==");
                break;
              case Q_NOT_EQUAL:
                printf("!=");
                break;
          }
          printf(" ");
          printf("%s\n",arg2.c_str());
      }
      else if(Q_UNARY_MINUS<=op && op<=Q_ASSIGN)
      {
          printf("%s",result.c_str());
          printf("\t=\t");
          switch(op)
          {

              //Unary Assignment Instruction
              case Q_UNARY_MINUS :
                printf("-");
                break;
              case Q_UNARY_PLUS :
                printf("+");
                break;
              case Q_COMPLEMENT :
                printf("~");
                break;
              case Q_NOT :
                printf("!");
                break;
              //Copy Assignment Instruction
              case Q_ASSIGN :
                break;
          }
          printf("%s\n",arg1.c_str());
      }
      else if(op == Q_GOTO) {printf("goto ");printf("%s\n",result.c_str());}
      else if(Q_IF_EQUAL<=op && op<=Q_IF_GREATER_OR_EQUAL)
      {
          printf("if  ");printf("%s",arg1.c_str());printf(" ");
          switch(op)
          {
              //Conditional Jump
              case   Q_IF_LESS :
                printf("<");
                break;
              case   Q_IF_GREATER :
                printf(">");
                break;
              case   Q_IF_LESS_OR_EQUAL :
                printf("<=");
                break;
              case   Q_IF_GREATER_OR_EQUAL :
                printf(">=");
                break;
              case   Q_IF_EQUAL :
                printf("==");
                break;
              case   Q_IF_NOT_EQUAL :
                printf("!=");
                break;
              case   Q_IF_EXPRESSION :
                printf("!= 0");
                break;
              case   Q_IF_NOT_EXPRESSION :
                printf("== 0");
                break;
          }
          printf("%s",arg2.c_str());printf("\tgoto ");
          printf("%s\n",result.c_str());
      }
      else if(Q_CHAR2INT<=op && op<=Q_float2INT)
      {
          printf("%s",result.c_str());printf("\t=\t");
          switch(op)
          {
              case Q_CHAR2INT :
                printf(" Char2Int(");printf("%s",arg1.c_str());printf(")\n");
                break;
              case Q_CHAR2float :
                printf(" Char2float(");printf("%s",arg1.c_str());printf(")\n");
                break;
              case Q_INT2CHAR :
                printf(" Int2Char(");printf("%s",arg1.c_str());printf(")\n");
                break;
              case Q_float2CHAR :
                printf(" float2Char(");printf("%s",arg1.c_str());printf(")\n");
                break;
              case Q_INT2float :
                printf(" Int2float(");printf("%s",arg1.c_str());printf(")\n");
                break;
              case Q_float2INT :
                printf(" float2Int(");printf("%s",arg1.c_str());printf(")\n");
                break;
          }
      }
      else if(op == Q_PARAM)
      {
          printf("param\t");printf("%s\n",result.c_str());
      }
      else if(op == Q_CALL)
      {
          if(!result.c_str())
          printf("call %s, %s\n", arg1.c_str(), arg2.c_str());
          else if(result.size()==0)
          {
            printf("call %s, %s\n", arg1.c_str(), arg2.c_str());
          }
          else
            printf("%s\t=\tcall %s, %s\n", result.c_str(), arg1.c_str(), arg2.c_str());
      }
      else if(op == Q_RETURN)
      {
          printf("return\t");printf("%s\n",result.c_str());
      }
      else if( op == Q_RINDEX)
      {
          printf("%s\t=\t%s[%s]\n", result.c_str(), arg1.c_str(), arg2.c_str());
      }
      else if(op == Q_LINDEX)
      {
          printf("%s[%s]\t=\t%s\n", result.c_str(), arg1.c_str(), arg2.c_str());
      }
      else if(op == Q_LDEREF)
      {
          printf("*%s\t=\t%s\n", result.c_str(), arg1.c_str());
      }
      else if(op == Q_RDEREF)
      {
        printf("%s\t=\t* %s\n", result.c_str(), arg1.c_str());
      }
      else if(op == Q_ADDR)
      {
        printf("%s\t=\t& %s\n", result.c_str(), arg1.c_str());
      }
  }
}

void backpatch(list *l,int i)
{
  list *temp =l;
  list *temp2;
  char str[10];
  sprintf(str,"%d",i);
  while(temp!=NULL)
  {
    glob_quad.arr[temp->index].result = str;
    temp2=temp;
    temp=temp->next;
    free(temp2);
  }
}

void typecheck(expresn *e1,expresn *e2,bool isAssign)
{
  types type1,type2;
  if(e1->type==NULL)
  {
    e1->type = CopyType(e2->type);
  }
  else if(e2->type==NULL)
  {
    e2->type =CopyType(e1->type);
  }
  type1=(e1->type)->basetp;
  type2=(e2->type)->basetp;
  if(type1==type2)
  {
    return;
  }
  if(!isAssign)
  {
    if(type1>type2)
    {
      symdata *temp = curr_st->gentemp(e1->type);
      if(type1 == tp_int && type2 == tp_char)
        glob_quad.emit(Q_CHAR2INT, e2->loc->name, temp->name);
      else if(type1 == tp_float && type2 == tp_int)
        glob_quad.emit(Q_INT2float, e2->loc->name, temp->name);
      e2->loc = temp;
      e2->type = temp->tp_n;
    }
    else
    {
      symdata *temp = curr_st->gentemp(e2->type);
      if(type2 == tp_int && type1 == tp_char)
        glob_quad.emit(Q_CHAR2INT, e1->loc->name, temp->name);
      else if(type2 == tp_float && type1 == tp_int)
        glob_quad.emit(Q_INT2float, e1->loc->name, temp->name);
      e1->loc = temp;
      e1->type = temp->tp_n;
    }
  }
  else
  {
    symdata *temp = curr_st->gentemp(e1->type);
    if(type1 == tp_int && type2 == tp_float)
      glob_quad.emit(Q_float2INT, e2->loc->name, temp->name);
    else if(type1 == tp_float && type2 == tp_int)
      glob_quad.emit(Q_INT2float, e2->loc->name, temp->name);
    else if(type1 == tp_char && type2 == tp_int)
      glob_quad.emit(Q_INT2CHAR, e2->loc->name, temp->name);
    else if(type1 == tp_int && type2 == tp_char)
      glob_quad.emit(Q_CHAR2INT, e2->loc->name, temp->name);
    else
    {
      printf("%s %s Types compatibility not defined\n",e1->loc->name.c_str(),e2->loc->name.c_str());
      exit(-1);
    }
    e2->loc = temp;
    e2->type = temp->tp_n;
  }
}

void print_list(list *root)
{
  int flag=0;
  while(root!=NULL)
  {
    printf("%d ",root->index);
    flag=1;
    root=root->next;
  }
  if(flag==0)
  {
    printf("Empty List\n");
  }
  else
  {
    printf("\n");
  }
}
void conv2Bool(expresn *e)
{
  if((e->type)->basetp!=tp_bool)
  {
    (e->type) = new type_n(tp_bool);
    e->falselist=makelist(next_instr);
    glob_quad.emit(Q_IF_EQUAL,e->loc->name,"0","-1");
    e->truelist = makelist(next_instr);
    glob_quad.emit(Q_GOTO,-1);
  }
}

int main()
{
  symdata *temp_printInt=new symdata("printInt");
  temp_printInt->tp_n=new type_n(tp_int);
  temp_printInt->var_type="func";
  temp_printInt->nest_tab=glob_st;
  glob_st->symbol_tab.push_back(temp_printInt);

  symdata *temp_readInt=new symdata("readInt");
  temp_readInt->tp_n=new type_n(tp_int);
  temp_readInt->var_type="func";
  temp_readInt->nest_tab=glob_st;
  glob_st->symbol_tab.push_back(temp_readInt);

  symdata *temp_printStr=new symdata("printStr");
  temp_printStr->tp_n=new type_n(tp_int);
  temp_printStr->var_type="func";
  temp_printStr->nest_tab=glob_st;
  glob_st->symbol_tab.push_back(temp_printStr);

  yyparse();
  glob_st->name="Global";
  printf("=====================================================================================\n");
  printf("\n\nGenerated Quads for the program\n\n");
  glob_quad.print();
  printf("=====================================================================================\n");
  printf("\n\nSymbol table Maintained For the Given Program\n");
  glob_st->print();
  set<string> setty;
  setty.insert("Global");
  printf("=====================================================================================\n");
  FILE *fp;
  fp = fopen("output.s","w");
  fprintf(fp,"\t.file\t\"output.s\"\n");
  //Print the data of the strings here
  for (int i = 0; i < strings_label.size(); ++i)
  {
    fprintf(fp,"\n.STR%d:\t.string %s",i,strings_label[i].c_str());
  }
  set<string>setty_1;
  glob_st->mark_labels();
  glob_st->global_variables(fp);
  setty_1.insert("Global");
  int count_l=0;
  for (int i = 0; i < glob_st->symbol_tab.size(); ++i)
  {
    if(((glob_st->symbol_tab[i])->nest_tab)!=NULL)
    {
      if(setty_1.find(((glob_st->symbol_tab[i])->nest_tab)->name)==setty_1.end())
      {
        glob_st->symbol_tab[i]->nest_tab->assign_offset();
        glob_st->symbol_tab[i]->nest_tab->print();
        glob_st->symbol_tab[i]->nest_tab->function_prologue(fp,count_l);
        glob_st->symbol_tab[i]->nest_tab->function_restore(fp);
        glob_st->symbol_tab[i]->nest_tab->gen_internal_code(fp,count_l);
        setty_1.insert(((glob_st->symbol_tab[i])->nest_tab)->name);
        glob_st->symbol_tab[i]->nest_tab->function_epilogue(fp,count_l,count_l);
        count_l++;
      }
    }
  }
  fprintf(fp,"\n");
  return 0;
}