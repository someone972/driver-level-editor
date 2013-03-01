#include "driver_den.hpp"

DentingSection::DentingSection()
{
    sectionSize = 0;
    sectionData = NULL;
};

DentingSection::~DentingSection()
{
    cleanup();
};

void DentingSection::cleanup()
{
    if(sectionData)
    delete[] sectionData;
    sectionData = NULL;
    sectionSize = 0;
};

int DentingSection::getNumVertices()
{
    return sectionSize;
};

void DentingSection::setData(const unsigned short* verts, int numVerts)
{
    cleanup();
    if(numVerts > 0 && verts)
    {
        sectionSize = numVerts;
        sectionData = new unsigned short[sectionSize];
        memcpy(sectionData,verts,sectionSize*sizeof(unsigned short));
    }
};

const unsigned short* DentingSection::data()
{
    return sectionData;
};

unsigned short DentingSection::getVertex(int idx)
{
    if(idx > 0 && idx < sectionSize)
    return sectionData[idx];
    return 0xFFFF;
};

void DentingSection::addVertex(int vidx)
{
    bool found = false;

    for(int i = 0; i < sectionSize; i++)
    {
        if(sectionData[i] == vidx)
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        unsigned short* temp = new unsigned short[sectionSize+1];
        if(sectionData)
        {
            memcpy(temp,sectionData,sectionSize*sizeof(unsigned short));
            delete[] sectionData;
        }
        sectionData = temp;
        sectionData[sectionSize] = vidx;
        sectionSize++;
    }
};

void DentingSection::removeVertex(int vidx)
{
    for(int i = 0; i < sectionSize; i++)
    {
        if(sectionData[i] == vidx)
        {
            if(sectionSize > 1)
            {
                unsigned short* temp = new unsigned short[sectionSize-1];
                memcpy(temp,sectionData,i*sizeof(unsigned short));
                memcpy(&temp[i],&sectionData[i+1],(sectionSize-(i+1))*sizeof(unsigned short));
                delete[] sectionData;
                sectionData = temp;
            }
            else sectionData = NULL;
            sectionSize--;
            break;
        }
    }
};

DentingEntry::DentingEntry()
{
    numShininessFaces = 0;
    numTextureFaces = 0;
    shininessData = NULL;
    textureNumbers = NULL;
};

DentingEntry::~DentingEntry()
{
    cleanup();
};

void DentingEntry::cleanup()
{
    for(int i = 0; i < 6; i++)
    {
        getDentingSection(i)->cleanup();
    }

    if(shininessData)
    delete[] shininessData;
    shininessData = NULL;
    numShininessFaces = 0;

    if(textureNumbers)
    delete[] textureNumbers;
    textureNumbers = NULL;
    numTextureFaces = 0;
};

DentingSection* DentingEntry::getDentingSection(int section)
{
    if(section >= 0 && section < 6)
    return &sections[section];
    return NULL;
};

int DentingEntry::getNumShininessFaces()
{
    return numShininessFaces;
};

int DentingEntry::getNumTextureIndexFaces()
{
    return numTextureFaces;
};

const unsigned char* DentingEntry::getShininessData()
{
    return shininessData;
};

void DentingEntry::setShininessData(const unsigned char* data, int numFaces)
{
    if(shininessData)
    delete[] shininessData;
    shininessData = NULL;
    numShininessFaces = 0;

    if(numFaces > 0 && data != NULL)
    {
        numShininessFaces = numFaces;
        shininessData = new unsigned char[numShininessFaces];
        memcpy(shininessData,data,numShininessFaces);
    }
};

void DentingEntry::setFaceShininess(int faceIdx, bool shiny)
{
    if(faceIdx >= 0)
    {
        if(faceIdx >= numShininessFaces)
        {
            unsigned char* temp = new unsigned char[faceIdx+1];
            if(shininessData)
            {
                memcpy(temp,shininessData,numShininessFaces);
                delete[] shininessData;
            }
            memset(temp+numShininessFaces,0,faceIdx+1-numShininessFaces);
            shininessData = temp;
            numShininessFaces = faceIdx+1;
        }
        if(shiny)
        shininessData[faceIdx] = 0x01;
        else shininessData[faceIdx] = 0x00;
    }
};

