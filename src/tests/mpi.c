#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

/*******************************************************************************
 * Random list generation
 ******************************************************************************/

static int getUniformInteger(const int min, const int max)
{
    long int diff = (long) max - (long) min + 1;
    return min + (rand() / (RAND_MAX + 1.0)) * diff;
}

static void makeListOfInts(int *list, int size)
{
    int i;
    long sum;

    srand(time(NULL));
    sum = 0;
    for (i = 0; i < size; i++) {
        list[i] = getUniformInteger(-2 * size, 2 * size);
        sum += list[i];
    }
    printf("Actual sum is: %ld\n", sum);
}

/*******************************************************************************
 * Main
 ******************************************************************************/

int main(int argc, char **argv)
{
    // Process info
    int numNodes, myId;
    // Vars
    int i;
    int *data = NULL, *locData, dataSize, dataPerNode;
    long sum, *sums;

    // Before MPI_Init
    printf("PID before MPI_Init: %d. PPID: %d\n", getpid(), getppid());

    // Init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numNodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);
    dataSize = 1000;
    dataPerNode = ceil(dataSize / (float) numNodes);

    // Hello world
    printf("Hello! I am process with PID: %d, and rank: %d of %d.\n", getpid(),
        myId, numNodes);

    // Root data
    if (myId == 0) {
        // Make data
        data = malloc(sizeof(int) * dataSize);
        makeListOfInts(data, dataSize);

        // Sums
        sums = malloc(sizeof(long) * numNodes);
    }

    // Scatter data
    locData = malloc(sizeof(int) * dataPerNode);
    MPI_Scatter(
        data, dataPerNode, MPI_INT, locData, dataPerNode, MPI_INT, 0,
        MPI_COMM_WORLD
    );

    // Working
    sum = 0;
    for (i = 0; i < dataPerNode; i++) {
        sum += locData[i];
    }
    printf("[%d] Local sum: %ld\n", myId, sum);

    // Send back the results
    MPI_Gather(&sum, 1, MPI_LONG, sums, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Printout results
    if (myId == 0) {
        sum = 0;
        for (i = 0; i < numNodes; i++) {
            sum += sums[i];
        }
        printf("Total sum: %ld\n", sum);
    }

    // Finalize
    free(locData);
    if (myId == 0) free(sums);
    if (myId == 0) free(data);
    MPI_Finalize();

    return 0;
}
