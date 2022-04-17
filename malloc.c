/*
 * ECE454 Lab 3 - Malloc
 * Designed by Lin Zhihao, 2021.
 * Attention: DO NOT attempt to copy this assignment. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "mm.h"
#include "memlib.h"

/*

Given functions:
    void *mem_sbrk(intptr_t incr); // Simulates sbrk
    void *mem_heap_lo(void);       // Get the lower bound address of the heap
    void *mem_heap_hi(void);       // Get the upper bound address of the heap
    size_t mem_heapsize(void);     // Get the current heap size
    size_t mem_pagesize(void);     // Get the page size

typedef struct {
    char *teamname; 
    char *name1;    
    char *id1;      
    char *name2;    
    char *id2;      
} team_t;

*/

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "一剑统治怜悯",
    /* First member's first name and last name */
    "Zhihao:Lin",
    /* First member's student number */
    "1005071299",
    /* Second member's first name and last name (do not modify if working alone) */
    "",
    /* Second member's student number (do not modify if working alone) */
    ""
};


/**********************************************************
 * Define Basic Structs and constants
**********************************************************/
#define DW_SIZE 16

typedef union header_type header_t;
typedef struct tail_type tail_t;
typedef struct link_type link_t;
#define NORMAL_HEADER_TYPE 0
#define MASTER_HEADER_TYPE 1
#define SLAVE_HEADER_TYPE 2

typedef struct normal_header_type {
    unsigned char type : 4;
    unsigned char free : 4;
    unsigned char idx;
    uint32_t dw_size;
    tail_t* tail;
} normal_header_t;

typedef struct slave_header_type {
    unsigned char type : 4;
    unsigned char free : 4;
    unsigned char idx;
    uint32_t slave_idx;
    header_t* master_header;
} slave_header_t;

typedef struct master_header_type {
    unsigned char type : 4;
    unsigned char free : 4;
    unsigned char idx;
    unsigned char total_slot_num;
    unsigned char free_slot_num;
    uint32_t bit_map;
    tail_t* tail;
} master_header_t;

typedef struct header_base_type {
    unsigned char type : 4;
    unsigned char free : 4;
    unsigned char idx;
} header_base_t;

union header_type {
    header_base_t base;
    normal_header_t normal_header;
    slave_header_t  slave_header;
    master_header_t master_header;
};

struct tail_type {
    header_t* header;
    uint32_t dw_size;
    unsigned short idx;
    unsigned short free;
};

struct link_type {
    header_t* next_header;
    header_t* pre_header;
};

#define RETURN_USER_PTR_FROM_HEADER(header) return ((void*) (header + 1))

#define GET_LINKER(header) (header == NULL ? NULL : ((link_t*) (header + 1)))
#define GET_SLAVE_HEADER_WITH_IDX(master_header, slave_idex, dw_len) ((header_t*) (master_header + 2 + (dw_len + 1) * slave_idex))

#define TAIL_CONSTRUCTOR(new_tail, Free, Idx, DW_len, Header) { \
    new_tail->free = Free; \
    new_tail->idx = Idx; \
    new_tail->dw_size = DW_len; \
    new_tail->header = Header; \
}

#define SLAVE_HEADER_CONSTRUCTOR(current_slave_header, Free, Slave_idx, DW_len, Master_header) { \
    current_slave_header->slave_header.free = Free; \
    current_slave_header->slave_header.type = SLAVE_HEADER_TYPE; \
    current_slave_header->slave_header.slave_idx = Slave_idx; \
    current_slave_header->slave_header.master_header = Master_header; \
}

#define MASTER_HEADER_CONSTRUCTOR(new_master_header, Free, Idx, slot_num, Tail) { \
    new_master_header->master_header.type = MASTER_HEADER_TYPE; \
    new_master_header->master_header.free = Free; \
    new_master_header->master_header.idx = Idx; \
    new_master_header->master_header.total_slot_num = slot_num; \
    new_master_header->master_header.free_slot_num = slot_num; \
    new_master_header->master_header.tail = Tail; \
    new_master_header->master_header.bit_map = 0; \
}

