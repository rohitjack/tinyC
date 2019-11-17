//handling global variables
int z = 6;
int main()
{
	printStr("Program to show global variables\n");
	int x;
	x = z-9; 
	printStr("x = ");
	printInt(x);
	printStr("\n");
	z = 15;
	x = z;   
	printStr("x = ");
	printInt(x);
	printStr("\n");
	return 0;
}