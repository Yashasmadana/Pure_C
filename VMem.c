//Virtual memories and Pointers

#include <stdio.h>
#include <stdlib.h>

int global_initialized = 42;     /* .data  */
int global_uninitialized;        /* .bss   */

void some_function(void)
{
    printf("  (this line runs inside some_function)\n");
}

int main(void)
{
    
    static int static_initialized = 100;  /* .data */
    static int static_uninitialized;      /* .bss  */
int *x=NULL;

    int stack_var = 7;                    /* stack */
    int *heap_var = malloc(sizeof(int));  /* heap  */
    *heap_var = 99;

    printf("code (function):            %p\n", (void *) some_function);
    printf("code (function):            %p\n", (void *) x);
    printf("global_initialized   (.data): %p  value=%d\n", (void *)&global_initialized, global_initialized);
    printf("static_initialized   (.data): %p  value=%d\n", (void *)&static_initialized, static_initialized);
    printf("global_uninitialized (.bss):  %p  value=%d\n", (void *)&global_uninitialized, global_uninitialized);
    printf("static_uninitialized (.bss):  %p  value=%d\n", (void *)&static_uninitialized, static_uninitialized);
    printf("heap_var (malloc):            %p  value=%d\n", (void *)heap_var, *heap_var);
    printf("stack_var:                    %p  value=%d\n", (void *)&stack_var, stack_var);

    some_function();
    free(heap_var);
    return 0;

    //Function decays to a pointer just like arrays
    // here some_function was casted as (void *) and it returned the address of the function.
    // so here it would give the address of the funtcion . like in arrays deferencing the array (*arr here) would
    // give u the first element of the array but with function its not possible 
    // all of these printf gives the same value
    // printf("code (function):            %p\n", (void *) some_function);-->00007FF616E81450
   // printf("code (function):            %p\n", some_function);-->00007FF616E81450
   // printf("code (function):            %p\n", &some_function);-->00007FF616E81450
   // printf("code (function):            %p\n", *some_function);-->00007FF616E81450
   // Each * just triggers "give me the function → which decays back to a pointer" (****some_function same result).
   //because there's no such thing as a "function object" you can hold by value in C, only functions and pointers to functions.

   //.bss and .data PART
   /*
   // CPU generates virtual address
        │
        ▼
   TLB lookup ──── hit ───► physical frame found ───┐
        │ miss                                       │
        ▼                                            │
 Walk page table (PML4→PDPT→PD→PT)                    │
        │                                            │
        ▼                                            │
  present bit set? ── no ──► page fault ── OS loads   │
        │ yes                  page, retries walk     │
        ▼                                            │
 got physical frame number                             │
        │                                            │
        ▼                                            │
   cache in TLB ◄───────────────────────────────────┘
        │
        ▼
physical address = frame << 12 | offset
        │
        ▼
   actual RAM access happens whats TLB full form and what is PML4->PDPT->PD->PT

   PML4 = Building directory      -> tells you which Floor (PDPT)
PDPT = Floor directory          -> tells you which Room (PD)
PD   = Room directory           -> tells you which Cabinet (PT)
PT   = Cabinet drawer index     -> tells you which physical Frame (the actual data)
*/
}