#include "models.hpp"

ModelNames::ModelNames()
{
    data = NULL;
    nameIndex = NULL;
    dataSize = 0;
    maxNumNames = 0;
};

ModelNames::~ModelNames()
{
    cleanup();
};

void ModelNames::cleanup()
{
    if(data)
    delete[] data;
    data = NULL;
    dataSize = 0;

    if(nameIndex)
    delete[] nameIndex;
    nameIndex = NULL;
    maxNumNames = 0;
};

int ModelNames::getMaxNumNames()
{
    return maxNumNames;
};

int ModelNames::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;
    if(size < 0)
    return 2;

    data = new char[size];
    dataSize = size;
    callbacks->read(data,1,size,handle);

    rebuildNameIndex();
    if(log)
    {
        if(log->getLogPriority() >= DEBUG_LEVEL_RIDICULOUS)
        {
            log->increaseIndent();
            for(int i = 0; i < maxNumNames; i++)
            {
                log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: %s", i, getName(i));
            }
            log->decreaseIndent();
        }
    }
    return 0;
};

unsigned int ModelNames::getRequiredSize()
{
    return dataSize;
};

int ModelNames::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(data,1,dataSize,handle);
    return 0;
};

void ModelNames::rebuildNameIndex()
{
    if(nameIndex)
    delete[] nameIndex;
    nameIndex = NULL;

    maxNumNames = 0;

    for(int i = 0; i < dataSize; i++)
    {
        if(data[i] == '\0')
        maxNumNames++;
    }

    nameIndex = new const char*[maxNumNames];

    char* start = data;

    for(int i = 0, j = 0; j < maxNumNames; i++)
    {
        if(data[i] == '\0')
        {
            nameIndex[j] = start;
            j++;
            start = &data[i+1];
        }
    }
};

const char* ModelNames::getName(int idx)
{
    if(idx < 0 || idx >= maxNumNames)
    return NULL;
    return nameIndex[idx];
};

int ModelNames::findName(const char* name)
{
    if(!name)
    return -1;

    for(int i = 0; i < maxNumNames; i++)
    {
        if(strncmp(name,nameIndex[i],dataSize-(nameIndex[i]-data)) == 0)
        return i;
    }
    return -1;
};

void ModelNames::setName(int idx,const char* name)
{
    int newSize;
    char* tempData;

    if(idx >= 0 && idx < maxNumNames && name)
    {
        newSize = dataSize+(strlen(name)-strlen(nameIndex[idx]));
        if(newSize == dataSize && newSize != 0)
        {
            strcpy(&data[nameIndex[idx]-data],name);
        }
        else
        {
            tempData = new char[newSize];

            memcpy(tempData,data,nameIndex[idx]-data);
            strcpy(&tempData[nameIndex[idx]-data],name);
            memcpy(&tempData[nameIndex[idx]-data+strlen(name)+1],nameIndex[idx+1],dataSize-(nameIndex[idx+1]-data));

            delete[] data;
            data = tempData;
            dataSize = newSize;
            rebuildNameIndex();
        }
    }
};

void ModelNames::insertName(int idx,const char* name)
{
    if(idx >= 0 && idx <= maxNumNames && name)
    {
        int newSize = dataSize+strlen(name)+1;

        char* tempData = new char[newSize];

        if(data)
        {
            int copySize;
            if(idx == maxNumNames)
            copySize = dataSize;
            else copySize = nameIndex[idx]-data;
            memcpy(tempData,data,copySize);
            strcpy(tempData+copySize,name);
            if(idx != maxNumNames)
            memcpy(tempData+copySize+strlen(name)+1,data+copySize,dataSize-copySize);
            delete[] data;
        }
        else strcpy(tempData,name);

        data = tempData;
        dataSize = newSize;
        rebuildNameIndex();
    }
};

void ModelNames::appendName(const char* name)
{
    insertName(maxNumNames,name);
};

ModelCollisionBound::ModelCollisionBound()
{
    type = 0;
};

ModelFace::ModelFace()
{
    type = 0;
    flags = 0;
    texture = 0;
    memset(vertexIndicies,0,4*sizeof(int));
    memset(normalIndicies,0,4*sizeof(int));
    for(int i = 0; i < 4; i++)
    {
        colors[i].r = 255;
        colors[i].g = 255;
        colors[i].b = 255;
    }
};

int ModelFace::calculateType(int _flags)
{
    int ret = 0;

    if(_flags & FACE_QUAD)
    ret += 1;

    if(_flags & FACE_VERTEX_RGB)
    ret += 2;
    else if(_flags & FACE_NORMAL)
    ret += 8;
    else if(_flags & FACE_VERTEX_NORMAL)
    ret += 10;

    if(_flags & FACE_TEXTURED)
    ret += 4;

    if(ret >= 8)
    ret += 8;

    return ret;
};

void ModelFace::setTexture(int tex)
{
    if(tex == -1)
    {
        texture = 0;
        flags &= ~FACE_TEXTURED;
    }
    else
    {
        texture = tex;
        flags |= FACE_TEXTURED;
    }
    type = calculateType(flags);
};

void ModelFace::setColor(color_3ub color)
{
    flags &= ~FACE_VERTEX_RGB;
    colors[0] = color;
    type = calculateType(flags);
};

void ModelFace::setColors(color_3ub c1, color_3ub c2, color_3ub c3)
{
    flags |= FACE_VERTEX_RGB;
    colors[0] = c1;
    colors[1] = c2;
    colors[2] = c3;
    type = calculateType(flags);
};

void ModelFace::setColors(color_3ub c1, color_3ub c2, color_3ub c3, color_3ub c4)
{
    flags |= FACE_VERTEX_RGB;
    colors[0] = c1;
    colors[1] = c2;
    colors[2] = c3;
    colors[3] = c4;
    type = calculateType(flags);
};

void ModelFace::setNormal(int normalIdx)
{
    if(normalIdx < 0)
    {
        flags &= ~(FACE_NORMAL & FACE_VERTEX_NORMAL);
        normalIndicies[0] = 0;
        normalIndicies[1] = 0;
        normalIndicies[2] = 0;
        normalIndicies[3] = 0;
    }
    else
    {
        flags &= ~FACE_VERTEX_NORMAL;
        flags |= FACE_NORMAL;
        normalIndicies[0] = normalIdx;
    }
    type = calculateType(flags);
};

