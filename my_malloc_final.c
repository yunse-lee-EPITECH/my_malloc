#include <sys/types.h>
#include <unistd.h>

void *base = NULL;

#define BLOCK_SIZE 12

typedef struct block_s {
    size_t size;
    struct block_s *next;
    int free;
    char data[1];
} block_t;

#define align4(x) (((((x) - 1) >> 2) << 2) + 4)

// find block
block_t *find_block(block_t **last, size_t size)
{
    block_t *b = base;

    while (b && !(b->free && b->size >= size )) {
        *last = b;
        b = b->next;
    }
    return b;
}

// extend heap
block_t *extend_heap(block_t *last, size_t s)
{
    block_t *b;
    b = sbrk(0);

    if (sbrk(BLOCK_SIZE + s) == (void*) -1)
        /* sbrk fails, go to die */
        return NULL;
    b->size = s;
    b->next = NULL;
    if (last)
        last->next = b;
    b->free = 0;
    return b;
}

// split block
void split_block(block_t *b, size_t s)
{
    block_t *new_block;

    new_block = (block_t*)(b->data + s);
    new_block->size = b->size - s - BLOCK_SIZE;
    new_block->next = b->next;
    new_block->free = 1;
    b->size = s;
    b->next = new_block;
}

void *my_malloc(size_t size)
{
    block_t *b;
    block_t *last;
    size_t s;
    s = align4(size);

    if (base) {
        /* First find a block */
        last = base;
        b = find_block(&last, s);
        if (b) {
            /* can we split */
            if ((b->size - s) >= (BLOCK_SIZE + 4))
                split_block(b, s);
            b->free = 0;
        } else {
            /* No fitting block, extend the heap */
            b = extend_heap(last, s);
            if (!b)
                return NULL;
        }
    } else {
        /* first time */
        b = extend_heap(NULL, s);
        if (!b)
            return NULL;
        base = b;
    }
    return b->data;
}
