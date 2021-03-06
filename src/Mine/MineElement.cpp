/* 
 * File:   MineElement.cpp
 * Author: bakira
 * 
 * Created on 15 décembre 2013, 11:30
 */

#include <sstream>

#include "MineElement.h"
#include "Utils.h"
#include <algorithm>

MineElement::MineElement() :
isTheTrolley(false),
_gender(EMPTY_SIGNE),
_canWalkIn(false),
_nbDiamands(0),
_ennemiPresent(false),
_priorDirection(DIR_NONE)
{
    _myStatus.asked = false;
    _myStatus.scoreDir = DIR_NONE;
    _myStatus.isActive = false;
    _myStatus.lastTurnUpdate = 0;
    _myStatus.diamants = 0;
    _myStatus.myScore = -1;
}

MineElement::~MineElement()
{
}

bool MineElement::askNeighbours(direction whoAmIForHim, objectives obj)
{
    bool ret = false;
    _myStatus.obj = obj;
    direction incomingOrder = getOpositeDirection(whoAmIForHim);
    switch (obj)
    {
    case OBJ_TARGET:
        lookingForTarget(whoAmIForHim);
        break;
    case OBJ_STONE:
        lookingForStone(incomingOrder);
        break;
    case OBJ_TROLLEY:
        lookingForTrolley(incomingOrder);
        break;
    case OBJ_INACTIVE:
        lookingForInactive(incomingOrder);
        break;
    case OBJ_MUD:
        lookingForMudNeirStone(incomingOrder);
        break;
    case OBJ_DIAMANDS:
    default:
        lookingForDiamands(incomingOrder);
        break;
    }
    if (_myStatus.myScore >= 0)
    {
        ret = true;
    }

    return ret;
}

void MineElement::computeMyScore()
{
    // compute our proper score
    switch (_myStatus.obj)
    {
    case OBJ_TARGET:
        if (_ennemiPresent)
        {
            _myStatus.myScore = 0;
        }
        break;
    case OBJ_TROLLEY:
        if (isTheTrolley)
        {
            _myStatus.myScore = 0;
        }
        break;
    case OBJ_INACTIVE:
        if (!_myStatus.isActive)
        {
            _myStatus.myScore = 0;
        }
        break;
    case OBJ_DIAMANDS:
        if (_nbDiamands > 0)
        {
            _myStatus.myScore = 0;
            _myStatus.diamants = _nbDiamands;
        }
        break;
    case OBJ_STONE:
    case OBJ_MUD:
    default:
        // Hard coded !!! 
        break;
    }
}

bool MineElement::computeScore(Status neighbourStatus)
{
    bool keepIt = false;
    /**
     * If the score is better there take its score
     * If the score is equal but more diamants take its score
     * Else keep our score
     */
    int myScore = _myStatus.myScore;
    int itsScore = neighbourStatus.myScore + 1;

    int myDiam = _myStatus.diamants;
    int itsDiam = neighbourStatus.diamants;


    if (itsScore < myScore || myScore == -1)
    {
        keepIt = true;
    }
    else if (_myStatus.obj == OBJ_DIAMANDS)
    {
        if (itsScore == myScore)
        {
            if (itsDiam > myDiam)
            {
                keepIt = true;
            }
        }
    }

    if (keepIt)
    {

        _myStatus.myScore = itsScore;
        _myStatus.diamants = itsDiam;
    }
    return keepIt;
}

void MineElement::reset(bool isGlobalReset)
{
    _myStatus.scoreDir = DIR_NONE;
    _myStatus.myScore = -1;
    _myStatus.diamants = 0;
    _myStatus.asked = false;
    if (isGlobalReset)
    {
        _ennemiPresent = false;
    }
}

void MineElement::refresh(string gender, int turn, int diamand)
{
    _gender = gender;
    _myStatus.lastTurnUpdate = turn;
    _myStatus.isActive = true;
    _nbDiamands = diamand;
    if (gender == EMPTY_SIGNE || gender == MUD_SIGNE || isTheTrolley)
    {
        _canWalkIn = true;
    }
    else
    {
        _canWalkIn = false;
    }

}

