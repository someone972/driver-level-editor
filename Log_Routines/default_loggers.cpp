#include "default_loggers.hpp"

CmdLogger::CmdLogger()
{
    priority = DEFAULT_PRIORITY;
};

void CmdLogger::writeTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);
    printf("%d:%d:%d - ",timeinfo->tm_hour,timeinfo->tm_hour,timeinfo->tm_sec);
};

void CmdLogger::Log(const char* base, ...)
{
    if(priority >= 0)
    {
        writeTime();
        va_list a_list;
        va_start(a_list,base);
        printf("%*c",indent*indentSize,' ');
        vprintf(base,a_list);
        printf("\n");
        va_end(a_list);
    }
};

void CmdLogger::Log(int level,const char* base,...)
{
    if(level <= priority && priority >= 0)
    {
        writeTime();
        va_list a_list;
        va_start(a_list,base);
        printf("%*c",indent*indentSize,' ');
        vprintf(base,a_list);
        printf("\n");
        va_end(a_list);
    };
};

int CmdLogger::getLogPriority()
{
    return priority;
};

void CmdLogger::setLogPriority(int logp)
{
    priority = logp;
};


FileLogger::FileLogger(const char* filename)
{
    priority = DEFAULT_PRIORITY;
    logfile = NULL;
    if(filename != NULL)
    createLogfile(filename);
};

FileLogger::FileLogger(FILE* file)
{
    priority = DEFAULT_PRIORITY;
    logfile = file;
};

int FileLogger::createLogfile(const char* filename)
{
    logfile = fopen(filename,"w");
    if(!logfile)
    return 1;
    return 0;
};

void FileLogger::writeTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);
    fprintf(logfile,"%d:%d:%d - ",timeinfo->tm_hour,timeinfo->tm_hour,timeinfo->tm_sec);
};

void FileLogger::Log(const char* base, ...)
{
    if(priority >= 0 && logfile != NULL)
    {
        writeTime();
        va_list a_list;
        va_start(a_list,base);
        fprintf(logfile,"%*c",indent*indentSize,' ');
        vfprintf(logfile,base,a_list);
        va_end(a_list);
        fprintf(logfile,"\n");
        fflush(logfile);
    }
};

void FileLogger::Log(int level,const char* base,...)
{
    if(level <= priority && priority >= 0 && logfile != NULL)
    {
        writeTime();
        va_list a_list;
        va_start(a_list,base);
        fprintf(logfile,"%*c",indent*indentSize,' ');
        vfprintf(logfile,base,a_list);
        va_end(a_list);
        fprintf(logfile,"\n");
        fflush(logfile);
    };
};

int FileLogger::getLogPriority()
{
    return priority;
};

void FileLogger::setLogPriority(int logp)
{
    priority = logp;
};

FileLogger::~FileLogger()
{
    if(logfile)
    fclose(logfile);
};
