
#include <stdio.h>
#include <stdlib.h>
#include <float.h>


#define N 10000000  
#define K 4


struct Point{
    float* x;
    float* y;
    int* centroid;
    float* minDist;
};

struct Centroid{
    float* x;
    float* y;
    int* points;
    float* sumX;
    float* sumY;
};

struct Point points;
struct Centroid centroids;

int iterations = 0;


void alloc(){

    points.x = (float*) malloc(N * sizeof(float));
    points.y = (float*) malloc(N * sizeof(float));
    points.centroid = (int*) malloc(N * sizeof(int));
    points.minDist = (float*) malloc(N * sizeof(float));

    centroids.x = (float*) malloc(N * sizeof(float));
    centroids.y = (float*) malloc(N * sizeof(float));
    centroids.points = (int*) malloc(N * sizeof(int));
    centroids.sumX = (float*) malloc(N * sizeof(float));
    centroids.sumY = (float*) malloc(N * sizeof(float));

}

void init(){

    srand(10);
    
    for( int i=0 ; i < N; i++ ){
        points.x[i] = (float) rand() / RAND_MAX;
        points.y[i] = (float) rand() / RAND_MAX;
        points.centroid[i] = 0;
        points.minDist[i] = FLT_MAX;
    }

    for(int i=0; i < K ; i++){
        centroids.x[i] = points.x[i];
        centroids.y[i] = points.y[i];
        centroids.points[i] = 0;
        centroids.sumX[i] = 0; 
        centroids.sumY[i] = 0;
    }
}

float dist(float x1, float y1, float x2, float y2){
    float result = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));

    return result;
}



void updateCentroidCoord(){

    for(int j=0; j<K; j++){

        centroids.sumX[j]=0;
        centroids.sumY[j]=0;
        centroids.points[j]=0;
    }

    for(int i=0; i<N ; i++){

        centroids.sumX[points.centroid[i]] += points.x[i];
        centroids.sumY[points.centroid[i]] += points.y[i];
        centroids.points[points.centroid[i]]++;
    }

    for(int j=0; j<K ; j++){

        centroids.x[j] = centroids.sumX[j] / (float) centroids.points[j];
        centroids.y[j] = centroids.sumY[j] / (float) centroids.points[j];
    }

}



void kmeans(){

    float min;
    int index;
    float newDist;
    int changedCentroid;

    do{
        changedCentroid = 0;

        // Verificar todos os pontos
        for(int i=0 ; i<N; i++){
            
            index = points.centroid[i];                                                    
            min = dist(centroids.x[index],centroids.y[index],points.x[i],points.y[i]);
            
            // Descobrir min para todos os centroids
            for(int j=0; j< K; j++ ){

                newDist = dist(centroids.x[j], centroids.y[j], points.x[i] , points.y[i] );

                if( min > newDist) {
                    index = j;
                    min = newDist;
                    changedCentroid = 1;
                }            
            }

            // Novo index se alterado ou o antigo
    	    points.centroid[i] = index;

        }

        updateCentroidCoord();

        iterations++;

    } while(changedCentroid);   
}


void print(){

    printf("Iterations: %d\n",iterations);
    printf("N = %d, k = %d\n",N,K);
    for(int i = 0; i<K ;i++)
        printf("Center: (%f,%f) -> Size: %d\n",centroids.x[i],centroids.y[i],centroids.points[i]);
    
}


int main(){
    alloc();
    init();
    kmeans();
    print();
}

