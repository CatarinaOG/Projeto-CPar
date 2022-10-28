#include <stdio.h>
#include <stdlib.h>

#define N 10000000
#define K 4


struct Point{
    float* x;           // lista de coordenada x
    float* y;           // lista de coordenada y
    int* centroid;      // lista de centroid atribuido
};

struct Centroid{
    float* x;           // lista de coordenada x
    float* y;           // lista de coordenada y
    int* points;        // lista de nº de pontos atributidos ao centroid
    float* sumX;        // lista de soma das coordenadas x dos pontos atribuidos
    float* sumY;        // lista de soma das coordenadas y dos pontos atribuidos
};

struct Point points;
struct Centroid centroids;

int iterations = 0;     //número de iterações do programa

// Alocação de espaço para as structs de listas
void alloc(){

    points.x = (float*) malloc(N * sizeof(float));
    points.y = (float*) malloc(N * sizeof(float));
    points.centroid = (int*) malloc(N * sizeof(int));

    centroids.x = (float*) malloc(N * sizeof(float));
    centroids.y = (float*) malloc(N * sizeof(float));
    centroids.points = (int*) malloc(N * sizeof(int));
    centroids.sumX = (float*) malloc(N * sizeof(float));
    centroids.sumY = (float*) malloc(N * sizeof(float));

}

// Inicializa as structs de listas com as informações relativamente aos pontos e centroids, com coordenadas random
void init(){

    srand(10);

    for( int i=0 ; i < N; i++ ){
        points.x[i] = (float) rand() / RAND_MAX;
        points.y[i] = (float) rand() / RAND_MAX;
        points.centroid[i] = 0;
    }

    for(int i=0; i < K ; i++){
        centroids.x[i] = points.x[i];
        centroids.y[i] = points.y[i];
        centroids.points[i] = 0;
        centroids.sumX[i] = 0;
        centroids.sumY[i] = 0;
    }
}

// Calcula a distância entre 2 pontos
float dist(float x1, float y1, float x2, float y2){
    float result = (x1 - x2)*(x1 - x2)  + (y1 - y2)*(y1 - y2);

    return result;
}


// Resetar Centroids e percorrer todos os pontos e adiciona-los ao centroid correto de modo a obter novas coordenadas
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


//Função principal do programa, executa um ciclo que executa as funções necessárias para calcular os centroids de cada ponto
//sendo que este ciclo para de executar quando não houver uma mudança dos centroids de nenhum ponto numa iteração 
void kmeans(){

    float min;
    int index;
    float newDist;
    int changedCentroid;

    do{
        changedCentroid = 0;

        // Atribuit um centroid a cada ponto
        for(int i=0 ; i<N; i++){

            
            index = points.centroid[i];
            min = dist(centroids.x[index],centroids.y[index],points.x[i],points.y[i]);

            // Descobrir o centroid mais perto do ponto
            for(int j=0; j< K; j++ ){
                
                //A cada iteração faz o calculo da distãncia para o centroid no indice igual à iteração do ciclo
                newDist = dist(centroids.x[j], centroids.y[j], points.x[i] , points.y[i] );
                
                //Caso a distância seja menor que o mínimo atual, o valor substitui min e o indice da iteração fica armazenado em index
                if( min > newDist) {
                    index = j;
                    min = newDist;
                    changedCentroid = 1;
                }
            }

            // Atribuido o novo centroid ou o antigo se não existir outro mais perto
            points.centroid[i] = index;

        }
        //No fim de uma iteração pelos pontos todos, é recalculado o valor das coordenadas dos centroids
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

