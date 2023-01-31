/*
    This program solves Laplace's equation on a regular 2D grid using simple Jacobi iteration.

    The stencil calculation stops when  iter > ITER_MAX
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define ITER_MAX 3000 // number of maximum iterations
#define CONV_THRESHOLD 1.0e-5f // threshold of convergence

// matrix to be solved
double **grid;

// auxiliary matrix
double **new_grid;

// size of each side of the grid
int size;

// number of threads
int num_threads;

// return the maximum value
double max(double a, double b){
    if(a > b)
        return a;
    return b;
}

// return the absolute value of a number
double absolute(double num){
    if(num < 0)
        return -1.0 * num;
    return num;
}

// allocate memory for the grid
void allocate_memory(){
    grid = (double **) malloc(size * sizeof(double *));
    new_grid = (double **) malloc(size * sizeof(double *));

    for(int i = 0; i < size; i++){
        grid[i] = (double *) malloc(size * sizeof(double));
        new_grid[i] = (double *) malloc(size * sizeof(double));
    }
}

// initialize the grid
void initialize_grid(){
    // seed for random generator
    srand(10);

    int linf = size / 2;
    int lsup = linf + size / 10;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            // inicializa região de calor no centro do grid
            if ( i>=linf && i < lsup && j>=linf && j<lsup)
                grid[i][j] = 100;
            else
               grid[i][j] = 0;
            new_grid[i][j] = 0.0;
        }
    }
}

// save the grid in a file
void save_grid(){

    char file_name[30];
    sprintf(file_name, "grid_pth.txt");

    // save the result
    FILE *file;
    file = fopen(file_name, "w");

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            fprintf(file, "%lf ", grid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// encapsulate the thread args
struct thread_data{
    int id; //thread id
    int begin; //begin iterator of the thread's chunk
    int end; //end iterator of the thread's chunk
};

// array to store the input data (args) of each thread
struct thread_data *thread_data_array_calc;
struct thread_data *thread_data_array_data;

void *calculate_laplace(void *);
void *pass_values(void *);

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Usage: ./laplace_seq N\n");
        printf("N: The size of each side of the domain (grid)\n");
        printf("T: The number of threads\n");
        exit(-1);
    }

    // variables to measure execution time
    struct timeval time_start;
    struct timeval time_end;

    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    // allocate memory to the grid (matrix)
    allocate_memory();

    // set grid initial conditions
    initialize_grid();

    // create an array of p_threads 
    pthread_t threads_calc[num_threads];
    pthread_t threads_values[num_threads];

    // store the input data of each thread
    thread_data_array_calc = (struct thread_data *) malloc(num_threads * sizeof(struct thread_data));
    thread_data_array_data = (struct thread_data *) malloc(num_threads * sizeof(struct thread_data));

    // calculate the chunck size
    int chunk = size / num_threads;

    int iter = 0;

    

    printf("Jacobi relaxation calculation: %d x %d grid\n", size, size);

    // get the start time
    gettimeofday(&time_start, NULL);

    // Jacobi iteration
    // This loop will end if either the maximum change reaches below a set threshold (convergence)
    // or a fixed number of maximum iterations have completed
    while ( iter <= ITER_MAX ) {
        // create the threads
        for(int i = 0; i < num_threads; i++){

            // set the thread args
            thread_data_array_calc[i].id = i;
            thread_data_array_calc[i].begin = i * chunk;
            thread_data_array_calc[i].end = thread_data_array_calc[i].begin + chunk;

            // the last thread might have more work
            // if num_terms % num_threads != 0
            if( i == num_threads-1 ){
                thread_data_array_calc[i].end = size-2;
            }
                
            // create the thread
            int error_code = pthread_create(&threads_calc[i], NULL, calculate_laplace, (void *) &thread_data_array_calc[i]);

            if (error_code){
                printf("ERROR: return code from pthread_create() is %d\n", error_code);
                exit(-1);
            };

        }

        // wait each thread to finish and get the local sum
        for(int i = 0; i < num_threads; i++){
            pthread_join(threads_calc[i], NULL);
        }

        // create the threads
        for(int i = 0; i < num_threads; i++){

            // set the thread args
            thread_data_array_data[i].id = i;
            thread_data_array_data[i].begin = i * chunk;
            thread_data_array_data[i].end = thread_data_array_data[i].begin + chunk;

            // the last thread might have more work
            // if num_terms % num_threads != 0
            if( i == num_threads-1 ){
                thread_data_array_data[i].end = size-2;
            }
                
            // create the thread
            int error_code = pthread_create(&threads_values[i], NULL, pass_values, (void *) &thread_data_array_data[i]);

            if (error_code){
                printf("ERROR: return code from pthread_create() is %d\n", error_code);
                exit(-1);
            };

        }

        // wait each thread to finish and get the local sum
        for(int i = 0; i < num_threads; i++){
            pthread_join(threads_values[i], NULL);
        }
    
        
        
        if(iter % 100 == 0){
            //printf("Iteration %d\n", iter);
        }
            

        iter++;
    }

    // get the end time
    gettimeofday(&time_end, NULL);

    double exec_time = (double) (time_end.tv_sec - time_start.tv_sec) +
                       (double) (time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    //save the final grid in file
    save_grid();

    free(thread_data_array_data);
    free(thread_data_array_calc);

    printf("Kernel executed in %lf seconds with %d iterations \n", exec_time, iter);

    return 0;
}


void *calculate_laplace(void *args){

    struct thread_data *argument = (struct thread_data *) args;

    int my_id = argument->id;
    int begin = argument->begin;
    int end = argument->end;

    // calculates the Laplace equation to determine each cell's next value
    // kernel 1

    for( int i = begin+1; i <= end; i++) {
        for(int j = 1; j < size-1; j++) {

            new_grid[i][j] = 0.25 * (grid[i][j+1] + grid[i][j-1] +
                                        grid[i-1][j] + grid[i+1][j]);

        }
    }

    
}

void *pass_values(void *args){
    
    struct thread_data *argument = (struct thread_data *) args;

    int my_id = argument->id;
    int begin = argument->begin;
    int end = argument->end;

    // copy the next values into the working array for the next iteration
    // kernel 2
    for( int i = begin+1; i <= end; i++) {
        for( int j = 1; j < size-1; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }

}