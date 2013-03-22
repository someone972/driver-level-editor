#ifndef MODEL_NAMES_HPP
#define MODEL_NAMES_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../Log_Routines/debug_logger.hpp"
#include "../../vector.hpp"

using namespace std;

const int FACE_TEXTURED = 0x01;
const int FACE_QUAD = 0x02;
const int FACE_NORMAL = 0x04;
const int FACE_VERTEX_NORMAL = 0x08;
const int FACE_VERTEX_RGB = 0x10;

extern const int faceTypeConversion[56];
extern const int faceTypeSize[56];

struct color_3ub
{
    unsigned char r,g,b,a;
};

class ModelNames
{
    public:
        ModelNames();
        ~ModelNames();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int findName(const char* name);
        const char* getName(int idx);
        void setName(int idx,const char* name);
        void insertName(int idx,const char* name);
        void appendName(const char* name);

        int getMaxNumNames();

    protected:
        void rebuildNameIndex();
        char* data;
        const char** nameIndex;
        int dataSize;
        //we call it maxNumNames because there may be more nulls than actual models
        int maxNumNames;
};

class ModelCollisionBound
{
    public:
        ModelCollisionBound();
        short type;
        Vector3s position;
        Vector3s rotation;
        Vector3s length;
};

class ModelFace
{
    public:
        ModelFace();

        static int calculateType(int _flags);

        void setTexture(int tex);
        void setNormal(int idx);
        void setNormals(int idx1, int idx2, int idx3, int idx4 = 0);
        void setVertices(int idx1, int idx2, int idx3, int idx4 = 0);
        void setTexCoords(Vector2f t1, Vector2f t2, Vector2f t3, Vector2f t4 = Vector2f());
        void setColor(color_3ub color);
        void setColors(color_3ub c1, color_3ub c2, color_3ub c3);
        void setColors(color_3ub c1, color_3ub c2, color_3ub c3, color_3ub c4);

        int getTexture();
        int getVertexIndex(int idx);
        int getNormalIndex(int idx);
        color_3ub getColor(int idx);
        Vector2f getTexCoord(int idx);
        bool hasAttribute(unsigned int flag);

        int type,flags;
        int texture;
        int vertexIndicies[4];
        int normalIndicies[4];
        Vector2f textureCoords[4];
        color_3ub colors[4];
};

class DriverModel
{
    public:
        DriverModel();
        ~DriverModel();
        void cleanup();

        int convertFromLevelFormat(unsigned char* data,int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize() const;
        void convertToLevelFormat(unsigned char* data) const; //Warning: assumes that the data buffer is at least getRequiredSize() big.

        int getModelReference() const;
        int createReferenceToModel(int modelIdx,DriverModel* mod);
        int dereferenceModel(DriverModel* referenced);

        int getNumVertices() const;
        Vector3f getVertex(int idx) const;

        int getNumNormals() const;
        Vector3f getNormal(int idx) const;

        int getNumTexturesUsed() const;
        int getTextureUsed(int idx) const;
        void recalculateTexturesUsed();

        int getNumFaces() const;
        ModelFace getFace(int idx) const;
        void setFace(int idx, ModelFace face);

        Vector3f getCenter() const;
        Vector3f getBounds() const; //gets x,y,z lengths, relative to the center point.
        float getBoundingCircleRadius() const; //radius ignoring y component, i.e. as a top down orthographic circle
        float getBoundingSphereRadius() const; //radius taking y into account

        unsigned int flags1,flags2;
    protected:
        int numVertices;
        Vector3f* vertices;

        int numNormals;
        Vector3f* normals;

        int numFaces;
        ModelFace* faces;
        Vector4f* cullingNormals;

        int numTexturesUsed;
        unsigned char* texturesUsed;

        int numCollisionBounds;
        ModelCollisionBound* collisionBounds;

        float boundingSphereRadius;
        float boundingCircleRadius;

        //Declared as mutable to allow for recalculating only upon request, even in const models
        mutable Vector3f centerVector,boundsVector;
        mutable bool centerDirty,boundsDirty;

        int modelRef;
};

class ModelContainer
{
    public:
        ModelContainer();
        ~ModelContainer();
        void cleanup();
        int load(IOHandle handle, IOCallbacks* callbacks,int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumModels();
        DriverModel* getModel(int idx);
        const DriverModel* getModel(int idx) const;
        void insertModel(int idx);
        void appendModel();

        DriverModel* getReferencedModel(DriverModel* mod);
        const DriverModel* getReferencedModel(const DriverModel* mod) const;
        int dereferenceModel(DriverModel* mod);
        int dereferenceModel(int idx);
    protected:
        int numModels;
        DriverModel** models;
};

#endif
