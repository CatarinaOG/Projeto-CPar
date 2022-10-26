
#include <stdio.h>
#include <stdlib.h>
#include <float.h>


#define N 10000000  
#define K 4


struct Point{
    float x;
    float y;
    int centroid;
    float minDist;
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
        points[i].minDist = FLT_MAX;
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



void updateCentroidCoord(){

    for(int j=0; j<K; j++){

        centroids[j].sumX=0;
        centroids[j].sumY=0;
        centroids[j].points=0;
    }

    for(int i=0; i<N ; i++){

        centroids[points[i].centroid].sumX += points[i].x;
        centroids[points[i].centroid].sumY += points[i].y;
        centroids[points[i].centroid].points++;
    }

    for(int j=0; j<K ; j++){

        centroids[j].x = centroids[j].sumX / (float) centroids[j].points;
        centroids[j].y = centroids[j].sumY / (float) centroids[j].points;
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
            
            index = points[i].centroid;                                                    
            min = dist(centroids[index].x,centroids[index].y,points[i].x,points[i].y);
            
            // Descobrir min para todos os centroids
            for(int j=0; j< K; j++ ){

                newDist = dist(centroids[j].x, centroids[j].y, points[i].x , points[i].y );

                if( min > newDist) {
                    index = j;
                    min = newDist;
                    changedCentroid = 1;
                }            
            }

            // Novo index se alterado ou o antigo
    	    points[i].centroid = index;

        }

        updateCentroidCoord();

        iterations++;

    } while(changedCentroid);   
}


void print(){

    printf("Iterations: %d\n",iterations);
    printf("N = %d, k = %d\n",N,K);
    for(int i = 0; i<K ;i++)
        printf("Center: (%f,%f) -> Size: %d\n",centroids[i].x,centroids[i].y,centroids[i].points);
    
}


int main(){
    alloc();
    init();
    kmeans();
    print();
    printf("size: %ld",sizeof(points[0]));
}

