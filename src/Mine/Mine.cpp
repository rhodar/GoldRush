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
_nbTour(0),
_nextStep(DIR_NONE)
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
                /*            stringstream name;
                            name << h << "/" << l;*/
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

        ResetMapScore();

        string str;
        getline(cin, str);
        vector<string> vecXYN = ParseMessage(str);

        log("Turn init : x = " + vecXYN.at(0) + "y = " + vecXYN.at(1) + " nb players = " + vecXYN.at(2));
        // if it's not the first turn and that the location is not a good one
        if (_nbTour > 0 && !_myMinor.evalExpectedLocation(atoi(vecXYN.at(0).c_str()), atoi(vecXYN.at(1).c_str())))
        {
            log(" >>> ERROR incorrect Y/X value : " + vecXYN.at(1) + "/" + vecXYN.at(0));
            cout << "\n";
        }
        else
        {

            // set the minor position
            _myMinor.setPosition(vecXYN.at(0), vecXYN.at(1));
            log("Minor new position = [" + vecXYN.at(1) + "/" + vecXYN.at(0) + "]");
            // get initial position to remember our trolley
            if (_nbTour == 0)
            {
                log("Trolley pos(" + vecXYN.at(1) + "/" + vecXYN.at(0) + ")");
                _myMinor._trolley.setPosition(vecXYN.at(0), vecXYN.at(1));
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
            if (!findTarget())
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
                                findStoneToMove();
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
                            findStoneToMove();
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
                            findStoneToMove();
                        }
                    }
                }
            }
            _nbTour++;
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

        _mineMap[atoi(vecEnnemieXY.at(1).c_str())][atoi(vecEnnemieXY.at(0).c_str())]->setEnnemies();
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

void Mine::ResetMapScore()
{
    for (int h = 0; h < _hauteur; h++)
    {
        for (int l = 0; l < _longueur; l++)
        {
            _mineMap[h][l]->reset();
        }
    }
}

void Mine::MoveToDirection(direction dir)
{
    log(" <<<<<<<<<  Go to the " + getDirection(dir));
    cout << getDirection(dir) << "\n";
    _myMinor.lookingDirection = dir;
    // on movement reinit the backtraced way
    _nextStep = expectedNextStep();
}

bool Mine::findDiamand()
{
    log("Find diamand");
    bool ret = false;
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_DIAMANDS, _nextStep))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to pick " << tempStatus.diamants << " diamands";
        log(logLine.str());


        if (tempStatus.myScore > 0)
        {
            //if diamants go to the direction
            MoveToDirection(tempStatus.scoreDir);
            _myMinor.computeExpectedLocation(tempStatus.scoreDir);
        }
        else
        {
            log("Pick diamants");
            // pick diamants
            cout << PICK_ORDER << "\n";
            _myMinor._nbDiamandInMyPocket += tempStatus.diamants;
            if (_myMinor._nbDiamandInMyPocket > 3)
            {
                _myMinor._nbDiamandInMyPocket = 3;
            }
            _myMinor.computeExpectedLocation();
            //on action reset the backtraceway
            _nextStep = DIR_NONE;
        }
    }
    return ret;
}

bool Mine::findTrolley()
{

    bool ret = false;
    ResetMapScore();
    log("Find trolley");
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_TROLLEY, _nextStep))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to drop " << _myMinor._nbDiamandInMyPocket << " diamands";
        log(logLine.str());


        if (tempStatus.myScore > 0)
        {
            //if diamants go to the direction
            MoveToDirection(tempStatus.scoreDir);
            _myMinor.computeExpectedLocation(tempStatus.scoreDir);
        }
        else
        {
            log("drop diamants");
            // pick diamants
            cout << DROP_ORDER << "\n";
            _myMinor._nbInDaTrolley += _myMinor._nbDiamandInMyPocket;
            _myMinor._nbDiamandInMyPocket = 0;
            _myMinor.computeExpectedLocation();
            //on action reset the backtraceway
            _nextStep = DIR_NONE;
        }
    }
    return ret;
}

bool Mine::findTarget()
{
    log("Find target");
    bool ret = false;
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(_myMinor.lookingDirection, OBJ_TARGET, _nextStep))
    {
        ret = true;
        log("Shoot target");
        // pick diamants
        cout << SHOOT_ORDER << "\n";
        _myMinor.computeExpectedLocation();
        //on action reset the backtraceway
        _nextStep = DIR_NONE;
    }
    return ret;
}

bool Mine::findInactive()
{
    bool ret = false;
    log("Find Inactive");
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_INACTIVE, _nextStep))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to found out incative";
        log(logLine.str());
        //Go to the direction
        MoveToDirection(tempStatus.scoreDir);
        _myMinor.computeExpectedLocation(tempStatus.scoreDir);
    }
    return ret;
}

bool Mine::findStoneToMove()
{
    bool ret = false;
    log("Find Stone to Moove");
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_STONE, _nextStep))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore << " steps to the " << getDirection(tempStatus.scoreDir) << " to move the stone";
        log(logLine.str());
        //Go to the direction
        MoveToDirection(tempStatus.scoreDir);
        _myMinor.computeExpectedLocation(tempStatus.scoreDir);
    }
    return ret;
}

direction Mine::expectedNextStep()
{
    // get the current pos and the score direction
    direction dir = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus.scoreDir;
    Positionable pos;
    if (dir != DIR_NONE)
    {
        pos = backTraceTravel(dir, _myMinor.getPositionX(), _myMinor.getPositionY());
        // Start on the way expected after moving
        dir = _mineMap[pos.getPositionY()][pos.getPositionX()]->_myStatus.scoreDir;
    }
    log(">> Expected next step = " + getDirection(dir));
    return dir;
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
            if(i== _myMinor.getPositionY()  && j == _myMinor.getPositionX())
            {
               line+="0" ;
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
