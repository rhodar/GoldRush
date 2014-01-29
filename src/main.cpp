/* 
 * File:   main.cpp
 * Author: bakira
 *
 * Created on 15 décembre 2013, 11:16
 */

#include <cstdlib>
#include "Mine/Mine.h"
#include <iostream>
#include "Mine/Utils.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
    string name = "SuperMonchu";
    // give the minor name
    puts(name.c_str());
    log("Minor :" + name + " join the game");
    // init map
    Mine myMine;
    myMine.Init();
    myMine.runRound();
    return 0;
}

