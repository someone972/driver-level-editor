#ifndef DEFAULT_LOGGERS_HPP
#define DEFAULT_LOGGERS_HPP
#include "debug_logger.hpp"
#include <iostream>
#include <ctime>

using namespace std;

class CmdLogger : public DebugLogger
{
    public:
        CmdLogger();
        void Log(const char* base, ...);
        void Log(int level,const char* base,...);
        int getLogPriority();
        void setLogPriority(int logp);
        void writeTime();
};

class FileLogger : public DebugLogger
{
    public:
        FileLogger(const char* filename = NULL);
        FileLogger(FILE* file);
        ~FileLogger();
        void Log(const char* base, ...);
        void Log(int level,const char* base,...);
        int getLogPriority();
        void setLogPriority(int logp);
        void writeTime();

        int createLogfile(const char* filename);
    protected:
        FILE* logfile;
};

#endif
