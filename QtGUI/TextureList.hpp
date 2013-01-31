#ifndef TEXTURE_LIST_HPP
#define TEXTURE_LIST_HPP

#include "../Driver_Routines/driver_levels.hpp"
#include "../Driver_Routines/driver_d3d.hpp"
#include <gl/GL.h>
#include <QtGUI>

class TexEntry
{
    public:
        TexEntry();
        ~TexEntry();
        void set(GLuint texid);
        void set(GLuint* texids,int num_ids);
        GLuint getTexture(int palette = -1) const;
        void setCurrentPalette(int palette);
        int getCurrentPalette() const;
        int getNumPalettes() const;
        void cleanup();

    protected:
        int numPalettes;
        int currentPalette;
        GLuint ids[16];
        GLuint* extendedIds;
};

class TextureList : public QObject
{
    Q_OBJECT

    public:
        TextureList();
        ~TextureList();
        void clearList();
        void addTexture(int texnum,GLuint* texids,int num_palettes);
        void addTexture(int texnum,GLuint texid);
        void removeTexture(int texnum);
        bool textureIsSet(int texnum) const;
        GLuint getTexture(int texnum,int palette = -1) const;
        void setCurrentPalette(int texnum,int palette);
        int getCurrentPalette(int texnum) const;
        int getNumPalettes(int texnum) const;

    signals:
        void listAltered();

    protected:
        TexEntry* list[256];
};

class LevelTextures : public TextureList, IDriverD3DEvents, IDriverTextureEvents
{
    Q_OBJECT

    public:
        LevelTextures();
        ~LevelTextures();
        void cleanup();
        void setTextureProvider(DriverTextures* texs);
        void setD3D(DriverD3D* d);
        void rebuildTexture(int texture,unsigned char* buffer = NULL);
        void rebuildAllTextures();
        void unallocateTextures(int texture);

        //Event handlers
        void textureRemoved(int idx);
        void textureChanged(int idx);
        void textureInserted(int idx);
        void textureMoved(int from, int to);
        void texturesDestroyed();
        void D3DReset(bool aboutToBe);
        void D3DOpened();
        void D3DDestroyed();
        void entryRemoved(int idx, int tex);
        void entryInserted(int idx);
        void entryTextureChanged(int idx, int old);
        void entryIndexChanged(int entryIdx, int idx);
        void entryIndexInserted(int entryIdx, int idx);
        void entryIndexRemoved(int entryIdx, int idx);

    protected:
        DriverTextures* textures;
        DriverD3D* d3d;
};

#endif
