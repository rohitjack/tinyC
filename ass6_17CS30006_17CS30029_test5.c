// Binary Search
int binary(int a[],int n,int m,int l,int u)
{

    int mid,c=0;
	if(l>u)
	{
		return -1;
	}
    mid=(l+u)/2;
	int t=a[mid];
	if (m<t)
	{
    	return binary(a,n,m,l,mid-1);
	}
	else if(m>t)
	{
		return binary(a,n,m,mid+1,u);
	}
	else
	{
         return mid;
    }
}
int main()
{
	int a[10],i,n,m,pos,lo,up;
    int err=1;
    printStr("Program to implement Binary search\n");
    printStr("Enter the size of an array: \n");
    n=readInt(&err);
    printStr("Enter the elements of the array in sorted order \n");
    i=0;
    while(i<n)
    {
         a[i]=readInt(&err);
         i=i+1;
    }

    printStr("Enter the number to be search: \n");
    m=readInt(&err);
	lo=0,up=n-1;
	pos=0;
    pos=binary(a,n,m,lo,up);
    if(pos!=-1)
    {
    	printStr("Number is found at index(1-based)-:\n");
		pos=pos+1;
		printInt(pos);
		printStr("\n");
	}
    else
    {
		printStr("Number is not found.\n");	
	}
	return 0;
}
