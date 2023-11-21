/* heap-1.c */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_BLOCKS 16
#define BLOCK_CAPACITY 1024

enum block_status {
    BLK_FREE = 0, BLK_ONE, BLK_FIRST, BLK_CONT, BLK_LAST
};

struct heap {
    struct block {
        char contents[BLOCK_CAPACITY];
    } blocks[HEAP_BLOCKS];
    enum block_status status[HEAP_BLOCKS];
} global_heap = {0};

struct block_id {
    size_t value;
    bool valid;
    struct heap *heap;
};

struct block_id block_id_new(size_t value, struct heap *from) {
    return (struct block_id) {
            .valid = true,
            .value = value,
            .heap = from
    };
}

struct block_id block_id_invalid() {
    return (struct block_id) {.valid = false};
}

bool block_id_is_valid(struct block_id bid) {
    return bid.valid && bid.value < HEAP_BLOCKS;
}

/* Find block */

bool block_is_free(struct block_id bid) {
    if (!block_id_is_valid(bid))
        return false;
    return bid.heap->status[bid.value] == BLK_FREE;
}

/* Allocate */
// find a free blocks, reserve it and return its id
struct block_id blocks_allocate(struct heap *heap, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (heap->status[i] == BLK_FREE) {
            return block_id_new(i, heap);
        }
    }
    return block_id_invalid();
}
/* Free */
// ? ? ?
void blocks_free(struct heap *heap, struct block_id block) {
    if (block_id_is_valid(block)) {
        size_t index = block.value;
        if (heap->status[index] == BLK_FIRST || heap->status[index] == BLK_CONT ||
            heap->status[index] == BLK_LAST || heap->status[index] == BLK_ONE) {
            for (size_t i = index; i < HEAP_BLOCKS; ++i) {
                if (heap->status[i] == BLK_ONE) {
                    heap->status[i] = BLK_FREE;
                    break;
                }
                if (heap->status[i]==BLK_LAST){
                    heap->status[i] = BLK_FREE;
                    break;
                }
                heap->status[i] = BLK_FREE;
            }
        }
    }
}

/* Printer */
const char *block_repr(struct block_id b) {
    static const char *const repr[] = {[BLK_FREE] = " .",
            [BLK_ONE] = " *",
            [BLK_FIRST] = "[=",
            [BLK_LAST] = "=]",
            [BLK_CONT] = " ="};
    if (b.valid)
        return repr[b.heap->status[b.value]];
    else
        return "INVALID";
}

void block_debug_info(struct block_id b, FILE *f) {
    fprintf(f, "%s", block_repr(b));
}

void block_foreach_printer(struct heap *h, size_t count,
                           void printer(struct block_id, FILE *f), FILE *f) {
    for (size_t c = 0; c < count; c++)
        printer(block_id_new(c, h), f);
}

void heap_debug_info(struct heap *h, FILE *f) {
    block_foreach_printer(h, HEAP_BLOCKS, block_debug_info, f);
    fprintf(f, "\n");
}

/*  -------- */

int main() {
    heap_debug_info(&global_heap, stdout);
    blocks_allocate(&global_heap, HEAP_BLOCKS);
    struct block_id bid = blocks_allocate(&global_heap, 1);
    bid.heap->status[bid.value] = BLK_ONE;
    struct block_id bid2 = blocks_allocate(&global_heap, 3);
    bid2.heap->status[bid2.value] = BLK_FREE;
    struct block_id bid1 = blocks_allocate(&global_heap, 2);
    bid1.heap->status[bid1.value] = BLK_FIRST;
    bid1.heap->status[bid1.value+1] = BLK_LAST;
    heap_debug_info(&global_heap, stdout);
    blocks_free(&global_heap,bid);
    heap_debug_info(&global_heap, stdout);
    return 0;

}
