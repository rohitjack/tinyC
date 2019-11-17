int insertion_sort(int arr[], int n)
{
   int i=1, key, j;
   while(i < n)
   {
       j = i-1;
       key = arr[i];
       while (j >= 0 && arr[j] > key)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
       i=i+1;
   }
   return 0;
}

int main()
{
    printStr("This is the program for insertion sort\n");
    int arr[500];
    int n; 
    int err = 1;
    printStr("Enter the size of an array: \n");
    n = readInt(&err);
    printStr("Enter the array to be sorted: \n");
    int i=0;
    while(i<n)
    {
         arr[i] = readInt(&err);
         i=i+1;
    }

    insertion_sort(arr, n);
    printStr("The sorted array is: \n");
    i=0;
    for (i < n-1)
    {
        printInt(arr[i]);
        printStr(", ");
        i=i+1;
    }
    printInt(arr[n-1]);
    printStr("\n");

    return 0;
}