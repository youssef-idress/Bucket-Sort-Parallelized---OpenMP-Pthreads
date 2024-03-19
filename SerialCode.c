#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NARRAY 100000   // Array size
#define NBUCKET 100 // Number of buckets
#define INTERVAL 1000 // Each bucket capacity
#define MAX_VALUE 100000



void quick_sort(int arr[], int left, int right) {
    if (left < right) {
        int pivot = arr[right];
        int i = left - 1;

        for (int j = left; j <= right - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                // Swap arr[i] and arr[j]
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
        // Swap arr[i+1] and arr[high] (or pivot)
        int temp = arr[i + 1];
        arr[i + 1] = arr[right];
        arr[right] = temp;

        int partition_index = i + 1;

        quick_sort(arr, left, partition_index - 1);
        quick_sort(arr, partition_index + 1, right);
    }
}

void bucket_sort(int arr[], int n) {
    int max_val = arr[0];
    int min_val = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max_val)
            max_val = arr[i];
        if (arr[i] < min_val)
            min_val = arr[i];
    }

    int bucket_range = (max_val - min_val) / NBUCKET + 1;

    int* buckets[NBUCKET];
    for (int i = 0; i < NBUCKET; i++) {
        buckets[i] = (int *)malloc(n * sizeof(int));
    }

    int bucket_sizes[NBUCKET] = {0};

    for (int i = 0; i < n; i++) {
        int bucket_index = (arr[i] - min_val) / bucket_range;
        buckets[bucket_index][bucket_sizes[bucket_index]++] = arr[i];
    }

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

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int array[NARRAY];
    srand(time(NULL));

    for (int i = 0; i < NARRAY; i++) {
        array[i] = rand() % MAX_VALUE + 1;
    }

    clock_t start_time, end_time;
    double total_time;

    printf("Original array: ");
    print_array(array, NARRAY);
    start_time = clock(); // Start the clock

    bucket_sort(array, NARRAY);
    end_time = clock(); // End the clock

    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC *1000;
    printf("Sorted array: ");
    print_array(array, NARRAY);
    printf("Total execution time: %.2f milliseconds\n", total_time);

    return 0;
}