void MineElement::registerNeighbour(MineElement* element, direction where)
{
    _neighbours[where] = element;
}

void MineElement::setEnnemies()
{
    log(display() + ">> Ennemi present");
    _ennemiPresent = true;
}

void MineElement::lookingForDiamands(direction incomingOrder)
{
    /**
     * Stop looking for an objective if :
     *  - we are inactiv
     *  - we have diamants here
     *  - we can't go throw this element
     *  - we are already asked
     */
    if (_nbDiamands > 0 || !_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        _myStatus.asked = true;
        // prior compute expected way
        if (_priorDirection != DIR_NONE)
        {
            log(display() + " >>> prior that way " + getDirection(_priorDirection));
            if (_neighbours.find(_priorDirection) != _neighbours.end())
            {
                _neighbours[_priorDirection]->askNeighbours(_priorDirection, _myStatus.obj);
            }
        }

        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    stringstream logline;
                    logline << display() << "  >> found out something on " <<
                            getDirection(direction(it->first)) << "(" << it->second->_myStatus.myScore << ")";
                    log(logline.str());
                    if (computeScore(it->second->_myStatus))
                    {
                        _myStatus.scoreDir = direction(it->first);
                    }
                }
            }
        }
    }
}

void MineElement::lookingForTrolley(direction incomingOrder)
{
    /**
     * Stop looking for an objective if :
     *  - we are inactiv
     *  - we are the trolley
     *  - we can't go throw this element
     *  - we are already asked
     */
    if (isTheTrolley == true || !_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        // prior compute expected way
        stringstream logline;
        if (_priorDirection != DIR_NONE)
        {
            if (_neighbours.find(_priorDirection) != _neighbours.end())
            {
                _neighbours[_priorDirection]->askNeighbours(_priorDirection, _myStatus.obj);
                logline << display() << " >>> prior that way " << getDirection(_priorDirection) << " ( " << _priorDirection << ")";
                log(logline.str());
            }
        }
        _myStatus.asked = true;
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    stringstream logline;
                    logline << display() << "  >> found out something on " <<
                            getDirection(direction(it->first)) << "(" << (it->second->_myStatus.myScore+1) << ")";
                    log(logline.str());
                    if (computeScore(it->second->_myStatus))
                    {
                        _myStatus.scoreDir = direction(it->first);
                    }

                }
            }
        }
    }
}

void MineElement::lookingForTarget(direction directionToShoot)
{
    /**
     * Stop looking for an objective if :
     *  - we are inactive
     *  - we have an ennemie here
     *  - we can't go throw this element
     *  - we are already asked
     */
    if (_ennemiPresent || _gender == MUD_SIGNE ||!_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        _myStatus.asked = true;
        if (_neighbours.find(directionToShoot) != _neighbours.end())
        {
            if (_neighbours[directionToShoot]->askNeighbours(directionToShoot, _myStatus.obj))
            {
                stringstream logline;
                logline << display() << " >> found out something on " <<
                        getDirection(directionToShoot) << "(" << _neighbours[directionToShoot]->_myStatus.myScore << ")";
                log(logline.str());
                if (computeScore(_neighbours[directionToShoot]->_myStatus))
                {
                    _myStatus.scoreDir = directionToShoot;
                }
            }
        }
    }
}

