int printStr(char *s)//prints function it inputs strings till '\0'
{
	int i;
	i = 0;
	int cnt = 0;
	char to_be_printed[10001];
	while(s[i]!='\0')
	{
		to_be_printed[i] = s[i];
		i++;
		cnt++;
	}
	/*inline asm commands for system call to print "to_be_printed" till "cnt" length to STDOUT*/
	__asm__ __volatile__ (
	"movl $1, %%eax \n\t"
	"movq $1, %%rdi \n\t"
	"syscall \n\t"
	:
	:"S"(to_be_printed),"d"(cnt)
	);
	return cnt;
}




int printInt(int n)//function to print int to STDOUT
{
	int i = 0;
	char to_be_printed[10001];
	int flag = 0;
	int n1 = n;
	int cnt = 0;
	if(n<0)flag = 1;		//if negative number, flag = 1 for future use
	if(flag){n1=-n;cnt++;}	//n1 = absolute(n) now
	if(n1==0)cnt=1;
	while(n1!=0)
	{
		n1=n1/10;		//finding the number of digits in n except '-'
		cnt++;
	}
	n1=n;
	if(flag)n1=-n;
	if(flag)to_be_printed[i++] = '-';		//first charecter is '-' if flag = 1
	if(n1==0)to_be_printed[i++]='0';		//if n is zero then the only charecter is '0'
	while(n1!=0)
	{
		int d = n1%10;		//(cnt-i)th digit
		if(flag)to_be_printed[cnt-i] = (char)( d + '0');	//adding the (cnt-i)th digit at (cnt-i) position
		else to_be_printed[cnt-i-1] = (char)( d + '0');		//adding the (cnt-i)th digit at (cnt-i) position
		i++;
		n1 = n1/10;		//removes remainder
	}
	//inline asm commands for system call to print "to_be_printed" till "cnt" length to STDOUT
	__asm__ __volatile__ (
	"movl $1, %%eax \n\t"
	"movq $1, %%rdi \n\t"
	"syscall \n\t"
	:
	:"S"(to_be_printed),"d"(cnt)
	);
	return cnt;
}


int printFloat(float f)
{
	int i = 0;
	char to_be_printed[10001];
	int flag = 0;
	if(f == (int)f)return printInt((int)f);		//if it is an integer, print integer
	float f1 = f;
	int cnt = 0;
	int count = 0;
	if(f<0)flag = 1;	//if negative number, flag = 1 for future use
	if(flag){f1=-f;cnt++;}		//f1 = abs(f)
	if(f1==0)cnt=1;
	while (f1!=(int)f1) {/*checking if the decimal point is reached in our iteration */
		f1*=10;
		count++;/*stores the distance of the decimal point from right hand side of the number*/
	}
	int n = (int)f1;
	while(n!=0)
	{
		n=n/10;		//counting no. of digits in numerical value of new f1
		cnt++;
	}
	n = (int)f1;
	if(flag)to_be_printed[i++] = '-';	//first charecter is '-' if flag = 1
	if(f1==0)to_be_printed[i++]='0';	//if f is zero then the only charecter is '0'	
	while(n!=0)
	{
		int d = n%10;	//accessing (cnt-i)th digit
		if(flag)
		{
			if(count==0)
			{
				to_be_printed[cnt+1-i] = '.';	//adding the '.' on cnt-i+1 position of array
				count--;		//'.' already done
				i++;
				continue;		
			}
			else if(count > 0)
			{
				to_be_printed[cnt+1-i] = (char)( d + '0');		//adding the 'digit' on cnt-i+1 position of array
				count--;		//decreasing the distance from '.'
			}
			else 
			{
				to_be_printed[cnt+1-i] = (char)( d + '0');		//adding the digit on cnt-i+1 position of array
			}
		}
		else 
		{
			if(count==0)
			{
				to_be_printed[cnt-i] = '.';		//adding the '.' on cnt-i position of array
				count--;		//'.' already done
				i++;
				continue;		
			}
			else if(count > 0)
			{
				to_be_printed[cnt-i] = (char)( d + '0');		//adding the digit on cnt-i position of array
				count--;		//decreasing the distance from '.'
			}
			else 
			{
				to_be_printed[cnt-i] = (char)( d + '0');		//adding the digit on cnt-i position of array
			}
		}
		i++;
		n = n/10;
	}
	int y = 0;
	if(count==0)		//special case for abs(f)<1
	{
		if(flag)	//f<0
		{
			to_be_printed[cnt+1-i] = '.';
			count--;
			for(i=cnt;i>=1;i--)to_be_printed[i+1] = to_be_printed[i];		//each charecter shifted one position to the right
			cnt += 1;		//increase cnt
			to_be_printed[0]='-';	//first charecter is '-'
			to_be_printed[1] = '0';	//second charecter is '0'
			y = 1;
		}
		else		//f>0
		{
			to_be_printed[cnt-i] = '.';
			count--;
			for(i=cnt;i>=0;i--)to_be_printed[i+1] = to_be_printed[i];		//each charecter shifted one position to the right
			cnt+=1;		//increase cnt
			to_be_printed[0] = '0';		//first charecter is '0'
		}		
	}
	cnt++;	//increase by 1 because of '.'
	if(f1==0)
	{
		cnt = 1;
		to_be_printed[0] = '0';
	}
	//inline asm commands for system call to print "to_be_printed" till "cnt" length to STDOUT
	__asm__ __volatile__ (
	"movl $1, %%eax \n\t"
	"movq $1, %%rdi \n\t"
	"syscall \n\t"
	:
	:"S"(to_be_printed),"d"(cnt)
	);
	//if(y)return cnt-1;
	return cnt;	
}


