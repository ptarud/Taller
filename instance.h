/* 
 * File:   instance.h
 * Author: diepeerk
 *
 * Created on 4 de octubre de 2010, 11:48 AM
 */

#ifndef INSTANCE_H
#define	INSTANCE_H

class instance {
public:
    instance();
    instance(const instance& orig);
    virtual ~instance();
    void read(char*);
    int getNurses();
    int getDays();
    int getShifts();
    int **getCoverageMatrix();
    int **getPreferenceMatrix();
    void printCoverageMatrix();
    void printPreferenceMatrix();
private:
    int nurses, days, shifts;
    int **coverage, **preference;


};

#endif	/* INSTANCE_H */