#define NORMAL_HEADER_CONSTRUCTOR(new_normal_header, Free, Idx, DW_len, Tail) { \
    new_normal_header->normal_header.type = NORMAL_HEADER_TYPE; \
    new_normal_header->normal_header.free = Free; \
    new_normal_header->normal_header.idx = Idx; \
    new_normal_header->normal_header.dw_size = DW_len; \
    new_normal_header->normal_header.tail = Tail; \
}

typedef header_t* segmenet_list;
#define SMALL_CELL_LIST_INCREMENT_SIZE 32
#define SMALL_CELL_LIST_INIT_SIZE 2
#define MIN_PARTITION_DW_LEN 10
#define IS_SMALL_BLOCK_IDX(idx) (idx <= 4)
// #define TABLE_SIZE 11
// const int32_t segment_table_size[TABLE_SIZE] = { 1, 2, 3, 4, 8, 16, 32, 64, 128, 256, 0x7fffffff };
#define TABLE_SIZE 22
const int32_t segment_table_size[TABLE_SIZE] = { 1, 2, 3, 4, 8, 16, 32, 64, 128, 256, 512, 768, 1024, 1280, 1536, 1792, 2048, 2560, 3072, 3584, 4096, 0x7fffffff };
segmenet_list global_segment_table[TABLE_SIZE];

/**********************************************************
 * Define Macros
**********************************************************/
#define ADDR_FRAME 0xfffffffffffffff0

#define DW_ROUNDUP(size_in_byte) ((size_in_byte + 15) >> 4)
#define IS_IDX_SMALL_BLOCK(idx) (idx > 3)

#define MIN(a, b) (a < b ? a : b)

#define DEBUG 0
#define LZH_ASSERT(x) if(DEBUG) { assert(x); }
#if DEBUG
int malloc_count;
#endif