void ModelFace::setNormals(int n1, int n2, int n3, int n4)
{
    flags &= FACE_NORMAL;
    flags |= FACE_VERTEX_NORMAL;
    normalIndicies[0] = n1;
    normalIndicies[1] = n2;
    normalIndicies[2] = n3;
    normalIndicies[3] = n4;
    type = calculateType(flags);
};

void ModelFace::setTexCoords(Vector2f t1, Vector2f t2, Vector2f t3, Vector2f t4)
{
    textureCoords[0] = t1;
    textureCoords[1] = t2;
    textureCoords[2] = t3;
    textureCoords[3] = t4;
};

int ModelFace::getTexture()
{
    if(flags & FACE_TEXTURED)
    return texture;
    return -1;
};

bool ModelFace::hasAttribute(unsigned int flag)
{
    return flags&flag;
};

int ModelFace::getVertexIndex(int idx)
{
    if(idx >= 0 && idx < 4)
        return vertexIndicies[idx];
    return -1;
};

int ModelFace::getNormalIndex(int idx)
{
    if(idx >= 0 && idx < 4)
    {
        if(flags&FACE_VERTEX_NORMAL)
            return normalIndicies[idx];
        else return normalIndicies[0];
    }
    return -1;
};

Vector2f ModelFace::getTexCoord(int idx)
{
    if(idx >= 0 && idx < 4)
        return textureCoords[idx];
    return Vector2f(0.0f,0.0f);
};

color_3ub ModelFace::getColor(int idx)
{
    if(idx >= 0 && idx < 4)
    {
        if(flags&FACE_VERTEX_RGB)
            return colors[idx];
        else return colors[0];
    }
    color_3ub temp;
    temp.r = 0;
    temp.g = 0;
    temp.b = 0;
    return temp;
};

DriverModel::DriverModel()
{
    boundingSphereRadius = 0.0f;
    boundingCircleRadius = 0.0f;
    modelRef = -1;
    flags1 = 0;
    flags2 = 0;

    numVertices = 0;
    vertices = NULL;
    numNormals = 0;
    normals = NULL;
    numFaces = 0;
    faces = NULL;
    cullingNormals = NULL;
    numTexturesUsed = 0;
    texturesUsed = NULL;
    numCollisionBounds = 0;
    collisionBounds = NULL;
    boundsDirty = false;
    centerDirty = false;
    boundsVector.set(0,0,0);
    centerVector.set(0,0,0);
};

DriverModel::~DriverModel()
{
    cleanup();
};

void DriverModel::cleanup()
{
    if(vertices)
    delete[] vertices;
    vertices = NULL;
    numVertices = 0;

    if(normals)
    delete[] normals;
    normals = NULL;
    numNormals = 0;

    if(faces)
    delete[] faces;
    faces = NULL;
    if(cullingNormals)
    delete[] cullingNormals;
    cullingNormals = NULL;
    numFaces = 0;

    if(texturesUsed)
    delete[] texturesUsed;
    texturesUsed = NULL;
    numTexturesUsed = 0;

    if(collisionBounds)
    delete[] collisionBounds;
    collisionBounds = NULL;
    numCollisionBounds = 0;

    boundsDirty = true;
    centerDirty = true;
};

//Converts the 56 possible face types into the 16 actual data representations. This is how it is done in the Driver exe.
const int faceTypeConversion[56] = { 0, 1, 2, 3, 4, 5, 6, 7,16,16,16,16,16,16,16,16,
                                     8, 9,10,11,12,13,14,15,16,16,16,16,16,16,16,16,
                                     0, 1, 2, 3, 4, 5, 6, 7,16,16,16,16,16,16,16,16,
                                     8, 9,10,11,12,13,14,15};

//Size of the data for each face type.
const int faceTypeSize[56] = {12,16,20,28,20,24,28,36, 0, 0, 0, 0, 0, 0, 0, 0,
                              16,16,20,24,20,24,24,32, 0, 0, 0, 0, 0, 0, 0, 0,
                              12,16,20,28,20,24,28,36, 0, 0, 0, 0, 0, 0, 0, 0,
                              16,16,20,24,20,24,24,32};