void MineElement::lookingForStone(direction incomingOrder)
{
    /**
     * Stop looking for an objective if :
     *  - we are inactive
     *  - we can't go throw this element
     *  - we are already asked
     */

    _myStatus.asked = true;
    if (_gender != STONE_SIGNE)
    {
        // prior compute expected way
        if (_priorDirection != DIR_NONE)
        {
            if (_neighbours.find(_priorDirection) != _neighbours.end())
            {
                log(display() + " >>> prior that way " + getDirection(_priorDirection));
                _neighbours[_priorDirection]->askNeighbours(_priorDirection, _myStatus.obj);
            }
        }
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    stringstream logline;
                    logline << display() << "  >> found out something on " <<
                            getDirection(direction(it->first)) << "(" << it->second->_myStatus.myScore << ")";
                    log(logline.str());
                    if (computeScore(it->second->_myStatus))
                    {
                        _myStatus.scoreDir = direction(it->first);
                    }
                }
            }
        }
    }
    else
    {
        if (_neighbours.find(getOpositeDirection(incomingOrder)) != _neighbours.end())
        {
            if ((_neighbours[getOpositeDirection(incomingOrder)]->getGender() == EMPTY_SIGNE) &&
                    ((_neighbours[incomingOrder]->getGender() == EMPTY_SIGNE) || (_neighbours[incomingOrder]->getGender() == MUD_SIGNE) || (_neighbours[incomingOrder]->isTheTrolley)))
            {
                _myStatus.myScore = 0;
                _myStatus.scoreDir = getOpositeDirection(incomingOrder);
            }
        }
    }
}

void MineElement::lookingForInactive(direction incomingOrder)
{
    /**
     * Stop looking for an objective if :
     *  - we are inactive
     *  - we can't go throw this element
     *  - we are already asked
     */
    if (!_myStatus.isActive || !_canWalkIn || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        _myStatus.asked = true;
        // prior compute expected way
        if (_priorDirection != DIR_NONE)
        {
            if (_neighbours.find(_priorDirection) != _neighbours.end())
            {
                _neighbours[_priorDirection]->askNeighbours(_priorDirection, _myStatus.obj);
                log(display() + " >>> prior that way " + getDirection(_priorDirection));
            }
        }
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    stringstream logline;
                    logline << display() << " >> found out something on " <<
                            getDirection(direction(it->first)) << "(" << it->second->_myStatus.myScore << ")";
                    log(logline.str());
                    if (computeScore(it->second->_myStatus))
                    {
                        _myStatus.scoreDir = direction(it->first);
                    }
                }
            }
        }
    }
}

void MineElement::lookingForMudNeirStone(direction incomingOrder)
{
    log(display());
    /**
     * Stop looking for an objective if :
     *  - we are inactive
     *  - we can't go throw this element
     *  - we are already asked
     */
    if (!_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        _myStatus.myScore = -1;
        _myStatus.scoreDir = DIR_NONE;
    }
    else
    {
        _myStatus.asked = true;
        if (_gender != MUD_SIGNE)
        {
            log(display() + " NOT MUD");
            // prior compute expected way
            if (_priorDirection != DIR_NONE)
            {
                if (_neighbours.find(_priorDirection) != _neighbours.end())
                {
                    log(display() + " >>> prior that way " + getDirection(_priorDirection));
                    _neighbours[_priorDirection]->askNeighbours(_priorDirection, _myStatus.obj);
                }
            }
            for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
            {
                if (it->first != incomingOrder)
                {
                    if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                    {
                        stringstream logline;
                        logline << display() << " >> found out something on " <<
                                getDirection(direction(it->first)) << "(" << it->second->_myStatus.myScore << ")";
                        log(logline.str());
                        if (computeScore(it->second->_myStatus))
                        {
                            _myStatus.scoreDir = direction(it->first);
                            log("choose => " + getDirection(direction(it->first)));
                        }
                    }
                }
            }
        }
        else
        {
            log(display() + " HERE MUD ");
            if (_neighbours.find(getOpositeDirection(incomingOrder)) != _neighbours.end())
            {
                if (_neighbours[getOpositeDirection(incomingOrder)]->getGender() == STONE_SIGNE)
                {
                    _myStatus.myScore = 1;
                    _myStatus.scoreDir = getOpositeDirection(incomingOrder);
                }
            }
        }
    }
}

void MineElement::savePriorDirection()
{
    if (_neighbours.find(_myStatus.scoreDir) != _neighbours.end())
    {
        _priorDirection = _myStatus.scoreDir;
    }
    else
    {
        resetPriorDirection();
    }
}

void MineElement::resetPriorDirection()
{
    _priorDirection = DIR_NONE;
}

string MineElement::display()
{
    return _gender + " " + Positionable::display();
}