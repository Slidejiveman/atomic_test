#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#ifdef __CYGWIN__
# define CLOCK CLOCK_MONOTONIC
#else
# define CLOCK CLOCK_MONOTONIC_RAW
#endif

#define THREAD_COUNT 8

/**
 * Converts the time spec to milliseconds.
 *
 * @param ts - A pointer to a timespec
 * @return The wall time in milliseconds
 */
double ts_to_ms(struct timespec* ts) {
    return (((double) ts->tv_sec) * 1000.0) + (((double) ts->tv_nsec) / 1000000.0);
}

int main()
{
    // timespecs needed to piece together wall clock time
    struct timespec start, finish;

    // number of loop iterations
    int n = 1000;

    // Test if each thread can execute this in approximately
    // linear (serial) time
    double start_time;
    double finish_time;
    double wall_time;

    // Multi-Threaded section
    printf("\n\n Multi-Threaded:\n");

    // initialize clock
    clock_gettime(CLOCK, &start);

    // execute code with atomic directive.
    // Overhead should be significantly less.
#    pragma omp parallel num_threads(THREAD_COUNT)
    {
        int i;
        double my_sum = 0.0;
        for (i = 0; i < n; i++) {
#            pragma omp atomic
            my_sum += sin(i);
        }
    }
    clock_gettime(CLOCK, &finish);

    // Calculate times and print result to screen
    start_time = ts_to_ms(&start);
    finish_time = ts_to_ms(&finish);
    wall_time = finish_time - start_time;
    printf("\nThe amount of wall time for ATOMIC: %f", wall_time);

    // Test if the critical section is much longer than the
    // atomic for same n
    clock_gettime(CLOCK, &start);

#    pragma omp parallel num_threads(THREAD_COUNT)
    {
        int i;
        double my_sum = 0.0;
        for (i = 0; i < n; i++) {
#            pragma omp critical
            my_sum += sin(i);
        }
    }
    clock_gettime(CLOCK, &finish);

    start_time = ts_to_ms(&start);
    finish_time = ts_to_ms(&finish);
    wall_time = finish_time - start_time;
    printf("\nThe amount of wall time for CRITICAL: %f", wall_time);

    // Control: How long does this take to execute without OMP?
    clock_gettime(CLOCK, &start);
    int i;
    double my_sum = 0.0;
    for (i = 0; i < n; i++) {
        my_sum += sin(i);
    }
    clock_gettime(CLOCK, &finish);

    start_time = ts_to_ms(&start);
    finish_time = ts_to_ms(&finish);
    wall_time = finish_time - start_time;
    printf("\nThe amount of wall time for SERIAL: %f", wall_time);

    // repeat the above code with thead count being only 1
# define THREAD_COUNT 1

    printf("\n\n Single Threaded:\n");
    clock_gettime(CLOCK, &start);

#    pragma omp parallel num_threads(THREAD_COUNT)
    {
        int i;
        double my_sum = 0.0;
        for (i = 0; i < n; i++) {
#            pragma omp atomic
            my_sum += sin(i);
        }
    }
    clock_gettime(CLOCK, &finish);

    start_time = ts_to_ms(&start);
    finish_time = ts_to_ms(&finish);
    wall_time = finish_time - start_time;
    printf("\nThe amount of wall time for ATOMIC: %f", wall_time);

    // Test if the critical section is much longer than the
    // atomic for same n
    clock_gettime(CLOCK, &start);

#    pragma omp parallel num_threads(THREAD_COUNT)
    {
        int i;
        double my_sum = 0.0;
        for (i = 0; i < n; i++) {
#            pragma omp critical(onethread)
            my_sum += sin(i);
        }
    }
    clock_gettime(CLOCK, &finish);

    start_time = ts_to_ms(&start);
    finish_time = ts_to_ms(&finish);
    wall_time = finish_time - start_time;
    printf("\nThe amount of wall time for CRITICAL: %f", wall_time);

    // Control: How long does this take to execute without OMP?
    clock_gettime(CLOCK, &start);
    my_sum = 0.0;
    for (i = 0; i < n; i++) {
        my_sum += sin(i);
    }
    clock_gettime(CLOCK, &finish);

    start_time = ts_to_ms(&start);
    finish_time = ts_to_ms(&finish);
    wall_time = finish_time - start_time;
    printf("\nThe amount of wall time for SERIAL: %f", wall_time);

    return 0;
}