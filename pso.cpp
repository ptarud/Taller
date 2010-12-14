/* 
 * File:   pso.cpp
 * Author: diepeerk
 * 
 * Created on 4 de octubre de 2010, 11:48 AM
 */

#include "pso.h"
#include <cstdlib>
#include <iostream>
#include "particle.h"
#include <cstdio>
#include <fstream>
#include <ctime>
using namespace std;
#define INERTIA_MAX 1
#define INERTIA_MIN 0

pso::pso(int nurses, int days, int shifts)
{
    int cN;
    this->nurses = nurses;
    this->days = days;
    this->shifts = shifts;
    
    /*asigno memoria dinamicamente a matriz de g_best (mejor posicion global) para obtener g_best[nurses+1][days*shifts] */
    g_best = (int**)malloc((nurses+1)*sizeof(int*));
    for(cN=0; cN<(nurses+1);cN++){
        g_best[cN] = (int*)malloc(days*shifts*sizeof(int*));
    }
}

pso::pso(const pso& orig) {
}
/*libero memoria asignada a g_best*/
pso::~pso() {

    for(int cN = 0; cN <= nurses ; cN++ ){
        free(g_best[cN]);
    }

    free(g_best);



}

void pso::setW(float value)
{
    w = value;
}

void pso::setR1()
{
    r1 = (float) rand()/RAND_MAX;
}

void pso::setR2()
{
    r2 = (float) rand()/RAND_MAX;
}

void pso::setC1(float value)
{
    c1 = value;
}

void pso::setC2(float value)
{
    c2 = value;
}

void pso::run(int SIZE, int ITERATIONS)
{
    clock_t start = clock();
    int iter;

    /*       INICIALIZO PSO            */
    particle *swarm[SIZE];
    int s, t, s_best, cN, cDS;
    s_best = 0; //indice de mi mejor global lo inicializo en 0
    /*inicializo el enjambre de particulas*/
    printf("PSO PARAMETERS\n\tc1: %f   c2: %f   w: %f   size: %d   iterations: %d\n\n",c1,c2,w,SIZE,ITERATIONS);
    for(s=0; s < SIZE; s++){
        swarm[s] = new particle(nurses,days,shifts);
        swarm[s]->setPsoParameters(c1,c2,r1,r2,w);
        swarm[s]->setCoverageMatrix(coverage);
        swarm[s]->setPreferenceMatrix(preference);
        swarm[s]->init();
    }
    g_fitness = swarm[s_best]->getFitness();
   /*copio la informacion del mejor global a g_best */
    for(cN = 0; cN <= nurses; cN++){
        for(cDS = 0; cDS < days*shifts ; cDS++){
            g_best[cN][cDS] = swarm[s_best]->getPositionMatrix()[cN][cDS];
        }
    }
    
    iter = 0;
    /*itero ITERATIONS veces*/
    for(t = 0 ; t < ITERATIONS ; t++){
        /*aplico control de parametros a la inercia y a los factores r1 y r2*/
        printf("Iteracion %d\tGBest = %d\n",t,g_fitness);
        iter++;
        if(iter == ITERATIONS*0.05){
            float random,random2,random3;
            iter = 0;

            if(c1 > 0.5){
                random = (float) rand()/RAND_MAX;
                c1 = c1 - random*c1;
            }else{
                random = (float) rand()/RAND_MAX;
                c1 = c1 + random*c1;
            }
            if(c2 > 0.5){
                random2 = (float) rand()/RAND_MAX;
                c2 = c2 - random2*c2;
            }else{
                random2 = (float) rand()/RAND_MAX;
                c2 = c2 + random2*c2;
            }
            if(w > 0.5){
                random3 = (float) rand()/RAND_MAX;
                w = w - random3*w;
            }else{
                random3 = (float) rand()/RAND_MAX;
                w = w + random3*w;
            }
            printf("CAMBIE a c1 = %f ; c2 = %f ; w = %f\n",c1,c2,w);
            
        }
        /*actualizo velocidad y posicion de cada particula del enjambre */
        for(s=0; s < SIZE; s++){
            swarm[s]->setPsoParameters(c1,c2,r1,r2,w);
            swarm[s]->update(g_best);
        }
        /*obtengo el indice del mejor global y lo guardo en s_best*/
        for(s=0; s < SIZE; s++){
            if(swarm[s]->getFitness() < g_fitness){
                iter = 0;
                s_best = s;
                g_fitness = swarm[s]->getFitness();
                /*copio la informacion del mejor global a g_best */
                for(cN = 0; cN <= nurses; cN++){
                    for(cDS = 0; cDS < days*shifts ; cDS++){
                        g_best[cN][cDS] = swarm[s_best]->getPositionMatrix()[cN][cDS];
                    }
                }
            }
        }
        
    }
    /*imprimo resultados*/
    //printf("\t\t\t\tPREFERENCE\n\n");
    //swarm[0]->printPreferenceMatrix();
    printf("\t\t\t\tBEST SOLUTION\n\n");
    printf("\tFitness: %d\n\n",g_fitness);
    printf("\tSolution:");
    printGBest();
    /*escribo en un archivo el fitness*/
    ofstream data("output.res");
    data << g_fitness << endl;
    data.close();
    
    /*libero memoria reservada al enjambre*/
    for(s=0; s < SIZE; s++){
        delete swarm[s];
   }

   printf ( "Tiempo de procesamiento: %f [segs]\n", ((double)clock() - start)/CLOCKS_PER_SEC);
}

void pso::setPreferenceMatrix(int **preference)
{
    this->preference = preference;
}

void pso::setCoverageMatrix(int **coverage)
{
    this->coverage = coverage;
}

void pso::printGBest(){

    int count = 0;
    for(int cN = 0; cN <= nurses; cN++){
        printf("\n\t");
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count ++;
            printf("%d  ", g_best[cN][cDS]);
            if(count == shifts){
                printf("\t");
                count = 0;
            }
        }
    }
    printf("\n");
}
