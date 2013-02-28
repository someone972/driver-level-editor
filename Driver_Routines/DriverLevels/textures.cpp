#include "textures.hpp"

TextureDefinition::TextureDefinition()
{
    memset(name,0,9);
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    texture = 0;
};

TextureDefinition::TextureDefinition(unsigned char nx,unsigned char ny,unsigned char nw,unsigned char nh,const char* newName)
{
    memset(name,0,9);
    x = nx;
    y = ny;
    w = nw;
    h = nh;
    strncpy(name,newName,8);
};

void TextureDefinition::setName(const char* newName)
{
    strncpy(name,newName,8);
};

const char* TextureDefinition::getName()
{
    return name;
};

int TextureDefinition::getX()
{
    return x;
};

int TextureDefinition::getY()
{
    return y;
};

int TextureDefinition::getW()
{
    return w;
};

int TextureDefinition::getH()
{
    return h;
};

int TextureDefinition::getTexture()
{
    return texture;
};

void TextureDefinition::setX(unsigned int nx)
{
    if(nx > 255)
    nx = 255;
    x = nx;
};

void TextureDefinition::setY(unsigned int ny)
{
    if(ny > 255)
    ny = 255;
    y = ny;
};

void TextureDefinition::setW(unsigned int nw)
{
    if(nw > 255)
    nw = 255;
    w = nw;
};

void TextureDefinition::setH(unsigned int nh)
{
    if(nh > 255)
    nh = 255;
    h = nh;
};

void TextureDefinition::setTexture(unsigned int tex)
{
    if(tex > 255)
    tex = 255;
    texture = tex;
};

TextureDefinitions::TextureDefinitions()
{
    numTextureDefinitions = 0;
    definitions = NULL;
};

TextureDefinitions::~TextureDefinitions()
{
    cleanup();

    //Raise destroyed event so handlers can clean up dangling pointers.
    eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsDestroyed)());
};

void TextureDefinitions::registerEventHandler(IDriverTexDefEvents* handler)
{
    eventManager.Register(handler);
};

void TextureDefinitions::unregisterEventHandler(IDriverTexDefEvents* handler)
{
    eventManager.Unregister(handler);
};

void TextureDefinitions::cleanup()
{
    //Raise definitions about to be reset event.
    eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsReset)(true));

    if(definitions)
    delete[] definitions;
    definitions = NULL;
    numTextureDefinitions = 0;

    //Raise definitions have been reset event.
    eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsReset)(false));
};

int TextureDefinitions::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();

    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numTextureDefinitions,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d texture definitions.", numTextureDefinitions);

    if(size < 4+numTextureDefinitions*16)
    {
        numTextureDefinitions = 0;
        log->Log("ERROR: Required size for texture definitions exceeds block size!");
        return 2;
    }

    definitions = new TextureDefinition[numTextureDefinitions];

    log->increaseIndent();
    for(int i = 0; i < numTextureDefinitions; i++)
    {
        char tempname[9];
        tempname[8] = '\0';
        callbacks->read(tempname,8,1,handle);
        definitions[i].setName(tempname);

        callbacks->read(&definitions[i].texture,1,1,handle);
        callbacks->read(&definitions[i].x,1,1,handle);
        callbacks->read(&definitions[i].y,1,1,handle);
        callbacks->read(&definitions[i].w,1,1,handle);
        callbacks->read(&definitions[i].h,1,1,handle);
        callbacks->seek(handle,3,SEEK_CUR); //dword align
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: texture: %hhu x: %hhu y: %hhu w: %hhu h: %hhu name: %s",
                 i, definitions[i].texture, definitions[i].x, definitions[i].y, definitions[i].w, definitions[i].h, definitions[i].getName());
    }
    log->decreaseIndent();

    //Raise definitions opened event.
    eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsOpened)());
    return 0;
};

unsigned int TextureDefinitions::getRequiredSize()
{
    return 4+16*numTextureDefinitions;
};

