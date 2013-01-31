#include "driver_d3d.hpp"

D3DEntry::D3DEntry()
{
    eventManager = NULL;
    index = -1;
    texture = 0;
    numPalettes = 0;
    palettes = NULL;
};

D3DEntry::D3DEntry(CEventMgr<IDriverD3DEvents>* eventMgr, int idx)
{
    eventManager = eventMgr;
    index = idx;
    texture = 0;
    numPalettes = 0;
    palettes = NULL;
};

D3DEntry::~D3DEntry()
{
    cleanup();
};

void D3DEntry::cleanup()
{
    if(palettes)
    delete[] palettes;
    palettes = NULL;
    numPalettes = 0;
};

int D3DEntry::getNumPaletteIndicies() const
{
    return numPalettes;
};

int D3DEntry::getTexture() const
{
    return texture;
};

int D3DEntry::getPaletteIndex(int idx) const
{
    if(idx >= 0 && idx < numPalettes)
    return palettes[idx];
    return -1;
};

void D3DEntry::setTexture(int tex)
{
    int oldTex = texture;
    texture = tex;
    if(eventManager)
        eventManager->Raise(EVENT(IDriverD3DEvents::entryTextureChanged)(index,oldTex));
};

void D3DEntry::setPaletteIndex(int idx, int pal)
{
    if(idx >= 0 && idx < numPalettes)
    {
        palettes[idx] = pal;
        if(eventManager)
            eventManager->Raise(EVENT(IDriverD3DEvents::entryIndexChanged)(index,idx));
    }
};

void D3DEntry::removePaletteIndex(int idx)
{
    if(idx >= 0 && idx < numPalettes)
    {
        int* temp = NULL;
        if(numPalettes > 0)
        {
            temp = new int[numPalettes-1];
            memcpy(temp,palettes,idx*sizeof(int));
            memcpy(&temp[idx],&palettes[idx+1],(numPalettes-idx-1)*sizeof(int));
        }
        if(palettes)
        delete[] palettes;
        palettes = temp;
        numPalettes--;
        if(eventManager)
            eventManager->Raise(EVENT(IDriverD3DEvents::entryIndexRemoved)(index, idx));
    }
};

void D3DEntry::insertPaletteIndex(int idx,int pal)
{
    if(idx >= 0 && idx <= numPalettes)
    {
        int* temp = new int[numPalettes+1];
        if(palettes)
        {
            memcpy(temp,palettes,idx*sizeof(int));

            if(idx < numPalettes)
            memcpy(&temp[idx+1],&palettes[idx],(numPalettes-idx)*sizeof(int));

            delete[] palettes;
        }
        temp[idx] = pal;
        palettes = temp;
        numPalettes++;
        if(eventManager)
            eventManager->Raise(EVENT(IDriverD3DEvents::entryIndexInserted)(index, idx));
    }
};

void D3DEntry::addPaletteIndex(int pal)
{
    insertPaletteIndex(getNumPaletteIndicies(),pal);
};

int D3DEntry::loadFromFile(FILE* file,int size)
{
    return load((IOHandle)(file),&fileCallbacks,size);
};

int D3DEntry::load(IOHandle handle, IOCallbacks* callbacks, int size)
{
    cleanup();
    if(!handle)
    return -1;

    callbacks->read(&texture,4,1,handle);
    callbacks->read(&numPalettes,4,1,handle);

    if(size >= 0 && size < 8+numPalettes*4)
    {
        numPalettes = 0;
        return -2;
    }

    palettes = new int[numPalettes];
    callbacks->read(palettes,4,numPalettes,handle);

    return 8+numPalettes*4;
};

int D3DEntry::saveToFile(FILE* file) const
{
    return save((IOHandle)(file), &fileCallbacks);
};

int D3DEntry::getRequiredSize() const
{
    return 8+numPalettes*4;
};

int D3DEntry::save(IOHandle handle, IOCallbacks* callbacks) const
{
    if(!handle)
    return -1;

    callbacks->write(&texture,4,1,handle);
    callbacks->write(&numPalettes,4,1,handle);
    callbacks->write(palettes,4,numPalettes,handle);

    return getRequiredSize();
};

DriverD3D::DriverD3D()
{
    numEntries = 0;
    entries = NULL;
};

DriverD3D::~DriverD3D()
{
    cleanup();

    //Raise destroyed event so handlers can clean up dangling pointers.
    eventManager.Raise(EVENT(IDriverD3DEvents::D3DDestroyed)());
};

void DriverD3D::cleanup()
{
    //Raise D3D about to be reset event.
    eventManager.Raise(EVENT(IDriverD3DEvents::D3DReset)(true));

    if(entries)
    {
        for(int i = 0; i < numEntries; i++)
        {
            if(entries[i])
                delete entries[i];
        }
        delete[] entries;
    }
    entries = NULL;
    numEntries = 0;

    //Raise D3D has been reset event.
    eventManager.Raise(EVENT(IDriverD3DEvents::D3DReset)(false));
};

int DriverD3D::getNumEntries() const
{
    return numEntries;
};

