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
#define MAX_SHIFTS 1
#define MIN_DAYS_OFF 2

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
    daysOffArray = (int*)malloc(nurses*sizeof(int*));

    shift_hours[0] = 8;
    shift_hours[1] = 12;
    shift_hours[2] = 12;
    shift_hours[3] = 8;
}

particle::particle(const particle& orig) {
}

particle::~particle()
{
    /*libero memoria de matrices daysOffArray, position y l_best */
     for(int cN=0; cN<(nurses+1);cN++){
        free(position[cN]);
        free(l_best[cN]);
     }

     free(position);
     free(l_best);
     free(daysOffArray);
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
    /*restricciones duras son arregladas por movimientos*/
    cobertureConstraint(coverage);
    minDaysOffPerWeek();
    maxShiftsPerDay();
    /*sumatoria de todas las restricciones blandas*/
    fitness =  daysOffTogether(5);

}

int particle::getFitness()
{
    calculateFitness();
    return fitness;
}
/*restriccion de cobertura (dura)*/
void particle::cobertureConstraint(int **coverage)
{
    int fitness, cN, cD, cS, count;
    fitness = 0;
    count = 0;
    
    for(cD = 0; cD < days; cD++){
        for(cS = 0; cS < shifts ; cS++){
            /*Sumo todas las enfermeras de mi solucion y comparo con la matriz de cobertura*/
            for(cN = 1; cN <=nurses ; cN++){ //parto de 1 porq la primera fila imprimo numeros ver mejor la matriz
                count =  count + position[cN][shifts*cD+cS]; //shifts*cD+cS por la forma de la matriz (revisar matriz)
            }
            /*comparo si las enfermeras de mis solucion son mas que las que necesito => mejoro solucion inteligentemente*/
            if(count > coverage[cD][cS]){
                /*mejoro solucion*/
                int borrar = count - coverage[cD][cS];
                improveResult1(borrar,cD,cS);
            }
            count = 0;
            
        }
    }
}
void particle::improveResult1(int borrar, int D, int S){
    
    int sum;
    //int borrados = 0;
    for(int cN = nurses; cN >=1 ; cN--){
        sum = 0;
        for(int cS = 0; cS < shifts; cS++){
            sum = sum + position[cN][shifts*D+cS];
        }
        if(borrar > 0){
            if(position[cN][shifts*D+S] == 1){
                borrar--;
                position[cN][shifts*D+S] = 0;
            }
        }
    }
}


/*restriccion de dos dias libres juntos (blanda)*/
int particle::daysOffTogether(int PESO)
{
    int fitness = 0;
    int sum = 0;
    int count = 0;
    int cN, cDS;
    bool hasFirst = false;
    bool cumple = false;
    int times = 0;
    int index = 0;
    for(cN = 1; cN<= nurses ; cN++){
        sum = 0;
        hasFirst = false; cumple = false;
        times = 0;
        index = 0; count = 0;
        for(cDS=0; cDS < days*shifts; cDS++){
            count ++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //si ya termino un dia
                count = 0;
                if(!hasFirst && sum == 0){
                    hasFirst = true;
                    index = cDS;
                }
                if(hasFirst && cDS == (index+shifts) && sum == 0){
                    hasFirst = false;
                    index = 0;
                    cumple = true;
                }
                if(hasFirst && sum != 0){
                    fitness = PESO + fitness;
                    hasFirst = false;
                    times++;
                }
                
                sum = 0; //como ya termino un dia hago sum = 0;
            }
        }
        if(cumple && times > 0){ //quiere decir q ya cumplio la restriccion una vez entonces resto penalizacion
            fitness = fitness - times*PESO;
        }
    }
    return fitness;

}

void particle::minDaysOffPerWeek(){
    int count;
    int daysOff,sum;
    for(int cN = 1; cN <=nurses ; cN++){
        daysOff = 0; sum = 0; count = 0;
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
        daysOffArray[cN-1] = daysOff; //guardo cuantos dias libres tiene la enfermera cN
    }
    improveResult2();
}

