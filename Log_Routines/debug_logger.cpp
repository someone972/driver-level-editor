#include"debug_logger.hpp"

DebugLogger::DebugLogger()
{
    priority = DEFAULT_PRIORITY;
    indent = 0;
    indentSize = 4;
};

void DebugLogger::Log(const char* /*base*/, ...)
{

};

void DebugLogger::Log(int /*level*/,const char* /*base*/,...)
{

};

int DebugLogger::getLogPriority()
{
    return priority;
};

void DebugLogger::setLogPriority(int logp)
{
    priority = logp;
};

void DebugLogger::increaseIndent()
{
    indent++;
};

void DebugLogger::decreaseIndent()
{
    if(indent > 0)
    indent--;
};

void DebugLogger::setIndent(int num)
{
    if(num >= 0)
    indent = num;
    else indent = 0;
};

void DebugLogger::setIndentSize(int length)
{
    indentSize = length;
};

int DebugLogger::getIndent()
{
    return indent;
};

DebugLogger::~DebugLogger()
{

};
