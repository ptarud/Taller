/* 
 * File:   particle.h
 * Author: diepeerk
 *
 * Created on 4 de octubre de 2010, 11:47 AM
 */

#ifndef PARTICLE_H
#define	PARTICLE_H

class particle {
public:
    particle(int,int,int);
    particle(const particle& orig);
    virtual ~particle();
    int **getPosition();
    void init();
    void calculateFitness(/*int**,int***/);
    int getFitness();
    int cobertureConstraint(int**); //restriccion de cobertura
    int daysOffTogether(); //restriccion de dias libres juntos
    int preferenceConstraint(int**); //restriccion de preferencias de las enfermeras
    void update(int **g_best);
    void printPosition(int nurses, int days, int shifts);
    int **getPositionMatrix();
    void setPsoParameters(float c1, float c2, float r1, float r2, float w);
    void setCoverageMatrix(int**);
    void setPreferenceMatrix(int**);
    int twoDaysOffPerWeek();
    int twoShiftsPerDay();
    void setPositionMatrix(int**); //metodo de prueba
    void printCoverageMatrix();
    void printPreferenceMatrix();
    void fixFitness();
private:
    int **position, **l_best;
    int days, shifts, nurses, fitness, l_fitness;
    float v_0, v_1, d1_0, d1_1, d2_0, d2_1;
    float w, c1, r1, c2, r2; //pso parameters
    int **coverage, **preference;
};

#endif	/* PARTICLE_H */

