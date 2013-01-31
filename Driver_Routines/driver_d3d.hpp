#ifndef DRIVER_D3D
#define DRIVER_D3D

#include <cstdio>
#include <cstring>
#include "ioFuncs.hpp"
#include "../EventMgr.hpp"

/*! \file driver_d3d.hpp
 * API for loading D3D files for the game Driver: You are the Wheelman.
 * D3D files are used to specify which palettes are used by which textures. This api can be used to safely and easily load, manipulate, and save such files.
 */

/*! \brief Contains a list of palette indicies and the texture number they correspond to.
 *
 * This describes which palettes a texture uses. This will not change which palette is used for a certain model,
 * but it allows for changing of the palette index so that new textures can be imported. This must be updated when
 * a texture gets a new palette, or the game will continue to use the previous palette index.
 */

class IDriverD3DEvents
{
        DECLARE_EVENTS(IDriverD3DEvents);
    public:
        DEFINE_EVENT0(IDriverD3DEvents, D3DDestroyed);
        DEFINE_EVENT0(IDriverD3DEvents, D3DOpened);
        DEFINE_EVENT1(IDriverD3DEvents, D3DReset, bool /*aboutToBe*/);
        DEFINE_EVENT1(IDriverD3DEvents, entryInserted, int /*idx*/);
        DEFINE_EVENT2(IDriverD3DEvents, entryRemoved, int /*idx*/, int /*texture*/);

        DEFINE_EVENT2(IDriverD3DEvents, entryTextureChanged, int /*entryIdx*/, int /*oldTex*/);
        DEFINE_EVENT2(IDriverD3DEvents, entryIndexChanged, int /*entryIdx*/, int /*index*/);
        DEFINE_EVENT2(IDriverD3DEvents, entryIndexInserted, int /*entryIdx*/, int /*index*/);
        DEFINE_EVENT2(IDriverD3DEvents, entryIndexRemoved, int /*entryIdx*/, int /*index*/);
};
IMPLEMENT_EVENTS(IDriverD3DEvents);

class DriverD3D;

class D3DEntry
{
    public:
        friend class DriverD3D; //Friends with DriverD3D so it can pass its event handler to us, but others can't.

        D3DEntry();
        ~D3DEntry();

        /*! Frees all memory allocated for the palette index list.
         *
         * This is automatically called by the destructor, but can be called if the user wishes to free up the memory without destroying the entry object.
         */
        void cleanup();

        /*! Returns the size of the palette index list.
         * \return Number of indicies.
         */
        int getNumPaletteIndicies() const;

        /*! Returns the texture index that this entry corresponds to.
         * \return Texture Index
         */
        int getTexture() const;

        /*! Gets the palette index for the requested position in the list.
         * \param idx The position in the list to grab, which must be less than the value returned by getNumPaletteIndicies().
         * \return Returns palette index on success, -1 if the index was invalid.
         */
        int getPaletteIndex(int idx) const;

        /*! Sets the index of the texture that this entry corresponds to.
         * \param tex The texture index to use.
         */
        void setTexture(int tex);

        /*! Sets the palette index for the given position in the list.
         * \param idx The position in the list, which must be less than the value returned by getNumPaletteIndicies().
         * \param pal The palette index to use.
         */
        void setPaletteIndex(int idx,int pal);

        /*! Removes the requested position from the list of palette indicies.
         * \param idx The position in the list to remove.
         */
        void removePaletteIndex(int idx);

        /*! Inserts a palette index into the list.
         *
         * The default action for this function is to insert an index before the position specified, effectively placing it at that position while moving the rest down.
         * The index can be inserted after the current position by setting the optional third arugment to 'false'.
         * \param idx The position in the list to insert the item.
         * \param pal The palette index to set as the value of the newly inserted index.
         * \param before [optional] If set to true (the default) the index will be inserted before the current position, otherwise will be inserted after the current position.
         */
         //TODO: Remove before argument, make it always before.
        void insertPaletteIndex(int idx,int pal);

        /*! Adds a new index to the end of the list.
         *
         * This is a convinience function that is essentially the same as calling insertPaletteIndex(getNumPaletteIndicies()-1,paletteIdx,false).
         * \param pal Palette index to add to the list.
         */
        void addPaletteIndex(int pal);

        /*! Loads entry from a file.
         *
         * Although this function is safe to use, it generally should not be used except by DriverD3D class. If size is greater than 0 then corruption checking is preformed.
         * \param file Pointer to a FILE structure to load from.
         * \param size [optional] Amount of data left in the file.
         * \return Returns number of bytes read on success, -1 if file was NULL, or -2 if the file was found to be corrupt.
         */
        int loadFromFile(FILE* file,int size = -1);

        /*! Loads entry from an IO handle.
         *
         * Although this function is safe to use, it generally should not be used except by DriverD3D class. If size is greater than 0 then corruption checking is preformed.
         * \param handle IO handle to be used.
         * \param callbacks Pointer to a callbacks structure for use with the handle.
         * \param size [optional] Amount of data left in the file.
         * \return Returns number of bytes read on success, -1 if handle was NULL, or -2 if the file was found to be corrupt.
         */
        int load(IOHandle handle, IOCallbacks* callbacks, int size = -1);

        /*! Saves entry to file.
         *
         * Although this function is safe to use, it generally should not be used except by DriverD3D class. Calls save(IOHandle, IOCallbacks* callbacks) internally.
         * \param file Pointer to a FILE structure to load from.
         * \return Returns number of bytes written.
         */
        int saveToFile(FILE* file) const;

