
#include <stdio.h>
#include <stdlib.h>



#define N 10
#define K 4
#define MAXDIST 5000000000



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
        points[i].y = (float) rand()/ RAND_MAX;
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

float calcDist(float x1, float y1, float x2, float y2){
    float result = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));

    return result;
}


void changePointsCentroid(int pIndex, int  cIndex){

    int oldCentroid = points[pIndex].centroid;

    // Mudanças no ponto
    points[pIndex].centroid = cIndex;

    // Mudanças no centroid atual
    centroids[cIndex].points++;
    centroids[cIndex].sumX += points[pIndex].x;
    centroids[cIndex].sumY += points[pIndex].y;

}




void updateCentroidCoord(int changed){

    if(changed == 1){

        for(int j=0; j<K ; j++){

            if(centroids[j].points != 0){

                float newX = centroids[j].sumX / centroids[j].points;
                float newY = centroids[j].sumY / centroids[j].points;

                centroids[j].x = newX;
                centroids[j].y = newY;
                centroids[j].points = 0;
                centroids[j].sumX = 0;
                centroids[j].sumY = 0;
            }
        }
    }

}

float getNewDist(int i){

    int cIndex = points[i].centroid;

    if(cIndex != -1)
        points[i].minDist = calcDist(points[i].x,points[i].y,centroids[cIndex].x,centroids[cIndex].y);

}


void kmeans(){

    int changed = 1;

    while(changed){

        float newDist;
        changed = 0;

        for(int i=0 ; i<N; i++){

            int index = -1;
            getNewDist(i);
            
            for(int j=0; j<K; j++ ){
                newDist = calcDist(points[i].x , points[i].y ,centroids[j].x, centroids[j].y);

                if(points[i].minDist > newDist) {
                    points[i].minDist = newDist;
                    index = j;
                    changed = 1;
                }            
            }

            if(changed == 1 && index != -1)
                changePointsCentroid( i , index);
            

        }
        updateCentroidCoord(changed);
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