bool DentingEntry::faceIsShiny(int faceIdx)
{
    if(faceIdx >= 0 && faceIdx < numShininessFaces)
    {
        if(shininessData[faceIdx] == 0x01)
        return true;
    }
    return false;
};

void DentingEntry::setTextureIndexData(const unsigned char* data, int numFaces)
{
    if(textureNumbers)
    delete[] textureNumbers;
    textureNumbers = NULL;
    numTextureFaces = 0;

    if(numFaces > 0 && data != NULL)
    {
        numTextureFaces = numFaces;
        textureNumbers = new unsigned char[numTextureFaces];
        memcpy(textureNumbers,data,numTextureFaces);
    }
};

int DentingEntry::getFaceTextureIndex(int faceIdx)
{
    if(faceIdx >= 0 && faceIdx < numTextureFaces)
    return textureNumbers[faceIdx];
    return -1;
};

void DentingEntry::setFaceTextureIntex(int faceIdx, int texIdx)
{
    if(faceIdx >= 0)
    {
        if(faceIdx >= numTextureFaces)
        {
            unsigned char* temp = new unsigned char[faceIdx+1];
            if(textureNumbers)
            {
                memcpy(temp,textureNumbers,numTextureFaces);
                delete[] textureNumbers;
            }
            memset(temp+numTextureFaces,0,faceIdx+1-numTextureFaces);
            textureNumbers = temp;
            numTextureFaces = faceIdx+1;
        }
        textureNumbers[faceIdx] = texIdx;
    }
};

const unsigned char* DentingEntry::getTextureIndexData()
{
    return textureNumbers;
};

DriverDenting::DriverDenting()
{

};

DriverDenting::~DriverDenting()
{

};

DentingEntry* DriverDenting::civilianDenting(int idx)
{
    if(idx >= 0 && idx < 11)
    return &civilianEntries[idx];
    return NULL;
};

DentingEntry* DriverDenting::playerDenting(int idx)
{
    if(idx >= 0 && idx < 18)
    return &playerEntries[idx];
    return NULL;
};

int DriverDenting::loadCivilianDentingFromFile(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return 1;
    int ret = loadCivilianDentingFromFile(file);
    fclose(file);
    return ret;
};

int DriverDenting::loadCivilianDentingFromFile(FILE* file)
{
    if(!file)
    return 1;

    return loadCivilianDenting(&fileCallbacks,(IOHandle)file);
};

int DriverDenting::loadCivilianDenting(IOCallbacks* callbacks, IOHandle handle)
{
    if(!callbacks || !handle)
    return 1;

    int version;
    int dentingSize,shininessSize,textureIndexSize;
    unsigned int offsets[12];

    callbacks->read(&version,4,1,handle);
    if(version != 2)
    return 2;

    callbacks->read(&dentingSize,4,1,handle);
    callbacks->read(&shininessSize,4,1,handle);
    callbacks->read(&textureIndexSize,4,1,handle);

    unsigned char* dentingData = new unsigned char[dentingSize];
    callbacks->read(offsets,4,11,handle);
    callbacks->read(dentingData,dentingSize,1,handle);
    offsets[11] = dentingSize;

    for(int i = 0; i < 11; i++)
    {
        unsigned int sectionOffset = 24;
        for(int j = 0; j < 6; j++)
        {
            civilianDenting(i)->getDentingSection(j)->setData((unsigned short*)(dentingData+offsets[i]+sectionOffset),*(int*)(dentingData+j*4));
            sectionOffset += 2*(*(int*)(dentingData+j*4));
        }
    }
    delete[] dentingData;

    unsigned char* shininessData = new unsigned char[shininessSize];
    callbacks->read(offsets,4,11,handle);
    callbacks->read(shininessData,shininessSize,1,handle);
    offsets[11] = shininessSize;

    for(int i = 0; i < 11; i++)
    {
        civilianDenting(i)->setShininessData(shininessData+offsets[i],offsets[i+1]-offsets[i]);
    }

    delete[] shininessData;

    unsigned char* textureIndexData = new unsigned char[textureIndexSize];
    callbacks->read(offsets,4,11,handle);
    callbacks->read(textureIndexData,textureIndexSize,1,handle);
    offsets[11] = textureIndexSize;

    for(int i = 0; i < 11; i++)
    {
        civilianDenting(i)->setTextureIndexData(textureIndexData+offsets[i],offsets[i+1]-offsets[i]);
    }

    delete[] textureIndexData;

    return 0;
};

