/*
* @Author Oguzhan BIRK, Musa Ogural
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define IMAGE_WIDTH  15
#define IMAGE_HEIGHT 15
#define NUM_THREADS  8
#define PATTERN_SIZE 5
#define NUMBER_OF_MAX_MATCHES ((IMAGE_HEIGHT - PATTERN_SIZE + 1) * (IMAGE_WIDTH - PATTERN_SIZE + 1))

typedef struct MatchedCoor
{
    int x;
    int y;
}MatchCoordinates;

// Define your 5x5 patterns A and B (replace with actual patterns)
unsigned short patternA[PATTERN_SIZE][PATTERN_SIZE] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

unsigned short patternB[PATTERN_SIZE][PATTERN_SIZE] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

unsigned short sampleImage[IMAGE_HEIGHT][IMAGE_WIDTH] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void displayImage(unsigned short image[][IMAGE_WIDTH])
{
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            printf("%d\t", image[i][j]);
        }
        printf("\n");
    }
}

// Function to load a sample image (you should replace this with your image loading logic)
void loadImage(unsigned short image[][IMAGE_WIDTH]) {
    srand(time(0));
    int r;
    // Initialize a simple example image
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            r = rand() % 1;
            /*image[i][j] = (i + j) % 256;*/  // Fill the image with some values (replace with actual values)
            image[i][j] = r;
        }
    }
}

// Function to perform the pattern matching
void searchPatterns(unsigned short image[][IMAGE_WIDTH]) {
    int matchA;
    int matchB;
    MatchCoordinates mA;
    MatchCoordinates mB;
    MatchCoordinates matchArrA[NUMBER_OF_MAX_MATCHES];
    MatchCoordinates matchArrB[NUMBER_OF_MAX_MATCHES];
    int aMatchIndexCounter = 0;
    int bMatchIndexCounter = 0;

    printf("************************************************\n");
    printf("Parallel\n");
    printf("************************************************\n");
    // Loop over the image to find matches using OpenMP parallelism
#pragma omp parallel for collapse(NUM_THREADS)
    for (int i = 2; i < IMAGE_HEIGHT - 2; i++) {
        for (int j = 2; j < IMAGE_WIDTH - 2; j++) {
            // Check for a match with pattern A
            matchA = 1;
            for (int m = 0; m < PATTERN_SIZE; m++) {
                for (int n = 0; n < PATTERN_SIZE; n++) {
                    if (image[i + m - 2][j + n - 2] != patternA[m][n]) {
                        matchA = 0;
                        break;
                    }
                }
                if (!matchA) break;
            }

            // Print the result for a match with pattern A
            if (matchA) {
                printf("Thread %d found a match with pattern A at (%d, %d)\n", omp_get_thread_num(), (i - 2), (j - 2));

                if ((j - 2) >= (i - 2))
                {
                    matchArrA[aMatchIndexCounter].x = (j - 2);
                    matchArrA[aMatchIndexCounter].y = (i - 2);
                    aMatchIndexCounter++;
                }
            }

            // Check for a match with pattern B
            matchB = 1;
            for (int m = 0; m < PATTERN_SIZE; m++) {
                for (int n = 0; n < PATTERN_SIZE; n++) {
                    if (image[i + m - 2][j + n - 2] != patternB[m][n]) {
                        matchB = 0;
                        break;
                    }
                }
                if (!matchB) break;
            }

            // Print the result for a match with pattern B
            if (matchB) {
                printf("Thread %d found b match with pattern B at (%d, %d)\n", omp_get_thread_num(), (i - 2), (j - 2));

                if ((j - 2) <= (i - 2))
                {
                    matchArrB[bMatchIndexCounter].x = (j - 2);
                    matchArrB[bMatchIndexCounter].y = (i - 2);
                    bMatchIndexCounter++;
                }
            }
        }
    }

    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        printf("Pattern A Match Coordinates : (%d,%d)\n", matchArrA[i].x, matchArrA[i].y);
    }

    for (size_t i = 0; i < bMatchIndexCounter; i++)
    {
        printf("Pattern B Match Coordinates : (%d,%d)\n", matchArrB[i].x, matchArrB[i].y);
    }

   
    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        for (size_t j = 0; j < bMatchIndexCounter; j++)
        {
            if ((matchArrA[i].x == matchArrB[j].y) && (matchArrA[i].y == matchArrB[j].x))
            {
                printf("Patterns are matched ---> (%d,%d) - (%d,%d)\n", matchArrA[i].x, matchArrA[j].y, matchArrB[j].x, matchArrB[i].y);
            }
        }
    }
    printf("************************************************\n\n\n");
}

