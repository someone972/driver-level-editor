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

    int version;
    int dentingSize,shininessSize,textureIndexSize;
    unsigned int offsets[12];

    fread(&version,4,1,file);
    if(version != 2)
    return 2;

    fread(&dentingSize,4,1,file);
    fread(&shininessSize,4,1,file);
    fread(&textureIndexSize,4,1,file);

    unsigned char* dentingData = new unsigned char[dentingSize];
    fread(offsets,4,11,file);
    fread(dentingData,dentingSize,1,file);
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
    fread(offsets,4,11,file);
    fread(shininessData,shininessSize,1,file);
    offsets[11] = shininessSize;

    for(int i = 0; i < 11; i++)
    {
        civilianDenting(i)->setShininessData(shininessData+offsets[i],offsets[i+1]-offsets[i]);
    }

    delete[] shininessData;

    unsigned char* textureIndexData = new unsigned char[textureIndexSize];
    fread(offsets,4,11,file);
    fread(textureIndexData,textureIndexSize,1,file);
    offsets[11] = textureIndexSize;

    for(int i = 0; i < 11; i++)
    {
        civilianDenting(i)->setTextureIndexData(textureIndexData+offsets[i],offsets[i+1]-offsets[i]);
    }

    delete[] textureIndexData;

    return 0;
};

int DriverDenting::loadCivilianDentingFromMemory(const unsigned char* data)
{
    if(!data)
    return 0;

    int version;
    int dentingSize,shininessSize,textureIndexSize;
    unsigned int offsets[12];

    version = *(int*)(data);
    data += 4;
    if(version != 2)
    return -1;

    dentingSize = *(int*)(data);
    shininessSize = *(int*)(data+4);
    textureIndexSize = *(int*)(data+8);
    data += 12;

    const unsigned char* dentingData;
    const unsigned char* shininessData;
    const unsigned char* textureIndexData;

    memcpy(offsets,data,44);
    data += 44;
    dentingData = data;
    data += dentingSize;
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

    memcpy(offsets,data,44);
    data += 44;
    shininessData = data;
    data += shininessSize;
    offsets[11] = shininessSize;

    for(int i = 0; i < 11; i++)
    {
        civilianDenting(i)->setShininessData(shininessData+offsets[i],offsets[i+1]-offsets[i]);
    }

    memcpy(offsets,data,44);
    data += 44;
    textureIndexData = data;
    data += textureIndexSize;
    offsets[11] = textureIndexSize;

    for(int i = 0; i < 11; i++)
    {
        civilianDenting(i)->setTextureIndexData(textureIndexData+offsets[i],offsets[i+1]-offsets[i]);
    }

    return 44*3+16+dentingSize+shininessSize+textureIndexSize;
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
    fseek(file,18*4,SEEK_SET); //offsets to individual cars, not important since we're loading all of them.

    int size = 0;
    unsigned char* data = NULL;
    int dataSize = 0;
    int sectionOffset;

    for(int i = 0; i < 18; i++)
    {
        fread(&size,4,1,file);
        if(size > dataSize)
        {
            dataSize = size;
            if(data)
            delete[] data;
            data = new unsigned char[dataSize];
        }
        fread(data,size,1,file);

        sectionOffset = 24;
        for(int j = 0; j < 6; j++)
        {
            playerDenting(i)->getDentingSection(j)->setData((unsigned short*)(data+sectionOffset),*(int*)(data+j*4));
            sectionOffset += (*(int*)(data+j*4))*2;
        }

        fread(&size,4,1,file);
        if(size > dataSize)
        {
            dataSize = size;
            if(data)
            delete[] data;
            data = new unsigned char[dataSize];
        }
        fread(data,size,1,file);

        playerDenting(i)->setShininessData(data,size);

        fread(&size,4,1,file);
        if(size > dataSize)
        {
            dataSize = size;
            if(data)
            delete[] data;
            data = new unsigned char[dataSize];
        }
        fread(data,size,1,file);

        playerDenting(i)->setTextureIndexData(data,size);
    }

    if(data)
    delete[] data;

    return 0;
};

