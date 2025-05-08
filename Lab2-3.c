#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>
#include <time.h>

int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// heavy_task для Pthreads (без изменений)
void *heavy_task(void *i) {
  int thread_num = *((int*) i);

//   // Пример критической секции с мьютексом
//   printf("\tThread #%d started\n", thread_num);
//   pthread_mutex_lock(&mutex);
//   printf("\t\tThread #%d acquired mutex\n", thread_num);
//   counter++;
//   printf("\t\t\tThread #%d, counter: %d\n", thread_num, counter);
//   printf("\t\tThread #%d released mutex\n", thread_num);
//   pthread_mutex_unlock(&mutex);

  // Long-running task
  for (int i = 0; i < 1e8; i++) {
    sqrt(i);
  }

  printf("\tThread #%d finished\n", thread_num);
  free(i);
}

// функция pthreads
void pthreads(int threads_num) {
  pthread_t threads[threads_num];
  int status;

  for (int i = 0; i < threads_num; i++) {
    printf("MAIN: starting thread %d\n", i);
    int *thread_num = (int*) malloc(sizeof(int));
    *thread_num = i;
    status = pthread_create(&threads[i], NULL, heavy_task, thread_num);
    if (status != 0) {
      fprintf(stderr, "pthread_create failed, error code %d\n", status);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < threads_num; i++) {
    pthread_join(threads[i], NULL);
  }
}

// heavy_task для OpenMP
void openmp_heavy_task() {
    printf("id: %d\n", omp_get_thread_num());
  int limit = 1e8;
  for (int i = 0; i < limit; i++) {
    sqrt(i);
  }
}

// функция openmp
void openmp(int thread_num) {
  omp_set_dynamic(0);
  printf("param: %d\n", thread_num);
  omp_set_num_threads(thread_num);
  printf("OpenMP threads: %d\n", omp_get_num_threads());
  #pragma omp parallel for num_threads(thread_num)
  for (int i = 0; i < thread_num; i++) {
    // printf("for: %d\n", omp_get_num_threads());
    openmp_heavy_task();
  }
}

// последовательная версия
void sequential(int num_tasks) {
    for (int i = 0; i < num_tasks; i++) {
        printf("\tSequential task #%d started\n", i);
        for (int j = 0; j < 1e8; j++) {
            sqrt(j);
        }
        printf("\tSequential task #%d finished\n", i);
    }
}


int main(int argc, char **argv) {
    int num_tasks = atoi(argv[1]);
    clock_t start, end;
    double time_spent;

    // Последовательное выполнение
    printf("\n=== Последовательное выполнение ===\n");
    start = clock();
    sequential(num_tasks);
    end = clock();
    time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Время последовательного выполнения: %.2f секунд\n", time_spent);

    // Pthreads выполнение
    printf("\n=== Pthreads выполнение ===\n");
    start = clock();
    pthreads(num_tasks);
    end = clock();
    time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Время выполнения Pthreads: %.2f секунд\n", time_spent);
    pthread_mutex_destroy(&mutex);

    // OpenMP выполнение
    printf("\n=== OpenMP выполнение ===\n");
    start = clock();
    openmp(num_tasks);
    end = clock();
    time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Время выполнения OpenMP: %.2f секунд\n", time_spent);

    return 0;
}