int TextureDefinitions::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    //Raise definitions about to be saved event.
    eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsSaved)(true));

    callbacks->write(&numTextureDefinitions,4,1,handle);

    for(int i = 0; i < numTextureDefinitions; i++)
    {
        char tempname[8];
        memset(tempname,0,8);
        strncpy(tempname,definitions[i].getName(),8);

        callbacks->write(tempname,8,1,handle);
        callbacks->write(&definitions[i].texture,1,1,handle);
        callbacks->write(&definitions[i].x,1,1,handle);
        callbacks->write(&definitions[i].y,1,1,handle);
        callbacks->write(&definitions[i].w,1,1,handle);
        callbacks->write(&definitions[i].h,1,1,handle);
        unsigned char padding[3] = {0,0,0};
        callbacks->write(padding,3,1,handle);
    }

    //Raise definitions have been saved event.
    eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsSaved)(true));
    return 0;
};


int TextureDefinitions::getNumTextureDefinitions()
{
    return numTextureDefinitions;
};

TextureDefinition* TextureDefinitions::getTextureDefinition(int idx)
{
    if(idx >= 0 && idx < numTextureDefinitions)
    return &definitions[idx];
    return NULL;
};

int TextureDefinitions::findTextureDefByName(const char* name)
{
    for(int i = 0; i < numTextureDefinitions; i++)
    {
        if(strncmp(definitions[i].getName(),name,8) == 0)
        return i;
    }
    return -1;
};

void TextureDefinitions::setTextureDefinition(int idx, TextureDefinition def)
{
    if(idx >= 0 && idx < numTextureDefinitions)
    {
        definitions[idx] = def;

        //Raise definition changed event.
        eventManager.Raise(EVENT(IDriverTexDefEvents::definitionChanged)(idx));
    }
};

void TextureDefinitions::removeTextureDefinition(int idx)
{
    if(idx >= 0 && idx < numTextureDefinitions)
    {
        TextureDefinition* temp = new TextureDefinition[numTextureDefinitions-1];
        memcpy(temp,definitions,idx*sizeof(TextureDefinition));
        memcpy(&temp[idx],&definitions[idx+1],(numTextureDefinitions-(idx+1))*sizeof(TextureDefinition));
        delete[] definitions;
        definitions = temp;
        numTextureDefinitions--;

        //Raise definition removed event.
        eventManager.Raise(EVENT(IDriverTexDefEvents::definitionRemoved)(idx));
    }
};

bool TextureDefinitions::getTextureDefinition(int idx, TextureDefinition* in)
{
    if(!in || idx < 0 || idx >= numTextureDefinitions)
    return false;
    *in = definitions[idx];
    return true;
};

int TextureDefinitions::getSortedPosition(int idx)
{
    if(idx >= 0 && idx < numTextureDefinitions)
    {
        int i = 0;
        for(i = idx; i > 0 && definitions[i-1].texture > definitions[i].texture; i--);

        if(i == idx)
        {
            for(i = idx; i < numTextureDefinitions-1 && definitions[i+1].texture < definitions[i].texture; i++);
        }
        return i;
    }
    return -1;
};

int TextureDefinitions::resortTextureDefinition(int idx)
{
    if(idx >= 0 && idx < numTextureDefinitions)
    {
        int dest = getSortedPosition(idx);
        if(dest != idx)
        {
            if(dest > idx)
            {
                TextureDefinition temp = definitions[idx];
                memmove(&definitions[idx],&definitions[idx+1],(dest-idx)*sizeof(TextureDefinition));
                definitions[dest] = temp;
            }
            else
            {
                TextureDefinition temp = definitions[idx];
                memmove(&definitions[dest+1],&definitions[dest],(idx-dest)*sizeof(TextureDefinition));
                definitions[dest] = temp;
            }

            //Raise definition moved event.
            eventManager.Raise(EVENT(IDriverTexDefEvents::definitionMoved)(idx, dest));
        }
        return dest;
    }
    return -1;
};

void TextureDefinitions::insertTextureDefinition(int idx,TextureDefinition def)
{
    insertTextureDefinitions(idx,1,def);
};

void TextureDefinitions::insertTextureDefinitions(int idx,int count,TextureDefinition def)
{
    if(idx >= 0 && idx <= numTextureDefinitions)
    {
        TextureDefinition* temp = new TextureDefinition[numTextureDefinitions+count];
        if(definitions)
        {
            memcpy(temp,definitions,idx*sizeof(TextureDefinition));
            if(idx < numTextureDefinitions)
            memcpy(&temp[idx+count],&definitions[idx],(numTextureDefinitions-idx)*sizeof(TextureDefinition));
            delete[] definitions;
        }
        definitions = temp;
        numTextureDefinitions += count;
        for(int i = 0; i < count; i++)
        {
            definitions[idx+i] = def;
        }

        //Raise definition changed event.
        eventManager.Raise(EVENT(IDriverTexDefEvents::definitionsInserted)(idx,count));
    }

};

