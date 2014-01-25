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

void Mine::Init()
{
    string strHaut, strLong, strNb;
    cin >> strLong;
    cin >> strHaut;
    cin >> strNb;
    log("Mine init : hauteur = " + strHaut + " longueur= " + strLong + "nb diamands = " + strNb);
    _hauteur = atoi(strHaut.c_str());
    _longueur = atoi(strLong.c_str());
    _nbDiamand = atoi(strNb.c_str());

    // init maps
    for (int h = 0; h < _hauteur; h++)
    {
        for (int l = 0; l < _longueur; l++)
        {
            stringstream name;
            name << h << "/" << l;
            _mineMap[h][l] = new MineElement(name.str());
        }
    }
    // register neighbour of element
    RegisterElements();
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

vector<string> Mine::ParseMessage(string input)
{
    vector<string> parsedMsg;
    string parsed;
    stringstream ss(input);
    if (getline(ss, parsed, ' '))
    {
        parsedMsg.push_back(parsed);
        log(parsed);
    }
    return parsedMsg;
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

        string x, y, n;
        cin >> x;
        cin >> y;
        cin >> n;

        log("Turn init : x = " + x + "y = " + y + " nb players = " + n);
        // set the minor position
        _myMinor.setPosition(x, y);
        log("Minor new position = [" + y + "/" + x + "]");
        // get initial position to remember our trolley
        if (_nbTour == 0)
        {
            log("Trolley pos(" + y + "/" + x + ")");
            _mineMap[atoi(y.c_str())][atoi(x.c_str())]->isTheTrolley = true;
        }
        // refresh the 5x5 map detail
        vector<string> details = getMapDetails();
        RefreshMapData(details);
        // set ennemies on map
        setEnnemies(atoi(n.c_str()));


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
                        log("fuck that trolley go for mud");
                        // give order to find Mud
                        if (!findMud())
                        {
                            log("ok i'm bored go for AFK");
                            findInactive();
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
                    log("fuck that trolley go for mud");
                    // give order to find Mud
                    if (!findMud())
                    {
                        log("ok i'm bored go for AFK");
                        findInactive();
                    }
                }
            }
            else
            {
                log("go pick some chicks");
                if (!findDiamand())
                {

                    log("fuck that trolley go for mud");
                    // give order to find Mud
                    if (!findMud())
                    {
                        log("ok i'm bored go for AFK");
                        findInactive();
                    }

                }
            }
        }
        _nbTour++;
    }
}

vector<string> Mine::getMapDetails()
{
    vector<string> details;
    string mapDetails;
    for (int i = 0; i < 5; i++)
    {
        mapDetails = "";
        for (int j = 0; j < 5; j++)
        {
            string tmp = "";
            cin >> tmp;
            details.push_back(tmp);
            mapDetails += tmp;
        }
        log(">>" + mapDetails);
    }
    return details;
}

void Mine::setEnnemies(int nbEnnemies)
{
    for (int i = 0; i < nbEnnemies; i++)
    {
        string tmpX, tmpY;
        cin >> tmpX;
        cin >> tmpY;
        _mineMap[atoi(tmpY.c_str())][atoi(tmpX.c_str())]->setEnnemies();
    }
}

void Mine::RefreshMapData(vector<string> mapDetails)
{
    for (uint i = 0; i < mapDetails.size(); i++)
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
        if(computedH >=0 && computedL >=0)
        {
        _mineMap[computedH][computedL]->refresh(gender, _nbTour, nbDiamant);
        }
        else
        {
            stringstream logline;
            logline << " POS ["<< computedH <<"/"<<computedL<< "] refesh droped";
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
        logLine << "go " << tempStatus.myScore - 1 << " steps to the " << getDirection(tempStatus.scoreDir) << " to pick " << tempStatus.diamants << " diamands";
        log(logLine.str());


        if (tempStatus.myScore > 1)
        {
            //if diamants go to the direction
            MoveToDirection(tempStatus.scoreDir);
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
        logLine << "go " << tempStatus.myScore - 1 << " steps to the " << getDirection(tempStatus.scoreDir) << " to drop " << _myMinor._nbDiamandInMyPocket << " diamands";
        log(logLine.str());


        if (tempStatus.myScore > 1)
        {
            //if diamants go to the direction
            MoveToDirection(tempStatus.scoreDir);
        }
        else
        {
            log("drop diamants");
            // pick diamants
            cout << DROP_ORDER << "\n";
            _myMinor._nbDiamandInMyPocket = 0;
        }
    }
    return ret;
}

bool Mine::findTarget()
{
    log("Find target");
    bool ret = false;
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(_myMinor.lookingDirection, OBJ_TARGET))
    {
        ret = true;
        log("Shoot target");
        // pick diamants
        cout << SHOOT_ORDER << "\n";

    }
    return ret;
}

bool Mine::findMud()
{
    bool ret = false;
    log("Find Mud");
    ResetMapScore();
    if (_mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->askNeighbours(DIR_NONE, OBJ_MUD))
    {
        ret = true;
        Status tempStatus = _mineMap[_myMinor.getPositionY()][_myMinor.getPositionX()]->_myStatus;
        stringstream logLine;
        logLine << "go " << tempStatus.myScore - 1 << " steps to the " << getDirection(tempStatus.scoreDir) << " to dig mud";
        log(logLine.str());
        //if mud go to the direction
        MoveToDirection(tempStatus.scoreDir);
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
        logLine << "go " << tempStatus.myScore - 1 << " steps to the " << getDirection(tempStatus.scoreDir) << " to found out incative";
        log(logLine.str());
        //Go to the direction
        MoveToDirection(tempStatus.scoreDir);
    }
    return ret;
}