//Converts a model from the level file format into a more manageable format.
//Returns size of model data read on success or -1 if data read will exceed size argument.
int DriverModel::convertFromLevelFormat(unsigned char* data, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    boundsDirty = true;
    centerDirty = true;

    int totalSize = 56;
    if(size < totalSize)
    {
        log->Log("ERROR: Data to be read for model exceeds size arg.");
        return -1;
    }

    flags1 = *(int*)(data);
    modelRef = *(int*)(data+4);
    flags2 = *(int*)(data+8);
    boundingSphereRadius = *(float*)(data+12);
    boundingCircleRadius = *(float*)(data+16);

    numVertices = *(short*)(data+20);
    numFaces = *(short*)(data+22);
    numNormals = *(short*)(data+24);
    numTexturesUsed = *(short*)(data+26);

    unsigned char* textureData = (unsigned char*)(*(int*)(data+28)+(long int)(data));
    unsigned char* vertexData = (unsigned char*)(*(int*)(data+32)+(long int)(data));
    unsigned char* cullingData = (unsigned char*)(*(int*)(data+36)+(long int)(data));
    unsigned char* normalData = (unsigned char*)(*(int*)(data+40)+(long int)(data));
    unsigned char* faceData = (unsigned char*)(*(int*)(data+44)+(long int)(data));
    unsigned char* collisionData = NULL;
    if(*(int*)(data+52) != 0)
    {
        collisionData = (unsigned char*)(*(int*)(data+52)+(long int)(data));
        numCollisionBounds = *(int*)(collisionData);
        collisionData += 4;
        totalSize += 4;
    }

    log->Log(DEBUG_LEVEL_DEBUG, "Flags1: 0x%x  Flags2: 0x%x  ModelRef: %d  SphereRadius: %f  CircleRadius: %f",flags1,flags2,modelRef,boundingSphereRadius,boundingCircleRadius);
    log->Log(DEBUG_LEVEL_DEBUG, "Vertices: %d  Faces: %d  Normals: %d  TexturesUsed: %d  CollisionBounds: %d",numVertices,numFaces,numNormals,numTexturesUsed,numCollisionBounds);

    if(size < totalSize + numTexturesUsed + numNormals*12 + numFaces*12 + (modelRef == -1 ? numCollisionBounds*20 + numFaces*16 + numVertices*12 : 0)) //numFaces*12 - 12 is the smallest size for a face, not perfect but it should catch most corrupted data.
    {
        cleanup();
        log->Log("ERROR: Data to be read for model exceeds size arg.");
        return -1;
    }

    if(numTexturesUsed > 0)
    {
        texturesUsed = new unsigned char[numTexturesUsed];
        memcpy(texturesUsed,textureData,numTexturesUsed);

        log->Log(DEBUG_LEVEL_DEBUG, "Textures...");
        if(log->getLogPriority() >= DEBUG_LEVEL_RIDICULOUS)
        {
            log->increaseIndent();
            for(int i = 0; i < numTexturesUsed; i++)
            {
                log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: %d", i, texturesUsed[i]);
            }
            log->decreaseIndent();
        }
        totalSize += numTexturesUsed;
    }

    if(modelRef == -1)
    {
        if(numVertices > 0)
        {
            log->Log(DEBUG_LEVEL_DEBUG, "Vertices... ");
            vertices = new Vector3f[numVertices];
            log->increaseIndent();
            for(int i = 0; i < numVertices; i++,vertexData += 12)
            {
                vertices[i].x = *(float*)(vertexData);
                vertices[i].y = *(float*)(vertexData+4);
                vertices[i].z = *(float*)(vertexData+8);
                log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: (%f, %f, %f)", i, vertices[i].x, vertices[i].y, vertices[i].z);
            }
            log->decreaseIndent();
            totalSize += numVertices*12;
        }

        if(numFaces > 0)
        {
            log->Log(DEBUG_LEVEL_DEBUG, "Culling Normals... ");
            cullingNormals = new Vector4f[numFaces];
            log->increaseIndent();
            for(int i = 0; i < numFaces; i++,cullingData += 16)
            {
                cullingNormals[i].x = *(float*)(cullingData);
                cullingNormals[i].y = *(float*)(cullingData+4);
                cullingNormals[i].z = *(float*)(cullingData+8);
                cullingNormals[i].w = *(float*)(cullingData+12);
                log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: (%f, %f, %f, %f)", i, cullingNormals[i].x, cullingNormals[i].y, cullingNormals[i].z, cullingNormals[i].w);
            }
            log->decreaseIndent();
            totalSize += numFaces*16;
        }

        if(numCollisionBounds > 0)
        {
            log->Log(DEBUG_LEVEL_DEBUG, "Collision Bounds...");
            collisionBounds = new ModelCollisionBound[numCollisionBounds];
            log->increaseIndent();
            for(int i = 0; i < numCollisionBounds; i++,collisionData += 20)
            {
                collisionBounds[i].type = *(short*)(collisionData);

                collisionBounds[i].position.x = *(short*)(collisionData+2);
                collisionBounds[i].position.y = *(short*)(collisionData+4);
                collisionBounds[i].position.z = *(short*)(collisionData+6);

                collisionBounds[i].rotation.x = *(short*)(collisionData+8);
                collisionBounds[i].rotation.y = *(short*)(collisionData+10);
                collisionBounds[i].rotation.z = *(short*)(collisionData+12);

                collisionBounds[i].length.x = *(short*)(collisionData+14);
                collisionBounds[i].length.y = *(short*)(collisionData+16);
                collisionBounds[i].length.z = *(short*)(collisionData+18);

                log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: Type: %d  Position: (%hd, %hd, %hd)  Rotation: (%hd, %hd, %hd)  Length: (%hd, %hd, %hd)",
                         collisionBounds[i].type,collisionBounds[i].position.x,collisionBounds[i].position.y,collisionBounds[i].position.z,
                         collisionBounds[i].rotation.x,collisionBounds[i].rotation.y,collisionBounds[i].rotation.z,
                         collisionBounds[i].length.x,collisionBounds[i].length.y,collisionBounds[i].length.z);
            }
            log->decreaseIndent();
            totalSize += numCollisionBounds*20;
        }
    }

    if(numNormals > 0)
    {
        log->Log(DEBUG_LEVEL_DEBUG, "Normals... ");
        normals = new Vector3f[numNormals];
        log->increaseIndent();
        for(int i = 0; i < numNormals; i++,normalData += 12)
        {
            normals[i].x = *(float*)(normalData);
            normals[i].y = *(float*)(normalData+4);
            normals[i].z = *(float*)(normalData+8);
            log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: (%f, %f, %f)", i, normals[i].x, normals[i].y, normals[i].z);
        }
        log->decreaseIndent();
        totalSize += numNormals*12;
    }

    if(numFaces > 0)
    {
        log->Log(DEBUG_LEVEL_DEBUG, "Faces...");
        faces = new ModelFace[numFaces];
        log->increaseIndent();
        for(int i = 0; i < numFaces; i++)
        {
            faces[i].type = *faceData;
            log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: Type: %d", i, faces[i].type);
            if(faces[i].type < 56)
            {
                int type = faceTypeConversion[faces[i].type];
                faces[i].texture = *(unsigned char*)(faceData+1);

                if(!(type&1) && type != 16)
                {
                    faces[i].vertexIndicies[2] = *(short*)(faceData+2);
                    faces[i].vertexIndicies[1] = *(short*)(faceData+4);
                    faces[i].vertexIndicies[0] = *(short*)(faceData+6);
                }
                else if((type&1) && type < 16)
                {
                    faces[i].vertexIndicies[3] = *(short*)(faceData+2);
                    faces[i].vertexIndicies[2] = *(short*)(faceData+4);
                    faces[i].vertexIndicies[1] = *(short*)(faceData+6);
                    faces[i].vertexIndicies[0] = *(short*)(faceData+8);
                    faces[i].flags |= FACE_QUAD;
                }

                switch(type)
                {
                    case 0:
                    case 2:
                        faces[i].colors[0].r = *(unsigned char*)(faceData+8);
                        faces[i].colors[0].g = *(unsigned char*)(faceData+9);
                        faces[i].colors[0].b = *(unsigned char*)(faceData+10);
                        break;
                    case 1:
                    case 3:
                    case 8:
                    case 9:
                        faces[i].colors[0].r = *(unsigned char*)(faceData+12);
                        faces[i].colors[0].g = *(unsigned char*)(faceData+13);
                        faces[i].colors[0].b = *(unsigned char*)(faceData+14);
                        break;
                    case 4:
                    case 6:
                    case 10:
                    case 12:
                        faces[i].colors[0].r = *(unsigned char*)(faceData+16);
                        faces[i].colors[0].g = *(unsigned char*)(faceData+17);
                        faces[i].colors[0].b = *(unsigned char*)(faceData+18);
                        break;
                    case 5:
                    case 7:
                    case 11:
                    case 13:
                    case 14:
                        faces[i].colors[0].r = *(unsigned char*)(faceData+20);
                        faces[i].colors[0].g = *(unsigned char*)(faceData+21);
                        faces[i].colors[0].b = *(unsigned char*)(faceData+22);
                        break;
                    case 15:
                        faces[i].colors[0].r = *(unsigned char*)(faceData+28);
                        faces[i].colors[0].g = *(unsigned char*)(faceData+29);
                        faces[i].colors[0].b = *(unsigned char*)(faceData+30);
                        break;
                    default:
                        faces[i].colors[0].r =  0;
                        faces[i].colors[0].g =  0;
                        faces[i].colors[0].b =  0;
                        break;
                }

                switch(type)
                {
                    case 2:
                        faces[i].colors[1].r = *(unsigned char*)(faceData+12);
                        faces[i].colors[1].g = *(unsigned char*)(faceData+13);
                        faces[i].colors[1].b = *(unsigned char*)(faceData+14);
                        faces[i].colors[2].r = *(unsigned char*)(faceData+16);
                        faces[i].colors[2].g = *(unsigned char*)(faceData+17);
                        faces[i].colors[2].b = *(unsigned char*)(faceData+18);
                        faces[i].flags |= FACE_VERTEX_RGB;
                        break;
                    case 3:
                        faces[i].colors[1].r = *(unsigned char*)(faceData+16);
                        faces[i].colors[1].g = *(unsigned char*)(faceData+17);
                        faces[i].colors[1].b = *(unsigned char*)(faceData+18);
                        faces[i].colors[2].r = *(unsigned char*)(faceData+20);
                        faces[i].colors[2].g = *(unsigned char*)(faceData+21);
                        faces[i].colors[2].b = *(unsigned char*)(faceData+22);
                        faces[i].colors[3].r = *(unsigned char*)(faceData+24);
                        faces[i].colors[3].g = *(unsigned char*)(faceData+25);
                        faces[i].colors[3].b = *(unsigned char*)(faceData+26);
                        faces[i].flags |= FACE_VERTEX_RGB;
                        break;
                    case 4:
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+10);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+11);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+12);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+13);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+15);
                        faces[i].flags |= FACE_TEXTURED;
                        break;
                    case 5:
                        faces[i].textureCoords[3].x = *(unsigned char*)(faceData+12);
                        faces[i].textureCoords[3].y = *(unsigned char*)(faceData+13);
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+15);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+16);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+17);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+18);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+19);
                        faces[i].flags |= FACE_TEXTURED;
                        break;
                    case 6:
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+10);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+11);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+12);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+13);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+15);
                        faces[i].colors[1].r = *(unsigned char*)(faceData+20);
                        faces[i].colors[1].g = *(unsigned char*)(faceData+21);
                        faces[i].colors[1].b = *(unsigned char*)(faceData+22);
                        faces[i].colors[2].r = *(unsigned char*)(faceData+24);
                        faces[i].colors[2].g = *(unsigned char*)(faceData+25);
                        faces[i].colors[2].b = *(unsigned char*)(faceData+26);
                        faces[i].flags |= FACE_TEXTURED;

                        faces[i].flags |= FACE_VERTEX_RGB;
                        break;
                    case 7:
                        faces[i].textureCoords[3].x = *(unsigned char*)(faceData+12);
                        faces[i].textureCoords[3].y = *(unsigned char*)(faceData+13);
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+15);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+16);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+17);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+18);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+19);
                        faces[i].colors[1].r = *(unsigned char*)(faceData+24);
                        faces[i].colors[1].g = *(unsigned char*)(faceData+25);
                        faces[i].colors[1].b = *(unsigned char*)(faceData+26);
                        faces[i].colors[2].r = *(unsigned char*)(faceData+28);
                        faces[i].colors[2].g = *(unsigned char*)(faceData+29);
                        faces[i].colors[2].b = *(unsigned char*)(faceData+30);
                        faces[i].colors[3].r = *(unsigned char*)(faceData+32);
                        faces[i].colors[3].g = *(unsigned char*)(faceData+33);
                        faces[i].colors[3].b = *(unsigned char*)(faceData+34);
                        faces[i].flags |= FACE_TEXTURED;

                        faces[i].flags |= FACE_VERTEX_RGB;
                        break;
                    case 8:
                        faces[i].normalIndicies[0] =  *(short*)(faceData+10);
                        faces[i].flags |= FACE_NORMAL;
                        break;
                    case 9:
                        faces[i].normalIndicies[0] =  *(short*)(faceData+10);
                        faces[i].flags |= FACE_NORMAL;
                        break;
                    case 10:
                        faces[i].normalIndicies[2] =  *(short*)(faceData+10);
                        faces[i].normalIndicies[1] =  *(short*)(faceData+12);
                        faces[i].normalIndicies[0] =  *(short*)(faceData+14);
                        faces[i].flags |= FACE_VERTEX_NORMAL;
                        break;
                    case 11:
                        faces[i].normalIndicies[3] =  *(short*)(faceData+12);
                        faces[i].normalIndicies[2] =  *(short*)(faceData+14);
                        faces[i].normalIndicies[1] =  *(short*)(faceData+16);
                        faces[i].normalIndicies[0] =  *(short*)(faceData+18);
                        faces[i].flags |= FACE_VERTEX_NORMAL;
                        break;
                    case 12:
                        faces[i].normalIndicies[0] =  *(short*)(faceData+8);
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+10);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+11);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+12);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+13);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+15);
                        faces[i].flags |= FACE_NORMAL;
                        faces[i].flags |= FACE_TEXTURED;
                        break;
                    case 13:
                        faces[i].normalIndicies[0] =  *(short*)(faceData+10);
                        faces[i].textureCoords[3].x = *(unsigned char*)(faceData+12);
                        faces[i].textureCoords[3].y = *(unsigned char*)(faceData+13);
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+15);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+16);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+17);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+18);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+19);
                        faces[i].flags |= FACE_NORMAL;
                        faces[i].flags |= FACE_TEXTURED;
                        break;
                    case 14:
                        faces[i].normalIndicies[2] =  *(short*)(faceData+8);
                        faces[i].normalIndicies[1] =  *(short*)(faceData+10);
                        faces[i].normalIndicies[0] =  *(short*)(faceData+12);
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+14);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+15);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+16);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+17);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+18);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+19);
                        faces[i].flags |= FACE_VERTEX_NORMAL;
                        faces[i].flags |= FACE_TEXTURED;
                        break;
                    case 15:
                        faces[i].normalIndicies[3] =  *(short*)(faceData+12);
                        faces[i].normalIndicies[2] =  *(short*)(faceData+14);
                        faces[i].normalIndicies[1] =  *(short*)(faceData+16);
                        faces[i].normalIndicies[0] =  *(short*)(faceData+18);
                        faces[i].textureCoords[3].x = *(unsigned char*)(faceData+20);
                        faces[i].textureCoords[3].y = *(unsigned char*)(faceData+21);
                        faces[i].textureCoords[2].x = *(unsigned char*)(faceData+22);
                        faces[i].textureCoords[2].y = *(unsigned char*)(faceData+23);
                        faces[i].textureCoords[1].x = *(unsigned char*)(faceData+24);
                        faces[i].textureCoords[1].y = *(unsigned char*)(faceData+25);
                        faces[i].textureCoords[0].x = *(unsigned char*)(faceData+26);
                        faces[i].textureCoords[0].y = *(unsigned char*)(faceData+27);
                        faces[i].flags |= FACE_VERTEX_NORMAL;
                        faces[i].flags |= FACE_TEXTURED;
                        break;
                    default:
                        break;
                }
            }
            log->Log(DEBUG_LEVEL_RIDICULOUS, "  Texture: %d  Verts: (%d, %d, %d, %d)  Norms: (%d, %d, %d, %d)", faces[i].texture, faces[i].vertexIndicies[0],
                     faces[i].vertexIndicies[1], faces[i].vertexIndicies[2], faces[i].vertexIndicies[3], faces[i].normalIndicies[0],
                     faces[i].normalIndicies[1], faces[i].normalIndicies[2], faces[i].normalIndicies[3]);
            faceData += faceTypeSize[faces[i].type];
            totalSize += faceTypeSize[faces[i].type];
        }
        log->decreaseIndent();
    }

    if(totalSize%4)
    totalSize += 4-totalSize%4;

    return totalSize;
};


