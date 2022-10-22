
#include <stdio.h>
#include <stdlib.h>



#define N 10000000
#define K 4
#define MAXDIST 5000000



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

    points = (double *) malloc(N * sizeof(struct Point));
    centroids = (double *) malloc(K * sizeof(struct Centroid));

}

void init(){

    srand(10);
    
    for( int i=0 ; i < N; i++ ){
        points[i].x = (double) rand() / RAND_MAX;
        points[i].y = (double) rand()/ RAND_MAX;
        points[i].centroid = -1;
        points[i].minDist = MAXDIST;
    }

    for(int i=0; i < K ; i++){
        centroids[i].x = points[i].x;
        centroids[i].y = points[i].y;
        centroids[i].points = 0;
        centroids[i].sumX = 0; 
        centroids[i].sumY = 0;
    }
}

double calcDist(float x1, float y1, float x2, float y2){
    double result = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));

    return result;
}


void changeCentroid(int pIndex, int  cIndex, double pDist){

    int oldCentroid = points[pIndex].centroid;

    // Mudanças no ponto
    points[pIndex].centroid = cIndex;
    points[pIndex].minDist = pDist;

    // Mudanças no centroid antigo
    centroids[oldCentroid].points--;
    centroids[oldCentroid].sumX -= points[pIndex].x;
    centroids[oldCentroid].sumY -= points[pIndex].y;

    // Mudanças no centroid atual
    centroids[cIndex].points++;
    centroids[cIndex].sumX += points[pIndex].x;
    centroids[cIndex].sumY += points[pIndex].y;

}




void updateCentroidCoord(){

    for(int j=0; j<K ; j++){

        if(centroids[j].points != 0){

            float newX = centroids[j].sumX / centroids[j].points;
            float newY = centroids[j].sumY / centroids[j].points;

            centroids[j].x = newX;
            centroids[j].y = newY;
        }
    }

}


void kmeans(){

    int changed = 1;

    while(changed){

        double newDist;
        changed = 0;

        for(int i=0 ; i<N; i++){

            double min = points[i].minDist ;
            int index = -1;
            
            for(int j=0; j< K; j++ ){
                newDist = calcDist(points[i].x , points[i].y ,centroids[j].x, centroids[j].y);

                if(points[i].centroid != j ){                                                                    
                    
                    if(min > newDist) {
                        min = newDist;
                        index = j;
                        changed = 1;
                    }            
                }
            }

            if(changed == 1 && index != -1){
                changeCentroid( i , index ,min);
            }
        }
        updateCentroidCoord();
        iterations++;

    }   
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
}