int DriverDenting::loadPlayerDentingFromMemory(const unsigned char* data)
{
    if(!data)
    return 0;

    data += 18*4; //offsets to individual cars, not important since we're loading all of them.

    int size = 0;
    int totalSize = 18*4;
    int sectionOffset;

    for(int i = 0; i < 18; i++)
    {
        size = *(int*)(data);
        data += 4;

        sectionOffset = 24;
        for(int j = 0; j < 6; j++)
        {
            playerDenting(i)->getDentingSection(j)->setData((unsigned short*)(data+sectionOffset),*(int*)(data+j*4));
            sectionOffset += (*(int*)(data+j*4))*2;
        }

        data += sectionOffset;
        totalSize += 4+sectionOffset;

        size = *(int*)(data);
        data += 4;
        totalSize += 4+size;

        playerDenting(i)->setShininessData(data,size);

        size = *(int*)(data);
        data += 4;
        totalSize += 4+size;

        playerDenting(i)->setTextureIndexData(data,size);
    }
    return totalSize;
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

    int version = 2;
    fwrite(&version,4,1,file);

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
    fwrite(&size,4,1,file);

    //write total size of shininess memory chunk
    size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += civilianDenting(i)->getNumShininessFaces();
    }
    fwrite(&size,4,1,file);

    //write total size of texture index block
    size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += civilianDenting(i)->getNumTextureIndexFaces();
    }
    fwrite(&size,4,1,file);

    //write offsets in memory block to each car's denting data first, then write all data
    int offset = 0;
    for(int i = 0; i < 11; i++)
    {
        fwrite(&offset,4,1,file);
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
            fwrite(&sectionSize,4,1,file);
        }

        for(int j = 0; j < 6; j++)
        {
            int sectionSize = civilianDenting(i)->getDentingSection(j)->getNumVertices();
            fwrite(civilianDenting(i)->getDentingSection(j)->data(),1,sectionSize*2,file);
        }
    }

    //write offsets in memory block to each car's shininess data first, then write all data
    offset = 0;
    for(int i = 0; i < 11; i++)
    {
        fwrite(&offset,4,1,file);
        offset += civilianDenting(i)->getNumShininessFaces();
    }

    for(int i = 0; i < 11; i++)
    {
        fwrite(civilianDenting(i)->getShininessData(),1,civilianDenting(i)->getNumShininessFaces(),file);
    }

    //write offsets in memory block to each car's data first, then write all data
    offset = 0;
    for(int i = 0; i < 11; i++)
    {
        fwrite(&offset,4,1,file);
        offset += civilianDenting(i)->getNumTextureIndexFaces();
    }

    for(int i = 0; i < 11; i++)
    {
        fwrite(civilianDenting(i)->getTextureIndexData(),1,civilianDenting(i)->getNumTextureIndexFaces(),file);
    }
    return 0;
};