void DriverModel::convertToLevelFormat(unsigned char* data) const
{
    *(int*)(data) = flags1;
    *(int*)(data+4) = modelRef;
    *(int*)(data+8) = flags2;
    *(float*)(data+12) = boundingSphereRadius;
    *(float*)(data+16) = boundingCircleRadius;

    *(short*)(data+20) = numVertices;
    *(short*)(data+22) = numFaces;
    *(short*)(data+24) = numNormals;
    *(short*)(data+26) = numTexturesUsed;

    int offset = 56;
    *(int*)(data+28) = offset; //texture data

    offset += numTexturesUsed;
    if(offset%4 != 0)
    offset += 4-(offset%4);
    *(int*)(data+32) = offset; //vertex data

    if(modelRef == -1)
    offset += 12*numVertices;
    *(int*)(data+36) = offset; //culling normals

    if(modelRef == -1)
    offset += 16*numFaces;
    *(int*)(data+40) = offset; //normals

    offset += 12*numNormals;
    if(numCollisionBounds == 0 || modelRef != -1)
    {
        *(int*)(data+52) = 0;
    }
    else
    {
        *(int*)(data+52) = offset;
        offset += 4+20*numCollisionBounds;
    }
    *(int*)(data+48) = 0;
    *(int*)(data+44) = offset;

    unsigned char* textureData = (unsigned char*)(*(int*)(data+28)+(long int)(data));
    unsigned char* vertexData = (unsigned char*)(*(int*)(data+32)+(long int)(data));
    unsigned char* cullingData = (unsigned char*)(*(int*)(data+36)+(long int)(data));
    unsigned char* normalData = (unsigned char*)(*(int*)(data+40)+(long int)(data));
    unsigned char* faceData = (unsigned char*)(*(int*)(data+44)+(long int)(data));
    unsigned char* collisionData = NULL;

    if(*(int*)(data+52) != 0)
    {
        collisionData = (unsigned char*)(*(int*)(data+52)+(long int)(data));
        *(int*)(collisionData) = numCollisionBounds;
        collisionData += 4;
    }

    if(numTexturesUsed > 0)
    {
        memcpy(textureData,texturesUsed,numTexturesUsed);
    }

    if(modelRef == -1)
    {
        if(numVertices > 0)
        {
            for(int i = 0; i < numVertices; i++,vertexData += 12)
            {
                *(float*)(vertexData) = vertices[i].x;
                *(float*)(vertexData+4) = vertices[i].y;
                *(float*)(vertexData+8) = vertices[i].z;
            }
        }
        if(numFaces > 0)
        {
            for(int i = 0; i < numFaces; i++,cullingData += 16)
            {
                *(float*)(cullingData) = cullingNormals[i].x;
                *(float*)(cullingData+4) = cullingNormals[i].y;
                *(float*)(cullingData+8) = cullingNormals[i].z;
                *(float*)(cullingData+12) = cullingNormals[i].w;
            }
        }
        if(numCollisionBounds > 0)
        {
            for(int i = 0; i < numCollisionBounds; i++,collisionData += 20)
            {
                *(short*)(collisionData) = collisionBounds[i].type;

                *(short*)(collisionData+2) = collisionBounds[i].position.x;
                *(short*)(collisionData+4) = collisionBounds[i].position.y;
                *(short*)(collisionData+6) = collisionBounds[i].position.z;

                *(short*)(collisionData+8) = collisionBounds[i].rotation.x;
                *(short*)(collisionData+10) = collisionBounds[i].rotation.y;
                *(short*)(collisionData+12) = collisionBounds[i].rotation.z;

                *(short*)(collisionData+14) = collisionBounds[i].length.x;
                *(short*)(collisionData+16) = collisionBounds[i].length.y;
                *(short*)(collisionData+18) = collisionBounds[i].length.z;
            }
        }
    }

    if(numNormals > 0)
    {
        for(int i = 0; i < numNormals; i++,normalData += 12)
        {
            *(float*)(normalData) = normals[i].x;
            *(float*)(normalData+4) = normals[i].y;
            *(float*)(normalData+8) = normals[i].z;
        }
    }

    if(numFaces > 0)
    {
        for(int i = 0; i < numFaces; i++)
        {
            *faceData = faces[i].type;
            if(faces[i].type < 56)
            {
                int type = faceTypeConversion[faces[i].type];
                *(unsigned char*)(faceData+1) = faces[i].texture;

                if(!(type&1) && type != 16)
                {
                    *(short*)(faceData+2) = faces[i].vertexIndicies[2];
                    *(short*)(faceData+4) = faces[i].vertexIndicies[1];
                    *(short*)(faceData+6) = faces[i].vertexIndicies[0];
                }
                else if((type&1) && type < 16)
                {
                    *(short*)(faceData+2) = faces[i].vertexIndicies[3];
                    *(short*)(faceData+4) = faces[i].vertexIndicies[2];
                    *(short*)(faceData+6) = faces[i].vertexIndicies[1];
                    *(short*)(faceData+8) = faces[i].vertexIndicies[0];
                }

                switch(type)
                {
                    case 0:
                    case 2:
                        *(unsigned char*)(faceData+8) = faces[i].colors[0].r;
                        *(unsigned char*)(faceData+9) = faces[i].colors[0].g;
                        *(unsigned char*)(faceData+10) = faces[i].colors[0].b;
                        break;
                    case 1:
                    case 8:
                    case 9:
                    case 3:
                        *(unsigned char*)(faceData+12) = faces[i].colors[0].r;
                        *(unsigned char*)(faceData+13) = faces[i].colors[0].g;
                        *(unsigned char*)(faceData+14) = faces[i].colors[0].b;
                        break;
                    case 4:
                    case 6:
                    case 10:
                    case 12:
                        *(unsigned char*)(faceData+16) = faces[i].colors[0].r;
                        *(unsigned char*)(faceData+17) = faces[i].colors[0].g;
                        *(unsigned char*)(faceData+18) = faces[i].colors[0].b;
                        break;
                    case 5:
                    case 7:
                    case 11:
                    case 13:
                    case 14:
                        *(unsigned char*)(faceData+20) = faces[i].colors[0].r;
                        *(unsigned char*)(faceData+21) = faces[i].colors[0].g;
                        *(unsigned char*)(faceData+22) = faces[i].colors[0].b;
                        break;
                    case 15:
                        *(unsigned char*)(faceData+28) = faces[i].colors[0].r;
                        *(unsigned char*)(faceData+29) = faces[i].colors[0].g;
                        *(unsigned char*)(faceData+30) = faces[i].colors[0].b;
                        break;
                }

                switch(type)
                {
                    case 2:
                        *(unsigned char*)(faceData+12) = faces[i].colors[1].r;
                        *(unsigned char*)(faceData+13) = faces[i].colors[1].g;
                        *(unsigned char*)(faceData+14) = faces[i].colors[1].b;
                        *(unsigned char*)(faceData+16) = faces[i].colors[2].r;
                        *(unsigned char*)(faceData+17) = faces[i].colors[2].g;
                        *(unsigned char*)(faceData+18) = faces[i].colors[2].b;
                        break;
                    case 3:
                        *(unsigned char*)(faceData+16) = faces[i].colors[1].r;
                        *(unsigned char*)(faceData+17) = faces[i].colors[1].g;
                        *(unsigned char*)(faceData+18) = faces[i].colors[1].b;
                        *(unsigned char*)(faceData+20) = faces[i].colors[2].r;
                        *(unsigned char*)(faceData+21) = faces[i].colors[2].g;
                        *(unsigned char*)(faceData+22) = faces[i].colors[2].b;
                        *(unsigned char*)(faceData+24) = faces[i].colors[3].r;
                        *(unsigned char*)(faceData+25) = faces[i].colors[3].g;
                        *(unsigned char*)(faceData+26) = faces[i].colors[3].b;
                        break;
                    case 4:
                        *(unsigned char*)(faceData+10) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+11) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+12) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+13) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[0].y;
                        break;
                    case 5:
                        *(unsigned char*)(faceData+12) = faces[i].textureCoords[3].x;
                        *(unsigned char*)(faceData+13) = faces[i].textureCoords[3].y;
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+16) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+17) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+18) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+19) = faces[i].textureCoords[0].y;
                        break;
                    case 6:
                        *(unsigned char*)(faceData+10) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+11) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+12) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+13) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[0].y;
                        *(unsigned char*)(faceData+20) = faces[i].colors[1].r;
                        *(unsigned char*)(faceData+21) = faces[i].colors[1].g;
                        *(unsigned char*)(faceData+22) = faces[i].colors[1].b;
                        *(unsigned char*)(faceData+24) = faces[i].colors[2].r;
                        *(unsigned char*)(faceData+25) = faces[i].colors[2].g;
                        *(unsigned char*)(faceData+26) = faces[i].colors[2].b;
                        break;
                    case 7:
                        *(unsigned char*)(faceData+12) = faces[i].textureCoords[3].x;
                        *(unsigned char*)(faceData+13) = faces[i].textureCoords[3].y;
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+16) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+17) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+18) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+19) = faces[i].textureCoords[0].y;
                        *(unsigned char*)(faceData+24) = faces[i].colors[1].r;
                        *(unsigned char*)(faceData+25) = faces[i].colors[1].g;
                        *(unsigned char*)(faceData+26) = faces[i].colors[1].b;
                        *(unsigned char*)(faceData+28) = faces[i].colors[2].r;
                        *(unsigned char*)(faceData+29) = faces[i].colors[2].g;
                        *(unsigned char*)(faceData+30) = faces[i].colors[2].b;
                        *(unsigned char*)(faceData+32) = faces[i].colors[3].r;
                        *(unsigned char*)(faceData+33) = faces[i].colors[3].g;
                        *(unsigned char*)(faceData+34) = faces[i].colors[3].b;
                        break;
                    case 8:
                        *(short*)(faceData+10) = faces[i].normalIndicies[0];
                        break;
                    case 9:
                        *(short*)(faceData+10) = faces[i].normalIndicies[0];
                        break;
                    case 10:
                        *(short*)(faceData+10) = faces[i].normalIndicies[2];
                        *(short*)(faceData+12) = faces[i].normalIndicies[1];
                        *(short*)(faceData+14) = faces[i].normalIndicies[0];
                        break;
                    case 11:
                        *(short*)(faceData+12) = faces[i].normalIndicies[3];
                        *(short*)(faceData+14) = faces[i].normalIndicies[2];
                        *(short*)(faceData+16) = faces[i].normalIndicies[1];
                        *(short*)(faceData+18) = faces[i].normalIndicies[0];
                        break;
                    case 12:
                        *(short*)(faceData+8) = faces[i].normalIndicies[0];
                        *(unsigned char*)(faceData+10) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+11) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+12) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+13) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[0].y;
                        break;
                    case 13:
                        *(short*)(faceData+10) = faces[i].normalIndicies[0];
                        *(unsigned char*)(faceData+12) = faces[i].textureCoords[3].x;
                        *(unsigned char*)(faceData+13) = faces[i].textureCoords[3].y;
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+16) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+17) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+18) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+19) = faces[i].textureCoords[0].y;
                        break;
                    case 14:
                        *(short*)(faceData+8) = faces[i].normalIndicies[2];
                        *(short*)(faceData+10) = faces[i].normalIndicies[1];
                        *(short*)(faceData+12) = faces[i].normalIndicies[0];
                        *(unsigned char*)(faceData+14) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+15) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+16) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+17) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+18) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+19) = faces[i].textureCoords[0].y;
                        break;
                    case 15:
                        *(short*)(faceData+12) = faces[i].normalIndicies[3];
                        *(short*)(faceData+14) = faces[i].normalIndicies[2];
                        *(short*)(faceData+16) = faces[i].normalIndicies[1];
                        *(short*)(faceData+18) = faces[i].normalIndicies[0];
                        *(unsigned char*)(faceData+20) = faces[i].textureCoords[3].x;
                        *(unsigned char*)(faceData+21) = faces[i].textureCoords[3].y;
                        *(unsigned char*)(faceData+22) = faces[i].textureCoords[2].x;
                        *(unsigned char*)(faceData+23) = faces[i].textureCoords[2].y;
                        *(unsigned char*)(faceData+24) = faces[i].textureCoords[1].x;
                        *(unsigned char*)(faceData+25) = faces[i].textureCoords[1].y;
                        *(unsigned char*)(faceData+26) = faces[i].textureCoords[0].x;
                        *(unsigned char*)(faceData+27) = faces[i].textureCoords[0].y;
                        break;
                }
            }
            faceData += faceTypeSize[faces[i].type];
        }
    }
};

