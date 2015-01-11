#include <stdlib.h>
#include <stdio.h>
#include "array.h"

void addToArray(array *a, int n)
{
    if ((a->len % ARRAY_BUFFER_SIZE) == 0)
    {
        if (a->len)
        {
            a->data = (int*)realloc(a->data,(a->len+ARRAY_BUFFER_SIZE)*sizeof(int));
            a->next = (int*)realloc(a->next,(a->len+ARRAY_BUFFER_SIZE+1)*sizeof(int));
        }else{
            a->data = (int*)malloc((ARRAY_BUFFER_SIZE)*sizeof(int));
            a->next = (int*)malloc((ARRAY_BUFFER_SIZE+1)*sizeof(int));
            a->next[0] = -1;
        }
        
    }
    a->data[a->len] = n;
    
    int i;
    
    for (i=a->len;a->next[i]==-1;i--)
    {
	if (i<0)
		break;

        a->next[i] = a->len;
    }

    a->len++;
    a->next[a->len] = -1;
    
}

void deleteIndexFromArray(array *a, int idxToDelete)
{
    int i;
    int t;
    
    t = a->next[idxToDelete];
    
    for (i=idxToDelete;a->next[i]==t;i--)
    {
	if (i<0)
		break;

        a->next[i] = a->next[i+1];
    }
    
}

void deleteValueFromArray(array *a, int valueToDelete)
{
	int i;
    
	for(i=0;i<a->len;i++)
    	{
        	if (a->data[i] == valueToDelete)
			break;
    	}

	if (i!=a->len)	
		deleteIndexFromArray(a,i);
}

void printArray(array a)
{
    int i;
    if (a.len == 0)
	return;
    for(i=a.next[0];i!=-1;i=a.next[i+1])
    {
        printf("%i: %i\n",i,a.data[i]);
    }printf("\n");
}

#ifdef _TEST_ARRAY_
int main()
{
    array a = {NULL,0,NULL};
    
    addToArray(&a, 2);
    addToArray(&a, 3);
    addToArray(&a, 0);
    addToArray(&a, 9);
    addToArray(&a, 4);

    printArray(a);
    deleteValueFromArray(&a,4);
    printArray(a);
    addToArray(&a, 13);
    printArray(a);
    addToArray(&a, 21);
    addToArray(&a, 5);
    addToArray(&a, 1);
    printArray(a);
    addToArray(&a, 16);
    deleteIndexFromArray(&a,0);
    addToArray(&a, 50);
    addToArray(&a, 100);
    printArray(a);
    deleteValueFromArray(&a,500);
    deleteValueFromArray(&a,100);
    addToArray(&a, 101);
    printArray(a);
    //deleteValueFromArray(&a,2);
    printArray(a);
    addToArray(&a, -16);
    addToArray(&a, -50);
    addToArray(&a, -100);
    addToArray(&a, -21);
    addToArray(&a, -5);
    addToArray(&a, -1);
    addToArray(&a, -81);
    addToArray(&a, -26);
    addToArray(&a, -25);
    addToArray(&a, -18);
    printArray(a);

    return 0;
}
#endif
