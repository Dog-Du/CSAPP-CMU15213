/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "memlib.h"
#include "mm.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "DogDu",
    /* First member's full name */
    "DogDu",
    /* First member's email address */
    "DogDu",
    /* Second member's full name (leave blank if none) */
    "DogDu",
    /* Second member's email address (leave blank if none) */
    "DogDu"};

typedef uint32_t uint;

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

// 头int，尾int，中间两个指针，怎么想都要16字节
// 在分配之后，这样的一个空闲块变成 8 + 4 = 12字节。
// 一个最小块为 16 字节。以8字节增加。

// HEAD/FEET : size[30bit] + front_free[1bit] + free[1bit]
// 定义空闲块大小 size 表示除头部之外的全部大小。
// size 保证为 4 的倍数，不保证为 8 的倍数，但是保证空闲块的头为 8 的倍数。

#define HEAD_SIZE (sizeof(uint))
#define FEET_SIZE (sizeof(uint))
#define PTR_SIZE (sizeof(void *))

#define MINBLOCK_SIZE (HEAD_SIZE + FEET_SIZE + PTR_SIZE * 2)
#define MINFREE_BLOCK_SIZE (PTR_SIZE * 2 + FEET_SIZE)

#define START_I (3)
#define SEG_LIST_SIZE (25)

#define IS_FRONT_FREE(ptr) ((*(uint *)(ptr)) & 2)
#define IS_FREE(ptr) ((*((uint *)(ptr)) & 1))

#define GET_SIZE(ptr) (*((uint *)(ptr)) >> 2)
#define GET_BACK_PTR(ptr) ((void *)(ptr) + GET_SIZE((ptr)) + FEET_SIZE)
#define GET_FRONT_PTR(ptr) \
  ((void *)(ptr)-GET_SIZE((void *)(ptr)-FEET_SIZE) - HEAD_SIZE)

#define GET_PREV_PTR(ptr) (*(void **)((void *)(ptr) + HEAD_SIZE))
#define GET_NEXT_PTR(ptr) (*(void **)((void *)(ptr) + HEAD_SIZE + HEAD_SIZE))
#define GET_FEET_PTR(ptr) ((ptr) + GET_SIZE(ptr))

#define GET_SEG_NEXT(seg_list, i) ((seg_list)[((i) << 1) + (i) + 2])
#define GET_SEG_PREV(seg_list, i) ((seg_list)[((i) << 1) + (i) + 1])
#define GET_SEG_HEAD(seg_list, i) ((seg_list)[((i) << 1) + (i) + 0])

#define GET_NEED_BLOCK_SIZE(size) \
  (size <= MINFREE_BLOCK_SIZE ? MINFREE_BLOCK_SIZE : ALIGN((size)-4) + 4)

#define SET_PREV_PTR(ptr, prev)                          \
  if ((ptr)) {                                           \
    (*(uint *)((uint)(ptr) + HEAD_SIZE) = (uint)(prev)); \
  } else {                                               \
  }

#define SET_NEXT_PTR(ptr, next)                                     \
  if ((ptr)) {                                                      \
    (*(uint *)((uint)(ptr) + HEAD_SIZE + PTR_SIZE) = (uint)(next)); \
  } else {                                                          \
  }

#define SET_SIZE(ptr, size)        \
  do {                             \
    uint x = *(uint *)(ptr);       \
    x = ((size) << 2) | (x & 0x3); \
    *(uint *)(ptr) = x;            \
  } while (0)

#define SET_FREEBIT(ptr, is_free)     \
  do {                                \
    uint x = *(uint *)(ptr);          \
    x = (x & (~0x1)) | (!!(is_free)); \
    *(uint *)(ptr) = x;               \
  } while (0)

#define SET_FRONT_FREEBIT(ptr, is_free)      \
  do {                                       \
    uint x = *(uint *)(ptr);                 \
    x = (x & (~0x2)) | ((!!(is_free)) << 1); \
    *(uint *)(ptr) = x;                      \
  } while (0)

#define REMOVE_FROM_LIST(ptr)         \
  do {                                \
    void *prev = GET_PREV_PTR((ptr)); \
    void *next = GET_NEXT_PTR((ptr)); \
    SET_PREV_PTR(next, prev);         \
    SET_NEXT_PTR(prev, next);         \
  } while (0)

// *3是为了对齐，因为一个链表的头，prev，next是 [4,4,4]
// 组成的，但是传递参数总为头。
// 每三个指针为一组，表示[head,prev,next]，这样减少了操作。
static void *seg_list[SEG_LIST_SIZE * 3];
static void *mm_start_brk; /* points to first byte of heap */
static void *mm_brk;       // 指向最后一个辅助块。