unsigned int DriverModel::getRequiredSize() const
{
    unsigned int size = 56;
    size += numTexturesUsed;
    size += numNormals*12;

    if(modelRef == -1)
    {
        size += 12*numVertices;
        if(numCollisionBounds > 0)
        {
            size += 4+20*numCollisionBounds;
        }
        size += 16*numFaces;
    }

    for(int i = 0; i < numFaces; i++)
    {
        size += faceTypeSize[faces[i].type];
    }

    if(size%4 != 0)
    {
        size += 4-(size%4); //dword align
    }
    return size;
};

int DriverModel::getModelReference() const
{
    return modelRef;
};

int DriverModel::createReferenceToModel(int modelIdx,DriverModel* mod)
{
    if(!mod)
    return 1;

    if(mod->getNumVertices() != numVertices)
    return 2;

    if(modelIdx < 0)
    return 3;

    if(vertices)
    delete[] vertices;
    vertices = NULL;

    modelRef = modelIdx;
    return 0;
};

int DriverModel::dereferenceModel(DriverModel* mod)
{
    if(!mod)
    return 1;

    if(mod->getNumVertices() != numVertices)
    return 2;

    if(modelRef == -1)
    return 3;

    if(vertices)
    delete[] vertices;
    vertices = new Vector3f[numVertices];

    for(int i = 0; i < numVertices; i++)
    {
        vertices[i] = mod->getVertex(i);
    }

    modelRef = -1;
    return 0;
};

