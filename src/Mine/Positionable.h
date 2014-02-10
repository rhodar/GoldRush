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
#include <sstream>


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
        stringstream name;
        name << _y << "/"<<_x;
        _display=name.str();
    }

    virtual void setPosition(int x, int y) {
        _x = x;
        _y = y;
        stringstream name;
        name << "[" <<_y << "/"<<_x << "]";
        _display=name.str();
    }

    
    virtual int getPositionX() {
        return _x;
    }

    virtual int getPositionY() {
        return _y;
    }
    virtual string display()
    {
        return _display;
    }
private:
    int _x;
    int _y;
    string _display;

};

#endif	/* POSITIONABLE_H */