D3DEntry* DriverD3D::getEntry(int idx) const
{
    if(idx >= 0 && idx < numEntries)
    return entries[idx];
    return NULL;
};

int DriverD3D::findTexture(int tex) const
{
    for(int i = 0; i < numEntries; i++)
    {
        if(entries[i]->getTexture() == tex)
        return i;
    }
    return -1;
};

D3DEntry* DriverD3D::getTextureEntry(int tex) const
{
    return getEntry(findTexture(tex));
};

void DriverD3D::removeEntry(int idx)
{
    if(idx >= 0 && idx < numEntries)
    {
        int texture = entries[idx]->getTexture();
        D3DEntry** temp = NULL;
        if(numEntries > 1)
        {
            temp = new D3DEntry*[numEntries-1];
            memcpy(temp,entries,idx*sizeof(D3DEntry*));
            memcpy(&temp[idx],&entries[idx+1],(numEntries-idx-1)*sizeof(D3DEntry*));
            for(int i = idx; i < numEntries-1; i++)
                temp[i]->index--; //Update index passed back in events.
        }
        delete entries[idx];
        delete[] entries;
        entries = temp;
        numEntries--;
        //Raise entry removed event.
        eventManager.Raise(EVENT(IDriverD3DEvents::entryRemoved)(idx, texture));
    }
};

void DriverD3D::removeTextureEntry(int tex)
{
    removeEntry(findTexture(tex));
};

void DriverD3D::addEntry(int tex)
{
    if(findTexture(tex) != -1)
    return;

    int index = 0;

    for(int i = 0; i < numEntries; i++)
    {
        if(entries[i]->getTexture() > tex)
        break;
        index = i;
    }

    D3DEntry** temp = new D3DEntry*[numEntries+1];
    if(numEntries > 0)
    {
        memcpy(temp,entries,index*sizeof(D3DEntry*));
        memcpy(&temp[index+1],&entries[index],(numEntries-index)*sizeof(D3DEntry*));

        for(int i = index; i < numEntries; i++)
            temp[i+1]->index++; //Update the index passed back for events
        delete[] entries;
    }
    entries = temp;
    entries[index] = new D3DEntry(&eventManager,index);
    entries[index]->setTexture(tex);
    numEntries++;

    //Raise entry inserted event.
    eventManager.Raise(EVENT(IDriverD3DEvents::entryInserted)(index));
};

int DriverD3D::loadFromFile(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return -1;
    int ret = loadFromFile(file);
    fclose(file);
    return ret;
};

int DriverD3D::loadFromFile(FILE* file)
{
    return load((IOHandle)file, &fileCallbacks);
};

int DriverD3D::load(IOHandle handle, IOCallbacks* callbacks)
{
    cleanup();
    if(!handle)
    return -1;

    unsigned long int start,end;

    start = callbacks->tell(handle);
    callbacks->seek(handle,0,SEEK_END);
    end = callbacks->tell(handle);
    callbacks->seek(handle,start,SEEK_SET);

    callbacks->read(&numEntries,4,1,handle);

    if(numEntries < 0 || (unsigned int)(12*numEntries+4) > end-start)
    {
        numEntries = 0;
        return -2;
    }

    unsigned long current = start+4;
    unsigned int totalRead = 4;

    if(numEntries > 0)
    {
        entries = new D3DEntry*[numEntries];
        memset(entries, 0, numEntries*sizeof(D3DEntry*));
        for(int i = 0; i < numEntries; i++)
        {
            entries[i] = new D3DEntry(&eventManager,i);
            int ret = entries[i]->load(handle,callbacks,end-current);
            if(ret < 0)
            {
                cleanup();
                return -2;
            }
            current += ret;
            totalRead += ret;
        }
    }

    //Raise d3d opened event.
    eventManager.Raise(EVENT(IDriverD3DEvents::D3DOpened)());
    return totalRead;
};

int DriverD3D::getRequiredSize() const
{
    int size = 4;
    for(int i = 0; i < numEntries; i++)
    {
        size += entries[i]->getRequiredSize();
    }
    return size;
};

int DriverD3D::saveToFile(const char* filename) const
{
    FILE* file = fopen(filename,"wb");
    if(!file)
    return -1;
    int ret = saveToFile(file);
    fclose(file);
    return ret;
};

int DriverD3D::saveToFile(FILE* file) const
{
    if(!file)
    return -1;

    return save((IOHandle)file, &fileCallbacks);
};

int DriverD3D::save(IOHandle handle, IOCallbacks* callbacks) const
{
    unsigned int totalWritten = 4;
    callbacks->write(&numEntries,4,1,handle);

    for(int i = 0; i < numEntries; i++)
    {
        totalWritten += entries[i]->save(handle,callbacks);
    }
    return totalWritten;
};

void DriverD3D::registerEventHandler(IDriverD3DEvents* handler)
{
    eventManager.Register(handler);
};

void DriverD3D::unregisterEventHandler(IDriverD3DEvents* handler)
{
    eventManager.Unregister(handler);
};
