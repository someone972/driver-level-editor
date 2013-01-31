#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <iostream>
#include <cstring>
#include <cstdio>
#include "../ioFuncs.hpp"
#include "../../EventMgr.hpp"

using namespace std;

class TextureDefinition
{
    public:
        TextureDefinition();
        TextureDefinition(unsigned char nx,unsigned char ny,unsigned char nw,unsigned char nh,const char* newName);
        void setName(const char* newName);
        const char* getName();
        int getX();
        int getY();
        int getW();
        int getH();
        int getTexture();
        void setX(unsigned int x);
        void setY(unsigned int y);
        void setW(unsigned int w);
        void setH(unsigned int h);
        void setTexture(unsigned int texture);

        //no harm in exposing these, they can't be overflowed under normal usage.
        unsigned char x,y,w,h,texture;
    protected:
        //This could easily be overflowed, so we hide it from the user to keep it safe.
        char name[9];
};

class IDriverTexDefEvents
{
        DECLARE_EVENTS(IDriverTexDefEvents);
    public:
        DEFINE_EVENT0(IDriverTexDefEvents, definitionsDestroyed);
        DEFINE_EVENT1(IDriverTexDefEvents, definitionsReset, bool /*aboutToBe*/);
        DEFINE_EVENT0(IDriverTexDefEvents, definitionsOpened);
        DEFINE_EVENT1(IDriverTexDefEvents, definitionsSaved, bool /*aboutToBe*/);
        DEFINE_EVENT2(IDriverTexDefEvents, definitionMoved, int /*fromIdx*/, int /*toIdx*/);
        DEFINE_EVENT1(IDriverTexDefEvents, definitionRemoved, int /*whichIdx*/);
        DEFINE_EVENT2(IDriverTexDefEvents, definitionsInserted, int /*whereIdx*/, int /*count*/);
        DEFINE_EVENT1(IDriverTexDefEvents, definitionChanged, int /*whichIdx*/);
};
IMPLEMENT_EVENTS(IDriverTexDefEvents);

class TextureDefinitions
{
    public:
        TextureDefinitions();
        ~TextureDefinitions();

        void registerEventHandler(IDriverTexDefEvents* handler);
        void unregisterEventHandler(IDriverTexDefEvents* handler);

        void cleanup();
        int load(IOHandle handle, IOCallbacks* callbacks, int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumTextureDefinitions();
        TextureDefinition* getTextureDefinition(int idx);
        bool getTextureDefinition(int idx,TextureDefinition* in);
        int findTextureDefByName(const char* name);
        int getSortedPosition(int idx);
        int resortTextureDefinition(int idx);
        void setTextureDefinition(int idx,TextureDefinition def);
        void insertTextureDefinition(int idx,TextureDefinition def = TextureDefinition());
        void insertTextureDefinitions(int idx,int count,TextureDefinition def = TextureDefinition());
        void removeTextureDefinition(int idx);
    protected:
        CEventMgr<IDriverTexDefEvents> eventManager;
        int numTextureDefinitions;
        TextureDefinition* definitions;
};

struct color_4ub
{
    unsigned char r,g,b,a;
};

class DriverPalette
{
    public:
        DriverPalette();

        short paletteNumber;
        color_4ub colors[256];
};

const unsigned int TEX_HAS_TRANSPARENCY = 1;
const unsigned int TEX_LOADS_TO_RAM = 2;
const unsigned int TEX_IS_CLEAN_TEXTURE = 4;
const unsigned int TEX_IS_DAMAGED_TEXTURE = 8;
const unsigned int TEX_USES_PALETTE = 16;
//There is also a flag at 64 which alternates between paletted textures (sometimes set on the first, sometimes on the second)
//which does not appear to be used.

class DriverTexture
{
    public:
        DriverTexture();
        DriverTexture(unsigned short newFlags, short newCarnum);
        DriverTexture(const DriverTexture& other);
        ~DriverTexture();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        void setTruecolor(bool isTruecolor);
        void setCarNumber(short newCarnum);
        void setFlags(unsigned short flags);

        bool usesPalette() const;
        bool hasTransparency() const;
        bool loadsToRam() const;
        bool isCleanTexture() const;
        bool isDamageTexture() const;
        int getCarNumber() const;
        unsigned short getFlags() const;

        const unsigned char* getData() const;
        const unsigned char* getScanLine(int line) const;

        void setData(const unsigned char* memory);
        void setScanLine(int y, const unsigned char* scanLineData);

        DriverTexture& operator=(const DriverTexture& source);

    protected:
        unsigned short flags;
        short carnum;
        unsigned char* data;
};

class IDriverTextureEvents
{
        DECLARE_EVENTS(IDriverTextureEvents);
    public:
        DEFINE_EVENT0(IDriverTextureEvents, texturesDestroyed);
        DEFINE_EVENT1(IDriverTextureEvents, texturesReset, bool /*aboutToBe*/); //Includes palettes reset
        DEFINE_EVENT0(IDriverTextureEvents, texturesOpened);                    //Includes palettes
        DEFINE_EVENT1(IDriverTextureEvents, texturesSaved, bool /*aboutToBe*/); //Includes palettes

        DEFINE_EVENT1(IDriverTextureEvents, textureInserted, int /*idx*/);
        DEFINE_EVENT1(IDriverTextureEvents, textureRemoved, int /*idx*/);
        DEFINE_EVENT2(IDriverTextureEvents, textureMoved, int /*from*/, int /*to*/);
        DEFINE_EVENT1(IDriverTextureEvents, textureChanged, int /*idx*/);

        DEFINE_EVENT1(IDriverTextureEvents, paletteInserted, int /*idx*/);
        DEFINE_EVENT1(IDriverTextureEvents, paletteRemoved, int /*idx*/);
        DEFINE_EVENT1(IDriverTextureEvents, paletteChanged, int /*idx*/);
};
IMPLEMENT_EVENTS(IDriverTextureEvents);

class DriverTextures
{
    public:
        DriverTextures();
        ~DriverTextures();
        void cleanup();

        void registerEventHandler(IDriverTextureEvents* handler);
        void unregisterEventHandler(IDriverTextureEvents* handler);

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumTextures();
        const DriverTexture* getTexture(int idx);
        void setTexture(int idx, const DriverTexture* tex);
        void removeTexture(int idx);
        void moveTexture(int from, int to);
        void addTexture(unsigned short flags, short carnum);
        void insertTexture(int idx, const DriverTexture* tex);
        void addTexture(const DriverTexture* tex);

        short getNumPalettes();
        DriverPalette* getPalette(int idx);
        DriverPalette* getIndexedPalette(int idx);
        void removePalette(int idx);
        void removeIndexedPalette(int idx);
        void setPalette(int idx, DriverPalette* palette);
        void setPaletteIndexed(DriverPalette* palette);

        int getNextOpenSlot();

    protected:
        CEventMgr<IDriverTextureEvents> eventManager;
        DriverTexture** textures;
        DriverPalette** palettes;
        int* paletteIndex;
        int paletteIndexSize;
        int numTextures;
        short numPalettes;
};

#endif
