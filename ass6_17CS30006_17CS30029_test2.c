//pointers
int gen(int *a)
{
  *a = 213;
  return 0;
}
int main()
{
  printStr("Program to handle pointers\n");
  int *p;
  int i = 50;
  p = &i;
  char *d;
  *p = 7;
  printInt(i);
  printStr("\n");
  int *q = p;
  *q = 18;
  printInt(i);
  printStr("\n");
  char c = 'B';
  
  d = &c;
  *d = 'l';
  if(c == 'l')
  {
    printStr("Y\n");
  }
  else
  {
    printStr("N\n");
  }
  gen(p);
  printInt(i);
  printStr("\n");

  return 0;
}
