/* 
 * File:   MIneElement.h
 * Author: bakira
 *
 * Created on 15 décembre 2013, 11:30
 */

#ifndef MINEELEMENT_H
#define	MINEELEMENT_H

#include "Positionable.h"
#include "Utils.h"

#include<map>
#include<vector>


using namespace std;

struct Status {
    bool asked;
    int lastTurnUpdate;
    bool isActive;
    int myScore;
    int diamants;
    direction scoreDir;
    objectives obj;
};

class MineElement :
public Positionable {
public:
    MineElement(string name);
    virtual ~MineElement();
    Status _myStatus;
    string _name;
    bool askNeighbours(direction from, objectives obj = OBJ_DIAMANDS);
    void reset();
    void refresh(string gender, int turn, int diamand);
    void registerNeighbour(MineElement* element, direction where);
    void setEnnemies();
    bool isTheTrolley;
private:
    map<int, MineElement*> _neighbours;
    void computeMyScore();
    bool computeScore(Status neighbourStatus);
    string _gender;
    bool _canWalkIn;
    int _nbDiamands;
    bool _ennemiPresent;
    void lookingForDiamands(direction incomingOrder);
    void lookingForTrolley(direction incomingOrder);
    void lookingForTarget(direction incomingOrder);
    void lookingForMud(direction incomingOrder);
    void lookingForInactive(direction incomingOrder);
    
};

#endif	/* MINEELEMENT_H */

