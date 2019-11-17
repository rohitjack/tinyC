#include "ass6_17CS30006_17CS30029_translator.h"
#include "y.tab.h"

extern quad_arr glob_quad;
extern int next_instr;
map<int,int> mp_set;
stack<string> params_stack;
stack<int> types_stack;
stack<int> offset_stack;
stack<int> ptrarr_stack;
extern std::vector< string > vs;
extern std::vector<string> cs;
int add_off;
void symtab::mark_labels()
{
	int cnt=0;
	int count=1;
	cnt=cnt+1;
	int i=0;
	cnt=cnt+1;
	while(i<next_instr)
	{
		if((glob_quad.arr[i].op==Q_GOTO)||(glob_quad.arr[i].op==Q_IF_EQUAL)||(glob_quad.arr[i].op==Q_IF_NOT_EQUAL)
		||(glob_quad.arr[i].op==Q_IF_EXPRESSION)||(glob_quad.arr[i].op==Q_IF_NOT_EXPRESSION)||(glob_quad.arr[i].op==Q_IF_LESS)
		||(glob_quad.arr[i].op==Q_IF_GREATER)||(glob_quad.arr[i].op==Q_IF_LESS_OR_EQUAL)||(glob_quad.arr[i].op==Q_IF_GREATER_OR_EQUAL))
		{
			if(glob_quad.arr[i].result!="-1")
			{
				if(mp_set.find(atoi(glob_quad.arr[i].result.c_str()))==mp_set.end())
				{
					mp_set[atoi(glob_quad.arr[i].result.c_str())]=count;
					cnt=cnt+1;
					count=count+1;
					cnt=cnt+1;
				}
			}
		}
		i=i+1;
		cnt=cnt+1;
	}
}

void symtab::function_prologue(FILE *fp,int count)
{
	int cnt=0;
	fprintf(fp,"\n\t.globl\t%s",name.c_str());
	cnt=cnt+1;
	fprintf(fp,"\n\t.type\t%s, @function",name.c_str());
	cnt=cnt+1;
	fprintf(fp,"\n%s:",name.c_str());
	cnt=cnt+1;
	fprintf(fp,"\n.LFB%d:",count);
	cnt=cnt+1;
	fprintf(fp,"\n\tpushq\t%%rbp");
	cnt=cnt+1;
	fprintf(fp,"\n\tmovq\t%%rsp, %%rbp");
	cnt=cnt+1;
	int t=-offset;
	cnt=cnt+1;
	fprintf(fp,"\n\tsubq\t$%d, %%rsp",t);
	cnt=cnt+1;
}

