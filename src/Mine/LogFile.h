/* 
 * File:   LogFile.h
 * Author: bakira
 *
 * Created on 19 janvier 2014, 12:59
 */

#ifndef LOGFILE_H
#define	LOGFILE_H
#include <iostream>
#include <fstream>
using namespace std;

class LogFile {
public:
    static LogFile* inst();
    virtual ~LogFile();
private:
    LogFile();
    static LogFile* _global;
    ofstream myfile;
};

#endif	/* LOGFILE_H */