DriverPalette::DriverPalette()
{
    paletteNumber = 0;
};

DriverTexture::DriverTexture()
{
    flags = 0;
    carnum = -1;
    data = NULL;
};

DriverTexture::DriverTexture(unsigned short newFlags, short newCarnum)
{
    flags = newFlags;
    carnum = newCarnum;
    if(usesPalette())
    data = new unsigned char[256*256];
    else data = new unsigned char[256*256*2];
};

DriverTexture::DriverTexture(const DriverTexture& other)
{
    flags = other.flags;
    carnum = other.carnum;

    if(usesPalette())
    {
        data = new unsigned char[256*256];
        memcpy(data, other.getData(), 256*256*sizeof(unsigned char));
    }
    else
    {
        data = new unsigned char[256*256*2];
        memcpy(data, other.getData(), 256*256*2*sizeof(unsigned char));
    }
};

DriverTexture::~DriverTexture()
{
    cleanup();
};

void DriverTexture::cleanup()
{
    if(data)
    delete[] data;
    data = NULL;
    flags = 0;
    carnum = -1;
};

DriverTexture& DriverTexture::operator=(const DriverTexture& source)
{
    if(this == &source)
        return *this;

    cleanup();
    flags = source.flags;
    carnum = source.carnum;

    if(source.data != NULL)
    {
        if(source.usesPalette())
        {
            data = new unsigned char[256*256];
            memcpy(data, source.data, 256*256*sizeof(unsigned char));
        }
        else
        {
            data = new unsigned char[256*256*2];
            memcpy(data, source.data, 256*256*2*sizeof(unsigned char));
        }
    }
    return *this;
};

int DriverTexture::load(IOHandle handle, IOCallbacks* callbacks)
{
    cleanup();
    if(!handle || !callbacks)
    return -1;

    callbacks->read(&flags,2,1,handle);
    callbacks->read(&carnum,2,1,handle);
    if(usesPalette())
    {
        data = new unsigned char[256*256];
        callbacks->read(data,1,256*256,handle);
    }
    else
    {
        data = new unsigned char[256*256*2];
        callbacks->read(data,1,2*256*256,handle);
    }
    return getRequiredSize();
};

void DriverTexture::setTruecolor(bool isTruecolor)
{
    if(isTruecolor && usesPalette())
    {
        if(data)
        delete[] data;
        data = new unsigned char[256*256*2];
        flags &= ~16;
    }
    else if(!isTruecolor && !usesPalette())
    {
        if(data)
        delete[] data;
        data = new unsigned char[256*256];
        flags |= 16;
    }
    if(!data)
    {
        if(usesPalette())
        data = new unsigned char[256*256];
        else data = new unsigned char[256*256*2];
    }
};

void DriverTexture::setCarNumber(short newCarnum)
{
    carnum = newCarnum;
};

void DriverTexture::setFlags(unsigned short newFlags)
{
    bool usedPalette = usesPalette();
    flags = newFlags;
    if(data)
    {
        if(usedPalette != usesPalette())
        {
            delete[] data;
            data = NULL;
        }
    }
    if(!data)
    {
        if(usesPalette())
        data = new unsigned char[256*256];
        else data = new unsigned char[256*256*2];
    }
};

unsigned int DriverTexture::getRequiredSize()
{
    if(usesPalette())
    return 4+256*256;
    return 4+256*256*2;
};

int DriverTexture::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&flags,2,1,handle);
    callbacks->write(&carnum,2,1,handle);

    if(usesPalette())
    {
        if(data)
        {
            callbacks->write(data,256*256,1,handle);
        }
        else
        {
            data = new unsigned char[256*256];
            memset(data,0,256*256);
            callbacks->write(data,256*256,1,handle);
            delete[] data;
            data = NULL;
        }
    }
    else
    {
        if(data)
        {
            callbacks->write(data,256*256*2,1,handle);
        }
        else
        {
            data = new unsigned char[256*256*2];
            memset(data,0,256*256*2);
            callbacks->write(data,256*256*2,1,handle);
            delete[] data;
            data = NULL;
        }
    }
    return 0;
};

bool DriverTexture::usesPalette() const
{
    return flags&TEX_USES_PALETTE;
};

