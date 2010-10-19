/* 
 * File:   particle.cpp
 * Author: diepeerk
 * 
 * Created on 4 de octubre de 2010, 11:47 AM
 */

#include "particle.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cmath>

particle::particle(int nurses, int days, int shifts)
{
    int cN;
    this->nurses = nurses;
    this->days = days;
    this->shifts = shifts;

    /*asigno memoria dinamicamente a matriz de posicion para obtener position[nurses+1][days*shifts] */
    position = (int**)malloc((nurses+1)*sizeof(int*));
    for(cN=0; cN<(nurses+1);cN++){
        position[cN] = (int*)malloc(days*shifts*sizeof(int*));
    }

    /*asigno memoria dinamicamente a matriz l_best (optimo local) para obtener l_best[nurses+1][days*shifts] */
    l_best = (int**)malloc((nurses+1)*sizeof(int*));
    for(cN=0; cN<(nurses+1);cN++){
        l_best[cN] = (int*)malloc(days*shifts*sizeof(int*));
    }
}

particle::particle(const particle& orig) {
}

particle::~particle()
{
    /*libero memoria de matrices position y l_best */
     for(int cN=0; cN<(nurses+1);cN++){
        free(position[cN]);
        free(l_best[cN]);
     }

     free(position);
     free(l_best);
}

int **particle::getPosition()
{
    return position;
}

/* Inicializo posicion y velocidad*/
void particle::init(){
    int i, cDS, cN,num;
    float probability;

    i = 1;
    /*inicializo primera columna de la matriz posicion*/
    for(cDS = 0; cDS < days*shifts; cDS++){
        position[0][cDS] = i ;
        i++;
        if(i == shifts+1){
            i = 1;
        }
    }
    /*genero posicion aleatoria*/
    for(cN = 1; cN <= nurses ; cN++){
        for(cDS = 0; cDS < days*shifts; cDS++){
            probability = (float) rand()/RAND_MAX;
            if(probability < 0.5){
                num = 0;
            }else{
                num = 1;
            }
            position[cN][cDS] = num;
        }
    }

    i = 1;

    /*copio position a l_best (el mejor local al principio es mi primera posicion)*/
    for(cN = 0; cN <= nurses ; cN++){
        for(cDS = 0; cDS < days*shifts; cDS++){
            l_best[cN][cDS] = position[cN][cDS];

        }
    }
    l_fitness = getFitness(); //el fitness inicial es su mejor local 

    /*inicializo v_0 (probabilidad de cambiar a 0) y v_1 (probabilidad de cambiar a 1)*/
    v_0 = (float) rand()/RAND_MAX;
    v_1 = (float) rand()/RAND_MAX;

}
/*cálculo del valor fitness*/
void particle::calculateFitness()
{
    /*sumatoria de todas las restricciones */
    fitness = cobertureConstraint(coverage) + daysOffTogether()  + twoDaysOffPerWeek() + twoShiftsPerDay();

}

int particle::getFitness()
{
    calculateFitness();
    return fitness;
}
/*restriccion de cobertura (dura)*/
int particle::cobertureConstraint(int **coverage)
{
    int PESO = 11111;
    int fitness, cN, cD, cS, count,cDS;
    fitness = 0;
    count = 0;
    
    for(cD = 0; cD < days; cD++){
        for(cS = 0; cS < shifts ; cS++){
            /*Sumo todas las enfermeras de mi solucion y comparo con la matriz de cobertura*/
            for(cN = 1; cN <=nurses ; cN++){ //parto de 1 porq la primera fila imprimo numeros ver mejor la matriz
                count =  count + position[cN][shifts*cD+cS]; //shifts*cD+cS por la forma de la matriz (revisar matriz)
            }
            /*comparo si las enfermeras de mis solucion son menos que las que necesito => sumo PESO a fitness */
            if(count < coverage[cD][cS]){
                    fitness = PESO + fitness;
            }
            count = 0;
            
        }
    }

    return fitness;

}
/*restriccion de dos dias libres juntos (blanda)*/
int particle::daysOffTogether()
{
    int PESO = 500;
    int fitness = 0;
    int sum = 0;
    int count = 0;
    int cN, cDS;
    bool hasFirst = false;
    for(cN = 1; cN<= nurses ; cN++){
        sum = 0;
        for(cDS=0; cDS < days*shifts; cDS++){
            count ++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //si ya termino un dia
                count = 0;
                if(!hasFirst && sum == 0){ //es un dia libre
                    hasFirst = true;
                }
                if(hasFirst && sum != 0){ //quiere decir que el siguiente dia no es libre
                    hasFirst = false;
                    fitness = fitness + PESO;
                }
                sum = 0; //como ya termino un dia hago sum = 0;
            }
        }
    }
    return fitness;

}

