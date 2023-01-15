#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <papi.h>

#define N 10000000
#define K 4


struct Point{
    float x;
    float y;
    int centroid;
};

struct Centroid{
    float x;
    float y;
    int points;
    float sumX;
    float sumY;
};

struct Point *points;
struct Centroid *centroids;

int iterations = 0;
int rank;
int nprocs;

struct Point *pPoints;

MPI_Datatype dt_point;
MPI_Datatype dt_centroid;

MPI_Op sum_centroids_op;
MPI_Op papi_reducee;


void alloc(){

    points = (struct Point *) malloc(N * sizeof(struct Point));
    centroids = (struct Centroid *) malloc(K * sizeof(struct Centroid));

}

void init(){

    srand(10);
    
    for( int i=0 ; i < N; i++ ){
        points[i].x = (float) rand() / RAND_MAX;
        points[i].y = (float) rand() / RAND_MAX;
        points[i].centroid = 0;
    }

    for(int i=0; i < K ; i++){
        centroids[i].x = points[i].x;
        centroids[i].y = points[i].y;
        centroids[i].points = 0;
        centroids[i].sumX = 0; 
        centroids[i].sumY = 0;
    }
}

float dist(float x1, float y1, float x2, float y2){
    float result = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));

    return result;
}


void centroid_reduce_function( void * in, void * out, int * len, MPI_Datatype *datatype){

    struct Centroid* in_data = (struct Centroid*) in;
    struct Centroid* out_data = (struct Centroid*) out;

     for(int i=0; i< *len ; i++){
        out_data[i].sumX += in_data[i].sumX;
        out_data[i].sumY += in_data[i].sumY;
        out_data[i].points += in_data[i].points;
     }
}

void papi_reduce_function( void * in, void * out, int * len, MPI_Datatype *datatype){

    long long int* in_data = (long long int *) in;
    long long int* out_data = (long long int *)out;

     for(int i=0; i< 5 ; i++){
        out_data[i] += in_data[i];
     }
}



void updateCentroidCoord(){

    struct Centroid *send = (struct Centroid *) malloc(sizeof(struct Centroid) * K);

    for(int j=0; j<K; j++){
        send[j].x=0;
        send[j].y=0;
        send[j].sumX=0;
        send[j].sumY=0;
        send[j].points=0;
    }
    
    for(int i=0; i<N/nprocs ; i++){

        send[pPoints[i].centroid].sumX += pPoints[i].x;
        send[pPoints[i].centroid].sumY += pPoints[i].y;
        send[pPoints[i].centroid].points++;

    }

    MPI_Reduce(send,centroids,K,dt_centroid,sum_centroids_op,0,MPI_COMM_WORLD);

    if(rank == 0){
        for(int j=0; j<K ; j++){
            centroids[j].x = centroids[j].sumX / (float) centroids[j].points;
            centroids[j].y = centroids[j].sumY / (float) centroids[j].points;
        }
    }
}





