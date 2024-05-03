#include <stdio.h>
#include <string.h>
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

int nrJobs;
int nrMachines;
int nrOperations;

void execute(int row, Solution (*solution)[nrMachines]) {
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

        // Reset machines
        for (int m = 0; m < nrMachines; m++) {
            machines[m].isFree = true;
            machines[m].endTime = 0;
        }
    };
}

void execute_parallel(Solution (*solution)[nrMachines]) {
    #pragma omp parallel num_threads(nrJobs)
    {
        int row = omp_get_thread_num();
        execute(row, solution);
    };
}

void execute_sequencial(Solution (*solution)[nrMachines]) {
    for (int row = 0; row < nrJobs; row++) {
        execute(row, solution);
    }
}

int main(int argc, char *argv[]) {
    int numero_medias = 1;

    if (argc < 3 || argc > 4) {
        printf("%s <input_file> <sequential|parallel> <?numero de medias>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(argc == 4) {
        numero_medias = atoi(argv[3]);
    }

    char *processing_mode = argv[2];
    if(!strcmp(processing_mode, "sequential") && !strcmp(processing_mode, "parallel")) {
        printf("Erro: Modo invalido!\n");
        return EXIT_FAILURE;
    }

    if((inputFile = fopen(argv[1], "r")) == NULL) {
        printf("Erro: Nao foi possivel abrir %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    if((outputFile = fopen("output.txt", "w")) == NULL) {
        printf("Erro: Nao foi possivel abrir output.txt\n");
        return EXIT_FAILURE;
    }

    if(fscanf(inputFile, "%d%d", &nrJobs, &nrMachines) != 2) {
        printf("Erro: Ficheiro corrompido\n");
        return EXIT_FAILURE;
    }

    nrOperations = nrMachines;

    Solution solution[nrJobs][nrMachines];

    // Inicializar memoria a zero
    memset(&solution, 0, sizeof(solution));

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

    double total_times = 0;

    for(int i = 0; i < numero_medias; i++) {
        double initTime = getClock();

        if(strcmp(processing_mode, "sequential")) {
            execute_sequencial(solution);
        } else {
            execute_parallel(solution);
        }

        total_times += (initTime - getClock()) ;
    }

    double time_avg = total_times/numero_medias;

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
    printf("\n\nTempo de execução (s): %.6f\n", time_avg);

    return EXIT_SUCCESS;
}