/* 参数为需要的空闲块大小 */
static void *extend_heap(size_t size) {
  void *ptr = mm_brk;
  uint need = size + HEAD_SIZE;
  uint have = mem_heap_hi() - mm_brk - HEAD_SIZE + 1;

  if (IS_FRONT_FREE(ptr)) {
    ptr = GET_FRONT_PTR(mm_brk);
    REMOVE_FROM_LIST(ptr);
    SET_FREEBIT(ptr, 0);
    SET_FRONT_FREEBIT(GET_BACK_PTR(ptr), 0);
    need -= GET_SIZE(ptr) + HEAD_SIZE;
  }

  if (have < need) {
    mem_sbrk(need - have);
  }

  mm_brk += need;

  // front_freebit 不需要修改。
  SET_SIZE(ptr, size);
  SET_FREEBIT(ptr, 0);

  SET_SIZE(mm_brk, 0);
  SET_FRONT_FREEBIT(mm_brk, 0);
  SET_FREEBIT(mm_brk, 0);
  return ptr;
}

static void insert_list(void *cur_ptr) {
  uint i;
  uint x;
  uint size;

  for (i = START_I, x = (1 << START_I) << 1, size = GET_SIZE(cur_ptr);
       i < SEG_LIST_SIZE && x <= size; ++i, x <<= 1) {
    ;
  }

  // 头插法插入。
  SET_PREV_PTR(cur_ptr, &GET_SEG_HEAD(seg_list, i));
  SET_NEXT_PTR(cur_ptr, GET_SEG_NEXT(seg_list, i));
  SET_PREV_PTR(GET_SEG_NEXT(seg_list, i), cur_ptr);
  GET_SEG_NEXT(seg_list, i) = cur_ptr;
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
  assert(ALIGNMENT / 2 == HEAD_SIZE && HEAD_SIZE == FEET_SIZE);

  // mem_init(); // we need not call.
  for (uint i = 0; i < SEG_LIST_SIZE; ++i) {
    GET_SEG_HEAD(seg_list, i) = NULL;
    GET_SEG_NEXT(seg_list, i) = NULL;
    GET_SEG_PREV(seg_list, i) = NULL;
  }

  mm_start_brk = mem_heap_lo();
  mm_brk = mem_heap_hi();

  uint brk = (uint)mm_start_brk;
  uint need =
      (ALIGNMENT - (brk % ALIGNMENT)) % ALIGNMENT + ALIGNMENT;
  uint have = mm_brk - mm_start_brk + 1;

  if (have < need) {
    mem_sbrk(need - have);
  }

  // 让空闲块的头对齐 ALIGNMENT 字节。
  mm_start_brk += (ALIGNMENT - (brk % ALIGNMENT)) % ALIGNMENT;
  mm_brk = mm_start_brk + ALIGNMENT / 2;

  SET_SIZE(mm_start_brk, 0);
  SET_SIZE(mm_brk, 0);
  SET_FREEBIT(mm_start_brk, 0);
  SET_FREEBIT(mm_brk, 0);
  SET_FRONT_FREEBIT(mm_start_brk, 0);
  SET_FRONT_FREEBIT(mm_brk, 0);
  return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
  uint i;
  size_t x;  // 链表最大值。
  void *ptr = NULL;
  void *best_ptr = NULL;
  uint need = GET_NEED_BLOCK_SIZE(size);
  uint have = 0;
  uint best_have = 0;
  uint least;
  void *split_head;

  for (i = START_I, x = (1 << START_I) << 1; i < SEG_LIST_SIZE; ++i, x <<= 1) {
    if (GET_SEG_NEXT(seg_list, i) == NULL || x <= need) {
      continue;
    }

    ptr = GET_SEG_NEXT(seg_list, i);

    while (ptr != NULL) {
      have = GET_SIZE(ptr);
      if (have >= need) {
        i = SEG_LIST_SIZE;

        if (!best_have || best_have > have) {
          best_have = have;
          best_ptr = ptr;
        }
      }

      ptr = GET_NEXT_PTR(ptr);
    }
  }

  if (i == SEG_LIST_SIZE + 1) {
    ptr = best_ptr;
    have = best_have;

    least = have - need;

    if (least >= MINBLOCK_SIZE) {
      // 分裂
      split_head = ptr + need + HEAD_SIZE;  // 分裂之后后半部分头。
      least -= HEAD_SIZE;  // 分裂之后后半部分的大小。

      SET_SIZE(split_head, least);
      SET_FREEBIT(split_head, 1);
      SET_FRONT_FREEBIT(split_head, 0);
      SET_SIZE(GET_FEET_PTR(split_head), least);
      SET_FRONT_FREEBIT(GET_FEET_PTR(split_head), 0);
      SET_FREEBIT(GET_FEET_PTR(split_head), 0);
      insert_list(split_head);

      SET_SIZE(ptr, need);
    }

    REMOVE_FROM_LIST(ptr);
    SET_FREEBIT(ptr, 0);
    SET_FRONT_FREEBIT(GET_BACK_PTR(ptr), 0);
  } else {
    ptr = extend_heap(need);
  }

  return ptr + HEAD_SIZE;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
  void *cur_ptr = ptr - HEAD_SIZE;
  void *front_ptr;
  void *back_ptr = GET_BACK_PTR(cur_ptr);

  while (IS_FREE(back_ptr)) {
    SET_SIZE(cur_ptr, GET_SIZE(cur_ptr) + GET_SIZE(back_ptr) + HEAD_SIZE);
    REMOVE_FROM_LIST(back_ptr);
    back_ptr = GET_BACK_PTR(cur_ptr);
  }

  while (IS_FRONT_FREE(cur_ptr)) {
    front_ptr = GET_FRONT_PTR(cur_ptr);
    SET_SIZE(front_ptr, GET_SIZE(front_ptr) + GET_SIZE(cur_ptr) + HEAD_SIZE);
    cur_ptr = front_ptr;
    REMOVE_FROM_LIST(cur_ptr);
  }

  SET_FREEBIT(GET_FEET_PTR(cur_ptr), 1);
  SET_FREEBIT(cur_ptr, 1);
  SET_FRONT_FREEBIT(GET_BACK_PTR(cur_ptr), 1);
  SET_FRONT_FREEBIT(GET_FEET_PTR(cur_ptr), 0);
  SET_SIZE(GET_FEET_PTR(cur_ptr), GET_SIZE(cur_ptr));

  insert_list(cur_ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
  void *cur_ptr = ptr - HEAD_SIZE;
  void *back_ptr = GET_BACK_PTR(cur_ptr);
  void *front_ptr;
  uint need = GET_NEED_BLOCK_SIZE(size);
  uint copy_size = GET_SIZE(cur_ptr);

  if (GET_SIZE(cur_ptr) >= need) {
    return ptr;
  }

  // 合并后半块。
  while (IS_FREE(back_ptr) && GET_SIZE(cur_ptr) < need) {
    SET_SIZE(cur_ptr, GET_SIZE(cur_ptr) + GET_SIZE(back_ptr) + HEAD_SIZE);
    REMOVE_FROM_LIST(back_ptr);
    back_ptr = GET_BACK_PTR(cur_ptr);
  }

  SET_FRONT_FREEBIT(GET_BACK_PTR(cur_ptr), 0);
  if (GET_SIZE(cur_ptr) >= need) {
    return ptr;
  }

  while (IS_FRONT_FREE(cur_ptr) && GET_SIZE(cur_ptr) < need) {
    front_ptr = GET_FRONT_PTR(cur_ptr);
    SET_SIZE(front_ptr, GET_SIZE(front_ptr) + GET_SIZE(cur_ptr) + HEAD_SIZE);
    cur_ptr = front_ptr;
    REMOVE_FROM_LIST(cur_ptr);
  }

  SET_FRONT_FREEBIT(GET_BACK_PTR(cur_ptr), 0);
  SET_FREEBIT(cur_ptr, 0);

  if (GET_SIZE(cur_ptr) >= need) {
    char *dest = cur_ptr + HEAD_SIZE;
    char *src = (char *)ptr;

    // copy_size 必然是 4 的倍数。
    for (uint i = 0; i < copy_size; i += 4) {
      dest[i + 0] = src[i + 0];
      dest[i + 1] = src[i + 1];
      dest[i + 2] = src[i + 2];
      dest[i + 3] = src[i + 3];
    }

    return cur_ptr + HEAD_SIZE;
  }

  void *oldptr = ptr;
  void *newptr;
  size_t copySize;

  newptr = mm_malloc(size);
  if (newptr == NULL) {
    return NULL;
  }

  copySize = *(size_t *)((char *)oldptr - HEAD_SIZE);

  if (size < copySize) {
    copySize = size;
  }

  memcpy(newptr, oldptr, copySize);
  mm_free(oldptr);
  return newptr;
}
