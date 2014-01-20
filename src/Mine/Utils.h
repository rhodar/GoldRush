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

static void log(string message) {
    ofstream myfile;
    myfile.open("SuperLog.log", ios::app);
    myfile << message << "\n";
    myfile.close();
}
#endif	/* UTILS_H */

