#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define RADIUS 1

struct Result{
    int total;
    int inside;
};

struct Input{
    int numberOfShots;
    struct Result* resultPointer;
};

/* generate a random floating point number from min to max */
double fRand()
{
   return (double)rand() / RAND_MAX;

}

double calculateDistanceToPoint (double x, double y) {
    return sqrt((x * x) + (y * y));
}

void writeTotal (int total1, int inside1, struct Result* rst) {
    rst->total = total1;
    rst->inside = inside1;

}

void* approxPi (void *args) {

    struct Input*  input = (struct Input*) args;
    int numberOfShots = input->numberOfShots;


    int total = 0;
    int inside = 0;

    srand(getpid());

    int i;
    for (i = 0; i < numberOfShots; i++) {
        double xCoor = fRand();
        double yCoor = fRand();

        double distance = calculateDistanceToPoint(xCoor, yCoor);
        total++;

        if (distance <= 1.0) {
            inside++;
        }
    }

    writeTotal(total, inside, input->resultPointer);

    return NULL;
}

int main(int argc, char **argv) {

    int numberOfTries;
    int numberOfThreads;

    if(argc != 3) {
        printf("Usage: $ ApproxPi numberOfShots numberOfThreads\n");
        return 1;
    }

    numberOfTries = strtol(argv[1], NULL, 0);
    numberOfThreads = strtol(argv[2], NULL, 0);

    int  total = 0;
    int inside = 0;
    int shotsByThread = numberOfTries / numberOfThreads;

    struct Input inputs[numberOfThreads];


    struct Result rst [numberOfThreads];

    pthread_t tid[numberOfThreads];
    for (int i = 0; i < numberOfThreads; i++) {

        inputs[i].numberOfShots = shotsByThread;
        inputs[i].resultPointer = &rst[i];

        void* args = (void*) &inputs[i];

        pthread_create(&tid[i], NULL, approxPi, args);
    }
    for (int i = 0; i<numberOfThreads; i++)
        pthread_join(tid[i], NULL);

    for (int i = 0; i<numberOfThreads; i++){
        total+= rst[i].total;
        inside+=rst[i].inside;

    }if(total != numberOfTries){
        printf("The threads did not perform all of the shots\n");
    }



    double pi = ((double)inside/(double)total)*4;
    printf("Approximation of Pi: %f\n", pi);

    return 0;
}