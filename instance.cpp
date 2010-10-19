/* 
 * File:   instance.cpp
 * Author: diepeerk
 * 
 * Created on 4 de octubre de 2010, 11:48 AM
 */

#include "instance.h"
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

instance::instance() {
}

instance::instance(const instance& orig) {
}

/*libero memoria de matrices coverage y preference*/
instance::~instance() {

    for(int cD=0; cD<days; cD++){
       free(coverage[cD]);
    }
    for(int cN=0; cN<nurses;cN++){
        free(preference[cN]);
    }

    free(coverage);
    free(preference);
}

void instance::read(char* file)
{
    ifstream data;
    data.open(file);
    data >> nurses >> days >> shifts;
    
    /*asigno memoria dinamicamente a matriz de cobertura para obtener coverage[days][shifts] */
    coverage = (int**)malloc(days*sizeof(int*));
    for (int cD=0; cD<days; cD++){
       coverage[cD] = (int*)malloc(shifts*sizeof(int*));
    }
    
    /*asigno memoria dinamicamente a matriz de preferencias para obtener preference[nurses][days*shifts] */
    preference = (int**)malloc(nurses*sizeof(int*));
    for(int cN=0; cN<nurses;cN++){
        preference[cN] = (int*)malloc(days*shifts*sizeof(int*));
    }

    /*Lleno las matrices como corresponde*/
    for(int i = 0; i < days; i++){
        for(int j = 0; j < shifts; j++)
            data >> coverage[i][j];
    }
    for(int i = 0; i < nurses; i++){
        for(int j = 0; j < days*shifts; j++)
            data >> preference[i][j];
    }

    data.close();

}

int instance::getNurses()
{
    return nurses;
}

int instance::getDays()
{
    return days;
}

int instance::getShifts()
{
    return shifts;
}

int **instance::getCoverageMatrix(){
    return coverage;

}

int **instance::getPreferenceMatrix(){
    return preference;
}

void instance::printCoverageMatrix()
{
    int cD, cS;
    for(cD=0;cD<days;cD++){
        printf("\n");
        for(cS=0;cS<shifts;cS++){
            printf("%d\t",coverage[cD][cS]);
        }
    }
}

void instance::printPreferenceMatrix()
{
    int cN,cDS;
    for(cN=0;cN<nurses;cN++){
        printf("\n");
        for(cDS=0;cDS<(shifts*days);cDS++){
            printf("%d\t",preference[cN][cDS]);
        }
    }
}