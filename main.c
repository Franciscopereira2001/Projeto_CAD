#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <stdbool.h>
#include<unistd.h>

#include <stdlib.h>

#define MAX_NUM 25

#ifdef _OPENMP
#include <omp.h>
#define getClock() omp_get_wtime()
#else
#define getClock() ((double)clock() / CLOCKS_PER_SEC)
#endif

typedef struct operation {
    int job;
    int time;
    int machine;
} Operation;

typedef struct  job {
    int n;
    Operation  operations[MAX_NUM];
} Job;

typedef  struct solution {
    int initialTime;
    int endTime;
    int machine;
} Solution;

typedef struct  machine {
    int n;
    bool isFree;
    int endTime;
} Machine;

FILE *inputFile;
FILE *outputFile;
Job jobs[MAX_NUM];
Machine machines[MAX_NUM];
int maxEndTime = 0;

int main(int argc, char **argv) {
    int nrJobs;
    int nrMachines;
    int nrOperations;
    int cost = 0;
    
    inputFile = fopen(argv[1], "r");
    outputFile = fopen("output.txt", "w");
    fscanf(inputFile, "%d%d", &nrJobs, &nrMachines);
    nrOperations = nrMachines;

    printf("nrJobs: %d\n", nrJobs);
    Solution solution[nrJobs][nrMachines];
    memset(&solution, 0, sizeof(solution));

    printf("Jobs: %d\n", nrJobs);
    printf("Machines: %d\n", nrMachines);
    printf("Operations: %d\n", nrOperations * nrJobs);

    for (int m = 0; m < nrMachines; m++) {
        machines[m].isFree = true;
        machines[m].n = m;
    };

    for (int i = 0; i < nrJobs; i++) {
        jobs[i].n = i;

        for (int j = 0; j < nrMachines; j++) {
            fscanf(inputFile, "%d%d", &jobs[i].operations[j].machine, &jobs[i].operations[j].time);
            jobs[i].operations[j].job = i;
        };
    };

    for (int k = 0; k < nrJobs; k++) {
        printf("\n\nJob: %d | Operations: \n", jobs[k].n);

        for (int kk = 0; kk < nrMachines; kk++) {
            printf("J%d M%d T%d | ",
                   jobs[k].operations[kk].job, jobs[k].operations[kk].machine, jobs[k].operations[kk].time);
        };
    };

    printf("\n\n\n");
    double initTime = getClock();

    #pragma omp parallel num_threads(nrJobs)
    {
        int row = omp_get_thread_num();

        for (int col = 0; col < nrOperations; col++) {
            #pragma omp critical

            if (machines[jobs[row].operations[col].machine].isFree) {
                printf("\nFree, row: %d", row);

                solution[row][col].initialTime = maxEndTime;
                solution[row][col].machine = jobs[row].operations[col].machine;
                solution[row][col].endTime = solution[row][col].initialTime + jobs[row].operations[col].time;

                machines[jobs[row].operations[col].machine].isFree = false;
                machines[jobs[row].operations[col].machine].endTime = solution[row][col].endTime;
            } else {
                printf("\nNot Free, row: %d", row);

                solution[row][col].initialTime = machines[jobs[row].operations[col].machine].endTime;
                solution[row][col].machine = jobs[row].operations[col].machine;
                solution[row][col].endTime = solution[row][col].initialTime + jobs[row].operations[col].time;

                machines[jobs[row].operations[col].machine].isFree = false;
                machines[jobs[row].operations[col].machine].endTime = solution[row][col].endTime;
            };

            #pragma omp barrier

            for (int mAux = 0; mAux < nrJobs; mAux ++) {
                if (maxEndTime < solution[mAux][col].endTime) {
                    maxEndTime = solution[mAux][col].endTime;
                };
            };

            for (int m = 0; m < nrMachines; m++) {
                machines[m].isFree = true;
                machines[m].endTime = 0;
            }
        };
    };

    double endTime = getClock();

    printf("\n\n\n");

    printf("\n\n************");
    printf("\n\nSolution:\n");
    for(int sr = 0; sr < nrJobs; sr++) {
        printf("Job %d | ", sr);

        for(int sc = 0; sc < nrMachines; sc ++) {
            printf("(%d)[%d,%d] | ", solution[sr][sc].machine ,solution[sr][sc].initialTime, solution[sr][sc].endTime);
            fprintf(outputFile, "%d ", solution[sr][sc].initialTime);
        };
        printf("\n");
        fprintf(outputFile, "\n");
    };
    printf("\n");
    printf("*******************");
    printf("\n\nTempo de execução (s): %.6f\n", endTime - initTime);
}

