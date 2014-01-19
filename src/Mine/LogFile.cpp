/* 
 * File:   LogFile.cpp
 * Author: bakira
 * 
 * Created on 19 janvier 2014, 12:59
 */

#include "LogFile.h"

LogFile* LogFile::_global = 0;

LogFile * LogFile::inst()
{
    if (!_global)
    {
        _global = new LogFile();
    }
    return _global;
}

LogFile::LogFile()
{
    myfile.open("SuperLog.log");
}

LogFile::~LogFile()
{
    myfile.close();
}

