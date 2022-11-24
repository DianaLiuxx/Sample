/*
 * Conway's Game of Life
 * Parallelize code
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define THREADS 16
// Matrix: N * N
const int N = 10000;

double get_wall_seconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
    return seconds;
}

void initial_state(char *plane0);
void move(char * __restrict plane_buffer, char * __restrict result_buffer, const int nIters);

int main(int argc, const char *argv[])
{
    double startTime = get_wall_seconds();
    if (argc != 2)
    {
        printf("Usage: %s steps\n", argv[0]);
        return -1;
    }
    const int nIters = atoi(argv[1]);
    char *plane = malloc(sizeof(char) * N * N);
    char *result = malloc(sizeof(char) * N * N);

    printf("Life Information: Grid Size: %i * %i, Steps: %i\n", N, N, nIters);
    // Innitial state
    initial_state(plane);
    move(plane, result, nIters);
    // Free
    free(plane);
    free(result);
    double timeTaken = get_wall_seconds() - startTime;
    printf("> timeTaken = %7.3f\n", timeTaken);
    return 0;
}

void initial_state(char *plane0)
{
    // Set state randomly: Live: state = 1, Dead: state = 0
    int i;
#pragma omp parallel for num_threads(THREADS)
    for (i = 0; i < N * N; i++)
        plane0[i] = rand() % 2;
}

void move(char * __restrict plane_buffer, char * __restrict result_buffer, const int nIters)
{
    int n, i, j, live;
    char *tmp;
    for (n = 0; n < nIters; n++)
    {
        for (i = 0; i < N; i++)
//#pragma omp parallel for num_threads(THREADS)
            for (j = 0; j < N; j++)
            {
                live = 0;
                // Count the numbers of alive neighbors of (i, j)
                // The offset of (i, j) is: i * N + j. But in order to deal with the boundary,
                // it is changed to: ((i + N) % N) * N + ((j + N) % N), tortus by calculate module
                live += plane_buffer[((i - 1 + N) % N) * N + ((j - 1 + N) % N)]; // Top left neighbor
                live += plane_buffer[((i - 1 + N) % N) * N + ((j + N) % N)];     // Top middle neighbor
                live += plane_buffer[((i - 1 + N) % N) * N + ((j + 1 + N) % N)]; // Top right neighbor
                live += plane_buffer[((i + N) % N) * N + ((j - 1 + N) % N)];     // Left neighbor
                live += plane_buffer[((i + N) % N) * N + ((j + 1 + N) % N)];     // Right neighbor
                live += plane_buffer[((i + 1 + N) % N) * N + ((j - 1 + N) % N)]; // Bottom left neighbor
                live += plane_buffer[((i + 1 + N) % N) * N + ((j + N) % N)];     // Bottom middle neighbor
                live += plane_buffer[((i + 1 + N) % N) * N + ((j + 1 + N) % N)]; // Bottom right neighbor
                // Apply the rules
                result_buffer[i * N + j] = (plane_buffer[i * N + j] && live == 2) || (live == 3);
            }
        tmp = plane_buffer;
        plane_buffer = result_buffer;
        result_buffer = tmp;
    }
}
