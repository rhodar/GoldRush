/* 
 * File:   Position.cpp
 * Author: bakira
 * 
 * Created on 19 janvier 2014, 12:55
 */

#include "Position.h"

Position::Position() :
_x(0),
_y(0)
{
}

Position::~Position()
{
}

int Position::getX()
{
    return _x;
}

int Position::getY()
{
    return _y;
}

void Position::setPosition(int x, int y)
{
    _x = x;
    _y = y;
}

