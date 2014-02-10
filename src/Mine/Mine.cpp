/* 
 * File:   MIne.cpp
 * Author: bakira
 * 
 * Created on 15 décembre 2013, 18:05
 */

#include "Mine.h"
#include <sstream>
#include "Utils.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

Mine::Mine() :
_end(false),
_myMinor(),
_hauteur(0),
_longueur(0),
_nbDiamand(0),
_nbTour(0)
{
}

bool Mine::initMineMap(vector<string> vec)
{
    bool ret = false;
    if (vec.size() == 3)
    {
        _longueur = atoi(vec.at(0).c_str());
        _hauteur = atoi(vec.at(1).c_str());
        _nbDiamand = atoi(vec.at(2).c_str());
        if (_longueur > 0 && _hauteur > 0 && _nbDiamand > 0)
        {
            log("Mine init : map = " + vec.at(0) + "/ " + vec.at(1) + " nb diamands = " + vec.at(2));
            ret = true;
        }
    }
    return ret;
}

void Mine::Init()
{
    string str;
    getline(cin, str);
    log(str);
    if (initMineMap(ParseMessage(str)))
    {
        // init maps
        for (int h = 0; h < _hauteur; h++)
        {
            for (int l = 0; l < _longueur; l++)
            {
                _mineMap[h][l] = new MineElement();
                _mineMap[h][l]->setPosition(l, h);
            }
        }
        // register neighbour of element
        RegisterElements();
    }
}

void Mine::RegisterElements()
{
    for (int h = 0; h < _hauteur; h++)
    {
        for (int l = 0; l < _longueur; l++)
        {
            // add a north neighbour
            if (h != 0)
            {
                _mineMap[h][l]->registerNeighbour(_mineMap[h - 1][l], DIR_NORTH);
            }
            // add a west neighbour
            if (l != 0)
            {
                _mineMap[h][l]->registerNeighbour(_mineMap[h][l - 1], DIR_WEST);
            }
            // add a south neighbour
            if (h != (_hauteur - 1))
            {
                _mineMap[h][l]->registerNeighbour(_mineMap[h + 1][l], DIR_SOUTH);
            }
            // add a east neighbour
            if (l != (_longueur - 1))
            {
                _mineMap[h][l]->registerNeighbour(_mineMap[h][l + 1], DIR_EAST);
            }
        }
    }
}

Mine::~Mine()
{
}

void Mine::runRound()
{
    while (!_end)
    {
        // check condition
        if (_nbTour == 1000)
        {
            _end = true;
        }
        ResetMapScore(true);
        string str;
        getline(cin, str);
        vector<string> vecXYN = ParseMessage(str);
        if (vecXYN.size() == 3)
        {
            log("Turn init : x = " + vecXYN.at(0) + "y = " + vecXYN.at(1) + " nb players = " + vecXYN.at(2));
            // if it's not the first turn and that the location is not a good one
            if (_nbTour > 0 && !_myMinor.evalExpectedLocation(atoi(vecXYN.at(0).c_str()), atoi(vecXYN.at(1).c_str())))
            {
                log(" >>> ERROR incorrect Y/X value : " + vecXYN.at(1) + "/" + vecXYN.at(0));
            }

            // set the minor position
            _myMinor.setPosition(vecXYN.at(0), vecXYN.at(1));
            log("Minor new position = [" + vecXYN.at(1) + "/" + vecXYN.at(0) + "]");
            // get initial position to remember our trolley
            if (_nbTour == 0)
            {
                log("Trolley pos(" + vecXYN.at(1) + "/" + vecXYN.at(0) + ")");
                _myMinor._trolley.setPosition(vecXYN.at(0), vecXYN.at(1));
                _mineMap[atoi(vecXYN.at(1).c_str())][atoi(vecXYN.at(0).c_str())]->isTheTrolley = true;
                _mineMap[atoi(vecXYN.at(1).c_str())][atoi(vecXYN.at(0).c_str())]->isTheTrolley = true;
            }
            // refresh the 5x5 map detail
            vector<string> details = getMapDetails();
            RefreshMapData(details);
            // set ennemies on map
            setEnnemies(atoi(vecXYN.at(2).c_str()));
            // display the whole computed map
            DisplayMap();
            // if we can shoot
            if (!findTarget(atoi(vecXYN.at(2).c_str())))
            {
                if (_myMinor._nbDiamandInMyPocket > 0 && _myMinor._nbDiamandInMyPocket < 3)
                {
                    log("still have some emptiness in my pocket");
                    // give order to find Diamant
                    if (!findDiamand())
                    {
                        log("fuck that diamands at least save my pocket's one");
                        // try to found the trolley
                        if (!findTrolley())
                        {
                            log("ok i'm bored go for AFK");
                            if (!findInactive())
                            {
                                log("Need to move something");
                                if (!findStoneToMove())
                                {
                                    log("Need to dig a bit to see clearer");
                                    findMudNeirStone();
                                }
                            }
                        }
                    }
                }
                else if (_myMinor._nbDiamandInMyPocket >= 3)
                {
                    log("pockets full of shiiit");
                    // try to find the trolley
                    if (!findTrolley())
                    {
                        log("ok i'm bored go for AFK");
                        if (!findInactive())
                        {
                            log("Need to move something");
                            if (!findStoneToMove())
                            {
                                log("Need to dig a bit to see clearer");
                                findMudNeirStone();
                            }
                        }
                    }
                }
                else
                {
                    log("go pick some chicks");
                    if (!findDiamand())
                    {
                        log("ok i'm bored go for AFK");
                        if (!findInactive())
                        {
                            log("Need to move something");
                            if (!findStoneToMove())
                            {
                                log("Need to dig a bit to see clearer");
                                findMudNeirStone();
                            }
                        }
                    }
                }

                _nbTour++;
            }
        }
        else
        {
            log("Turn init failed : " + str);
        }
    }
}

