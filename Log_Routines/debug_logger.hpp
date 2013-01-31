#ifndef DEBUG_LOGGER_HPP
#define DEBUG_LOGGER_HPP
#include <cstdarg>
#include <cstdio>

const int DEFAULT_PRIORITY = 1;
const int DEBUG_LEVEL_NONE = -1;
const int DEBUG_LEVEL_MINIMAL = 0;
const int DEBUG_LEVEL_IMPORTANT_ONLY = 1;
const int DEBUG_LEVEL_NORMAL = 2;
const int DEBUG_LEVEL_VERBOSE = 3;
const int DEBUG_LEVEL_DEBUG = 4;
const int DEBUG_LEVEL_RIDICULOUS = 5;

class DebugLogger
{
    public:
        DebugLogger();
        virtual void Log(const char* base, ...);
        virtual void Log(int level,const char* base,...);
        virtual int getLogPriority();
        virtual void setLogPriority(int logp);
        virtual void increaseIndent();
        virtual void decreaseIndent();
        virtual int getIndent();
        virtual void setIndent(int num);
        virtual void setIndentSize(int length);
        virtual ~DebugLogger();

    protected:
        int priority;
        int indent;
        int indentSize;
};

#endif