int DriverModel::getNumVertices() const
{
    return numVertices;
};

Vector3f DriverModel::getVertex(int idx) const
{
    if(idx >= 0 && idx < numVertices && modelRef == -1)
    return vertices[idx];
    return Vector3f(0,0,0);
};

int DriverModel::getNumNormals() const
{
    return numNormals;
};

Vector3f DriverModel::getNormal(int idx) const
{
    if(idx >= 0 && idx < numNormals)
    return normals[idx];
    return Vector3f(0,0,0);
};

int DriverModel::getNumCollisionBounds() const
{
    return numCollisionBounds;
};

//getCenter and getBounds still considered as const because the object appears immutable from the user's perspective.
Vector3f DriverModel::getCenter() const
{
    if(centerDirty)
    {
        if(numVertices == 0)
        {
            centerVector = Vector3f(0,0,0);
        }
        else
        {
            Vector3f max,min;
            max = min = vertices[0];
            for(int i = 1; i < numVertices; i++)
            {
                if(vertices[i].x > max.x)
                max.x = vertices[i].x;
                else if(vertices[i].x < min.x)
                min.x = vertices[i].x;
                if(vertices[i].y > max.y)
                max.y = vertices[i].y;
                else if(vertices[i].y < min.y)
                min.y = vertices[i].y;
                if(vertices[i].z > max.z)
                max.z = vertices[i].z;
                else if(vertices[i].z < min.z)
                min.z = vertices[i].z;
            }
            centerVector = (max+min)/2;
        }
        centerDirty = false;
    }
    return centerVector;
};

