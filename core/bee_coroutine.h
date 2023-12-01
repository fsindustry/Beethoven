//
// Created by fsindustry on 23-11-30.
//

#ifndef BEETHOVEN_BEE_COROUTINE_H
#define BEETHOVEN_BEE_COROUTINE_H
#define _USE_UCONTEXT

#ifdef _USE_UCONTEXT

#include <ucontext.h>

#endif

#include <stdint.h>
#include <sys/poll.h>
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

#ifndef _USE_UCONTEXT
typedef struct _bee_cpu_ctx{
  void *rsp;
  void *rbp;
  void *rip;
  void *rdi;
  void *rbx;
  void *r1;
  void *r2;
  void *r3;
  void *r4;
  void *r5;
};
#endif // _USE_UCONTEXT

typedef struct _bee_schedule {
  uint64_t birth;
#ifdef _USE_UCONTEXT
  ucontext_t ctx;
#endif // _USE_UCONTEXT
  void *stack;
  size_t stack_size;
  int spawned_coroutines;
  uint64_t default_timeout;
  struct _bee_coroutine *curr_thread;
  int page_size;

  int poller_fd;
  int eventfd;
  struct epoll_event eventlist[BEE_CO_MAX_EVENTS];
  int nevents;

  int num_new_events;
  pthread_mutex_t defer_mutex;

  bee_coroutine_queue ready;
  bee_coroutine_queue defer;
  bee_coroutine_queue busy;

  bee_coroutine_rbtree_sleep sleeping;
  bee_coroutine_rbtree_wait waiting;
} bee_schedule;


typedef struct _bee_coroutine {

#ifdef _USE_UCONTEXT
  ucontext_t ctx;
#else
  bee_cpu_ctx ctx;
#endif // _USE_UCONTEXT

  coroutine_handler handler;
  void *arg;
  void *data;
  size_t stack_size;
  size_t last_stack_size;

  bee_coroutine_status status;
  bee_schedule *sched;

  uint64_t birth;
  uint64_t id;

#if CANCEL_FD_WAIT_UINT64
  int fd;
  unsigned short events;
#else
  int64_t fd_wait;
#endif

  char funcname[64];
  struct _bee_coroutine *co_join;

  void **co_exit_ptr;
  void *stack;
  void *rbp;
  uint32_t ops;
  uint64_t sleep_usecs;

  RB_ENTRY(_bee_coroutine) sleep_node;
  RB_ENTRY(_bee_coroutine) wait_node;

  LIST_ENTRY(_bee_coroutine) busy_next;

  TAILQ_ENTRY(_bee_coroutine) read_next;
  TAILQ_ENTRY(_bee_coroutine) defer_next;
  TAILQ_ENTRY(_bee_coroutine) cond_next;
  TAILQ_ENTRY(_bee_coroutine) io_next;
  TAILQ_ENTRY(_bee_coroutine) compute_next;

  struct {
    void *buf;
    size_t nbytes;
    int fd;
    int ret;
    int err;
  } io;

  struct _bee_coroutine_compute_sched *compute_sched;
  int ready_fds;
  struct pollfd *pfds;
  nfds_t nfds;
} bee_coroutine;

typedef struct _bee_coroutine_compute_sched {
#ifdef _USE_UCONTEXT
  ucontext_t ctx;
#else
  bee_cpu_ctx ctx;
#endif // _USE_UCONTEXT

  bee_coroutine_queue coroutines;
  bee_coroutine  *curr_coroutine;

  pthread_mutex_t run_mutex;
  pthread_cond_t run_cond;

  pthread_mutex_t co_mutex;
  LIST_ENTRY(_bee_coroutine_compute_sched) compute_next;

  bee_coroutine_compute_status compute_status;
} bee_coroutine_compute_sched;

extern pthread_key_t global_sched_key;


#endif //BEETHOVEN_BEE_COROUTINE_H
