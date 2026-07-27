#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
int main(int argc, char *argv[])
{
    
/* 
The type of this pointer depends on whether you are using an older
K&R compiler or the newer ANSI type compiler. With the older compiler the type of the
returned pointer is char, with the ANSI compiler it is void.
*/

int *arr;
arr=(int*)(malloc(10*sizeof(int)));
// Memory allocation failure:
// Why malloc() can fail
// ** malloc() asks the OS/runtime for a contiguous block of heap memory. This request can fail for several reasons:
// -->
// **Heap exhaustion — no contiguous block of the requested size is available (common with fragmentation, even if total
// free memory is technically enough).
// **Requested size is unreasonably large — e.g., a corrupted or attacker-controlled size value.
// **On embedded/RTOS systems — the heap is often a small, fixed-size pool (not backed by virtual memory/swap like a 
//desktop OS), so it's much easier to exhaust than on a PC. This makes the NULL check more critical in firmware than
// in typical desktop software, not less.

// allocation failure check
if (arr == NULL) {
    printf("malloc failed\n");
    exit(EXIT_FAILURE);
}

MEMORYSTATUSEX statex;
statex.dwLength = sizeof(statex);
GlobalMemoryStatusEx(&statex);
printf("Available: %llu bytes\n", statex.ullAvailPhys);

    return EXIT_SUCCESS;
}