void symtab::global_variables(FILE *fp)
{
	int i=0;
	int cnt=0;
	while(i<symbol_tab.size())
	{
		if(symbol_tab[i]->name[0]!='t' &&symbol_tab[i]->tp_n!=NULL&&symbol_tab[i]->var_type!="func")
		{
			if(symbol_tab[i]->tp_n->basetp==tp_char)
			{
				cs.push_back(symbol_tab[i]->name);
				cnt=cnt+1;
				if(symbol_tab[i]->isInitialized!=false)
				{
					fprintf(fp,"\n\t.globl\t%s",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n\t.data");
					cnt=cnt+1;
					fprintf(fp,"\n\t.type\t%s, @object",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n\t.size\t%s ,1",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n%s:",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n\t.byte %c",symbol_tab[i]->i_val.char_val);
					cnt=cnt+1;
				}
				else
				{
					fprintf(fp,"\n\t.comm\t%s,1,1",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
				}
		    }
			if(symbol_tab[i]->tp_n->basetp==tp_int)
			{
				vs.push_back(symbol_tab[i]->name);
				cnt=cnt+1;
				if(symbol_tab[i]->isInitialized!=false)
				{
					fprintf(fp,"\n\t.globl\t%s",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n\t.data");
					cnt=cnt+1;
					fprintf(fp,"\n\t.align 4");
					cnt=cnt+1;
					fprintf(fp,"\n\t.type\t%s, @object",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n\t.size\t%s ,4",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n%s:",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
					fprintf(fp,"\n\t.long %d",symbol_tab[i]->i_val.int_val);
					cnt=cnt+1;
				}
				else
				{
					fprintf(fp,"\n\t.comm\t%s,4,4",symbol_tab[i]->name.c_str());
					cnt=cnt+1;
				}
		    }
		    
		}
		i=i+1;
		cnt=cnt+1;
	}
	fprintf(fp,"\n\t.text");
}
void symtab::assign_offset()
{
	int curr_offset=0;
	int cnt=0;
	int param_offset=16;
	no_params=0;
	cnt=cnt+1;
	int i=(symbol_tab).size()-1;
	cnt=cnt+1;
	for(;i>=0;i=i-1)
    {
        if(!symbol_tab[i]->ispresent)
        	continue;
        if(symbol_tab[i]->var_type=="param" && symbol_tab[i]->isdone==false)
        {
        	no_params=no_params+1;
        	cnt=cnt+1;
        	if(symbol_tab[i]->tp_n && symbol_tab[i]->tp_n->basetp==tp_arr)
        	{
        		if(symbol_tab[i]->tp_n->size==-1)
        		{
        			symbol_tab[i]->isptrarr=true;
        		}
        		symbol_tab[i]->size=8;
        	}
        	symbol_tab[i]->offset=curr_offset-symbol_tab[i]->size;
        	cnt=cnt+1;
        	curr_offset=curr_offset-symbol_tab[i]->size;
        	cnt=cnt+1;
        	symbol_tab[i]->isdone=true;
        	cnt=cnt+1;
        }
        if(no_params==6)
        	break;
    }
    i=0;
    cnt=cnt+1;
    for(;i<(symbol_tab).size();i=i+1)
    {
        if(!symbol_tab[i]->ispresent)
        	continue;
        if(symbol_tab[i]->var_type=="param" && symbol_tab[i]->isdone==false)
        {
        	if(symbol_tab[i]->tp_n && symbol_tab[i]->tp_n->basetp==tp_arr)
        	{
        		if(symbol_tab[i]->tp_n->size==-1)
        		{
        			symbol_tab[i]->isptrarr=true;
        			cnt=cnt+1;
        			symbol_tab[i]->size=8;
        			cnt=cnt+1;
        		}
        		else
        		{
        			symbol_tab[i]->size=8;
        			cnt=cnt+1;
        		}
        		symbol_tab[i]->offset=param_offset;
        		cnt=cnt+1;
	        	param_offset=param_offset+symbol_tab[i]->size;
	        	cnt=cnt+1;
	        	symbol_tab[i]->isdone=true;
	        	cnt=cnt+1;
	        	no_params=no_params+1;
	        	cnt=cnt+1;
        	}
        	else
        	{
        		symbol_tab[i]->offset=param_offset;
        		cnt=cnt+1;
        		param_offset=param_offset+symbol_tab[i]->size;
        		cnt=cnt+1;
        		symbol_tab[i]->isdone=true;
        		cnt=cnt+1;
        		no_params=no_params+1;
        		cnt=cnt+1;
        	}
        }
        else if(symbol_tab[i]->var_type!="return"&&symbol_tab[i]->var_type!="param" && symbol_tab[i]->isdone==false)
        {
        	symbol_tab[i]->offset=curr_offset-symbol_tab[i]->size;
        	cnt=cnt+1;
        	symbol_tab[i]->isdone=true;
        	cnt=cnt+1;
        	curr_offset=curr_offset-symbol_tab[i]->size;
        	cnt=cnt+1;
        }
    }
    offset=curr_offset;
    cnt=cnt+1;
}
string symtab::assign_reg(int type_of,int no)
{
	int cnt=0;
	string s="NULL";
	cnt=cnt+1;
    if(type_of == tp_int)
    {
    	if(no==5)
            return s = "r9d";
        cnt=cnt+1;
        if(no==4)
            return s = "r8d";
        cnt=cnt+1;
        if(no==3)
            return s = "ecx";
        cnt=cnt+1;
        if(no==2)
            return s = "edx";
        cnt=cnt+1;
        if(no==1)
            return s = "esi";
        cnt=cnt+1;
        if(no==0)
            return s = "edi";
        cnt=cnt+1;
    }
	else if(type_of==tp_char)
    {
    	if(no==5)
            return s = "r9b";
        cnt=cnt+1;
        if(no==4)
            return s = "r8b";
        cnt=cnt+1;
        if(no==3)
            return s = "cl";
        cnt=cnt+1;
        if(no==2) 
            return s = "dl";
        cnt=cnt+1;
        if(no==1)
            return s = "sil";
        cnt=cnt+1;
        if(no==0) 
           return s = "dil";
       	cnt=cnt+1;
    }
    else
    {
    	if(no==5)
           return s = "r9";
       	cnt=cnt+1;
       	if(no==4)
           return s = "r8";
       	cnt=cnt+1;
       	if(no==3)
           return s = "rcx";
       	cnt=cnt+1;
        if(no==2)
            return s = "rdx";
        cnt=cnt+1;
        if(no==1)
            return s = "rsi";
        cnt=cnt+1;
        if(no==0)
            return s = "rdi";
        cnt=cnt+1;
    }
}
int symtab::function_call(FILE *fp)
{

    int c=0;
    int cnt=0;
    cnt=cnt+1;
    fprintf(fp,"\n\tpushq %%rbp");
    cnt=cnt+1;
    int count=0;
    cnt=cnt+1;
    while(params_stack.size())
    {
        if(count>=6)
            break;
        string p=params_stack.top();
        cnt=cnt+1;
        int btp=types_stack.top();
        cnt=cnt+1;
        int off=offset_stack.top();
        cnt=cnt+1;
        int parr=ptrarr_stack.top();
        cnt=cnt+1;
        params_stack.pop();
        cnt=cnt+1;
        types_stack.pop();
        cnt=cnt+1;
        offset_stack.pop();
        cnt=cnt+1;
        ptrarr_stack.pop();
        cnt=cnt+1;
        string temp_str=assign_reg(btp,count);
        if(temp_str!="NULL")
        {
            //printf("%s Basetype--> %d--> %d\n",p.c_str(),btp,tp_int);
            if(btp==tp_arr)
            {
            	cnt=cnt+1;
                fprintf(fp,"\n\tleaq\t%d(%%rbp), %%%s",off,temp_str.c_str());
            }
            else if(btp==tp_arr && parr==1)
            {
            	cnt=cnt+1;
                fprintf(fp,"\n\tmovq\t%d(%%rbp), %%%s",off,temp_str.c_str());
            }
            else if(btp==tp_char)
            {
            	cnt=cnt+1;
                fprintf(fp,"\n\tmovb\t%d(%%rbp), %%%s",off,temp_str.c_str());
            }
            else if(btp==tp_int)
            {
            	cnt=cnt+1;
                fprintf(fp,"\n\tmovl\t%d(%%rbp) , %%%s",off,temp_str.c_str());
            } 
            else
            {
            	cnt=cnt+1;
                fprintf(fp,"\n\tmovq\t%d(%%rbp), %%%s",off,temp_str.c_str());
            }
            cnt=cnt+1;
            count=count+1;
        }
    }
    while(params_stack.size()>0)
    {
    	cnt=cnt+1;
        string p=params_stack.top();
        cnt=cnt+1;
        int btp=types_stack.top();
        cnt=cnt+1;
        int off=offset_stack.top();
        cnt=cnt+1;
        int parr=ptrarr_stack.top();
        cnt=cnt+1;
        params_stack.pop();
        cnt=cnt+1;
        types_stack.pop();
        cnt=cnt+1;
        offset_stack.pop();
        cnt=cnt+1;
        ptrarr_stack.pop();
        if(btp==tp_char)
        {
        	cnt=cnt+1;
            fprintf(fp,"\n\tsubq $4, %%rsp");
            cnt=cnt+1;
            fprintf(fp,"\n\tmovsbl\t%d(%%rbp), %%eax",off);
            cnt=cnt+1;
            fprintf(fp,"\n\tmovl\t%%eax, (%%rsp)");
            cnt=cnt+1;
            c=c+4;
        }
        else if(btp==tp_arr)
        {
        	cnt=cnt+1;
            fprintf(fp,"\n\tsubq $8, %%rsp");
            cnt=cnt+1;
            fprintf(fp,"\n\tleaq\t%d(%%rbp), %%rax",off);
            cnt=cnt+1;
            fprintf(fp,"\n\tmovq\t%%rax, (%%rsp)");
            cnt=cnt+1;
            c=c+8;
        }
        else if(btp==tp_arr && parr==1)
        {
        	cnt=cnt+1;
            fprintf(fp,"\n\tsubq $8, %%rsp");
            cnt=cnt+1;
            fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rax",off);
            cnt=cnt+1;
            fprintf(fp,"\n\tmovq\t%%rax, (%%rsp)");
            cnt=cnt+1;
            c=c+8;
        }
        else if(btp==tp_int)
        {
        	cnt=cnt+1;
            fprintf(fp,"\n\tsubq $4, %%rsp");
            cnt=cnt+1;
            fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off);
            cnt=cnt+1;
            fprintf(fp,"\n\tmovl\t%%eax, (%%rsp)");
            cnt=cnt+1;
            c=c+4;
        }
        else
        {
        	cnt=cnt+1;
            fprintf(fp,"\n\tsubq $8, %%rsp");
            cnt=cnt+1;
            fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rax",off);
            cnt=cnt+1;
            fprintf(fp,"\n\tmovq\t%%rax, (%%rsp)");
            cnt=cnt+1;
            c=c+8;
        }
    }
    return c;

}
void symtab::function_restore(FILE *fp)
{
	int count=0;
	int cnt=0;
	string regname;
	cnt=cnt+1;
	int i=symbol_tab.size()-1;
	cnt=cnt+1;
	for(;i>=0;i=i-1)
	{
	    if(!symbol_tab[i]->ispresent)
	    	continue;
	    if(symbol_tab[i]->var_type=="param" && symbol_tab[i]->offset<0)
	    {
	    	if(symbol_tab[i]->tp_n->basetp == tp_int)
	    	{
	            regname = assign_reg(tp_int,count);
	            cnt=cnt+1;
	            fprintf(fp,"\n\tmovl\t%%%s, %d(%%rbp)",regname.c_str(),symbol_tab[i]->offset);
	            cnt=cnt+1;
	        }
		    else if(symbol_tab[i]->tp_n->basetp == tp_char)
		    {
	            regname = assign_reg(tp_char,count);
	            cnt=cnt+1;
	            fprintf(fp,"\n\tmovb\t%%%s, %d(%%rbp)",regname.c_str(),symbol_tab[i]->offset);
	            cnt=cnt+1;
	        }
	        else 
	        {
	            regname = assign_reg(10,count);
	            cnt=cnt+1;
	            fprintf(fp,"\n\tmovq\t%%%s, %d(%%rbp)",regname.c_str(),symbol_tab[i]->offset);
	            cnt=cnt+1;
	        }
	    	count=count+1;
	    	cnt=cnt+1;
	    }
	    if(count>5)
	    	break;
    }
}
void symtab::gen_internal_code(FILE *fp,int ret_count)
{
	int i = start_quad;
	int cnt=0;
	for(;i <=end_quad; i=i+1)
	{
		opcode &opx =glob_quad.arr[i].op;
		cnt=cnt+1;
		string &arg1x =glob_quad.arr[i].arg1;
		cnt=cnt+1;
		string &arg2x =glob_quad.arr[i].arg2;
		cnt=cnt+1;
		string &resx =glob_quad.arr[i].result;
		cnt=cnt+1;
		int flag[3];
		cnt=cnt+1;
		int offr,off1,off2,j;
		cnt=cnt+1;
		int k=0;
		cnt=cnt+1;
		for(k=0;k<3;k=k+1)
			flag[k]=1;
		fprintf(fp,"\n# %d:",i);
		cnt=cnt+1;
		if(search(resx))
		{
			offr = search(resx)->offset;
			cnt=cnt+1;
			fprintf(fp,"res = %s ",search(resx)->name.c_str());
			cnt=cnt+1;
		}
		else if(glob_quad.arr[i].result!=""&& findg(glob_quad.arr[i].result))
		{
			flag[2]=0;
			cnt=cnt+1;
		}
		if(search(arg1x))
		{
			off1 = search(arg1x)->offset;
			cnt=cnt+1;
			fprintf(fp,"arg1 = %s ",search(arg1x)->name.c_str());
			cnt=cnt+1;
		}
		else if(glob_quad.arr[i].arg1!="" && findg(glob_quad.arr[i].arg1))
		{
			flag[0]=0;
			cnt=cnt+1;
		}
		if(search(arg2x))
		{
			off2 = search(arg2x)->offset;
			cnt=cnt+1;
			fprintf(fp,"arg2 = %s ",search(arg2x)->name.c_str());
			cnt=cnt+1;
		}
		else if(glob_quad.arr[i].arg2!="" && findg(glob_quad.arr[i].arg2))
		{
			flag[1]=0;
			cnt=cnt+1;
		}
		if(flag[0]!=1)
		{
			if(findg(arg1x)!=2)
				fprintf(fp,"\n\tmovl\t%s(%%rip), %%eax",arg1x.c_str());
			else
				fprintf(fp,"\n\tmovzbl\t%s(%%rip), %%eax",arg1x.c_str());

			cnt=cnt+1;
		}
		if(flag[1]!=1)
		{
			if(findg(arg1x)!=2)
				fprintf(fp,"\n\tmovl\t%s(%%rip), %%edx",arg2x.c_str());
			else
				fprintf(fp,"\n\tmovzbl\t%s(%%rip), %%edx",arg2x.c_str());	
		}
		if(mp_set.find(i)!=mp_set.end())
		{
			fprintf(fp,"\n.L%d:",mp_set[i]);
			cnt=cnt+1;
		}
		if(opx==Q_MULT)
		{
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				if(flag[0]==1)
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				if(flag[1]==1)
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\timull\t%%edx, %%eax");
				cnt=cnt+1;
				if(flag[2]==1)
					fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				else
					fprintf(fp,"\n\tmovb\t%%al, %s(%%rip)",resx.c_str());
			}
			else
			{
				if(flag[0]==1)
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				if(flag[1]==1)
				{
					if(arg2x[0]>='0' && arg2x[0]<='9')
					{
						cnt=cnt+1;
						fprintf(fp,"\n\tmovl\t$%s, %%ecx",arg2x.c_str());
						cnt=cnt+1;
						fprintf(fp,"\n\timull\t%%ecx, %%eax");
					}
					else
						fprintf(fp,"\n\timull\t%d(%%rbp), %%eax",off2);
				}
				if(flag[2]==0)
					fprintf(fp,"\n\tmovl\t%%eax, %s(%%rip)",resx.c_str());
				else
					fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
			}
		}
		if(opx==Q_PLUS)
		{
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				if(flag[0]==1)
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				if(flag[1]==1)
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\taddl\t%%edx, %%eax");
				cnt=cnt+1;
				if(flag[2]==1)
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				else
					fprintf(fp,"\n\tmovb\t%%al, %s(%%rip)",resx.c_str());
			}
			else
			{
				if(flag[0]==1)
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				if(flag[1]==1)
				{
					if(arg2x[0]>='0' && arg2x[0]<='9')
						fprintf(fp,"\n\tmovl\t$%s, %%edx",arg2x.c_str());
					else
						fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				}
				fprintf(fp,"\n\taddl\t%%edx, %%eax");
				cnt=cnt+1;
				if(flag[2]==1)
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				else
					fprintf(fp,"\n\tmovl\t%%eax, %s(%%rip)",resx.c_str());
			}
		}
		if(opx==Q_MINUS)
		{
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				if(flag[0]==1)
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				if(flag[1]==1)
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tsubl\t%%edx, %%eax");
				cnt=cnt+1;
				if(flag[2]==1)
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				else
					fprintf(fp,"\n\tmovb\t%%al, %s(%%rip)",resx.c_str());
			}
			else
			{
				if(flag[0]==1)
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				// Direct Number access
				if(flag[1]==1)
				{if(arg2x[0]>='0' && arg2x[0]<='9')
					fprintf(fp,"\n\tmovl\t$%s, %%edx",arg2x.c_str());
				else
					fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);}
				fprintf(fp,"\n\tsubl\t%%edx, %%eax");
				cnt=cnt+1;
				if(flag[2]==1)
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				else
					fprintf(fp,"\n\tmovl\t%%eax, %s(%%rip)",resx.c_str());
				cnt=cnt+1;

			}
		}
		if(opx==Q_UNARY_MINUS)
		{
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tnegl\t%%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tnegl\t%%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
		}
		if(opx==Q_DIVIDE)
		{
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				if(flag[0]==1)
					fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				fprintf(fp,"\n\tcltd");
				cnt=cnt+1;
				if(flag[1]!=1)
					fprintf(fp,"\n\tidivl\t%%edx, %%eax");
				else
					fprintf(fp,"\n\tidivl\t%d(%%rbp), %%eax",off2);
				if(flag[2]!=1)
					fprintf(fp,"\n\tmovb\t%%al, %s(%%rip)",resx.c_str());
				else
					fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
			}
			else
			{
				if(flag[0]==1)
					fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				fprintf(fp,"\n\tcltd");
				cnt=cnt+1;
				if(flag[1]!=1)
					fprintf(fp,"\n\tidivl\t%%edx, %%eax");
				else
					fprintf(fp,"\n\tidivl\t%d(%%rbp), %%eax",off2);
				if(flag[2]!=1)
					fprintf(fp,"\n\tmovl\t%%eax, %s(%%rip)",resx.c_str());
				else
					fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
			}
		}
		if(opx==Q_MODULO)
		{
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcltd");
				cnt=cnt+1;
				fprintf(fp,"\n\tidivl\t%d(%%rbp), %%eax",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcltd");
				cnt=cnt+1;
				fprintf(fp,"\n\tidivl\t%d(%%rbp), %%eax",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%%edx, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
		}
		if(opx==Q_GOTO)
		{
			if(resx!="-1"&& atoi(resx.c_str())<=end_quad)
				fprintf(fp,"\n\tjmp .L%d",mp_set[atoi(resx.c_str())]);
			else
				fprintf(fp,"\n\tjmp\t.LRT%d",ret_count);
		}
		if(opx==Q_ASSIGN)
		{
			//Check if the second argument is a constant
			if(arg1x[0]>='0' && arg1x[0]<='9')	//first character is number
			{
				if(flag[0]!=0)
				fprintf(fp,"\n\tmovl\t$%s, %d(%%rbp)",arg1x.c_str(),offr);

				cnt=cnt+1;
			}
			else if(arg1x[0] == '\'')
			{
				//Character
				fprintf(fp,"\n\tmovb\t$%d, %d(%%rbp)",(int)arg1x[1],offr);
				cnt=cnt+1;
			}
			else if(flag[0] && search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else if(flag[0]&&search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_int)
			{
				if(flag[0]!=0)
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);

				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else if(search(resx)!=NULL && search(resx)->tp_n!=NULL)
			{
				fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovq\t%%rax, %d(%%rbp)",offr);
				cnt=cnt+1;	
			}
			else
			{
				if(flag[2]!=0)
				{
					fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
					cnt=cnt+1;
					fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
					cnt=cnt+1;
				}
				else
				{
					fprintf(fp,"\n\tmovl\t%%eax, %s(%%rip)",resx.c_str());
					cnt=cnt+1;
				}
			}
		}
		if(opx==Q_PARAM)
		{
			if(resx[0] != '_')
			{
				params_stack.push(resx);
				cnt=cnt+1;
				types_stack.push(search(resx)->tp_n->basetp);
				cnt=cnt+1;
				offset_stack.push(offr);
				cnt=cnt+1;
				if(search(resx)->isptrarr==false)
				{
					ptrarr_stack.push(0);
					cnt=cnt+1;
				}
				else
				{
					ptrarr_stack.push(1);
					cnt=cnt+1;	
				}
				
			}
			else
			{
				char* temp = (char*)resx.c_str();
				cnt=cnt+1;
				fprintf(fp,"\n\tmovq\t$.STR%d,\t%%rdi",atoi(temp+1));	
				cnt=cnt+1;
			}
		}
		if(opx==Q_IF_LESS_OR_EQUAL)
		{
			if(search(arg1x)!=NULL && search(arg1x)->tp_n!=NULL&&search(arg1x)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpb\t%d(%%rbp), %%al",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tjle .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tjle .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
		}
		if(opx==Q_CALL)
		{
			add_off=function_call(fp);
			fprintf(fp,"\n\tcall\t%s",arg1x.c_str());
			if(resx=="")
			{
				cnt=cnt+1;
			}
			else if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_int)
			{
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else if(search(resx)!=NULL && search(resx)->tp_n!=NULL)
			{
				fprintf(fp,"\n\tmovq\t%%rax, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			if(arg1x=="printStr")
			{
				fprintf(fp,"\n\taddq $8 , %%rsp");
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\taddq $%d , %%rsp",add_off);
				cnt=cnt+1;
			}
		}
		if(opx==Q_IF_LESS)
		{
			if(search(arg1x)!=NULL && search(arg1x)->tp_n!=NULL&&search(arg1x)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpb\t%d(%%rbp), %%al",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tjl .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tjl .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
		}
		if(opx==Q_IF_EQUAL)
		{
			if(search(arg1x)!=NULL && search(arg1x)->tp_n!=NULL&&search(arg1x)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpb\t%d(%%rbp), %%al",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tje .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tje .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
		}
		if(opx==Q_IF_GREATER)
		{
			if(search(arg1x)!=NULL && search(arg1x)->tp_n!=NULL&&search(arg1x)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpb\t%d(%%rbp), %%al",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tjg .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tjg .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
		}
		if(opx==Q_IF_GREATER_OR_EQUAL)
		{
			if(search(arg1x)!=NULL && search(arg1x)->tp_n!=NULL&&search(arg1x)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpb\t%d(%%rbp), %%al",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tjge .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tjge .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
		}
		if(opx==Q_IF_NOT_EQUAL)
		{
			if(search(arg1x)!=NULL && search(arg1x)->tp_n!=NULL&&search(arg1x)->tp_n->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpb\t%d(%%rbp), %%al",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tjne .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tcmpl\t%%edx, %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tjne .L%d",mp_set[atoi(resx.c_str())]);
				cnt=cnt+1;
			}
		}
		if(opx==Q_RDEREF)
		{
			fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rax",off1);
			cnt=cnt+1;
			fprintf(fp,"\n\tmovl\t(%%rax), %%eax");
			cnt=cnt+1;
			fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
			cnt=cnt+1;
		}
		if(opx==Q_ADDR)
		{
			fprintf(fp,"\n\tleaq\t%d(%%rbp), %%rax",off1);
			cnt=cnt+1;
			fprintf(fp,"\n\tmovq\t%%rax, %d(%%rbp)",offr);
			cnt=cnt+1;
		}
		if(opx==Q_LDEREF)
		{
			fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rax",offr);
			cnt=cnt+1;
			fprintf(fp,"\n\tmovl\t%d(%%rbp), %%edx",off1);
			cnt=cnt+1;
			fprintf(fp,"\n\tmovl\t%%edx, (%%rax)");
			cnt=cnt+1;
		}
		if(opx==Q_RETURN)
		{
			//printf("return %s\n",resx.c_str());
			if(resx=="")
			{
				fprintf(fp,"\n\tmovl\t$0, %%eax");
				cnt=cnt+1;
			}
			else
			{
				if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->basetp == tp_char)
				{
					fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",offr);
					cnt=cnt+1;
				}
				else
				{
					fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",offr);
					cnt=cnt+1;
				}
			}
			//printf("Happy\n");
			fprintf(fp,"\n\tjmp\t.LRT%d",ret_count);
			cnt=cnt+1;
		}
		if(opx==Q_RINDEX)
		{
			// Get Address, subtract offset, get memory
			if(search(arg1x)&&search(arg1x)->isptrarr==false)
			{
				fprintf(fp,"\n\tleaq\t%d(%%rbp), %%rdx",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovslq\t%d(%%rbp), %%rax",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\taddq\t%%rax, %%rdx");
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rdx",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovslq\t%d(%%rbp), %%rax",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\taddq\t%%rax, %%rdx");
				cnt=cnt+1;					
			}
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->next&&search(resx)->tp_n->next->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t(%%rdx), %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tmovb\t%%al, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t(%%rdx), %%eax");
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%%eax, %d(%%rbp)",offr);
				cnt=cnt+1;
			}
		}
		if(opx==Q_LINDEX)
		{
			// Get Address, subtract offset, get memory
			if(search(resx)&&search(resx)->isptrarr==false)
			{
				fprintf(fp,"\n\tleaq\t%d(%%rbp), %%rdx",offr);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovslq\t%d(%%rbp), %%rax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\taddq\t%%rax, %%rdx");
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovq\t%d(%%rbp), %%rdx",offr);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovslq\t%d(%%rbp), %%rax",off1);
				cnt=cnt+1;
				fprintf(fp,"\n\taddq\t%%rax, %%rdx");
				cnt=cnt+1;
			}
			if(search(resx)!=NULL && search(resx)->tp_n!=NULL&&search(resx)->tp_n->next && search(resx)->tp_n->next->basetp == tp_char)
			{
				fprintf(fp,"\n\tmovzbl\t%d(%%rbp), %%eax",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovb\t%%al, (%%rdx)");
				cnt=cnt+1;
			}
			else
			{
				fprintf(fp,"\n\tmovl\t%d(%%rbp), %%eax",off2);
				cnt=cnt+1;
				fprintf(fp,"\n\tmovl\t%%eax, (%%rdx)");
				cnt=cnt+1;
			}
		}	
		
	}
}

void symtab::function_epilogue(FILE *fp,int count,int ret_count)
{
	fprintf(fp,"\n.LRT%d:",ret_count);
	fprintf(fp,"\n\taddq\t$%d, %%rsp",offset);
	fprintf(fp,"\n\tmovq\t%%rbp, %%rsp");
	fprintf(fp,"\n\tpopq\t%%rbp");
	fprintf(fp,"\n\tret");
	fprintf(fp,"\n.LFE%d:",count);
	fprintf(fp,"\n\t.size\t%s, .-%s",name.c_str(),name.c_str());
}
