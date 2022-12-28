#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

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
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    alloc();

    if(rank == 0) 
        init();

    kmeans();
    
    if(rank == 0) 
        print();
    
    MPI_Finalize();
    
    return 0;
}