        /*! Saves entry to handle.
         *
         * Generally should not be used except by DriverD3D class.
         * \param handle IO handle to save to.
         * \param callbacks Pointer to a callbacks structure. Must have at least write defined.
         * \return Returns number of bytes written.
         */
        int save(IOHandle handle, IOCallbacks* callbacks) const;

        /*! Gets the amount of space required to write this entry.
         * \return Returns required space in bytes.
         */
        int getRequiredSize() const;

    protected:
        D3DEntry(CEventMgr<IDriverD3DEvents>* eventMgr, int idx);
        CEventMgr<IDriverD3DEvents>* eventManager;
        int index;

        int texture;
        int numPalettes;
        int* palettes;
};

/*! \brief Container that .D3D files are loaded into, provided additional manipulation functions.
 *
 * This is the class used to open and manipulate D3D files. Once open, entries can be added or removed. Functions are also provided for
 * finding texture indicies in the list of entries.
 */

class DriverD3D
{
    public:
        DriverD3D();
        ~DriverD3D();
        /*! Frees memory for all allocated D3DEntries.
         * This function is automatically called by the destructor and when loading a new D3D file, but is exposed here so that the user can free up memory without destroying the object.
         */
        void cleanup();

        /*! Convinience function that takes a filename instead of an open FILE structure for loading d3d files.
         * \param filename Path and filename to D3D file to open.
         * \return Returns number of bytes read on success; -1 if the file failed to open; and -2 if the file was found to be corrupt.
         */
        int loadFromFile(const char* filename);

        /*! Loads a .d3d file.
         * \param file Pointer to a FILE structure to load from.
         * \return Returns number of bytes read on success; -1 if handle was NULL; and -2 if the file was found to be corrupt.
         */
        int loadFromFile(FILE* file);

        /*! Loads a .d3d file from a handle.
         * \param handle IO handle to load from.
         * \param callbacks Pointer to a callback structure.
         * \return Returns number of bytes read on success; -1 if handle was NULL; and -2 if the file was found to be corrupt.
         */
        int load(IOHandle handle, IOCallbacks* callbacks);

        /*! Convienience function that opens the provided filename for writing, then saves the D3D entries to the file.
         * \param filename Path and filename of file to save to.
         * \return Returns number of bytes saved on success; -1 if the file failed to open.
         */
        int saveToFile(const char* filename) const;

        /*! Saves D3D entries to file. Calls save(IOHandle handle, IOCallbacks* callbacks) internally.
         * \param file Pointer to an open FILE structure to save to.
         * \return Returns number of bytes saved on success; -1 if the file was NULL.
         */
        int saveToFile(FILE* file) const;

        /*! Saves a .d3d file to a handle.
         * \param handle IO handle to save to.
         * \param callbacks Pointer to a callback structure.
         * \return Returns number of bytes written on success; -1 if handle was NULL.
         */
        int save(IOHandle handle, IOCallbacks* callbacks) const;

        /*! Gets the size required to save the entire D3D structure.
         * \return Returns required size in bytes.
         */
        int getRequiredSize() const;

        /*! Gets the number of D3DEntry structures that are currently allocated.
         * \return Returns number of entries.
         */
        int getNumEntries() const;

        /*! Gets a pointer to the D3D entry at the specified index.
         * \param idx Index to entry which lies between 0 and getNumEntries()
         * \return Returns pointer to D3D entry on success, or NULL if the index was invalid.
         */
        D3DEntry* getEntry(int idx) const;

        /*! Gets the index to the D3D entry, if one exists, for the given texture index.
         * \param tex Texture index to find entry for.
         * \return Returns an index >= 0 on success; -1 if there were no entries for the given texture index.
         */
        int findTexture(int tex) const;

        /*! Convienience function that gets a pointer to an entry based on texture index rather than entry index. Equivilent to calling getEntry(findTexture(textureIndex)).
         * \param tex Texture index to get entry for.
         * \return Returns pointer to D3D entry on success, or NULL if no entry exists for the texture index given.
         */
        D3DEntry* getTextureEntry(int tex) const; //Convinience function

        /*! Removes the entry at the given index. Not to be confused with removing the entry for a texture index.
         * \param idx Index of entry to remove.
         */
        void removeEntry(int idx);

        /*! Removes the entry for the given texture index, if one exists.
         * \param tex Index of the texture used to find entry to remove.
         */
        void removeTextureEntry(int tex); //Convinience function

        /*! Adds a D3D entry to the list for the given texture, if one does not already exist.
         * \param tex Texture index to use for new entry.
         */
        void addEntry(int tex);

        /*! Register a class implementing some or all of IDriverD3DEvents for recieving events. You MUST unregister the handler before destroying it, or events will be passed to an invalid object.
         * \param handler Pointer to a subclass of IDriverD3DEvents.
         */
        void registerEventHandler(IDriverD3DEvents* handler);

        /*! Unregister a previously registered event handler. You MUST unregister the handler before destroying it, or events will be passed to an invalid object.
         * \param handler Pointer to a subclass of IDriverD3DEvents.
         */
        void unregisterEventHandler(IDriverD3DEvents* handler);

    protected:
        CEventMgr<IDriverD3DEvents> eventManager;
        int numEntries;
        D3DEntry** entries; //Why pointer array? So we don't destroy objects when resizing!
};

#endif