Vector3f DriverModel::getBounds() const
{
    if(boundsDirty)
    {
        if(numVertices < 2)
        {
            boundsVector = Vector3f(0,0,0);
        }
        else
        {
            Vector3f max,min;
            max = min = vertices[0];
            for(int i = 1; i < numVertices; i++)
            {
                if(vertices[i].x > max.x)
                max.x = vertices[i].x;
                else if(vertices[i].x < min.x)
                min.x = vertices[i].x;
                if(vertices[i].y > max.y)
                max.y = vertices[i].y;
                else if(vertices[i].y < min.y)
                min.y = vertices[i].y;
                if(vertices[i].z > max.z)
                max.z = vertices[i].z;
                else if(vertices[i].z < min.z)
                min.z = vertices[i].z;
            }
            boundsVector = max-min;
        }
        boundsDirty = false;
    }
    return boundsVector;
};

float DriverModel::getBoundingCircleRadius() const
{
    return boundingCircleRadius;
};

float DriverModel::getBoundingSphereRadius() const
{
    return boundingSphereRadius;
};

int DriverModel::getNumTexturesUsed() const
{
    return numTexturesUsed;
};

int DriverModel::getTextureUsed(int idx) const
{
    if(idx >= 0 && idx < numTexturesUsed)
    return texturesUsed[idx];
    return -1;
};

void DriverModel::recalculateTexturesUsed()
{
    int textures[256];
    int num = 0;
    for(int i = 0; i < numFaces; i++)
    {
        if(faces[i].getTexture() != -1)
        {
            bool found = false;
            for(int j = 0; j < num; j++)
            {
                if(textures[j] == faces[i].getTexture())
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                textures[num] = faces[i].getTexture();
                num++;
            }
        }
    }
    if(texturesUsed)
    delete[] texturesUsed;
    numTexturesUsed = num;
    texturesUsed = NULL;
    if(numTexturesUsed > 0)
    {
        texturesUsed = new unsigned char[numTexturesUsed];
        for(int i = 0; i < numTexturesUsed; i++)
        {
            texturesUsed[i] = textures[i];
        }
    }
};

