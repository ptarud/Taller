/* 
 * File:   main.cpp
 * Author: diepeerk
 *
 * Created on 4 de octubre de 2010, 11:03 AM
 */
#include "instance.h"
#include "pso.h"
#include "particle.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ctime>

using namespace std;

int main(int argc, char** argv)
{
    srand(1); //inicializo funcion random
    if(argc == 7){
        instance instancia;
        instancia.read(argv[6]);
        pso algoritmo(instancia.getNurses(), instancia.getDays(), instancia.getShifts());
        algoritmo.setR1();
        algoritmo.setR2();
        algoritmo.setC1(atof(argv[2]));
        algoritmo.setC2(atof(argv[3]));
        algoritmo.setW(atof(argv[4]));
        algoritmo.setCoverageMatrix(instancia.getCoverageMatrix());
        algoritmo.setPreferenceMatrix(instancia.getPreferenceMatrix());
        algoritmo.run(atoi(argv[1]), atoi(argv[5]));
        return 0;
    }else{
        printf("Argumentos inválidos\n");
        printf("./taller tamaño cognitivo social inercia iteraciones archivo\n");
        return 0;
    }
}

