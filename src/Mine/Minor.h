/* 
 * File:   Minor.h
 * Author: bakira
 *
 * Created on 20 janvier 2014, 20:00
 */

#ifndef MINOR_H
#define	MINOR_H

#include "Positionable.h"
#include "Trolley.h"


class Minor : public Positionable
{
public:
    Minor();
    virtual ~Minor();
    
    Trolley _myTrolley;

};

#endif	/* MINOR_H */