int particle::twoDaysOffPerWeek(){
    int fitness = 0;
    int PESO = 8888;
    int count = 0;
    int sum = 0;
    int daysOff = 0;
    for(int cN = 1; cN <=nurses ; cN++){
        if(daysOff< 2){
            fitness = fitness + PESO;
        }
        daysOff = 0;
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //quiere decir que ya termino un dia
                count = 0;
                if(sum == 0){
                    daysOff++;

                }
                sum = 0;
            }
        }
    }

    return fitness;
}

int particle::twoShiftsPerDay(){
    int fitness = 0;
    int PESO = 50000;
    int count = 0;
    int sum = 0;

    for(int cN = 1; cN <=nurses ; cN++){
        sum = 0;
        count = 0;
       for(int cDS = 0; cDS < days*shifts ; cDS++){
            count++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //si termino un dia
                if(sum > 2){ //si trabaja mas de un turno diario
                    fitness = fitness + PESO;
                }
                sum = 0;
                count = 0;
            }
        }

    }

    return fitness;
}


/*restriccion de preferencia de las enfermeras (blanda)*/
int particle::preferenceConstraint(int **preference)
{
    int fitness = 0;
    
    return fitness;
}


void particle::fixFitness()
{
    int b_index, fitness;
    int count = 0;
    int sum = 0;
    for(int cDS = 0 ; cDS < nurses*shifts ; cDS++){
        for(int cN = 1; cN < nurses; cN++){

            sum = sum + position[cN][cDS];
            /*calculo el fitness de una enfermera*/
            if(count == shifts){
                count = 0;
                sum = 0;

            }

        }
    }



}
/*actualizacion de posicion y velocidad*/
void particle::update(int **g_best)
{
    int cN, cDS, cN2, cDS2;
    float random;

    /*actualizo r1 y r2*/
    r1 = (float) rand()/RAND_MAX;
    r2 = (float) rand()/RAND_MAX;
    
    for(cN = 1; cN <= nurses; cN++){
        for(cDS = 0; cDS < days*shifts; cDS++){
            if(l_best[cN][cDS] == 1){
                d1_1 = c1*r1;
                d1_0 = -c1*r1;
            }else{
                d1_1 = -c1*r1;
                d1_0 = c1*r1;
            }
            if(g_best[cN][cDS] == 1){
                d2_1 = c2*r2;
                d2_0 = -c2*r2;
            }else{
                d2_1 = -c2*r2;
                d2_0 = c2*r2;
            }
            v_1 = w*v_1 + d1_1 + d2_1;
            v_0 = w*v_0 + d1_0 + d2_0;
            
            /*normalizo las velocidades para [0,1]*/
            v_1 = 1/(1+exp(-v_1));
            v_0 = 1/(1+exp(-v_0));

            random = (float) rand()/RAND_MAX;
            if(position[cN][cDS] == 1){ //si el bit es 1
                if(random < v_0){
                    position[cN][cDS] = 0;
                }
            }else{ //si el bit es 0
                if(random < v_1){
                    position[cN][cDS] = 1;
                }
            }
            /*comparo si la que obtuve es mejor que la anterior*/
            if(getFitness() < l_fitness){
                l_fitness = getFitness(); //guardo el fitness del mejor local
                /*copio la nueva solcion en mi mejor local (l_best)*/
                for(cN2 = 0; cN2 <= nurses; cN2++){
                    for(cDS2 = 0; cDS2 < days*shifts ; cDS2++){
                        l_best[cN2][cDS2] = position[cN2][cDS2];
                    }
                }
            }
        }
    }
}

void particle::printPosition(int nurses, int days, int shifts)
{
    int cN,cDS;
    int count = 0;
    for(cN = 0; cN <= nurses ; cN++){
        printf("\n\t");
        for(cDS = 0; cDS < days*shifts; cDS++){
            count++;
            if(count == shifts){
                printf("\t");
                count = 0;
            }
            printf("%d  ",position[cN][cDS]);

        }
    }
}

int **particle::getPositionMatrix()
{
    return position;
}

void particle::setPsoParameters(float c1, float c2, float r1, float r2, float w)
{
    this->c1 = c1;
    this->c2 = c2;
    this->r1 = r1;
    this->r2 = r2;
    this->w = w;
}

void particle::setCoverageMatrix(int** matrix)
{
    coverage = matrix;

}

void particle::setPreferenceMatrix(int** matrix)
{
    preference = matrix;

}


/*              METODOS DE PRUEBA               */

void particle::setPositionMatrix(int **matrix)
{
    int i,j;
    for(i=0; i<= nurses; i++){
        for(j=0; j < days*shifts; j++){
            position[i][j] = matrix[i][j];

        }
    }

}

void particle::printCoverageMatrix()
{
    int cD, cS;
    for(cD=0;cD<days;cD++){
        printf("\n");
        for(cS=0;cS<shifts;cS++){
            printf("%d\t",coverage[cD][cS]);
        }
    }
}

void particle::printPreferenceMatrix()
{
    int cN,cDS;
    for(cN=0;cN<nurses;cN++){
        printf("\n");
        for(cDS=0;cDS<(shifts*days);cDS++){
            printf("%d\t",preference[cN][cDS]);
        }
    }
}