bool DriverTexture::hasTransparency() const
{
    return flags&TEX_HAS_TRANSPARENCY;
};

bool DriverTexture::isCleanTexture() const
{
    return flags&TEX_IS_CLEAN_TEXTURE;
};

bool DriverTexture::isDamageTexture() const
{
    return flags&TEX_IS_DAMAGED_TEXTURE;
};

bool DriverTexture::loadsToRam() const
{
    return flags&TEX_LOADS_TO_RAM;
};

int DriverTexture::getCarNumber() const
{
    return carnum;
};

unsigned short DriverTexture::getFlags() const
{
    return flags;
};

const unsigned char* DriverTexture::getData() const
{
    return data;
};

const unsigned char* DriverTexture::getScanLine(int i) const
{
    if(i < 0 || i > 255)
    return NULL;

    if(usesPalette())
    return data+256*i;
    return data+512*i;
};

void DriverTexture::setData(const unsigned char* memory)
{
    if(!memory)
    return;

    if(!data)
    {
        if(usesPalette())
        data = new unsigned char[256*256];
        else data = new unsigned char[256*256*2];
    }

    memcpy(data,memory,(usesPalette() ? 256*256 : 256*256*2));
};

void DriverTexture::setScanLine(int y, const unsigned char* scanLineData)
{
    if(!scanLineData || y < 0 || y > 255)
    return;

    if(!data)
    {
        if(usesPalette())
        data = new unsigned char[256*256];
        else data = new unsigned char[256*256*2];
    }

    if(usesPalette())
    memcpy(data+256*y,scanLineData,256);
    else memcpy(data+256*2*y,scanLineData,256*2);
};

DriverTextures::DriverTextures()
{
    numTextures = 0;
    textures = NULL;
    numPalettes = 0;
    palettes = NULL;
    paletteIndexSize = 0;
    paletteIndex = NULL;
};

DriverTextures::~DriverTextures()
{
    cleanup();

    //Raise destroyed event so handlers can clean up dangling pointers.
    eventManager.Raise(EVENT(IDriverTextureEvents::texturesDestroyed)());
};

void DriverTextures::registerEventHandler(IDriverTextureEvents* handler)
{
    eventManager.Register(handler);
};

void DriverTextures::unregisterEventHandler(IDriverTextureEvents* handler)
{
    eventManager.Unregister(handler);
};

void DriverTextures::cleanup()
{
    //Raise textures about to be reset event.
    eventManager.Raise(EVENT(IDriverTextureEvents::texturesReset)(true));
    if(textures)
    {
        for(int i = 0; i < numTextures; i++)
        {
            delete textures[i];
        }
        delete[] textures;
    }
    textures = NULL;
    numTextures = 0;

    if(palettes)
    {
        for(int i = 0; i < numPalettes; i++)
        {
            delete palettes[i];
        }
        delete[] palettes;
    }
    palettes = NULL;
    numPalettes = 0;

    if(paletteIndex)
    delete[] paletteIndex;
    paletteIndex = NULL;
    paletteIndexSize = 0;

    //Raise textures have been reset event.
    eventManager.Raise(EVENT(IDriverTextureEvents::texturesReset)(false));
};

int DriverTextures::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();

    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numPalettes,2,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %hd palettes.", numPalettes);

    if(size < 2+numPalettes*1026)
    {
        numPalettes = 0;
        log->Log("ERROR: Required size for textures/palettes exceeds block size.");
        return 2;
    }
    size -= 2+numPalettes*1026;

    palettes = new DriverPalette*[numPalettes];

    int greatestSlot = 0;

    log->increaseIndent();
    for(int i = 0; i < numPalettes; i++)
    {
        palettes[i] = new DriverPalette();

        callbacks->read(&palettes[i]->paletteNumber,2,1,handle);

        if(palettes[i]->paletteNumber > greatestSlot)
        greatestSlot = palettes[i]->paletteNumber;

        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: slot: %hd", i, palettes[i]->paletteNumber);

        for(int j = 0; j < 256; j++)
        {
            callbacks->read(&palettes[i]->colors[j].b,1,1,handle);
            callbacks->read(&palettes[i]->colors[j].g,1,1,handle);
            callbacks->read(&palettes[i]->colors[j].r,1,1,handle);
            callbacks->read(&palettes[i]->colors[j].a,1,1,handle);
        }
    }
    log->decreaseIndent();

    paletteIndexSize = greatestSlot+1;
    paletteIndex = new int[paletteIndexSize];
    memset(paletteIndex,0xFF,sizeof(int)*paletteIndexSize); //set all slots to -1
    for(int i = 0; i < numPalettes; i++)
    {
        //TODO: bounds check
        paletteIndex[palettes[i]->paletteNumber] = i;
    }

    callbacks->read(&numTextures,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d textures.", numTextures);

    textures = new DriverTexture*[numTextures];
    for(int i = 0; i < numTextures; i++)
    {
        textures[i] = new DriverTexture();
    }

    log->increaseIndent();
    for(int i = 0; i < numTextures; i++)
    {
        size -= textures[i]->load(handle,callbacks);

        if(size < 0)
        {
            cleanup();
            log->decreaseIndent();
            log->Log("ERROR: Required size for textures/palettes exceeds block size.");
            return 2;
        }
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: flags: %hx carNumber: %hd", i, textures[i]->getFlags(), textures[i]->getCarNumber());
    }
    log->decreaseIndent();

    //Raise textures opened event.
    eventManager.Raise(EVENT(IDriverTextureEvents::texturesOpened)());
    return 0;
};

