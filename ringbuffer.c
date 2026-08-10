#include <stdio.h>

struct ringbuffer
{
    int value;
    struct ringbuffer* next;
};

int main()
{

    struct ringbuffer a,b,c;
    
    
    a.next = &b;
    b.next = &c;
    c.next = &a;

    printf("Enter value for a, b, c\n");
    scanf("%d %d %d", &a.value, &b.value, &c.value);

    struct ringbuffer* p = &a;
    for (int i = 0; i < 3; i++) {
        printf("%d\n", p->value);
        p = p->next;
    }

    return 0;
}






















