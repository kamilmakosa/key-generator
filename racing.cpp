#include <stdio.h>
#include <pthread.h> // -lpthread

const size_t NUMTHREADS = 8;

int done = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void * ThreadEntry(void * id) {
  // force the pointer to be a 64bit integer
  const int myid = (long) id;

  // we're going to manipulate done and use the cond, so we need the mutex
  pthread_mutex_lock( & mutex);

  // increase the count of threads that have finished their work.
  done++;

  // wait up the main thread (if it is sleeping) to test the value of done
  pthread_cond_signal( & cond);
  pthread_mutex_unlock( & mutex);

  return NULL;
}

int race_time() {
  clock_t start = clock();
  pthread_t threads[NUMTHREADS];

  for (int t = 0; t < NUMTHREADS; t++) {
    pthread_create( & threads[t], NULL, ThreadEntry, (void * )(long) t);
  }

  // we're going to test "done" so we need the mutex for safety
  pthread_mutex_lock( & mutex);

  // are the other threads still busy?
  while (done < NUMTHREADS) {
    /* block this thread until another thread signals cond. While
    blocked, the mutex is released, then re-aquired before this
    thread is woken up and the call returns. */
    pthread_cond_wait( & cond, & mutex);

    /* we go around the loop with the lock held */
  }
  pthread_mutex_unlock( & mutex);

  clock_t end = clock();
  double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  int time = ((int)(cpu_time_used * 1000 * 1000)) % 1000;

  return time;
}