int DriverDenting::loadPlayerDentingFromFile(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return 1;
    int ret = loadPlayerDentingFromFile(file);
    fclose(file);
    return ret;
};

int DriverDenting::loadPlayerDentingFromFile(FILE* file)
{
    if(!file)
    return 1;

    return loadPlayerDenting(&fileCallbacks, (IOHandle)file);
};

int DriverDenting::loadPlayerDenting(IOCallbacks* callbacks, IOHandle handle)
{
    if(!callbacks || !handle)
    return 1;

    callbacks->seek(handle,18*4,SEEK_SET); //offsets to individual cars, not important since we're loading all of them.

    int size = 0;
    unsigned char* data = NULL;
    int dataSize = 0;
    int sectionOffset;

    for(int i = 0; i < 18; i++)
    {
        callbacks->read(&size,4,1,handle);
        if(size > dataSize)
        {
            dataSize = size;
            if(data)
            delete[] data;
            data = new unsigned char[dataSize];
        }
        callbacks->read(data,size,1,handle);

        sectionOffset = 24;
        for(int j = 0; j < 6; j++)
        {
            playerDenting(i)->getDentingSection(j)->setData((unsigned short*)(data+sectionOffset),*(int*)(data+j*4));
            sectionOffset += (*(int*)(data+j*4))*2;
        }

        callbacks->read(&size,4,1,handle);
        if(size > dataSize)
        {
            dataSize = size;
            if(data)
            delete[] data;
            data = new unsigned char[dataSize];
        }
        callbacks->read(data,size,1,handle);

        playerDenting(i)->setShininessData(data,size);

        callbacks->read(&size,4,1,handle);
        if(size > dataSize)
        {
            dataSize = size;
            if(data)
            delete[] data;
            data = new unsigned char[dataSize];
        }
        callbacks->read(data,size,1,handle);

        playerDenting(i)->setTextureIndexData(data,size);
    }

    if(data)
    delete[] data;

    return 0;
};

int DriverDenting::getCivilianDentingSize()
{
    int size = 412;

    for(int i = 0; i < 11; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            size += civilianDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }
        size += civilianDenting(i)->getNumTextureIndexFaces();
        size += civilianDenting(i)->getNumShininessFaces();
    }

    return size;
};

int DriverDenting::saveCivilianDentingToFile(const char* filename)
{
    FILE* file = fopen(filename,"wb");
    if(!file)
    return 1;
    int ret = saveCivilianDentingToFile(file);
    fclose(file);
    return ret;
};

int DriverDenting::saveCivilianDentingToFile(FILE* file)
{
    if(!file)
    return 1;

    return saveCivilianDenting(&fileCallbacks, (IOHandle)file);
};

