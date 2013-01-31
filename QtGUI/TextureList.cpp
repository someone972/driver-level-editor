#include "TextureList.hpp"

TexEntry::TexEntry()
{
    memset(ids,0,sizeof(GLuint)*16);
    numPalettes = 0;
    currentPalette = 0;
    extendedIds = NULL;
};

TexEntry::~TexEntry()
{
    cleanup();
};

void TexEntry::cleanup()
{
    if(extendedIds)
    {
        delete[] extendedIds;
        extendedIds = NULL;
    }
    memset(ids,0,sizeof(GLuint)*16);
    numPalettes = 0;
    currentPalette = 0;
};

void TexEntry::set(GLuint texid)
{
    cleanup();
    numPalettes = 1;
    currentPalette = 0;
    ids[0] = texid;
};

void TexEntry::set(GLuint* texids,int num_ids)
{
    cleanup();
    if(texids != NULL && num_ids > 0)
    {
        numPalettes = num_ids;
        if(numPalettes > 16)
        {
            extendedIds = new GLuint[numPalettes-16];
            memcpy(ids,texids,sizeof(GLuint)*16);
            memcpy(extendedIds,&texids[16],sizeof(GLuint)*(numPalettes-16));
        }
        else
        {
            memcpy(ids,texids,sizeof(GLuint)*numPalettes);
        }
    }
};

GLuint TexEntry::getTexture(int palette) const
{
    if(numPalettes > 0)
    {
        if(palette < 0 || palette >= numPalettes)
        palette = 0;
        if(palette < 16)
        return ids[palette];
        else return extendedIds[palette-16];
    }
    return 0;
};

void TexEntry::setCurrentPalette(int palette)
{
    if(palette >= 0 && palette < numPalettes)
    {
        currentPalette = palette;
    }
};

int TexEntry::getCurrentPalette() const
{
    return currentPalette;
};

int TexEntry::getNumPalettes() const
{
    return numPalettes;
};

TextureList::TextureList()
{
    memset(list,0,sizeof(TexEntry*)*256);
};

TextureList::~TextureList()
{
    clearList();
};

void TextureList::clearList()
{
    for(int i = 0; i < 256; i++)
    {
        removeTexture(i);
    }
};

void TextureList::addTexture(int texnum,GLuint* texids,int numPalettes)
{
    if(texnum < 256 && texnum >= 0 && texids != NULL && numPalettes > 0)
    {
        if(list[texnum] == NULL)
        {
            list[texnum] = new TexEntry();
        }
        list[texnum]->set(texids,numPalettes);
    }
};

void TextureList::addTexture(int texnum,GLuint texid)
{
    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] == NULL)
        {
            list[texnum] = new TexEntry();
        }
        list[texnum]->set(texid);
    }
};

void TextureList::removeTexture(int texnum)
{
    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] != NULL)
        {
            delete list[texnum];
            list[texnum] = NULL;
        }
    }
};

bool TextureList::textureIsSet(int texnum) const
{
    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] != NULL)
        return true;
    }
    return false;
};

GLuint TextureList::getTexture(int texnum,int palette) const
{
    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] != NULL)
        return list[texnum]->getTexture(palette);
    }
    return 0;
};

void TextureList::setCurrentPalette(int texnum,int palette)
{

    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] != NULL)
        list[texnum]->setCurrentPalette(palette);
    }
};

int TextureList::getCurrentPalette(int texnum) const
{

    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] != NULL)
        return list[texnum]->getCurrentPalette();
    }
    return -1;
};

int TextureList::getNumPalettes(int texnum) const
{
    if(texnum < 256 && texnum >= 0)
    {
        if(list[texnum] != NULL)
        return list[texnum]->getNumPalettes();
    }
    return -1;
};

LevelTextures::LevelTextures() : TextureList()
{
    textures = NULL;
    d3d = NULL;
};

LevelTextures::~LevelTextures()
{
    if(d3d)
        d3d->unregisterEventHandler(this);
    if(textures)
        textures->unregisterEventHandler(this);
    cleanup();
};

void LevelTextures::setTextureProvider(DriverTextures* texs)
{
    if(textures)
        textures->unregisterEventHandler(this);
    textures = texs;
    if(textures)
        textures->registerEventHandler(this);
};

void LevelTextures::setD3D(DriverD3D* d)
{
    if(d3d)
        d3d->unregisterEventHandler(this);
    d3d = d;
    if(d3d)
        d3d->registerEventHandler(this);
};

void LevelTextures::cleanup()
{
    for(int i = 0; i < 256; i++)
    {
        unallocateTextures(i);
        removeTexture(i);
    }
};

void LevelTextures::unallocateTextures(int tex)
{
    if(textureIsSet(tex))
    {
        for(int i = 0; i < getNumPalettes(tex); i++)
        {
            GLuint temp = getTexture(tex,i);
            glDeleteTextures(1,&temp);
        }
    }
};

void LevelTextures::rebuildAllTextures()
{
    if(textures)
    {
        bool origState = signalsBlocked();
        blockSignals(true);

        unsigned char* buffer = new unsigned char[256*256*4];
        for(int i = 0; i < textures->getNumTextures(); i++)
        {
            rebuildTexture(i,buffer);
        }
        delete[] buffer;

        blockSignals(origState);
        emit listAltered();
    }
};

