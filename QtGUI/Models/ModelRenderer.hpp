#ifndef MODEL_RENDERER_HPP
#define MODEL_RENDERER_HPP

#include <QtOpenGL>
#include <vector>
#include "../../Driver_Routines/DriverLevels/models.hpp"
#include "../TextureList.hpp"
#include "../../Log_Routines/debug_logger.hpp"

#define APIENTRY __stdcall
#define GLAPIENTRY APIENTRY

typedef void (GLAPIENTRY * PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (GLAPIENTRY * PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void (GLAPIENTRY * PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);

extern PFNGLBINDVERTEXARRAYPROC myGlBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC myGlDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC myGlGenVertexArrays;

using namespace std;

typedef struct MyGLVertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
} MyGLVertex;

typedef struct MyGLVertex_Tex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
    GLfloat s0;
    GLfloat t0;
} MyGLVertex_Tex;

typedef struct MyGLVertex_Norm
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
} MyGLVertex_Norm;

typedef struct MyGLVertex_Norm_Tex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
    GLfloat s0;
    GLfloat t0;
} MyGLVertex_Norm_Tex;

class ModelShaders
{
    public:
        ModelShaders(QGLContext* context, DebugLogger* logger = NULL);

        void bind(bool normal, bool textured);
        void release();

        void setProjectionMatrix(QMatrix4x4 projection);
        void setViewMatrix(QMatrix4x4 view);
        void setModelMatrix(QMatrix4x4 model);

        int initializeShaders();

    protected:

        QGLShaderProgram colored;
        QGLShaderProgram colored_normal;
        QGLShaderProgram colored_textured;
        QGLShaderProgram colored_normal_textured;
        QGLShaderProgram* shaders[4];

        DebugLogger dummy;
        DebugLogger* log;
};

class ModelMatrixHandler
{
    public:
        ModelMatrixHandler(QGLContext* context_p, bool useLegacy, ModelShaders* program);

        void useLegacyRendering(bool use);
        void setPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
        void setFrustum(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom, GLdouble zNear, GLdouble zFar);
        void setView(QMatrix4x4 newView);
        void setModelPosition(float x, float y, float z, float xRot, float yRot, float zRot);

    protected:
        void applyProjectionMatrix();
        void applyViewMatrix();
        void applyModelMatrix();

        ModelShaders* shaders;
        QGLContext* context;
        bool useLegacy;

        QMatrix4x4 projection;
        QMatrix4x4 view;
        QMatrix4x4 model;
};

const unsigned int CAMERA_DEFAULT = 0;     //Standard camera where the position is the camera's position.
const unsigned int CAMERA_FOCAL_POINT = 1; //Camera where the camera points in at position at a certain distance away.
const unsigned int CAMERA_MAX_MODE = 2;

class BasicCamera
{
    public:
        BasicCamera();
        QMatrix4x4* viewMatrix();
        void setMatrixHandler(ModelMatrixHandler* handler);
        void setCameraMode(unsigned int nmode);

        void setPosition(double nx, double ny, double nz);
        void setRotation(double nyaw, double npitch, double nroll);
        void setYaw(double nyaw);
        void setPitch(double npitch);
        void setRoll(double nroll);
        void setDistance(double dist);

        void addPosition(double nx, double ny, double nz);
        void addRotation(double nyaw, double npitch, double nroll);
        void addYaw(double nyaw);
        void addPitch(double npitch);
        void addRoll(double nroll);
        void addDistance(double dist);

        double getXPosition();
        double getYPosition();
        double getZPosition();

        double getYaw();
        double getPitch();
        double getRoll();
        double getDistance();

        unsigned int getCameraMode();

    protected:
        void buildMatrix();

        unsigned int mode;

        ModelMatrixHandler* matrixHandler;
        double x, y, z;
        double yaw, pitch, roll;
        double distance;
        QMatrix4x4 cameraMatrix;
};

class ModelTextureGroup
{
    public:
        int texture;
        int start[4];
        int end[4];
};

class ModelRenderer : public QGLFunctions
{
    public:
        ModelRenderer(QGLContext* glcontext, bool useLegacy, ModelShaders* _shaders = NULL, DebugLogger* logger = NULL);
        ~ModelRenderer();
        void cleanup();

        void useLegacyRendering(bool use);
        void buildRenderData(const DriverModel* model, const DriverModel* reference);
        void render(int group = -1);
        int getNumGroups();
        int getTextureUsed(int idx);
        static bool hasMissingFunctions();

    protected:
        void initializeMissingFunctions();

        DebugLogger dummy;
        DebugLogger* log;
        bool legacyRendering;
        bool hasNonTextured;
        ModelTextureGroup* groups;
        int numTextureGroups;
        ModelShaders* shaders;
        GLuint iboId;
        GLuint vboIds[4];
        GLuint vaoIds[4];

        //The following are used for building model and are kept around if legacy rendering is enabled for drawing.
        vector<MyGLVertex> vertices;
        vector<MyGLVertex_Norm> verticesNorm;
        vector<MyGLVertex_Tex> verticesTex;
        vector<MyGLVertex_Norm_Tex> verticesNormTex;
        GLushort* indicies;
};

class ModelRendererList
{
    public:
        ModelRendererList();
        ~ModelRendererList();
    protected:
        ModelRenderer** renderList;
};

#endif // MODEL_RENDERER_HPP
