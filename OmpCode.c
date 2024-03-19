#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define NARRAY 100000   // Array size
#define NBUCKET 100     // Number of buckets
#define MAX_VALUE 100000

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

void bucket_sort(int arr[], int n) {
    int max_val = arr[0], min_val = arr[0];
    #pragma omp parallel for reduction(max:max_val) reduction(min:min_val)
    for (int i = 1; i < n; i++) {
        if (arr[i] > max_val) max_val = arr[i];
        if (arr[i] < min_val) min_val = arr[i];
    }

    int bucket_range = (max_val - min_val) / NBUCKET + 1;
    int* buckets[NBUCKET];
    int bucket_sizes[NBUCKET] = {0};

    for (int i = 0; i < NBUCKET; i++) {
        buckets[i] = (int *)malloc(n * sizeof(int));
    }

    for (int i = 0; i < n; i++) {
        int bucket_index = (arr[i] - min_val) / bucket_range;
        buckets[bucket_index][bucket_sizes[bucket_index]++] = arr[i];
    }

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < NBUCKET; i++) {
        quick_sort(buckets[i], 0, bucket_sizes[i] - 1);
    }

    int index = 0;
    for (int i = 0; i < NBUCKET; i++) {
        for (int j = 0; j < bucket_sizes[i]; j++) {
            arr[index++] = buckets[i][j];
        }
        free(buckets[i]);
    }
}

int main() {

    omp_set_num_threads(16);
    int array[NARRAY];
    srand(time(NULL));

    for (int i = 0; i < NARRAY; i++) {
        array[i] = rand() % MAX_VALUE + 1;
    }

    printf("Initial array: ");
    for (int i = 0; i < NARRAY; i++)
        printf("%d ", array[i]);
    printf("\n");

    clock_t start_time, end_time;
    double total_time;
    start_time = clock(); // Start the clock

    bucket_sort(array, NARRAY);

    end_time = clock(); // End the clock

    printf("Sorted array: ");
    for (int i = 0; i < NARRAY; i++)
        printf("%d ", array[i]);
    printf("\n");
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
      printf("Total execution time: %.2f milliseconds\n", total_time);

    return 0;
}