void LevelTextures::rebuildTexture(int texture,unsigned char* buffer)
{
    if(textures)
    {
        if(texture < textures->getNumTextures() && texture >= 0)
        {
            unallocateTextures(texture);
            removeTexture(texture);

            unsigned char* data;

            if(buffer == NULL)
            data = new unsigned char[256*256*4];
            else data = buffer;

            if(textures->getTexture(texture)->usesPalette())
            {
                int numPalettes = 1;
                DriverPalette* palette;
                DriverPalette temppal;
                for(int i = 0; i < 256; i++)
                {
                    temppal.colors[i].r = i;
                    temppal.colors[i].g = i;
                    temppal.colors[i].b = i;
                    temppal.colors[i].a = 0;
                }
                if(d3d)
                {
                    D3DEntry* entry = d3d->getTextureEntry(texture);
                    if(entry)
                    numPalettes = entry->getNumPaletteIndicies();
                    else numPalettes = 0;

                    if(numPalettes <= 0)
                    numPalettes = 1;
                }

                GLuint* texlist = new GLuint[numPalettes];
                glGenTextures(numPalettes, texlist);

                for(int i = 0; i < numPalettes; i++)
                {
                    if(d3d)
                    {
                        D3DEntry* entry = d3d->getTextureEntry(texture);
                        if(entry)
                        palette = textures->getIndexedPalette(entry->getPaletteIndex(i));
                        else palette = NULL;

                        if(!palette)
                        {
                            palette = &temppal;
                        }
                    }
                    else
                    {
                        if(textures->getNumPalettes() > 0)
                        {
                            palette = textures->getPalette(0);
                        }
                        else palette = &temppal;
                    }

                    const DriverTexture* tex = textures->getTexture(texture);
                    for(int k = 0; k < 0x10000; k++)
                    {
                        unsigned int index = (int)*(unsigned char*)(tex->getData()+k);
                        *(unsigned int*)(data+4*k) = palette->colors[index].r+(palette->colors[index].g<<8)+(palette->colors[index].b<<16);
                        if(tex->hasTransparency())
                        *(unsigned int*)(data+4*k)+=(palette->colors[index].a<<24);
                        else *(unsigned int*)(data+4*k)+=(0xFF<<24);
                    }

                    glBindTexture(GL_TEXTURE_2D, texlist[i]);
                    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
                    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
                    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                }
                addTexture(texture,texlist,numPalettes);
                delete[] texlist;
            }
            else
            {
                for(unsigned int j = 0; j < 0x10000; j++)
                {
                    unsigned short color = *(unsigned short*)(textures->getTexture(texture)->getData()+j*2);
                    if(color == 33825)
                    {
                        *(unsigned int*)(data+4*j) = 0;
                    }
                    else *(unsigned int*)(data+4*j) = (((color&0x7C00)>>10)*256/32)+((((color&0x03E0)>>5)*256/32)<<8)+(((color&0x001F)*256/32)<<16)+(0xFF<<24);
                }

                GLuint texture_num;
                glGenTextures( 1, &texture_num );
                glBindTexture( GL_TEXTURE_2D, texture_num );
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                addTexture(texture,texture_num);
            }

            if(buffer == NULL)
            delete[] data;
        }
    }
    emit listAltered();
};

void LevelTextures::D3DReset(bool aboutToBe)
{
    if(!aboutToBe)
    {
        cleanup();
        emit listAltered();
    }
};

void LevelTextures::D3DOpened()
{
    rebuildAllTextures();
};

void LevelTextures::entryRemoved(int idx, int old)
{
    rebuildTexture(old);
};

void LevelTextures::entryInserted(int idx)
{
    D3DEntry* entry = d3d->getEntry(idx);
    rebuildTexture(entry->getTexture());
};

void LevelTextures::entryIndexChanged(int entryIdx, int idx)
{
    D3DEntry* entry = d3d->getEntry(entryIdx);
    rebuildTexture(entry->getTexture());
};

void LevelTextures::entryIndexInserted(int entryIdx, int idx)
{
    D3DEntry* entry = d3d->getEntry(entryIdx);
    rebuildTexture(entry->getTexture());
};

void LevelTextures::entryIndexRemoved(int entryIdx, int idx)
{
    D3DEntry* entry = d3d->getEntry(entryIdx);
    rebuildTexture(entry->getTexture());
};

void LevelTextures::entryTextureChanged(int entryIdx, int oldtex)
{
    D3DEntry* entry = d3d->getEntry(entryIdx);
    rebuildTexture(entry->getTexture());
    rebuildTexture(oldtex);
};

void LevelTextures::D3DDestroyed()
{
    d3d = NULL;
    cleanup();
    emit listAltered();
};

void LevelTextures::texturesDestroyed()
{
    textures = NULL;
    cleanup();
    emit listAltered();
};

void LevelTextures::textureRemoved(int idx)
{
    unallocateTextures(idx);
    removeTexture(idx);
    //remove texture removes the entry, but does not move others.
    //therefore we must shift all textures down.
    for(int i = idx; i < 255; i++)
        list[i] = list[i+1];
    list[255] = NULL;
    emit listAltered();
};

void LevelTextures::textureMoved(int from, int to)
{
    TexEntry* old = list[from];
    if(from > to)
    {
        for(int i = from; i > to; i--)
        {
            list[i] = list[i-1];
        }
    }
    else
    {
        for(int i = from; i < to; i++)
        {
            list[i] = list[i+1];
        }
    }
    list[to] = old;
    emit listAltered();
};

void LevelTextures::textureInserted(int idx)
{
    if(list[255] != NULL)
        delete list[255];
    list[255] = NULL;

    for(int i = 255; i > idx; i--)
        list[i] = list[i-1];
    list[idx] = NULL;
    rebuildTexture(idx);
};

void LevelTextures::textureChanged(int idx)
{
    rebuildTexture(idx);
};
