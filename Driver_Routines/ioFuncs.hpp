#ifndef IO_FUNCS_HPP
#define IO_FUNCS_HPP
#include <cstdio>

typedef void* IOHandle;

typedef size_t (*IOCallbackRead) (void *ptr, size_t size, size_t nmemb, IOHandle handle);

typedef size_t (*IOCallbackWrite) (const void *ptr, size_t size, size_t nmemb, IOHandle handle);

typedef int (*IOCallbackSeek) (IOHandle handle, long int offset, int whence);

typedef long int (*IOCallbackTell) (IOHandle handle);

typedef int (*IOCallbackEof) (IOHandle handle);

typedef int (*IOCallbackClose) (IOHandle handle);

typedef struct
{
    IOCallbackRead read;
    IOCallbackWrite write;
    IOCallbackSeek seek;
    IOCallbackTell tell;
    IOCallbackEof eof;
    IOCallbackClose close;
} IOCallbacks;


size_t freadWrapper(void *ptr, size_t size, size_t nmemb, IOHandle handle);
size_t fwriteWrapper(const void *ptr, size_t size, size_t nmemb, IOHandle handle);
int fseekWrapper(IOHandle handle, long int offset, int whence);
long int ftellWrapper(IOHandle handle);
int feofWrapper(IOHandle handle);
int fcloseWrapper(IOHandle handle);

extern IOCallbacks fileCallbacks;

#endif
