/* 
 * File:   pso.h
 * Author: diepeerk
 *
 * Created on 4 de octubre de 2010, 11:48 AM
 */
//#include "particle.h"

#ifndef PSO_H
#define	PSO_H

class pso {
public:
    pso(int nurses, int days, int shifts);
    pso(const pso& orig);
    virtual ~pso();
    void setW(float); //setear inercia
    void setR1(); //set random (por ahora)
    void setR2(); //set random (por ahora)
    void setC1(float); //setear factor cognitivo
    void setC2(float); //setear factor social
    void setSize(int);
    void setPreferenceMatrix(int**);
    void setCoverageMatrix(int**);
    void run(int,int);
    void printGBest();
private:
    float w, c1, c2 ,r1 ,r2;
    int nurses, days, shifts, g_fitness;
    int **g_best, **coverage, **preference;

};

#endif	/* PSO_H */