vector<string> Mine::getMapDetails()
{
    vector<string> details;
    for (int i = 0; i < 5; i++)
    {
        string tmp;
        getline(cin, tmp);
        log(tmp);
        vector<string> tmpVec = ParseMessage(tmp);
        for (int j = 0; j < tmpVec.size(); j++)
        {
            details.push_back(tmpVec.at(j));
        }
    }
    return details;
}

void Mine::setEnnemies(int nbEnnemies)
{
    for (int i = 0; i < nbEnnemies; i++)
    {
        string tmp;
        getline(cin, tmp);
        vector<string> vecEnnemieXY = ParseMessage(tmp);
        log(tmp);
        if (vecEnnemieXY.size() == 2)
        {
            _mineMap[atoi(vecEnnemieXY.at(1).c_str())][atoi(vecEnnemieXY.at(0).c_str())]->setEnnemies();
        }
        else
        {
            log("Ennemie definition failed");
        }
    }
}

void Mine::RefreshMapData(vector<string> mapDetails)
{
    for (int i = 0; i < mapDetails.size(); i++)
    {
        // compute ratio
        int hRatio = getRatio(i / 5);
        int lRatio = getRatio(i % 5);
        // check value
        int nbDiamant = 0;
        string gender = mapDetails.at(i);
        if (gender != TROLLEY_SIGNE && gender != MUD_SIGNE && gender != STONE_SIGNE && gender != EMPTY_SIGNE)
        {
            // its a diamant things
            nbDiamant = atoi(gender.c_str());
            gender = EMPTY_SIGNE;
        }
        // refresh element based on Minor position
        int computedH = _myMinor.getPositionY() + hRatio;
        int computedL = _myMinor.getPositionX() + lRatio;
        if (computedH >= 0 && computedL >= 0 && computedH < _hauteur && computedL < _longueur)
        {
            _mineMap[computedH][computedL]->refresh(gender, _nbTour, nbDiamant);
        }
        else
        {
            stringstream logline;
            logline << " POS [" << computedH << "/" << computedL << "] refesh droped";
            log(logline.str());
        }
    }

}

int Mine::getRatio(int input)
{
    int ret = 0;
    switch (input)
    {
    case 0:
        ret = -2;
        break;
    case 1:
        ret = -1;
        break;
    case 2:
        ret = 0;
        break;
    case 3:
        ret = 1;
        break;
    case 4:
        ret = 2;
        break;
    default:
        break;
    }
    return ret;
}

void Mine::ResetMapScore(bool isGlobalReset)
{
    for (int h = 0; h < _hauteur; h++)
    {
        for (int l = 0; l < _longueur; l++)
        {
            _mineMap[h][l]->reset(isGlobalReset);
        }
    }
}

void Mine::MoveToDirection(direction dir)
{
    log(" <<<<<<<<<  Go to the " + getDirection(dir));
    puts(getDirection(dir).c_str());
    _myMinor.lookingDirection = dir;
    // on movement save prior direction
    saveMapPriorDirection();
}

