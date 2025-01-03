#if 0
#include <assert.h>
#include <chrono>
#include <iostream>
#include <pthread.h>

#ifndef NR_THREADS
#define NR_THREADS 4
#endif

#ifndef ARG_FIB_N
#define ARG_FIB_N 32
#endif

#ifdef SYNC_SPINLOCK
pthread_spinlock_t lock;
#define PTHREAD_LOCK() pthread_spin_lock(&lock)
#define PTHREAD_UNLOCK() pthread_spin_unlock(&lock)
#else
pthread_mutex_t mutex;
#define PTHREAD_LOCK() pthread_mutex_lock(&mutex)
#define PTHREAD_UNLOCK() pthread_mutex_unlock(&mutex)
#endif

void acquire_lock() {
  const int resp = PTHREAD_LOCK();
  assert(resp == 0);
}

void release_lock() {
  const int resp = PTHREAD_UNLOCK();
  assert(resp == 0);
}

struct arg_fib {
  int seq[ARG_FIB_N + 1];
  int index;
};

void *worker(void *arg) {
  arg_fib *d = (arg_fib *)arg;
  while (true) {
    acquire_lock();
    if (d->index > ARG_FIB_N) {
      release_lock();
      break;
    }
    const int n = d->index;
    d->seq[n] = d->seq[n - 1] + d->seq[n - 2];
    d->index++;
    release_lock();
  }
  return nullptr;
}

void fib_seq(arg_fib *d) {
  pthread_t threads[NR_THREADS] = {};
  for (int i = 0; i < NR_THREADS; ++i) {
    pthread_create(&threads[i], nullptr, worker, d);
  }
  for (int i = 0; i < NR_THREADS; ++i) {
    pthread_join(threads[i], nullptr);
  }
}

int main(int argc, char *argv[]) {
  using clock = std::chrono::high_resolution_clock;
  const auto beg = clock::now();
  arg_fib d = {
      .seq = {0, 1},
      .index = 2,
  };
  fib_seq(&d);
  const auto end = clock::now();
  std::cout << "fib(" << ARG_FIB_N << ") = " << d.seq[ARG_FIB_N] << " cost "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - beg)
                   .count()
            << " us" << std::endl;
  return 0;
}
#else
#include <assert.h>
#include <chrono>
#include <iostream>
#include <pthread.h>
#include <stddef.h>

#ifndef NR_THREADS
#define NR_THREADS 4
#endif

#ifndef ARG_FIB_N
#define ARG_FIB_N 32
#endif

#ifdef SYNC_SPINLOCK
pthread_spinlock_t lock;
#define PTHREAD_LOCK() pthread_spin_lock(&lock)
#define PTHREAD_UNLOCK() pthread_spin_unlock(&lock)
#define PTHREAD_LOCK_INIT() pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE)
#else
pthread_mutex_t mutex;
#define PTHREAD_LOCK() pthread_mutex_lock(&mutex)
#define PTHREAD_UNLOCK() pthread_mutex_unlock(&mutex)
#define PTHREAD_LOCK_INIT() pthread_mutex_init(&mutex, nullptr)
#endif

void lock_init() { PTHREAD_LOCK_INIT(); }

void acquire_lock() {
  const int resp = PTHREAD_LOCK();
  assert(resp == 0);
}

void release_lock() {
  const int resp = PTHREAD_UNLOCK();
  assert(resp == 0);
}

struct arg_fib {
  int seq[ARG_FIB_N + 1];
  int task_index;
  int data_index;
};

void *worker(void *arg) {
  arg_fib *d = (arg_fib *)arg;
  while (true) {
    acquire_lock();
    if (d->task_index > ARG_FIB_N) {
      release_lock();
      break;
    }
    const int n = d->task_index++;
    const int m = d->data_index;
    const int t = n - m;
    const bool ready = t + 1 <= m;
    if (ready) {
      release_lock();
    }
    d->seq[n] = d->seq[t + 1] * d->seq[n - t] + d->seq[t] * d->seq[n - t - 1];
#ifndef NDEBUG
    printf("f(%d)=f(%d)*f(%d)+f(%d)*f(%d)=%d\n", n, t + 1, n - t, t, n - t - 1,
           d->seq[n]);
#endif
    if (ready) {
      acquire_lock();
    }
    while (true) {
      if (d->data_index + 1 == n) {
        d->data_index = n;
        break;
      }
      release_lock();
      sched_yield();
      acquire_lock();
    }
    release_lock();
  }
  return nullptr;
}

void fib_seq(arg_fib *d) {
  pthread_t threads[NR_THREADS] = {};
  for (int i = 0; i < NR_THREADS; ++i) {
    pthread_create(&threads[i], nullptr, worker, d);
  }
  for (int i = 0; i < NR_THREADS; ++i) {
    pthread_join(threads[i], nullptr);
  }
}

int main(int argc, char *argv[]) {
  using clock = std::chrono::high_resolution_clock;
  lock_init();
  const auto beg = clock::now();
  arg_fib d = {
      .seq = {0, 1, 1},
      .task_index = 3,
      .data_index = 2,
  };
  fib_seq(&d);
  const auto end = clock::now();
  std::cout << "fib(" << ARG_FIB_N << ") = " << d.seq[ARG_FIB_N] << " cost "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - beg)
                   .count()
            << " us" << std::endl;
  return 0;
}
#endif
