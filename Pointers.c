#include <stdio.h>
#include <stdlib.h>

int main()
{

   // Pointer declaration way 1
   int num = 15;
   int *pi = &num;

printf("Address of num: %d Value: %d\n",&num, num);
printf("Address of pi: %d Value: %d \n",&pi, *pi);
// Pointer declaration way 2
int *p1;
p1=&num;
printf("Address of p1: %p Value of p1 : %p it points to: %p \n",&p1, (void *) p1,*p1);

// using %p instead of %d as type specifier:
// printf("%p", (void*)ptr); the pointer 
// should be casted as void (safer side) even if the pointer is an integer pointer .
//Reason-->a pointer's size varies by platform (4 bytes on 32-bit systems, 8 bytes on 64-bit systems), and it doesn't reliably match with int.
// so to get the exact address always use %p , %d would end up giving only the lower 32 bit in 64 bit system.
// int *ptr;
// Anything if u want to represent in Hexadecimal use %p.
//printf("%p", (void*)ptr);   // correct, standard-conforming
//printf("%p", ptr);          // works in practice on your platform, but technically UB


    return 0;
}