/**********************************************************
* Helper functions
**********************************************************/
static inline int32_t get_index_from_size(int32_t dw_len) {
    int left = 0;
    int right = TABLE_SIZE - 1;
    int mid;
    while (left != right) {
        mid = (left + right) / 2;
        if (segment_table_size[mid] < dw_len) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

static inline void insert_to_list(header_t* new_header, int32_t idx) {
    link_t* new_linker = GET_LINKER(new_header);
    link_t* old_linker = GET_LINKER(global_segment_table[idx]);
    new_linker->next_header = global_segment_table[idx];
    new_linker->pre_header = NULL;
    global_segment_table[idx] = new_header;
    if (old_linker) {
        old_linker->pre_header = new_header;
    }
}

static inline void remove_from_list(header_t* current_header) {
    link_t* curr_linker = GET_LINKER(current_header);
    link_t* pre_linker = GET_LINKER(curr_linker->pre_header);
    link_t* next_linker = GET_LINKER(curr_linker->next_header);
    if (pre_linker) {
        pre_linker->next_header = curr_linker->next_header;
    } else {
        global_segment_table[current_header->base.idx] = curr_linker->next_header;
    }
    if (next_linker) {
        next_linker->pre_header = curr_linker->pre_header;
    }
}

static inline void* traversing_list(int32_t dw_len, int32_t idx) {
    if (IS_SMALL_BLOCK_IDX(idx)) {
        /* This is a small block. We only need to traverse from it's own list. */
        header_t* current = global_segment_table[idx];
        header_t* target = NULL;
        while (current != NULL) {
            if (current->master_header.free_slot_num) {
                target = current;
                break;
            }
            current = GET_LINKER(current)->next_header;
        }

        if (!target) {
            uint32_t master_dw_len = SMALL_CELL_LIST_INCREMENT_SIZE * (dw_len + 1) + 1; // Slave slots number + linker
            header_t* new_master_header = mem_sbrk((master_dw_len + 2) * 16);
            tail_t* new_master_tail = (tail_t*) (new_master_header + master_dw_len + 1);

            /* Set up header */
            MASTER_HEADER_CONSTRUCTOR(new_master_header, 0, idx, SMALL_CELL_LIST_INCREMENT_SIZE, new_master_tail);

            /* Set up tail */
            TAIL_CONSTRUCTOR(new_master_tail, 0, idx, master_dw_len, new_master_header);

            /* Set up each slave header */
            header_t* current_slave_header = new_master_header + 2;
            const int incre_dw_len = dw_len + 1;
            for (int i = 0; i < SMALL_CELL_LIST_INCREMENT_SIZE; ++i, current_slave_header += incre_dw_len) {
                SLAVE_HEADER_CONSTRUCTOR(current_slave_header, 1, i, dw_len, new_master_header);
            }

            /* Set up the linker, and insert the list to the front. */
            insert_to_list(new_master_header, idx);

            target = new_master_header;
        }

        LZH_ASSERT(target);

        /* Now, let's allocate the slave block. */
        const int bit_map = target->master_header.bit_map;
        const int total_slot = target->master_header.total_slot_num;
        int slave_idx = 0;
        while (slave_idx < total_slot) {
            if (((1 << slave_idx) & bit_map) == 0) {
                break;
            }
            ++slave_idx;
        }
        LZH_ASSERT(slave_idx < total_slot);

        /* Update the master header */
        target->master_header.bit_map |= (1 << slave_idx);
        --(target->master_header.free_slot_num);
        target->base.free = 0;
        (target->master_header.tail)->free = 0;

        /* Prepare the header and return the pointer. */
        header_t* ret_slave_header = GET_SLAVE_HEADER_WITH_IDX(target, slave_idx, dw_len);
        ret_slave_header->slave_header.free = 0;
        RETURN_USER_PTR_FROM_HEADER(ret_slave_header);
    } else {
        /* We are dealing with large lists. */
        uint32_t list_idx = idx;
        header_t* current = global_segment_table[list_idx];
        header_t* target = NULL;

        /* First, search in the idx list */
        while (current != NULL) {
            if (current->normal_header.dw_size >= dw_len) {
                target = current;
                break;
            }
            current = GET_LINKER(current)->next_header;
        }

        /* If target is not found, then we keep traversing for the later lists */
        if (!target) {
            ++list_idx;
            while (list_idx < TABLE_SIZE) {
                if (global_segment_table[list_idx]) {
                    LZH_ASSERT(global_segment_table[list_idx]->normal_header.dw_size > dw_len);
                    target = global_segment_table[list_idx];
                    break;
                }
                ++list_idx;
            }
        }

        if (target) {
            /* We find a block. */
            remove_from_list(target);
            target->base.free = 0;
            int left_size = target->normal_header.dw_size - dw_len;
            if (left_size > MIN_PARTITION_DW_LEN) {
                /* We will make a partition here. Insert the small segment to the smaller list. */
                tail_t* partition_tail = target->normal_header.tail;
                tail_t* current_tail = (tail_t*) (target + 1 + dw_len);
                header_t* partition_header = (header_t*) (current_tail + 1);

                /* Reset the current header and tail */
                target->normal_header.dw_size = dw_len;
                target->normal_header.idx = idx;
                target->normal_header.tail = current_tail;
                TAIL_CONSTRUCTOR(current_tail, 0, idx, dw_len, target);

                /* Initialize the partition header and tail */
                uint32_t partition_dw_size = left_size - 2;
                uint32_t partition_idx = get_index_from_size(partition_dw_size);
                NORMAL_HEADER_CONSTRUCTOR(partition_header, 1, partition_idx, partition_dw_size, partition_tail);
                TAIL_CONSTRUCTOR(partition_tail, 1, partition_idx, partition_dw_size, partition_header);
                insert_to_list(partition_header, partition_idx);
            }
            (target->normal_header.tail)->free = 0;
            RETURN_USER_PTR_FROM_HEADER(target);
        } else {
            /* We are running out of blocks. Using malloc to get some. */
            header_t* new_header_ptr = mem_sbrk((dw_len + 2) << 4);
            tail_t* new_tail_ptr = (tail_t*) (new_header_ptr + dw_len + 1);
            NORMAL_HEADER_CONSTRUCTOR(new_header_ptr, 0, idx, dw_len, new_tail_ptr);
            TAIL_CONSTRUCTOR(new_tail_ptr, 0, idx, dw_len, new_header_ptr);
            RETURN_USER_PTR_FROM_HEADER(new_header_ptr);
        }
    }
}

/**********************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the
 * prologue and epilogue
 **********************************************************/
int mm_init(void)
{
    // printf("Size of header: %d\n", sizeof(header_t));
    // printf("Size of tail: %d\n", sizeof(tail_t));
    // printf("Size of link: %d\n", sizeof(link_t));
    for (int i = 0; i < TABLE_SIZE; ++i) {
        global_segment_table[i] = NULL;
    }

    int dw_len = 1;
    int idx = 0;
    uint32_t master_dw_len = SMALL_CELL_LIST_INIT_SIZE * (dw_len + 1) + 1; // Slave slots number + linker
    header_t* new_master_header = mem_sbrk((master_dw_len + 2) * 16);
    tail_t* new_master_tail = (tail_t*) (new_master_header + master_dw_len + 1);
    /* Set up header */
    MASTER_HEADER_CONSTRUCTOR(new_master_header, 1, idx, SMALL_CELL_LIST_INIT_SIZE, new_master_tail);
    /* Set up tail */
    TAIL_CONSTRUCTOR(new_master_tail, 1, idx, master_dw_len, new_master_header);
    /* Set up each slave header */
    header_t* current_slave_header = new_master_header + 2;
    const int incre_dw_len = dw_len + 1;
    for (int i = 0; i < SMALL_CELL_LIST_INIT_SIZE; ++i, current_slave_header += incre_dw_len) {
        SLAVE_HEADER_CONSTRUCTOR(current_slave_header, 1, i, dw_len, new_master_header);
    }
    /* Set up the linker, and insert the list to the front. */
    insert_to_list(new_master_header, idx);

#if DEBUG
    malloc_count = 0;
#endif
    return 0;
}

/**********************************************************
 * mm_free
 * Free the block and coalesce with neighbouring blocks
 **********************************************************/
void mm_free(void *bp)
{
    header_t* header_ptr = ((header_t*) bp) - 1;

    LZH_ASSERT(!header_ptr->base.free);
    header_ptr->base.free = 1; /* Marked as free. */

    if (header_ptr->base.type == SLAVE_HEADER_TYPE) {
        const int slave_idx = header_ptr->slave_header.slave_idx;
        header_t* master_header_ptr = header_ptr->slave_header.master_header;
        master_header_ptr->master_header.bit_map &= (~(1 << slave_idx));
        ++(master_header_ptr->master_header.free_slot_num);
        if (master_header_ptr->master_header.bit_map == 0) {
            LZH_ASSERT(master_header_ptr->master_header.free_slot_num == master_header_ptr->master_header.total_slot_num);
            tail_t* tail_ptr = master_header_ptr->master_header.tail;
            master_header_ptr->base.free = 1;
            tail_ptr->free = 1;

            /* We have to coalescing with neighbours */
            uint64_t heap_bot = (uint64_t) mem_heap_lo();
            uint64_t heap_top = (uint64_t) mem_heap_hi();

            bool coalesced = false;
            header_t* coalesced_header = master_header_ptr;
            tail_t* coalesced_tail = tail_ptr;
            uint32_t coalesced_dw_size = tail_ptr->dw_size;

            tail_t* pre_tail = (tail_t*) (master_header_ptr - 1);
            if ( ((uint64_t) pre_tail >= heap_bot) && pre_tail->free ) {
                coalesced = true;
                coalesced_header = pre_tail->header;
                coalesced_dw_size += (pre_tail->dw_size + 2);
                remove_from_list(coalesced_header);
            }

            header_t* next_header = (header_t*) (tail_ptr + 1);
            if ( ((uint64_t) next_header < heap_top) && next_header->base.free ) {
                coalesced = true;
                coalesced_tail = (next_header->base.type == NORMAL_HEADER_TYPE) ? 
                    next_header->normal_header.tail : next_header->master_header.tail;
                coalesced_dw_size += (coalesced_tail->dw_size + 2);
                remove_from_list(next_header);
            } 

            if (coalesced) {
                remove_from_list(master_header_ptr);
                uint32_t coalesced_idx = get_index_from_size(coalesced_dw_size);
                NORMAL_HEADER_CONSTRUCTOR(coalesced_header, 1, coalesced_idx, coalesced_dw_size, coalesced_tail);
                TAIL_CONSTRUCTOR(coalesced_tail, 1, coalesced_idx, coalesced_dw_size, coalesced_header);
                insert_to_list(coalesced_header, coalesced_header->base.idx);
            } 

            return;
        }
    } else {
        tail_t* tail_ptr = header_ptr->normal_header.tail;
        tail_ptr->free = 1;
        uint64_t heap_bot = (uint64_t) mem_heap_lo();
        uint64_t heap_top = (uint64_t) mem_heap_hi();

        bool coalesced = false;
        header_t* coalesced_header = header_ptr;
        tail_t* coalesced_tail = tail_ptr;
        uint32_t coalesced_dw_size = tail_ptr->dw_size;
        
        tail_t* pre_tail = (tail_t*) (header_ptr - 1);
        if ( ((uint64_t) pre_tail >= heap_bot) && pre_tail->free ) {
            coalesced = true;
            coalesced_header = pre_tail->header;
            coalesced_dw_size += (pre_tail->dw_size + 2);
            remove_from_list(coalesced_header);
        }

        header_t* next_header = (header_t*) (tail_ptr + 1);
        if ( ((uint64_t) next_header < heap_top) && next_header->base.free ) {
            coalesced = true;
            coalesced_tail = (next_header->base.type == NORMAL_HEADER_TYPE) ? 
                next_header->normal_header.tail : next_header->master_header.tail;
            coalesced_dw_size += (coalesced_tail->dw_size + 2);
            remove_from_list(next_header);
        } 

        if (coalesced) {
            uint32_t coalesced_idx = get_index_from_size(coalesced_dw_size);
            NORMAL_HEADER_CONSTRUCTOR(coalesced_header, 1, coalesced_idx, coalesced_dw_size, coalesced_tail);
            TAIL_CONSTRUCTOR(coalesced_tail, 1, coalesced_idx, coalesced_dw_size, coalesced_header);
        } 

        insert_to_list(coalesced_header, coalesced_header->base.idx);
        return;
    }
}

/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in place(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/
void *mm_malloc(size_t size) {
#if DEBUG
    malloc_count ++;
#endif
    int32_t dw_len = DW_ROUNDUP(size);
    int32_t idx = get_index_from_size(dw_len);
    if (idx == 4) {
        dw_len = segment_table_size[idx];
    }
    return traversing_list(dw_len, idx);
}

/**********************************************************
 * mm_realloc
 * Implemented simply in terms of mm_malloc and mm_free
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return mm_malloc(size);
    }

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    header_t* old_header = ((header_t*) ptr) - 1;
    if (old_header->base.type == NORMAL_HEADER_TYPE) {
        tail_t* old_tail = old_header->normal_header.tail;
        int32_t extra_dw_len = DW_ROUNDUP(size - ((old_header->normal_header.dw_size) << 4));
        if (((uint64_t) mem_heap_hi() + 1) == (uint64_t) (old_tail + 1) && extra_dw_len >= 0) {
            mem_sbrk(extra_dw_len << 4);
            tail_t* new_tail = old_tail + extra_dw_len;
            int new_size = old_tail->dw_size + extra_dw_len;
            int new_idx = get_index_from_size(new_size);
            TAIL_CONSTRUCTOR(new_tail, 0, new_idx, new_size, old_header);
            old_header->normal_header.dw_size = new_size;
            old_header->normal_header.idx = new_idx;
            old_header->normal_header.tail = new_tail;
            return ptr;
        }
    }

    void* retval = mm_malloc(size);
    header_t* new_header = ((header_t*) retval) - 1;
    int32_t old_size, new_size;
    old_size = (old_header->base.type == SLAVE_HEADER_TYPE) ? segment_table_size[old_header->base.idx] : old_header->normal_header.dw_size;
    new_size = (new_header->base.type == SLAVE_HEADER_TYPE) ? segment_table_size[new_header->base.idx] : new_header->normal_header.dw_size;
    int copy_byte_len = MIN(old_size, new_size) << 4;
    memcpy(retval, ptr, copy_byte_len);
    mm_free(ptr);
    return retval;
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void){
    return 1;
}