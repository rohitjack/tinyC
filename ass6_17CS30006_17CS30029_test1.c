//lcs
int main()
{
	int n, a[500];
	int err =1;
	printStr("Program to find length of Longest Increasing Subsequence\nEnter size of array:\n");
	n = readInt(&err);
	printStr("Enter array: (elements on new lines)\n");
	int i, j;
	i=0;
	while(i<n)
	{
		a[i] = readInt(&err);
		i=i+1;
	}
	int res[500];
	int max = -50000;
	res[0]=1;
	
	i=1;
	while(i<n)
	{
		res[i] = 1;
		j=0;
		while(j<i)
		{
			if(a[j]<a[i] && res[j]+1>res[i])
				res[i] = res[j] +1;
			j=j+1;
		}
		i=i+1;
	}
	int ans = -1;
	i=0;
	while(i<n)
	{
		if(ans<res[i])
			ans = res[i];
		i=i+1;
	}
	printStr("Length of LIS is: ");
	printInt(ans);
	printStr("\n");
}
