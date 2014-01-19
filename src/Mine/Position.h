/* 
 * File:   Position.h
 * Author: bakira
 *
 * Created on 19 janvier 2014, 12:55
 */

#ifndef POSITION_H
#define	POSITION_H

class Position {
public:
    Position();
    virtual ~Position();
    int getX();
    int getY();
    void setPosition(int x, int y);
private:
    int _x;
    int _y;

};

#endif	/* POSITION_H */

