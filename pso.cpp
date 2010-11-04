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

    /*itero ITERATIONS veces*/
    for(t = 0 ; t < ITERATIONS ; t++){
        /*actualizo velocidad y posicion de cada particula del enjambre */
        for(s=0; s < SIZE; s++){
            swarm[s]->update(g_best);
        }
        /*obtengo el indice del mejor global y lo guardo en s_best*/
        for(s=0; s < SIZE; s++){
            if(swarm[s]->getFitness() < g_fitness){
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
    printf("PREFERENCE MATRIX\n");
    swarm[0]->printPreferenceMatrix();
    printf("\t\t\t\tBEST SOLUTION\n\n");
    printf("\tFitness: %d\n\n",g_fitness);
    printf("\tSolution:");
    printGBest();

    /*libero memoria reservada al enjambre*/
    for(s=0; s < SIZE; s++){
        delete swarm[s];
   }


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