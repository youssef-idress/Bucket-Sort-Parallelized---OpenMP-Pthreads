#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NARRAY 100000   // Array size
#define NBUCKET 100     // Number of buckets
#define MAX_VALUE 100000
#define NTHREADS 8      // Number of threads to use

int array[NARRAY];
int global_min = MAX_VALUE, global_max = 0;
int bucket_ranges[NBUCKET + 1];
int* buckets[NBUCKET];
int bucket_sizes[NBUCKET] = {0};
pthread_mutex_t min_max_mutex;

typedef struct {
    int start;
    int end;
    int thread_id;
} thread_arg_t;

void* find_min_max(void* arg) {
    thread_arg_t* args = (thread_arg_t*) arg;
    int local_min = MAX_VALUE, local_max = 0;
    for (int i = args->start; i < args->end; i++) {
        if (array[i] < local_min) local_min = array[i];
        if (array[i] > local_max) local_max = array[i];
    }
    pthread_mutex_lock(&min_max_mutex);
    if (local_min < global_min) global_min = local_min;
    if (local_max > global_max) global_max = local_max;
    pthread_mutex_unlock(&min_max_mutex);
    return NULL;
}

void quick_sort(int arr[], int left, int right) {
    if (left < right) {
        int pivot = arr[right], i = left - 1;
        for (int j = left; j <= right - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
        int temp = arr[i + 1];
        arr[i + 1] = arr[right];
        arr[right] = temp;
        quick_sort(arr, left, i);
        quick_sort(arr, i + 2, right);
    }
}

void* bucket_Sort(void* arg) {
    thread_arg_t* args = (thread_arg_t*) arg;
    int range = (global_max - global_min) / NBUCKET + 1;

    // Fill buckets
    for (int i = args->start; i < args->end; i++) {
        int bucket_index = (array[i] - global_min) / range;
        pthread_mutex_lock(&min_max_mutex); // Ensure thread-safe access
        int index = bucket_sizes[bucket_index]++;
        buckets[bucket_index][index] = array[i];
        pthread_mutex_unlock(&min_max_mutex);
    }

    // Sort buckets assigned to this thread
    for (int i = args->thread_id; i < NBUCKET; i += NTHREADS) {
        quick_sort(buckets[i], 0, bucket_sizes[i] - 1);
    }

    return NULL;
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; ++i) { // Limiting the number of printed elements
        printf("%d ", arr[i]);
    }
     printf("\n");
}

int main() {
    srand(time(NULL));
    pthread_t threads[NTHREADS];
    thread_arg_t args[NTHREADS];
    pthread_mutex_init(&min_max_mutex, NULL);

    for (int i = 0; i < NARRAY; i++) {
        array[i] = rand() % MAX_VALUE + 1;
    }
    for (int i = 0; i < NBUCKET; i++) {
        buckets[i] = (int*) malloc(NARRAY * sizeof(int));
    }

    printf("Original array: ");
    print_array(array, NARRAY);

    clock_t start_time = clock();

    for (int i = 0; i < NTHREADS; i++) {
        args[i].start = i * (NARRAY / NTHREADS);
        args[i].end = (i + 1) * (NARRAY / NTHREADS);
        pthread_create(&threads[i], NULL, find_min_max, &args[i]);
    }
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NTHREADS; i++) {
        args[i].thread_id = i;
        pthread_create(&threads[i], NULL, bucket_Sort, &args[i]);
    }
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC *1000;


    int index = 0;
    for (int i = 0; i < NBUCKET; i++) {
        for (int j = 0; j < bucket_sizes[i]; j++) {
            array[index++] = buckets[i][j];
        }
        free(buckets[i]);
    }

    printf("Sorted array: ");
    print_array(array, NARRAY);
    printf("Bucket sort took %.2f msseconds.\n", time_taken);

    pthread_mutex_destroy(&min_max_mutex);

    return 0;
}