int DriverModel::getNumFaces() const
{
    return numFaces;
};

ModelFace DriverModel::getFace(int idx) const
{
    ModelFace ret;
    if(idx >= 0 && idx < numFaces)
    ret = faces[idx];
    return ret;
};

void DriverModel::setFace(int idx, ModelFace face)
{
    if(idx >= 0 && idx < numFaces)
    faces[idx] = face;
};

ModelContainer::ModelContainer()
{
    numModels = 0;
    models = NULL;
};

ModelContainer::~ModelContainer()
{
    cleanup();
    eventManager.Raise(EVENT(IDriverModelEvents::modelsDestroyed)(this));
};

void ModelContainer::cleanup()
{
    eventManager.Raise(EVENT(IDriverModelEvents::modelsReset)(this, true));
    if(models)
    {
        for(int i = 0; i < numModels; i++)
        {
            delete models[i];
        }
        delete[] models;
    }
    models = NULL;
    numModels = 0;
    eventManager.Raise(EVENT(IDriverModelEvents::modelsReset)(this, false));
};

int ModelContainer::dereferenceModel(DriverModel* mod)
{
    if(!mod)
    return 4;

    if(mod->getModelReference() < -1 || mod->getModelReference() >= numModels)
    return 5;

    return mod->dereferenceModel(models[mod->getModelReference()]);
};

int ModelContainer::dereferenceModel(int idx)
{
    if(idx >= 0 && idx < numModels)
    return dereferenceModel(models[idx]);
    return 5;
};

int ModelContainer::getNumModels()
{
    return numModels;
};

DriverModel* ModelContainer::getModel(int idx)
{
    if(idx >= 0 && idx < numModels)
    return models[idx];
    return NULL;
};

const DriverModel* ModelContainer::getModel(int idx) const
{
    if(idx >= 0 && idx < numModels)
    return models[idx];
    return NULL;
};

void ModelContainer::appendModel()
{
    insertModel(numModels);
};

void ModelContainer::insertModel(int idx)
{
    if(idx >= 0 && idx <= numModels)
    {
        DriverModel** newModels = new DriverModel*[numModels+1];
        newModels[idx] = new DriverModel();
        if(models)
        {
            memcpy(newModels,models,idx*sizeof(DriverModel*));
            memcpy(&newModels[idx+1],&models[idx],(numModels-idx)*sizeof(DriverModel*));
            delete[] models;
        }
        models = newModels;
        numModels++;

        eventManager.Raise(EVENT(IDriverModelEvents::modelInserted)(this, idx));
    }
};

DriverModel* ModelContainer::getReferencedModel(DriverModel* in)
{
    if(!in)
    return NULL;

    DriverModel* mr = in;
    while(mr->getModelReference() != -1)
    {
        if(mr->getModelReference() >= 0 && mr->getModelReference() < numModels && getModel(mr->getModelReference()) != mr)
        mr = getModel(mr->getModelReference());
        else return NULL;
    }
    return mr;
};

const DriverModel* ModelContainer::getReferencedModel(const DriverModel* in) const
{
    if(!in)
    return NULL;

    const DriverModel* mr = in;
    while(mr->getModelReference() != -1)
    {
        if(mr->getModelReference() >= 0 && mr->getModelReference() < numModels && getModel(mr->getModelReference()) != mr)
        mr = getModel(mr->getModelReference());
        else return NULL;
    }
    return mr;
};

int ModelContainer::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numModels,4,1,handle);
    size -= 4;
    log->Log(DEBUG_LEVEL_NORMAL, "Loading %d models...", numModels);

    models = new DriverModel*[numModels];
    for(int i = 0; i < numModels; i++)
    {
        models[i] = new DriverModel;
    }

    unsigned char* tempData = new unsigned char[0x200000]; //Initial 2MB size
    int dataSize = 0x200000;
    int modelSize;

    for(int i = 0; i < numModels; i++)
    {
        callbacks->read(&modelSize,4,1,handle);
        unsigned int pos = callbacks->tell(handle);
        log->Log(DEBUG_LEVEL_VERBOSE,"Loading model %d (%d bytes) at location %x...", i, modelSize, pos);

        if(modelSize > dataSize)
        {
            delete[] tempData;
            dataSize = modelSize;
            tempData = new unsigned char[dataSize];
        }
        callbacks->read(tempData,1,modelSize,handle);
        log->increaseIndent();
        int actualSize = models[i]->convertFromLevelFormat(tempData,modelSize,log);
        log->decreaseIndent();
        if(actualSize < 0)
        {
            log->Log("ERROR: Model size exceeded size of data.");
            delete[] tempData;
            cleanup();
            return 2;
        }
        else if(actualSize != modelSize)
        {
            log->Log(DEBUG_LEVEL_NORMAL,"WARNING: Actual size of model data (%d) does not match expected size (%d).",actualSize,modelSize);
        }
    }
    delete[] tempData;

    eventManager.Raise(EVENT(IDriverModelEvents::modelsOpened)(this));
    return 0;
};

unsigned int ModelContainer::getRequiredSize()
{
    int size = 0;
    for(int i = 0; i < numModels; i++)
    {
        size += models[i]->getRequiredSize();
    }
    return 4+4*numModels+size;
};

int ModelContainer::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    eventManager.Raise(EVENT(IDriverModelEvents::modelsSaved)(this, true));

    callbacks->write(&numModels,4,1,handle);

    unsigned char* tempData = new unsigned char[0x200000]; //Initial 2MB size
    int dataSize = 0x200000;
    int modelSize;

    for(int i = 0; i < numModels; i++)
    {
        modelSize = models[i]->getRequiredSize();
        callbacks->write(&modelSize,4,1,handle);
        if(modelSize > dataSize)
        {
            delete[] tempData;
            dataSize = modelSize;
            tempData = new unsigned char[dataSize];
        }
        models[i]->convertToLevelFormat(tempData);
        callbacks->write(tempData,1,modelSize,handle);
    }
    delete[] tempData;
    eventManager.Raise(EVENT(IDriverModelEvents::modelsSaved)(this, false));
    return 0;
};

void ModelContainer::registerEventHandler(IDriverModelEvents* handler)
{
    eventManager.Register(handler);
};

void ModelContainer::unregisterEventHandler(IDriverModelEvents* handler)
{
    eventManager.Unregister(handler);
};