unsigned int DriverTextures::getRequiredSize()
{
    unsigned int palettePart = 2+1026*numPalettes;
    unsigned int texturePart = 4;
    for(int i = 0; i < numTextures; i++)
    {
        texturePart += textures[i]->getRequiredSize();
    }
    return palettePart+texturePart;
};

int DriverTextures::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    //Raise textures about to be saved event.
    eventManager.Raise(EVENT(IDriverTextureEvents::texturesSaved)(true));

    callbacks->write(&numPalettes,2,1,handle);

    for(int i = 0; i < numPalettes; i++)
    {
        callbacks->write(&palettes[i]->paletteNumber,2,1,handle);

        for(int j = 0; j < 256; j++)
        {
            callbacks->write(&palettes[i]->colors[j].b,1,1,handle);
            callbacks->write(&palettes[i]->colors[j].g,1,1,handle);
            callbacks->write(&palettes[i]->colors[j].r,1,1,handle);
            callbacks->write(&palettes[i]->colors[j].a,1,1,handle);
        }
    }

    callbacks->write(&numTextures,4,1,handle);

    for(int i = 0; i < numTextures; i++)
    {
        textures[i]->save(handle,callbacks);
    }

    //Raise textures have been saved event.
    eventManager.Raise(EVENT(IDriverTextureEvents::texturesSaved)(false));
    return 0;
};

int DriverTextures::getNumTextures()
{
    return numTextures;
};

const DriverTexture* DriverTextures::getTexture(int idx)
{
    if(idx >= 0 && idx < numTextures)
    return textures[idx];
    return NULL;
};

void DriverTextures::setTexture(int idx, const DriverTexture* tex)
{
    if(tex && idx >= 0 && idx < numTextures)
    {
        *textures[idx] = *tex; //assignment operator does a deep copy.
        eventManager.Raise(EVENT(IDriverTextureEvents::textureChanged)(idx));
    }
};

void DriverTextures::moveTexture(int from, int to)
{
    if(from >= 0 && from < numTextures && to >= 0 && to < numTextures && from != to)
    {
        DriverTexture* tex = textures[from];
        if(from < to)
        memmove(&textures[from], &textures[from+1], (to-from)*sizeof(DriverTexture*));
        else memmove(&textures[to+1], &textures[to], (from-to)*sizeof(DriverTexture*));
        textures[to] = tex;
        eventManager.Raise(EVENT(IDriverTextureEvents::textureMoved)(from,to));
    }
};

void DriverTextures::removeTexture(int idx)
{
    if(idx >= 0 && idx < numTextures)
    {
        delete textures[idx];
        DriverTexture** temp = NULL;
        if(numTextures > 1)
        {
            temp = new DriverTexture*[numTextures-1];
            memcpy(temp,textures,sizeof(DriverTexture*)*idx);
            memcpy(&temp[idx],&textures[idx+1],sizeof(DriverTexture*)*(numTextures-(idx+1)));
        }
        delete[] textures;
        numTextures--;
        textures = temp;
        eventManager.Raise(EVENT(IDriverTextureEvents::textureRemoved)(idx));
    }
};

