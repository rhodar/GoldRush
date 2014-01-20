/* 
 * File:   Positionable.h
 * Author: bakira
 *
 * Created on 20 janvier 2014, 20:26
 */
#ifndef POSITIONABLE_H
#define	POSITIONABLE_H
#include<string>
#include <cstdlib>
using namespace std;

class Positionable {
public:

    Positionable() : _x(0), _y(0) {

    }

    virtual~Positionable() {

    }

    virtual void setPosition(string x, string y) {
        _x = atoi(x.c_str());
        _y = atoi(y.c_str());
    }

    virtual int getPositionX() {
        return _x;
    }

    virtual int getPositionY() {
        return _y;
    }
private:
    int _x;
    int _y;

};

#endif	/* POSITIONABLE_H */

