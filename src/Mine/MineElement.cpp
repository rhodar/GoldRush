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

MineElement::MineElement(string name) :
_name(name),
isTheTrolley(false),
_gender(EMPTY_SIGNE),
_canWalkIn(false),
_nbDiamands(0),
_ennemiPresent(false)
{
    _myStatus.asked = false;
    _myStatus.scoreDir = DIR_NONE;
    _myStatus.isActive = false;
    _myStatus.lastTurnUpdate = 0;
    _myStatus.diamants = 0;
    _myStatus.myScore = 0;
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
    case OBJ_MUD:
        lookingForMud(incomingOrder);
        break;
    case OBJ_TROLLEY:
        lookingForTrolley(incomingOrder);
        break;
    case OBJ_INACTIVE:
        lookingForInactive(incomingOrder);
        break;
    case OBJ_DIAMANDS:
    default:
        lookingForDiamands(incomingOrder);
        break;
    }
    if (_myStatus.myScore > 0)
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
            _myStatus.myScore = 1;
        }
        break;
    case OBJ_TROLLEY:
        if (isTheTrolley)
        {
            _myStatus.myScore = 1;
        }
        break;
    case OBJ_MUD:
        if (_gender == MUD_SIGNE)
        {
            _myStatus.myScore = 1;
        }
        break;
    case OBJ_INACTIVE:
        if (!_myStatus.isActive)
        {
            _myStatus.myScore = 1;
        }
        break;
    case OBJ_DIAMANDS:
        if (_nbDiamands > 0)
        {
            _myStatus.myScore = 1;
            _myStatus.diamants = _nbDiamands;
        }
        break;
    }
}

bool MineElement::computeScore(Status neighbourStatus)
{
    bool ret = false;
    stringstream logline;
    logline << "["<<_name << "] >> neighbours score : " << neighbourStatus.myScore;
    log(logline.str());
    /**
     * If the score is better there take its score
     * If the score is equal but more diamants take its score
     * Else keep our score
     */
    int myScore = _myStatus.myScore;
    int itsScore = neighbourStatus.myScore + 1;

    int myDiam = _myStatus.diamants;
    int itsDiam = neighbourStatus.diamants;

    bool keepIt = false;

    if (itsScore < myScore || myScore == 0)
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
        _myStatus.myScore = neighbourStatus.myScore + 1;
        _myStatus.diamants = neighbourStatus.diamants;
        log(" >>>>>>>>>> OK");
        ret = true;
    }
    else
    {
        log(" >>>>>>>>>> KO");
    }
    return ret;
}

void MineElement::reset()
{
    _myStatus.scoreDir = DIR_NONE;
    _myStatus.myScore = 0;
    _myStatus.diamants = 0;
    _myStatus.asked = false;
    _ennemiPresent = false;
}

void MineElement::refresh(string gender, int turn, int diamand)
{
    stringstream logline;
    logline << "[" << _name << "] >> Reset (" << gender << ", " << turn << ", " << diamand;
    log(logline.str());
    _gender = gender;
    _myStatus.lastTurnUpdate = turn;
    _myStatus.isActive = true;
    _nbDiamands = diamand;
    if (gender == EMPTY_SIGNE || gender == MUD_SIGNE || isTheTrolley)
    {
        _canWalkIn = true;
    }

}

void MineElement::registerNeighbour(MineElement* element, direction where)
{
    _neighbours[where] = element;
}

void MineElement::setEnnemies()
{
    log("[" + _name + "]>> Ennemi present");
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
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    log("[" + _name + "] >> found out something on " + getDirection(direction(it->first)));
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
    if (isTheTrolley > 0 || !_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        _myStatus.asked = true;
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    log("[" + _name + "] >> found out something on " + getDirection(direction(it->first)));
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
    if (_ennemiPresent || !_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        _myStatus.asked = true;

        if (_neighbours[directionToShoot]->askNeighbours(directionToShoot, _myStatus.obj))
        {
            log("[" + _name + "] >> found out something on " + getDirection(directionToShoot));
            if (computeScore(_neighbours[directionToShoot]->_myStatus))
            {
                _myStatus.scoreDir = directionToShoot;
            }
        }
    }
}

void MineElement::lookingForMud(direction incomingOrder)
{
    /**
     * Stop looking for an objective if :
     *  - we are inactive
     *  - we are a MUD case
     *  - we can't go throw this element
     *  - we are already asked
     */
    if (_gender == MUD_SIGNE || !_canWalkIn || !_myStatus.isActive || _myStatus.asked)
    {
        computeMyScore();
    }
    else
    {
        _myStatus.asked = true;
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    log("[" + _name + "] >> found out something on " + getDirection(direction(it->first)));
                    if (computeScore(it->second->_myStatus))
                    {
                        _myStatus.scoreDir = direction(it->first);
                    }
                }
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
        for (map<int, MineElement *>::iterator it = _neighbours.begin(); it != _neighbours.end(); it++)
        {
            if (it->first != incomingOrder)
            {
                if (it->second->askNeighbours(direction(it->first), _myStatus.obj))
                {
                    log("[" + _name + "] >> found out something on " + getDirection(direction(it->first)));
                    if (computeScore(it->second->_myStatus))
                    {
                        _myStatus.scoreDir = direction(it->first);
                    }
                }
            }
        }
    }
}