bool Mine::findDiamand()
{
    log("Find diamand");
    bool ret = false;
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_DIAMANDS))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to pick " << tempStatus.diamants << " diamands";
        log(logLine.str());
        if (tempStatus.myScore > 0)
        {
            _myMinor.computeExpectedLocation(tempStatus.scoreDir);
            //if diamants go to the direction
            MoveToDirection(tempStatus.scoreDir);
        }
        else
        {
            log("Pick diamants");
            // pick diamants
            _myMinor._nbDiamandInMyPocket += tempStatus.diamants;
            if (_myMinor._nbDiamandInMyPocket > 3)
            {
                _myMinor._nbDiamandInMyPocket = 3;
            }
            _myMinor.computeExpectedLocation();
            puts(PICK_ORDER);
            //on action reset the prior Direction
            resetMapPriorDirection();
        }
    }
    return ret;
}

bool Mine::findTrolley()
{
    bool ret = false;
    ResetMapScore();
    log("Find trolley");
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_TROLLEY))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to drop " << _myMinor._nbDiamandInMyPocket << " diamands";
        log(logLine.str());
        if (tempStatus.myScore > 0)
        {
            _myMinor.computeExpectedLocation(tempStatus.scoreDir);
            //if diamants go to the direction
            MoveToDirection(tempStatus.scoreDir);
        }
        else
        {
            log("drop diamants");
            _myMinor._nbInDaTrolley += _myMinor._nbDiamandInMyPocket;
            _myMinor._nbDiamandInMyPocket = 0;
            _myMinor.computeExpectedLocation();
            //on action reset the prior Direction
            resetMapPriorDirection();
            // drop diamants
            puts(DROP_ORDER);
        }
    }
    return ret;
}

bool Mine::findTarget(int nbEnnemies)
{
    bool ret = false;
    if (nbEnnemies > 0)
    {
        log("Find target");
        ResetMapScore();
        if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(_myMinor.lookingDirection, OBJ_TARGET))
        {
            ret = true;
            log("Shoot target");
            _myMinor.computeExpectedLocation();
            //on action reset the prior Direction
            resetMapPriorDirection();
            // Shoot target
            puts(SHOOT_ORDER);
        }
    }
    return ret;
}

bool Mine::findInactive()
{
    bool ret = false;
    log("Find Inactive");
    ResetMapScore();

    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_INACTIVE))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to found out incative";
        log(logLine.str());
        _myMinor.computeExpectedLocation(tempStatus.scoreDir);
        //Go to the direction
        MoveToDirection(tempStatus.scoreDir);
        if (tempStatus.myScore == 2)
        {
            log("inactive close ");
            resetMapPriorDirection();
        }
    }
    return ret;
}

bool Mine::findMudNeirStone()
{
    bool ret = false;
    log("Find Mud");
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_MUD))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to found out MUD";
        log(logLine.str());
        _myMinor.computeExpectedLocation(tempStatus.scoreDir);
        //Go to the direction
        MoveToDirection(tempStatus.scoreDir);
        if (tempStatus.myScore == 1)
        {
            log("MUD close ");
            resetMapPriorDirection();
        }
    }
    return ret;
}

bool Mine::findStoneToMove()
{
    bool ret = false;
    log("Find Stone to Moove");
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_STONE))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to move the stone";
        log(logLine.str());
        //Go to the direction
        _myMinor.computeExpectedLocation(tempStatus.scoreDir);
        MoveToDirection(tempStatus.scoreDir);
        if (tempStatus.myScore == 2)
        {
            log("Stone close ");
            resetMapPriorDirection();
        }
    }
    return ret;
}

void Mine::saveMapPriorDirection()
{
    for (int h = 0; h < _hauteur; h++)
    {
        for (int l = 0; l < _longueur; l++)
        {
            _mineMap[h][l]->savePriorDirection();
        }
    }
}

void Mine::resetMapPriorDirection()
{
    for (int h = 0; h < _hauteur; h++)
    {
        for (int l = 0; l < _longueur; l++)
        {
            _mineMap[h][l]->resetPriorDirection();
        }
    }
}

void Mine::DisplayMap()
{
    stringstream logline;
    logline << " ----------------- TOUR :  " << _nbTour << " ----------------------";
    log(logline.str());
    for (int i = 0; i < _hauteur; i++)
    {
        string line = "";
        for (int j = 0; j < _longueur; j++)
        {
            if (i == _myMinor.getPositionY() && j == _myMinor.getPositionX())
            {
                line += "0";
            }
            else if (_mineMap[i][j]->_myStatus.isActive)
            {
                line += _mineMap[i][j]->getGender();
            }
            else
            {
                line += "I";
            }
        }
        log("++ " + line + " ++");
    }
}
