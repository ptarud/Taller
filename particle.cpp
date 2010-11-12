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
#define MAX_ENS 3
#define MIN_DAYS_OFF 2
#define DS 0
#define EDS 1
#define ENS 2
#define PS 3

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
    
    /*asigno memoria a arreglo que conoce los dias off de cada enfermera*/
    daysOffArray = (int*)malloc(nurses*sizeof(int*));

    /*asigno memoria a arreglo que conoce los turnos usados por cada dia en cada enfermera*/
    shiftsPerDayArray = (int**)malloc(nurses*sizeof(int*));
    for(cN = 0; cN < nurses ; cN++){
        shiftsPerDayArray[cN] = (int*)malloc(days*sizeof(int*));
    }

    shift_hours[DS] = 8;
    shift_hours[EDS] = 12;
    shift_hours[ENS] = 12;
    shift_hours[PS] = 8;
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
     /*libero memoria de matriz shiftsPerDayArray*/
     for(int cN = 0; cN < nurses ; cN++){
        free(shiftsPerDayArray[cN]);
     }

     free(position);
     free(l_best);
     free(daysOffArray);
     free(shiftsPerDayArray);
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
    calculateFitness();
    l_fitness = getFitness(); //el fitness inicial es su mejor local

    /*inicializo v_0 (probabilidad de cambiar a 0) y v_1 (probabilidad de cambiar a 1)*/
    v_0 = (float) rand()/RAND_MAX;
    v_1 = (float) rand()/RAND_MAX;

}
/*cálculo del valor fitness*/
void particle::calculateFitness()
{
    cobertureConstraint(coverage);
    minDaysOffPerWeek();
    
    /*sumatoria de restricciones blandas y duras si se viola una dura es arreglada por movimiento en la sgte iteracion */
    fitness =  daysOffTogether(10) + preferenceConstraint(preference) + maxNightShifts(45)+ maxShiftsPerDay(160);

}

int particle::getFitness()
{
    return fitness;
}
/*restriccion de cobertura (dura)*/
void particle::cobertureConstraint(int **coverage)
{
    int cN, cD, cS, count;
    this->fitness = 0;
    count = 0;
    
    for(cD = 0; cD < days; cD++){
        for(cS = 0; cS < shifts ; cS++){
            count = 0;
            /*Sumo todas las enfermeras de mi solucion y comparo con la matriz de cobertura*/
            for(cN = 1; cN <=nurses ; cN++){ //parto de 1 porq la primera fila imprimo numeros ver mejor la matriz
                count =  count + position[cN][shifts*cD+cS]; //shifts*cD+cS por la forma de la matriz (revisar matriz)
            }
            /*comparo si las enfermeras de mis solucion son mas que las que necesito => mejoro solucion inteligentemente*/
            if(count > coverage[cD][cS]){
                /*mejoro solucion*/
                int borrar = count - coverage[cD][cS];
                improveResult1(borrar,cD,cS);
                this->fitness = this->fitness + 300;

            }
            if(count < coverage[cD][cS]){
                int times = coverage[cD][cS] - count;
                this->fitness = this->fitness + 500;
                makeFeasible(times,cD,cS);
            }
        }
    }
}
void particle::makeFeasible(int times, int day, int shift){

    int cN;
    while(times > 0){
        cN = (rand() % nurses) +1; //busco enfermera aleatoria
        if(position[cN][shifts*day + shift] == 0){
            position[cN][shifts*day + shift] = 1;
            times--;
        }
    }
}

void particle::improveResult1(int borrar, int D, int S){
    
    int cN;

    while(borrar > 0){
        cN = (rand() % nurses) + 1; //busco enfermera aleatoria
        if(position[cN][shifts*D+S] == 1){
            borrar--;
            position[cN][shifts*D+S] = 0;
        }
    }
}


