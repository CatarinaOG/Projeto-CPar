
#include <stdio.h>
#include <stdlib.h>



#define N 10000000
#define K 4
#define POINTSIZE 4 // x,y,centroid,minDist
#define CENTROIDSIZE 5
#define MAXDIST 5000000



double *points;
double *centroids;
int iterations = 0;


void alloc(){

    points = (double *) malloc(POINTSIZE * N * sizeof(double));
    centroids = (double *) malloc(CENTROIDSIZE * K * sizeof(double));

}

void init(){

    srand(10);
    
    for( int i=0 ; i < N; i++ ){
        points[i * POINTSIZE] = (double) rand() / RAND_MAX;
        points[i * POINTSIZE + 1] = (double) rand()/ RAND_MAX;
        points[i * POINTSIZE + 2] = -1;
        points[i * POINTSIZE + 3] = MAXDIST;
    }

    for(int i=0; i < K ; i++){
        centroids[i * CENTROIDSIZE] = points[i * POINTSIZE ];
        centroids[i * CENTROIDSIZE + 1] = points[i * POINTSIZE + 1];
        centroids[i * CENTROIDSIZE + 2] = 0; // numero de pontos
        centroids[i * CENTROIDSIZE + 3] = 0; // soma das coordenadas x
        centroids[i * CENTROIDSIZE + 4] = 0; // soma das coordenadas y
    }
}

double calcDist(float x1, float y1, float x2, float y2){
    double result = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));

    return result;
}


void changeCentroid(int pIndex, float  cIndex, double pDist){

    int oldCentroid = points[pIndex * POINTSIZE + 2];

    // Mudanças no ponto
    points[pIndex * POINTSIZE + 2] = cIndex;
    points[pIndex * POINTSIZE + 3] = pDist;

    // Mudanças no centroid antigo
    centroids[oldCentroid * CENTROIDSIZE + 2]--;
    centroids[oldCentroid * CENTROIDSIZE + 3] -= points[pIndex * POINTSIZE ];
    centroids[oldCentroid * CENTROIDSIZE + 4] -= points[pIndex * POINTSIZE + 1];

    // Mudanças no centroid atual
    centroids[(int) cIndex * CENTROIDSIZE + 2]++;
    centroids[(int) cIndex * CENTROIDSIZE + 3] += points[pIndex * POINTSIZE];
    centroids[(int) cIndex * CENTROIDSIZE + 4] += points[pIndex * POINTSIZE + 1];

}




void updateCentroidCoord(){

    for(int j=0; j<K ; j++){

        if(centroids[j * CENTROIDSIZE + 2] != 0){

            float newX = centroids[j * CENTROIDSIZE + 3] / centroids[j * CENTROIDSIZE + 2];
            float newY = centroids[j * CENTROIDSIZE + 4] / centroids[j * CENTROIDSIZE + 2];

            centroids[j * CENTROIDSIZE] = newX;
            centroids[j * CENTROIDSIZE + 1] = newY;


        }
    }

}


void kmeans(){

    int changed = 1;

    while(changed){

        double newDist;
        changed = 0;

        for(int i=0 ; i<N; i++){

            double min = points[i * POINTSIZE + 3] ;
            float index = -1;
            
            for(int j=0; j< K; j++ ){
                newDist = calcDist(points[i * POINTSIZE] , points[i * POINTSIZE + 1],centroids[j * CENTROIDSIZE ],centroids[j * CENTROIDSIZE +1]);

                if(points[i * POINTSIZE + 2] != (float) j ){                                                                    
                    
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
        printf("Center: (%f,%f) -> Size: %f\n",centroids[i* CENTROIDSIZE],centroids[i* CENTROIDSIZE+1],centroids[i* CENTROIDSIZE+2]);
    
}


int main(){
    alloc();
    init();
    kmeans();
    print();
}