int readInt(int* n)
{
	char tmp[1];
	char x[200];
	int val=0,cnt=0,i;
	int success = 1;
	while (1) {
	
	 __asm__ __volatile__ ("syscall"::"a"(0), "D"(0), "S"(tmp), "d"(1));/*reading inputs one by one from STDIN to to_be_printed*/
		if(tmp[0] == '\t' || tmp[0] == '\n' || tmp[0] == ' ') break;/*breaks at the first encounter of whitespace*/
		else if (((int)tmp[0] - '0' > 9 || (int)tmp[0] - '0' < 0) && tmp[0] != '-') success = 0;/*only '-' or digits are allowed, else error*/
		else
		{
			x[cnt++] = tmp[0]; 
		}
	}
	if(cnt > 9 || cnt == 0)		/*less than 9 bits allowed, keeping in mind the range of int in C*/
	{
		success = 0;
		*n = 0;
		return success;
	}

	if (x[0] == '-') 
	{
		if(cnt == 1) 
		{
			*n = 0;
			success = 0;
			return success;
		}
		for (i = 1;i < cnt;i++) 
		{
			if(x[i] == '-') success = 0;		/*a number can contain '-' only at the starting of the number*/
			val *= 10;
			val += (int)x[i] - '0';
		}
		val = -val;
	}
	else{
		for (i = 0;i < cnt;i++) {
			if (x[i] == '-') success = 0;		/*a number can contain '-' only at the starting of the number*/
			val *= 10;
			val += (int)x[i]-'0';
		}
	}
	*n = val;
	return success;
}

int readFloat(float *f)
{
	char tmp[1];
	char x[200];
	float val=0.0;
	int cnt=0,i,flag=0;

	while (1) 
	{
		/*inline asm commands for system call to read to "tmp" till "1" length from STDIN*/
	 __asm__ __volatile__ ("syscall"::"a"(0), "D"(0), "S"(tmp), "d"(1));/*reading inputs one by one from STDIN to tmp*/
		if(tmp[0]=='\t'||tmp[0]=='\n'||tmp[0]==' ') break;/*breaks at the first encounter of whitespace*/
		else if (((int)tmp[0]-'0'>9||(int)tmp[0]-'0'<0) && tmp[0]!='-' && tmp[0]!='.') return 0;/*only '-','.' or digits are allowed, else error*/
		else
		{
			x[cnt++]=tmp[0]; 
		}
	}
	float mul = 1.0;
	if(cnt > 12 || cnt == 0){/*float is limited to 12 bits to avoid overflow*/
		*f = 0.0;
		return 0;
	}
	if(x[0] == '.') return 0;
	if (x[0] == '-') {
		if(cnt == 1) {
			*f = 0.0;
			return 0;
		}
		if(x[1] == '.') return 0;
		for (i = 1;i < cnt;i++) {
			if(x[i] == '-') return 0;/*a number can contain '-' only at the starting of the number*/
			if(x[i] == '.' && flag==1) return 0;/*a number can contain '.' only once*/
			if(x[i] == '.' && flag==0) {
				flag = 1;
				continue;
			}
			if(flag) {
				mul *= 10.0;
				val += (float)((int)x[i] - '0')/mul;
			}
			else{
			val *= 10;
			val+=(float)((int)x[i] - '0');
		}
		}
		val =- val;
	}

	else{
		for (i = 0;i < cnt;i++) {
			if(x[i] == '-') return 0;/*a number can contain '-' only at the starting of the number*/
			if(x[i] == '.' && flag==1) return 0;/*a number can contain '.' only once*/
			if(x[i] == '.' && flag==0) {
				flag=1;
				continue;
			}
			if(flag) {
				mul *= 10.0;
				val += (float)((int)x[i] - '0')/mul;
			}
			else{
			val *= 10;
			val += (float)((int)x[i] - '0');
		}
		}
	}
	*f = val;	// number is stored in *f
	return 1;
}