int DriverDenting::saveCivilianDentingToMemory(unsigned char* data)
{
    if(!data)
    return 0;

    int version = 2;
    *(int*)(data) = version;
    data += 4;

    unsigned char* startData = data;

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
    *(int*)(data) = size;
    data += 4;

    //write total size of shininess memory chunk
    size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += civilianDenting(i)->getNumShininessFaces();
    }
    *(int*)(data) = size;
    data += 4;

    //write total size of texture index block
    size = 0;
    for(int i = 0; i < 11; i++)
    {
        size += civilianDenting(i)->getNumTextureIndexFaces();
    }
    *(int*)(data) = size;
    data += 4;

    //write offsets in memory block to each car's denting data first, then write all data
    int offset = 0;
    for(int i = 0; i < 11; i++)
    {
        *(int*)(data) = offset;
        data += 4;
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

            *(int*)(data) = sectionSize;
            data += 4;
        }

        for(int j = 0; j < 6; j++)
        {
            int sectionSize = civilianDenting(i)->getDentingSection(j)->getNumVertices();
            memcpy(data,civilianDenting(i)->getDentingSection(j)->data(),sectionSize*2);
            data += sectionSize*2;
        }
    }

    //write offsets in memory block to each car's shininess data first, then write all data
    offset = 0;
    for(int i = 0; i < 11; i++)
    {
        *(int*)(data) = offset;
        data += 4;
        offset += civilianDenting(i)->getNumShininessFaces();
    }

    for(int i = 0; i < 11; i++)
    {
        memcpy(data,civilianDenting(i)->getShininessData(),civilianDenting(i)->getNumShininessFaces());
        data += civilianDenting(i)->getNumShininessFaces();
    }

    //write offsets in memory block to each car's data first, then write all data
    offset = 0;
    for(int i = 0; i < 11; i++)
    {
        *(int*)(data) = offset;
        data += 4;
        offset += civilianDenting(i)->getNumTextureIndexFaces();
    }

    for(int i = 0; i < 11; i++)
    {
        memcpy(data,civilianDenting(i)->getTextureIndexData(),civilianDenting(i)->getNumTextureIndexFaces());
        data += civilianDenting(i)->getNumTextureIndexFaces();
    }
    return data-startData;
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

    int offset = 0;
    for(int i = 0; i < 18; i++)
    {
        fwrite(&offset,4,1,file);
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
        fwrite(&size,4,1,file);

        //write denting data section sizes
        for(int j = 0; j < 6; j++)
        {
            size = playerDenting(i)->getDentingSection(j)->getNumVertices();
            fwrite(&size,4,1,file);
        }
        //write denting data
        for(int j = 0; j < 6; j++)
        {
            fwrite(playerDenting(i)->getDentingSection(j)->data(),playerDenting(i)->getDentingSection(j)->getNumVertices()*2,1,file);
        }

        //write shininess data
        size = playerDenting(i)->getNumShininessFaces();
        fwrite(&size,4,1,file);
        fwrite(playerDenting(i)->getShininessData(),size,1,file);

        //write texture index data (shouldn't be needed for playercars, but for uniformity it is included.
        size = playerDenting(i)->getNumTextureIndexFaces();
        fwrite(&size,4,1,file);
        fwrite(playerDenting(i)->getTextureIndexData(),size,1,file);
    }
    return 0;
};

int DriverDenting::savePlayerDentingToMemory(unsigned char* data)
{
    if(!data)
    return 0;

    unsigned char* startData = data;

    int offset = 0;
    for(int i = 0; i < 18; i++)
    {
        *(int*)(data) = offset;
        data += 4;
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
        *(int*)(data) = size;
        data += 4;

        //write denting data section sizes
        for(int j = 0; j < 6; j++)
        {
            size = playerDenting(i)->getDentingSection(j)->getNumVertices();
            *(int*)(data) = size;
            data += 4;
        }
        //write denting data
        for(int j = 0; j < 6; j++)
        {
            memcpy(data,playerDenting(i)->getDentingSection(j)->data(),playerDenting(i)->getDentingSection(j)->getNumVertices()*2);
            data += playerDenting(i)->getDentingSection(j)->getNumVertices()*2;
        }

        //write shininess data
        size = playerDenting(i)->getNumShininessFaces();
        *(int*)(data) = size;
        data += 4;
        memcpy(data,playerDenting(i)->getShininessData(),size);
        data += size;

        //write texture index data (shouldn't be needed for playercars, but for uniformity it is included.
        size = playerDenting(i)->getNumTextureIndexFaces();
        *(int*)(data) = size;
        data += 4;
        memcpy(data,playerDenting(i)->getTextureIndexData(),size);
        data += size;
    }
    return data-startData;
};
