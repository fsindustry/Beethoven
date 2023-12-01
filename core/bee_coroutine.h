//
// Created by fsindustry on 23-11-30.
//

#ifndef BEETHOVEN_BEE_COROUTINE_H
#define BEETHOVEN_BEE_COROUTINE_H

#include "nty_queue.h"
#include "nty_tree.h"

#define BEE_CO_MAX_EVENTS (1024*1024)
#define BEE_CO_MAX_STACK_SIZE (128*1024)

#define SET_BIT(x) (1 << (x))
#define CLEAR_BIT(x) ~(1 << (x))

#define CANCEL_FD_WAIT_UINT64 1

typedef void (*coroutine_handler)(void *);

typedef enum {
  BEE_COROUTINE_STATUS_WAIT_READ,
  BEE_COROUTINE_STATUS_WAIT_WRITE,
  BEE_COROUTINE_STATUS_NEW,
  BEE_COROUTINE_STATUS_READY,
  BEE_COROUTINE_STATUS_EXITED,
  BEE_COROUTINE_STATUS_BUSY,
  BEE_COROUTINE_STATUS_SLEEPING,
  BEE_COROUTINE_STATUS_EXPIRED,
  BEE_COROUTINE_STATUS_FDEOF,
  BEE_COROUTINE_STATUS_DETACH,
  BEE_COROUTINE_STATUS_CANCELLED,
  BEE_COROUTINE_STATUS_PENDING_RUNCOMPUTE,
  BEE_COROUTINE_STATUS_RUNCOMPUTE,
  BEE_COROUTINE_STATUS_WAIT_IO_READ,
  BEE_COROUTINE_STATUS_WAIT_IO_WRITE,
  BEE_COROUTINE_STATUS_WAIT_MULTI
} bee_coroutine_status;

typedef enum {
  BEE_COROUTINE_COMPUTE_BUSY,
  BEE_COROUTINE_COMPUTE_FREE
} bee_coroutine_compute_status;

typedef enum {
  BEE_COROUTINE_EV_READ,
  BEE_COROUTINE_EV_WRITE
} bee_coroutine_event;

/**
 * define basic data structures
 */
// define a linked list named _bee_coroutine_linked_list with node named _bee_coroutine.
LIST_HEAD(_bee_coroutine_linked_list, _bee_coroutine);
// define a queue named _bee_coroutine_queue with elements named _bee_coroutine.
TAILQ_HEAD(_bee_coroutine_queue, _bee_coroutine);

// define a red-black tree named _bee_coroutine_rbtree_sleep with elements named _bee_coroutine
RB_HEAD(_bee_coroutine_rbtree_sleep, _bee_coroutine);
// define a red-black tree named _bee_coroutine_rbtree_wait with elements named _bee_coroutine
RB_HEAD(_bee_coroutine_rbtree_wait, _bee_coroutine);

typedef struct _bee_coroutine_linked_list bee_coroutine_linked_list;
typedef struct _bee_coroutine_queue bee_coroutine_queue;

typedef struct _bee_coroutine_rbtree_sleep bee_coroutine_rbtree_sleep;
typedef struct _bee_coroutine_rbtree_wait bee_coroutine_rbtree_wait;





#endif //BEETHOVEN_BEE_COROUTINE_H
