#include <sys/types.h>
#include <unistd.h>

void *my_malloc(size_t size)
{
    printf("my_malloc\n");
    void *p;

    p = sbrk (0);
    if (sbrk(size) == (void*) -1)
        return NULL;
    return p;
}

typedef struct block_s {
    size_t size;
    struct block_s next;
    int free;
} block_t;

int main()
{
    char *str = my_malloc(sizeof(char) * 3);

    str[0] = 'a';
    str[1] = 'b';
    str[2] = '\0';

    printf("%s\n", str);

    str[3] = 'z';
    return 0;
}