void kmeans(){

    float min;
    int index;
    float newDist;
    int changedCentroid;
    int pChangedCentroid;


    int blocklengths[3] = {1,1,1};
    MPI_Aint displacement[3] = {0,sizeof(float),2*sizeof(float)};
    MPI_Datatype types[3] = {MPI_FLOAT,MPI_FLOAT,MPI_INT};
    MPI_Type_create_struct(3,blocklengths,displacement,types,&dt_point);
    MPI_Type_commit(&dt_point);


    int blocklengths1[5] = {1,1,1,1,1};
    MPI_Aint displacement1[5] = {0,sizeof(float),2*sizeof(float),2*sizeof(float) + sizeof(int), 3*sizeof(float) + sizeof(int)};
    MPI_Datatype types1[5] = {MPI_FLOAT,MPI_FLOAT,MPI_INT,MPI_FLOAT,MPI_FLOAT};
    MPI_Type_create_struct(5,blocklengths1,displacement1,types1,&dt_centroid);
    MPI_Type_commit(&dt_centroid);

    int size = N / nprocs;
    pPoints = (struct Point *) malloc(size * sizeof(struct Point));
    MPI_Scatter(points, size, dt_point , pPoints, size, dt_point ,0,MPI_COMM_WORLD);
    
    MPI_Op_create(centroid_reduce_function,1,&sum_centroids_op);
    
    do{

        MPI_Bcast(centroids,K,dt_centroid,0,MPI_COMM_WORLD);

        changedCentroid = 0;
        pChangedCentroid = 0;


        for(int i=0 ; i<N/nprocs; i++){
            
            index = pPoints[i].centroid;                                                    
            min = dist(centroids[index].x,centroids[index].y,pPoints[i].x,pPoints[i].y);

            for(int j=0; j< K; j++ ){

                newDist = dist(centroids[j].x, centroids[j].y, pPoints[i].x , pPoints[i].y );

                if( min > newDist) {
                    index = j;
                    min = newDist;
                    pChangedCentroid = 1;
                }            
            }

            pPoints[i].centroid = index;
        }

        updateCentroidCoord();
        MPI_Allreduce(&pChangedCentroid,&changedCentroid,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
        iterations++;

    } while(changedCentroid);   
    
}


void print(){

    printf("Iterations: %d\n",iterations);
    printf("N = %d, k = %d\n",N,K);
    for(int i = 0; i<K ;i++)
        printf("Center: (%f,%f) -> Size: %d\n",centroids[i].x,centroids[i].y,centroids[i].points);
    
}





int main(int argc, char *argv[]) {
    MPI_Status status;
    long long int papi_results_local[2], papi_results_global[2], papi_results_global2[5];
    int events[2] = { PAPI_TOT_INS, PAPI_TOT_CYC /*,PAPI_L1_DCM,PAPI_L2_TCM,PAPI_L3_TCM*/ };

    double start_time, end_time;    


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    MPI_Op_create(papi_reduce_function,0,&papi_reducee);


    start_time = MPI_Wtime();

    PAPI_library_init(PAPI_VER_CURRENT);
    int event_set = PAPI_NULL;
    PAPI_create_eventset(&event_set);

    if (PAPI_add_events(event_set, events, 2)!= PAPI_OK) printf("Erro\n\n\n");


    PAPI_start(event_set);


    alloc();

    if(rank == 0) 
        init();

    kmeans();
    PAPI_stop(event_set, papi_results_local);   

        //printf("RANK : %d  ------  instructions: %lld\n",rank, papi_results_local[0]);
        //printf("RANK : %d  ------  cycles: %lld\n",rank, papi_results_local[1]);
        //printf("RANK : %d  ------  L1 Cache Misses: %lld\n",rank, papi_results_local[2]);
        //printf("RANK : %d  ------  L2 Cache Misses: %lld\n",rank, papi_results_local[3]);

        //printf("RANK : %d  ------  L3 Cache Misses: %lld\n",rank, papi_results_local[4]);

    MPI_Reduce(papi_results_local, papi_results_global,2, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    //MPI_Reduce(papi_results_local,papi_results_global2,5,MPI_LONG_LONG,papi_reducee,0,MPI_COMM_WORLD);




    end_time = MPI_Wtime();

    if(rank == 0){
        print();
        printf("Total instructions: %lld\n", papi_results_global[0]);
        //printf("2    Total instructions: %lld\n", papi_results_global2[0]);

        printf("Total cycles: %lld\n", papi_results_global[1]);
        //printf("2    Total cycles: %lld\n", papi_results_global2[1]);
/*
        printf("Total L1 Cache Misses: %lld\n", papi_results_global[2]);
        //printf("2    Total L1 Cache Misses: %lld\n", papi_results_global2[2]);

        printf("Total L2 Cache Misses: %lld\n", papi_results_global[3]);
        //printf("2    Total L2 Cache Misses: %lld\n", papi_results_global2[3]);

        printf("Total L3 Cache Misses: %lld\n", papi_results_global[4]);
        //printf("2    Total L3 Cache Misses: %lld\n", papi_results_global2[4]);
*/
        printf("Execution time: %f\n", end_time - start_time);
    }
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);
    
    MPI_Finalize();
    
    return 0;
}



