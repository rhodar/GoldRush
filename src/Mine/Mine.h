/* 
 * File:   Mine.h
 * Author: bakira
 *
 * Created on 15 décembre 2013, 18:05
 */

#ifndef MINE_H
#define	MINE_H
#include"MineElement.h"
#include"Minor.h"
#include<map>
#include<vector>
#include<string>

using namespace std;

#define MINE_INIT_MSG_SIZE 3

#define PICK_ORDER "PICK"
#define DROP_ORDER "DROP"
#define SHOOT_ORDER "SHOOT"

typedef enum {
    INIT_LARGEUR,
    INIT_HAUTEUR,
    INIT_DIAMAND
} INIT_STATE;

typedef enum {
    MINOR_X,
    MINOR_Y,
    MINOR_PROX
} MINOR_STATE;


class Mine {
public:
    Mine();
    void Init();
    virtual ~Mine();
    void runRound();
    bool _end;
    Minor _myMinor;
private:
    int _hauteur;
    int _longueur;
    int _nbDiamand;
    int _nbTour;
    
    void RegisterElements();
    void RefreshMapData(vector<string> mapDetails);
    void ResetMapScore(bool isGlobalReset = false);
    map< int, map< int, MineElement* > > _mineMap;
    int getRatio(int input);
    
    vector<string> getMapDetails();
    void setEnnemies(int nbEnnemies);
    void MoveToDirection(direction dir);
    bool findDiamand();
    bool findTrolley();
    bool findTarget(int nbEnnemies);
    bool findInactive();
    bool findStoneToMove();
    bool findMudNeirStone();
    void saveMapPriorDirection();
    void resetMapPriorDirection();
    //decode string input from cin
    bool initMineMap(vector<string> vec);
    
    void DisplayMap();
};

#endif	/* MINE_H */