// Function to perform the pattern matching
void searchPatternsSerial(unsigned short image[][IMAGE_WIDTH]) {
    int matchA;
    int matchB;
    MatchCoordinates mA;
    MatchCoordinates mB;
    MatchCoordinates matchArrA[NUMBER_OF_MAX_MATCHES];
    MatchCoordinates matchArrB[NUMBER_OF_MAX_MATCHES];
    int aMatchIndexCounter = 0;
    int bMatchIndexCounter = 0;

    printf("************************************************\n");
    printf("Serial\n");
    printf("************************************************\n");
    // Loop over the image to find matches using OpenMP parallelism
    for (int i = 2; i < IMAGE_HEIGHT - 2; i++) {
        for (int j = 2; j < IMAGE_WIDTH - 2; j++) {
            // Check for a match with pattern A
            matchA = 1;
            for (int m = 0; m < PATTERN_SIZE; m++) {
                for (int n = 0; n < PATTERN_SIZE; n++) {
                    if (image[i + m - 2][j + n - 2] != patternA[m][n]) {
                        matchA = 0;
                        break;
                    }
                }
                if (!matchA) break;
            }

            // Print the result for a match with pattern A
            if (matchA) {
                printf("Found a match with pattern A at (%d, %d)\n",(i - 2), (j - 2));

                if ((j - 2) >= (i - 2))
                {
                    matchArrA[aMatchIndexCounter].x = (j - 2);
                    matchArrA[aMatchIndexCounter].y = (i - 2);
                    aMatchIndexCounter++;
                }
            }

            // Check for a match with pattern B
            matchB = 1;
            for (int m = 0; m < PATTERN_SIZE; m++) {
                for (int n = 0; n < PATTERN_SIZE; n++) {
                    if (image[i + m - 2][j + n - 2] != patternB[m][n]) {
                        matchB = 0;
                        break;
                    }
                }
                if (!matchB) break;
            }

            // Print the result for a match with pattern B
            if (matchB) {
                printf("Found b match with pattern B at (%d, %d)\n",(i - 2), (j - 2));

                if ((j - 2) <= (i - 2))
                {
                    matchArrB[bMatchIndexCounter].x = (j - 2);
                    matchArrB[bMatchIndexCounter].y = (i - 2);
                    bMatchIndexCounter++;
                }
            }
        }
    }

    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        printf("Pattern A Match Coordinates : (%d,%d)\n", matchArrA[i].x, matchArrA[i].y);
    }

    for (size_t i = 0; i < bMatchIndexCounter; i++)
    {
        printf("Pattern B Match Coordinates : (%d,%d)\n", matchArrB[i].x, matchArrB[i].y);
    }

    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        for (size_t j = 0; j < bMatchIndexCounter; j++)
        {
            if ((matchArrA[i].x == matchArrB[j].y) && (matchArrA[i].y == matchArrB[j].x))
            {
                printf("Patterns are matched ---> (%d,%d) - (%d,%d)\n", matchArrA[i].x, matchArrA[j].y, matchArrB[j].x, matchArrB[i].y);
            }
        }
    }

    printf("************************************************\n\n\n");
}

int main() {
    // Load the image into a 2D array
    unsigned short image[IMAGE_HEIGHT][IMAGE_WIDTH];
    double serial_start_time;
    double serial_end_time;
    double serial_elapsed_time;
    double parallel_start_time;
    double parallel_end_time;
    double parallel_elapsed_time;
    double speedup;
    double efficiency;

    /* This function is used for create random image*/
    loadImage(image);
    displayImage(sampleImage);

    /*Parallel*/
    parallel_start_time = omp_get_wtime();
    omp_set_num_threads(NUM_THREADS);  // Set the desired number of threads
    // Perform pattern matching
    searchPatterns(sampleImage);
    parallel_end_time = omp_get_wtime();
    parallel_elapsed_time = parallel_end_time - parallel_start_time;

    /*Serial*/
    serial_start_time = omp_get_wtime();
    searchPatternsSerial(sampleImage);
    serial_end_time = omp_get_wtime();
    serial_elapsed_time = serial_end_time - serial_start_time;

    speedup = serial_elapsed_time / parallel_elapsed_time;
    efficiency = speedup / NUM_THREADS;

    /* Metrics */
    printf("Serial Elapsed Time: %f seconds\n", serial_elapsed_time);
    printf("Parallel Elapsed Time: %f seconds\n", parallel_elapsed_time);
    printf("Speedup: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    return 0;
}