void DriverTextures::insertTexture(int idx, const DriverTexture* tex)
{
    if(idx >= 0 && idx <= numTextures)
    {
        DriverTexture** temp = new DriverTexture*[numTextures+1];
        if(textures)
        {
            memcpy(temp,textures,sizeof(DriverTexture*)*idx);
            memcpy(&temp[idx+1],&textures[idx],sizeof(DriverTexture*)*(numTextures-idx));
            delete[] textures;
        }
        textures = temp;
        textures[idx] = new DriverTexture(*tex);
        numTextures++;
        eventManager.Raise(EVENT(IDriverTextureEvents::textureInserted)(idx));
    }
};

void DriverTextures::addTexture(const DriverTexture* tex)
{
    insertTexture(numTextures,tex);
};

void DriverTextures::addTexture(unsigned short flags, short carnum)
{
    DriverTexture** temp = new DriverTexture*[numTextures+1];
    if(textures)
    {
        memcpy(temp,textures,sizeof(DriverTexture*)*numTextures);
        delete[] textures;
    }
    textures = temp;
    textures[numTextures] = new DriverTexture(flags,carnum);
    numTextures++;
    //TODO: update this func
};

short DriverTextures::getNumPalettes()
{
    return numPalettes;
};

DriverPalette* DriverTextures::getPalette(int idx)
{
    if(idx >= 0 && idx < numPalettes)
    return palettes[idx];
    return NULL;
};

DriverPalette* DriverTextures::getIndexedPalette(int idx)
{
    if(idx >= 0 && idx < paletteIndexSize)
    {
        if(paletteIndex[idx] >= 0)
        return palettes[paletteIndex[idx]];
    }
    return NULL;
};

void DriverTextures::setPalette(int idx, DriverPalette* palette)
{
    if(idx >= 0 && idx < numPalettes)
    {
        *palettes[idx] = *palette;

        //Raise palette changed event.
        eventManager.Raise(EVENT(IDriverTextureEvents::paletteChanged)(idx));
    }
};

void DriverTextures::setPaletteIndexed(DriverPalette* palette)
{
    int slot = palette->paletteNumber;
    bool isNew = false;
    if(slot < 0)
    slot = 0;

    if(slot >= paletteIndexSize)
    {
        int* temp = new int[slot+1];
        if(paletteIndex)
        {
            memcpy(temp,paletteIndex,sizeof(int)*paletteIndexSize);
            delete[] paletteIndex;
        }
        memset(&temp[paletteIndexSize],0xFF,sizeof(int)*((slot+1)-paletteIndexSize));
        paletteIndex = temp;
    }
    if(paletteIndex[slot] == -1)
    {
        DriverPalette** temp = new DriverPalette*[numPalettes+1];
        if(palettes)
        {
            memcpy(temp,palettes,sizeof(DriverPalette*)*numPalettes);
            delete[] palettes;
        }
        palettes = temp;
        palettes[numPalettes] = new DriverPalette();
        paletteIndex[slot] = numPalettes;
        numPalettes++;
        isNew = true;
    }
    *palettes[paletteIndex[slot]] = *palette;

    //Raise palette inserted event.
    if(isNew)
    eventManager.Raise(EVENT(IDriverTextureEvents::paletteInserted)(numPalettes-1));
};

int DriverTextures::getNextOpenSlot()
{
    //TODO: Find out if we need to start searching at index 2, skipping slots for tsd palettes
    for(int i = 0; i < paletteIndexSize; i++)
    {
        if(paletteIndex[i] == -1)
        return i;
    }
    return paletteIndexSize;
};

void DriverTextures::removePalette(int idx)
{
    if(idx >= 0 && idx < numPalettes)
    {
        for(int i = 0; i < paletteIndexSize; i++)
        {
            if(paletteIndex[i] == idx)
            paletteIndex[i] = -1;
        }
        DriverPalette** temp = new DriverPalette*[numPalettes-1];
        memcpy(temp,palettes,sizeof(DriverPalette*)*idx);
        memcpy(&temp[idx],&palettes[idx+1],sizeof(DriverPalette*)*(numPalettes-idx));
        delete[] palettes;
        numPalettes--;
        palettes = temp;

        //Raise palette removed event.
        eventManager.Raise(EVENT(IDriverTextureEvents::paletteRemoved)(idx));
    }
};

void DriverTextures::removeIndexedPalette(int idx)
{
    if(idx >= 0 && idx < paletteIndexSize)
    removePalette(paletteIndex[idx]);
};
