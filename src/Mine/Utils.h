/* 
 * File:   Utils.h
 * Author: bakira
 *
 * Created on 20 janvier 2014, 20:49
 */

#ifndef UTILS_H
#define	UTILS_H
#include <fstream>
#include <iostream>
#include <vector>
#include<string>

#include "Positionable.h"

#define TROLLEY_SIGNE "X"
#define MUD_SIGNE "M"
#define STONE_SIGNE "S"
#define EMPTY_SIGNE "E"

typedef enum {
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NONE
} direction;

typedef enum {
    OBJ_DIAMANDS, // we don't yet have 3 diamands in our pocket
    OBJ_TROLLEY,  // we have 3 diamands in our pocket
    OBJ_TARGET,   // we are looking for a target to shoot
    OBJ_STONE,    // we can't do anything so we're lookong to a stone to moove
    OBJ_INACTIVE, // find out inactive case to be refreshed  
    OBJ_MUD
} objectives;

static void log(string message) {
    ofstream myfile;
    myfile.open("SuperLog.log", ios::app);
    myfile << message << "\n";
    myfile.close();
}

static string getDirection(direction initialDirection) {
    string ret = "NONE";
    switch (initialDirection) {
        case DIR_NORTH:
            ret = "NORTH";
            break;
        case DIR_EAST:
            ret = "EAST";
            break;
        case DIR_SOUTH:
            ret = "SOUTH";
            break;
        case DIR_WEST:
            ret = "WEST";
            break;
        default:
            ret = "NONE";

            break;
    }
    return ret;
}

static direction getOpositeDirection(direction initialDirection) {
    direction ret = DIR_NONE;
    switch (initialDirection) {
        case DIR_NORTH:
            ret = DIR_SOUTH;
            break;
        case DIR_EAST:
            ret = DIR_WEST;
            break;
        case DIR_SOUTH:
            ret = DIR_NORTH;
            break;
        case DIR_WEST:
            ret = DIR_EAST;
            break;
        default:
            ret = DIR_NONE;

            break;
    }
    return ret;
}

static vector<string> ParseMessage(string input) {
    vector<string> parsedMsg;
    string parsed;
    stringstream ss(input);
    while (getline(ss, parsed, ' ')) {
        parsedMsg.push_back(parsed);
    }
    return parsedMsg;
}

static Positionable backTraceTravel(direction dir, int x, int y) {
    Positionable ret;
    // set with current position
    ret.setPosition(x, y);

    switch (dir) {
        case DIR_EAST:
            ret.setPosition(ret.getPositionX() + 1, ret.getPositionY());
            break;
        case DIR_WEST:
            ret.setPosition(ret.getPositionX() - 1, ret.getPositionY());
            break;
        case DIR_SOUTH:
            ret.setPosition(ret.getPositionX(), ret.getPositionY() + 1);
            break;
        case DIR_NORTH:
            ret.setPosition(ret.getPositionX(), ret.getPositionY() - 1);
            break;
        default:

            break;
    }

    return ret;
}

#endif	/* UTILS_H */