/*restriccion de dos dias libres juntos (blanda)*/
int particle::daysOffTogether(int PESO)
{
    int fitness = 0;
    int sum = 0;
    int count = 0;
    int day = 0;
    int cN, cDS;
    bool hasFirst = false;
    bool cumple = false;
    int times = 0;
    int index = 0;
    for(cN = 1; cN<= nurses ; cN++){
        sum = 0;
        hasFirst = false; cumple = false;
        times = 0; day = 0;
        index = 0; count = 0;
        for(cDS=0; cDS < days*shifts; cDS++){
            count ++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //si ya termino un dia
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
                day++;
                count = 0;
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
    int count,day;
    int daysOff,sum;
    for(int cN = 1; cN <=nurses ; cN++){
        daysOff = 0; sum = 0; count = 0; day = 0;
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //quiere decir que ya termino un dia
                shiftsPerDayArray[cN-1][day] = sum;
                day++;
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
    int src,dest;
    for(src = 0; src < nurses ; src++){
        day = 0; sum = 0; count = 0;
        /*mientras la enfermera no cumple la restriccion le cambio un dia con una enfermera que si cumpla y pueda*/
        while(daysOffArray[src] < MIN_DAYS_OFF){
            /*busco una enfermera aleatoriamente que tenga mas dias libres y le hago un swap de ese dia*/
            dest = (rand() % nurses) + 1;
            //for(int dest = 0; dest < nurses ; dest++){
                if(daysOffArray[dest] > MIN_DAYS_OFF){
                    swapDay(src,dest);
              //  }
            }
        }
    }
}

/*busco enfermera */
void particle::swapDay(int nurseSrc, int nurseDest){
    /*busco dia que tenga off para cambiar*/
    for(int d = 0; d < days ; d++){
        if(shiftsPerDayArray[nurseDest][d] == 0){
            shiftsPerDayArray[nurseDest][d] = shiftsPerDayArray[nurseDest][d] + 1;
            shiftsPerDayArray[nurseSrc][d] = shiftsPerDayArray[nurseSrc][d] - 1;
            /*cambio un turno del dia*/
            for(int s = 0; s < shifts ; s++){
                int aux;
                aux = position[nurseSrc+1][d*shifts + s];
                position[nurseSrc+1][d*shifts + s] = position[nurseDest+1][d*shifts + s];
                position[nurseDest+1][d*shifts + s] = aux;
                
            }
        }
    }
    daysOffArray[nurseSrc] = daysOffArray[nurseSrc] + 1;
    daysOffArray[nurseDest] = daysOffArray[nurseDest] - 1;


}
/*restriccion de maximos turnos por dia definido arriba*/
int particle::maxShiftsPerDay(int PESO){
   int day, sum, count;
   int fitness = 0;

   for(int cN = 1; cN <=nurses ; cN++){
        sum = 0; count = 0; day = 0;
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count++;
            sum = position[cN][cDS] + sum;
            if(count == shifts){ //si termino un dia
                if(sum > MAX_SHIFTS){ //si trabaja mas de los turnos permitidos diarios
                    fitness = fitness + PESO;//printPosition(nurses,days,shifts);
                    fixDay(cN, day, (sum - MAX_SHIFTS));
                }
                sum = 0;
                count = 0;
                day++;
            }
        }

    }
    return fitness;
}

/*Reparo día que tenga mas de los turnos permitidos diarios*/
void particle::fixDay(int nurse, int day, int borrar){
    int d,sum,count,cN;
    bool cambie;
    /*mientras no he cambiado todos los turnos*/
    while(borrar > 0){
        /*busco alguna enfermera aleatoriamente que tenga mas de los minimos dias off*/
        cN = (rand() % nurses) + 1;
        if(daysOffArray[cN - 1] > MIN_DAYS_OFF){
            for(d = 0; d < days ; d++){
                cambie = false;
                if(shiftsPerDayArray[cN-1][d] == 0 && d == day){
                    for(int s = 0; s < shifts; s++){
                        if(position[nurse][d*shifts + s] == 1 && !cambie){
                            position[nurse][d*shifts + s] = 0;
                            position[cN][d*shifts + s] = 1;
                            daysOffArray[cN-1] = daysOffArray[cN-1] - 1;
                            cambie = true;
                            borrar--;
                        }
                    }
                }
            }
        }
    }
}

/*restriccion de maximos turnos noches por semana*/
int particle::maxNightShifts(int PESO){
    
    int sum;
    int fitness = 0;
    int day;

    for(int cN = 1; cN <=nurses ; cN++){
        sum = 0; day = 0;
        for(int cD = 0; cD < days; cD++){
            if(position[cN][cD*days + ENS] == 1){
                sum++;
            }
        }
        if(sum > MAX_ENS){
            fitness = fitness + PESO;
        }
        
    }

    return fitness;
}

/*restriccion de preferencia de las enfermeras (blanda)*/
int particle::preferenceConstraint(int **preference)
{
    int count = 0;
    int fitness = 0;
    int day = 0;
    for(int cN = 1; cN <= nurses ; cN++){
        count = 0; day = 0;
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count++;
            if(count == shifts){
                day++;
                count = 0;
            }
            if(position[cN][cDS] == 1){
                if(preference[cN-1][cDS] == 3){
                    fitness = fitness + 5;
                }else if(preference[cN-1][cDS] == 2){
                    fitness = fitness + 10;
                }else if(preference[cN-1][cDS] == 1){
                    fitness = fitness + 15;
                }
            }
        }
    }
    
    return fitness;
}
/*mejoro fitness de preferencias de las enfermeras trato de asignarle el turno con mayor preferencia en la siguiente iteracion*/
void particle::repairFitness(int day, int nurseSrc, int shift)
{
    int bestNurse = 1; //indice de la enfermera con el mejor bestPreference para hacer el swap
    int bestPreference = 0; //variable que conoce la mejor preferencia encontrada hasta el momento

    /*busco enfermera destino para cambiarle el dia-turno*/
    for(int cN = 1; cN <= nurses; cN++){
        if(preference[cN-1][day*shifts + shift] == 4 && shiftsPerDayArray[cN-1][day] < MAX_SHIFTS){
            /*hago el cambio */
            int aux = position[cN][day*shifts + shift];
            position[cN][day*shifts + shift] = position[nurseSrc][day*shifts + shift];
            position[nurseSrc][day*shifts + shift] = aux;
            return;
        }else if(preference[cN-1][day*shifts + shift] == 3 && shiftsPerDayArray[cN-1][day] < MAX_SHIFTS){
            if(bestPreference < 3){
                bestPreference = 3;
                bestNurse = cN;
            }
        }else if(preference[cN-1][day*shifts + shift] == 2 && shiftsPerDayArray[cN-1][day] < MAX_SHIFTS){
            if(bestPreference < 2){
                bestPreference = 2;
                bestNurse = cN;
            }
        }else if(preference[cN-1][day*shifts + shift] == 1 && shiftsPerDayArray[cN-1][day] < MAX_SHIFTS){
            if(bestPreference < 1){
                bestPreference = 1;
                bestNurse = cN;
            }
        }
    }
    /*cambiar a con el mejor fitness y tengo el index tb de la enfermera*/
    int aux = position[bestNurse][day*shifts + shift];
    position[bestNurse][day*shifts + shift] = position[nurseSrc][day*shifts + shift];
    position[nurseSrc][day*shifts + shift] = aux;
    return;
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
            calculateFitness();
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
            printf("%d  ",coverage[cD][cS]);
        }
    }
    printf("\n");
}

void particle::printPreferenceMatrix()
{
    int count = 0;
    for(int cN = 0; cN < nurses; cN++){
        printf("\n\t");
        for(int cDS = 0; cDS < days*shifts ; cDS++){
            count ++;
            printf("%d  ", preference[cN][cDS]);
            if(count == shifts){
                printf("\t");
                count = 0;
            }
        }
    }
    printf("\n");
}
