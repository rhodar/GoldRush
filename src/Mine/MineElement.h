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
    MineElement();
    virtual ~MineElement();
    Status _myStatus;
    bool askNeighbours(direction from, objectives obj);
    void reset(bool isGlobalReset);
    void refresh(string gender, int turn, int diamand);
    void savePriorDirection();
    void resetPriorDirection();
    void registerNeighbour(MineElement* element, direction where);
    void setEnnemies();
    string getGender(){return _gender;};
    bool isTheTrolley;
    virtual string display();
private:
    map<int, MineElement*> _neighbours;
    void computeMyScore();
    bool computeScore(Status neighbourStatus);
    string _gender;
    bool _canWalkIn;
    int _nbDiamands;
    bool _ennemiPresent;
    direction _priorDirection;
    void lookingForDiamands(direction incomingOrder);
    void lookingForTrolley(direction incomingOrder);
    void lookingForTarget(direction incomingOrder);
    void lookingForStone(direction incomingOrder);
    void lookingForInactive(direction incomingOrder);
    void lookingForMudNeirStone(direction incomingOrder);
    
};

#endif	/* MINEELEMENT_H */

