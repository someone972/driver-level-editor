#include "ioFuncs.hpp"

size_t freadWrapper(void *ptr, size_t size, size_t nmemb, IOHandle handle)
{
    return fread(ptr,size,nmemb,(FILE*)handle);
};

size_t fwriteWrapper(const void *ptr, size_t size, size_t nmemb, IOHandle handle)
{
    return fwrite(ptr,size,nmemb,(FILE*)handle);
};

int fseekWrapper(IOHandle handle, long int offset, int whence)
{
    return fseek((FILE*)handle,offset,whence);
};

long int ftellWrapper(IOHandle handle)
{
    return ftell((FILE*)handle);
};

int feofWrapper(IOHandle handle)
{
    return feof((FILE*)handle);
};

int fcloseWrapper(IOHandle handle)
{
    return fclose((FILE*)handle);
};

IOCallbacks fileCallbacks = {&freadWrapper,&fwriteWrapper,&fseekWrapper,&ftellWrapper,&feofWrapper,&fcloseWrapper};