/*movimiento que arregla la violación de la restricción dura de 2 dias a la semana libres*/
void particle::improveResult2(){ /*swap turno de un dia con otra enfermera q cumpla restriccion*/

    int sum, day;
    int count = 0;
    int src;
    for(src = 0; src < nurses ; src++){
        day = 0; sum = 0; count = 0;
        /*mientras la enfermera no cumple la restriccion le cambio un dia con una enfermera que si cumpla y pueda*/
        while(daysOffArray[src] < MIN_DAYS_OFF){
            daysOffArray[src] = daysOffArray[src] + 1;
            /*busco una enfermera que tenga mas dias libres y le hago un swap de ese dia*/
            for(int dest = 0; dest < nurses ; dest++){
                while(daysOffArray[dest] > MIN_DAYS_OFF){
                    daysOffArray[dest] = daysOffArray[dest] - 1;
                    swapDay(src,dest);
                }
            }
        }
    }
}

void particle::swapDay(int nurseSrc, int nurseDest){
    
   /*busco un dia y hago el swap de ese dia*/
    int count = 0;
    int sum = 0;
    int day = 0;
    for(int cDS = 0; cDS < days*shifts ; cDS++){
        count++;
        sum = sum + position[nurseDest+1][cDS];
        if(count == shifts){
            count = 0;
            if(sum == 0){
                for(int j = 0; j < shifts; j++){
                    position[nurseDest+1][day*shifts + j] = position[nurseSrc+1][day*shifts + j];
                    position[nurseSrc+1][day*shifts + j] = 0;
                }
                return;
            }
            day++;
            sum = 0;
        }

    }

}

void particle::maxShiftsPerDay(){
   int day, sum, count;

   for(int cN = 1; cN <=nurses ; cN++){
        sum = 0; count = 0; day = 0;
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //si termino un dia
                if(sum > MAX_SHIFTS){ //si trabaja mas de los turnos permitidos diarios
                    fixDay(cN, day, (sum - MAX_SHIFTS));
                }
                sum = 0;
                count = 0;
                day++;
            }
        }

    }
}

/*Reparo día que tenga mas de los turnos permitidos diarios*/
void particle::fixDay(int nurse, int day, int borrar){
    int d,sum,count;
    bool cambie;
    /*mientras no he cambiado todos los turnos*/
    while(borrar > 0){
        borrar--;
        /*busco alguna enfermera que tenga mas de los minimos dias off dias offs*/
        for(int cN = nurse+1; cN <= nurses; cN++){
            d = 0; sum = 0; count = 0; cambie = false;
            if(daysOffArray[cN - 1] > MIN_DAYS_OFF){
                for(int cDS = 0; cDS < days*shifts ; cDS++){
                    count++;
                    sum = position[cN][cDS] + sum;
                    if(count == shifts){
                        /*si es el mismo dia que quiero cambiar y es libre */
                        if(day == d && sum == 0){
                            /*lo cambio*/
                            for(int s = 0; s < shifts; s++){
                                if(position[nurse][d*shifts + s] == 1 && !cambie){
                                    position[nurse][d*shifts + s] = 0;
                                    position[cN][d*shifts + s] = 1;
                                    daysOffArray[cN-1] = daysOffArray[cN-1] - 1;
                                    cambie = true;
                                }
                            }
                        }
                        sum = 0;
                        count = 0;
                        d++;
                        cambie = false;
                    }
                }
            }
        }
    }
}

/*restriccion de preferencia de las enfermeras (blanda)*/
int particle::preferenceConstraint(int **preference)
{
    int fitness = 0;
    
    return fitness;
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
    int count = 0;
    for(int cN = 0; cN <= nurses; cN++){
        printf("\n\t");
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count ++;
            printf("%d  ", position[cN][cDS]);
            if(count == shifts){
                printf("\t");
                count = 0;
            }
        }
    }
    printf("\n");
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