int DriverDenting::saveCivilianDenting(IOCallbacks* callbacks, IOHandle handle)
{
    if(!callbacks || !handle)
        return 1;

    int version = 2;
    callbacks->write(&version,4,1,handle);

    //write total size of denting data memory chunk
    int size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += 24;
        for(int j = 0; j < 6; j++)
        {
            size += civilianDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }
    }
    callbacks->write(&size,4,1,handle);

    //write total size of shininess memory chunk
    size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += civilianDenting(i)->getNumShininessFaces();
    }
    callbacks->write(&size,4,1,handle);

    //write total size of texture index block
    size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += civilianDenting(i)->getNumTextureIndexFaces();
    }
    callbacks->write(&size,4,1,handle);

    //write offsets in memory block to each car's denting data first, then write all data
    int offset = 0;
    for(int i = 0; i < 11; i++)
    {
        callbacks->write(&offset,4,1,handle);
        offset += 24;
        for(int j = 0; j < 6; j++)
        {
            offset += civilianDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }
    }

    for(int i = 0; i < 11; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            int sectionSize = civilianDenting(i)->getDentingSection(j)->getNumVertices();
            callbacks->write(&sectionSize,4,1,handle);
        }

        for(int j = 0; j < 6; j++)
        {
            int sectionSize = civilianDenting(i)->getDentingSection(j)->getNumVertices();
            callbacks->write(civilianDenting(i)->getDentingSection(j)->data(),1,sectionSize*2,handle);
        }
    }

    //write offsets in memory block to each car's shininess data first, then write all data
    offset = 0;
    for(int i = 0; i < 11; i++)
    {
        callbacks->write(&offset,4,1,handle);
        offset += civilianDenting(i)->getNumShininessFaces();
    }

    for(int i = 0; i < 11; i++)
    {
        callbacks->write(civilianDenting(i)->getShininessData(),1,civilianDenting(i)->getNumShininessFaces(),handle);
    }

    //write offsets in memory block to each car's data first, then write all data
    offset = 0;
    for(int i = 0; i < 11; i++)
    {
        callbacks->write(&offset,4,1,handle);
        offset += civilianDenting(i)->getNumTextureIndexFaces();
    }

    for(int i = 0; i < 11; i++)
    {
        callbacks->write(civilianDenting(i)->getTextureIndexData(),1,civilianDenting(i)->getNumTextureIndexFaces(),handle);
    }
    return 0;
};

int DriverDenting::getPlayerDentingSize()
{
    int size = 18*4;
    for(int i = 0; i < 18; i++)
    {
        size += 4+6*4+4+playerDenting(i)->getNumShininessFaces()+4+playerDenting(i)->getNumTextureIndexFaces();
        for(int j = 0; j < 6; j++)
        {
            size += playerDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }
    }
    return size;
};

int DriverDenting::savePlayerDentingToFile(const char* filename)
{
    FILE* file = fopen(filename,"wb");
    if(!file)
    return 1;
    int ret = savePlayerDentingToFile(file);
    fclose(file);
    return ret;
};

int DriverDenting::savePlayerDentingToFile(FILE* file)
{
    if(!file)
        return 1;

    return savePlayerDenting(&fileCallbacks, (IOHandle)file);
};

int DriverDenting::savePlayerDenting(IOCallbacks* callbacks, IOHandle handle)
{
    if(!callbacks || !handle)
    return 1;

    int offset = 0;
    for(int i = 0; i < 18; i++)
    {
        callbacks->write(&offset,4,1,handle);
        offset += 24+12;//24(4*6) for denting section sizes, 12(4*3) for each data section
        for(int j = 0; j < 6; j++)
        {
            offset += playerDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }
        offset += playerDenting(i)->getNumShininessFaces();
        offset += playerDenting(i)->getNumTextureIndexFaces();
    }

    int size = 0;
    for(int i = 0; i < 18; i++)
    {
        //write size of denting data memory chunk
        size = 24;
        for(int j = 0; j < 6; j++)
        {
            size += playerDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }
        callbacks->write(&size,4,1,handle);

        //write denting data section sizes
        for(int j = 0; j < 6; j++)
        {
            size = playerDenting(i)->getDentingSection(j)->getNumVertices();
            callbacks->write(&size,4,1,handle);
        }
        //write denting data
        for(int j = 0; j < 6; j++)
        {
            callbacks->write(playerDenting(i)->getDentingSection(j)->data(),playerDenting(i)->getDentingSection(j)->getNumVertices()*2,1,handle);
        }

        //write shininess data
        size = playerDenting(i)->getNumShininessFaces();
        callbacks->write(&size,4,1,handle);
        callbacks->write(playerDenting(i)->getShininessData(),size,1,handle);

        //write texture index data (shouldn't be needed for playercars, but for uniformity it is included.
        size = playerDenting(i)->getNumTextureIndexFaces();
        callbacks->write(&size,4,1,handle);
        callbacks->write(playerDenting(i)->getTextureIndexData(),size,1,handle);
    }
    return 0;
};
