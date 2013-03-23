#include "ModelRenderer.hpp"

PFNGLBINDVERTEXARRAYPROC myGlBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC myGlDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYSPROC myGlGenVertexArrays = NULL;


ModelShaders::ModelShaders(QGLContext* context, DebugLogger* logger) :
    colored(context, NULL), colored_normal(context, NULL), colored_textured(context, NULL), colored_normal_textured(context, NULL)
{
    if(logger)
        log = logger;
    else log = &dummy;

    shaders[0] = &colored;
    shaders[1] = &colored_normal;
    shaders[2] = &colored_textured;
    shaders[3] = &colored_normal_textured;

    initializeShaders();
};

void ModelShaders::initializeShaders()
{
    QString vertexShaderFilenames[4] = {QString("data/shaders/PC.vert"), QString("data/shaders/PCN.vert"), QString("data/shaders/PCT.vert"), QString("data/shaders/PCNT.vert")};
    QString fragmentShaderFilenames[4] = {QString("data/shaders/PC.frag"), QString("data/shaders/PCN.frag"), QString("data/shaders/PCT.frag"), QString("data/shaders/PCNT.frag")};

    log->Log("Attempting to create shaders.");

    for(int i = 0; i < 4; i++)
    {
        bool success = shaders[i]->addShaderFromSourceFile(QGLShader::Vertex, vertexShaderFilenames[i]);
        if(!success)
        {
            log->Log("WARNING: Could not create vertex shader %d: %s", i, shaders[i]->log().toLocal8Bit().data());
        }

        success = shaders[i]->addShaderFromSourceFile(QGLShader::Fragment, fragmentShaderFilenames[i]);
        if(!success)
        {
            log->Log("WARNING: Could not create fragment shader %d: %s", i, shaders[i]->log().toLocal8Bit().data());
        }

        success = shaders[i]->link();
        if(!success)
        {
            log->Log("WARNING: Could not link shader program %d: %s", i, shaders[i]->log().toLocal8Bit().data());
        }

        success = shaders[i]->bind();
        if(!success)
        {
            log->Log("WARNING: Could not bind shader program %d!", i);
        }
    }
    colored_textured.setUniformValue("colorTexture",0);
    colored_normal_textured.setUniformValue("colorTexture",0);

    log->Log("Finished creating shaders.");
};

void ModelShaders::bind(bool normal, bool textured)
{
    bool success = shaders[normal+textured*2]->bind();
    if(!success)
    {
        log->Log("WARNING: Could not bind shader program (%s, %s)!", (normal ? "has normal" : "no normal"), (textured ? "has texture" : "no texture"));
    }
};

void ModelShaders::release()
{
    for(int i = 0; i < 4; i++)
    shaders[i]->release();
};

void ModelShaders::setProjectionMatrix(QMatrix4x4 projection)
{
    for(int i = 0; i < 4; i++)
    {
        shaders[i]->bind();
        shaders[i]->setUniformValue("ProjectionMatrix", projection);
    }
};

void ModelShaders::setViewMatrix(QMatrix4x4 view)
{
    for(int i = 0; i < 4; i++)
    {
        shaders[i]->bind();
        shaders[i]->setUniformValue("ViewMatrix", view);
    }
};

void ModelShaders::setModelMatrix(QMatrix4x4 model)
{
    for(int i = 0; i < 4; i++)
    {
        shaders[i]->bind();
        shaders[i]->setUniformValue("ModelMatrix", model);
    }
};

ModelMatrixHandler::ModelMatrixHandler(QGLContext* glcontext, ModelShaders* program)
{
    shaders = program;
    context = glcontext;
    useLegacy = false;
    model.setToIdentity();
    projection.setToIdentity();
    view.setToIdentity();
    applyProjectionMatrix();
    applyViewMatrix();
    applyModelMatrix();
};

void ModelMatrixHandler::useLegacyRendering(bool use)
{
    useLegacy = use;
};

void ModelMatrixHandler::setPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    projection.setToIdentity();
    projection.perspective(fovy,aspect,zNear,zFar);
    applyProjectionMatrix();
};

void ModelMatrixHandler::setFrustum(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom, GLdouble zNear, GLdouble zFar)
{
    projection.setToIdentity();
    projection.frustum(left,right,bottom,top,zNear,zFar);
    applyProjectionMatrix();
};

void ModelMatrixHandler::setView(QMatrix4x4 newView)
{
    view = newView;
    applyViewMatrix();
};

void ModelMatrixHandler::setModelPosition(float x, float y, float z, float xRot, float yRot, float zRot)
{
    model.setToIdentity();
    model.rotate(zRot, 0.0, 0.0, 1.0);
    model.rotate(xRot, 1.0, 0.0, 0.0);
    model.rotate(yRot, 0.0, 1.0, 0.0);
    model.translate(x,y,z);
    applyModelMatrix();
};

void ModelMatrixHandler::applyProjectionMatrix()
{
    if(context)
        context->makeCurrent();
    if(useLegacy == true)
    {
        glMatrixMode(GL_PROJECTION);
        GLdouble mat[16];
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                mat[i*4+j] = projection(j,i);
            }
        }
        glLoadMatrixd(mat);
        glMatrixMode(GL_MODELVIEW);
    }
    else
    {
        if(shaders)
            shaders->setProjectionMatrix(projection);
    }
};

void ModelMatrixHandler::applyViewMatrix()
{
    if(context)
        context->makeCurrent();
    if(useLegacy == true)
    {
        glMatrixMode(GL_MODELVIEW);
        QMatrix4x4 temp = view*model;
        GLdouble mat[16];
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                mat[i*4+j] = temp(j,i);
            }
        }
        glLoadMatrixd(mat);
    }
    else
    {
        if(shaders)
            shaders->setViewMatrix(view);
    }
};

void ModelMatrixHandler::applyModelMatrix()
{
    if(context)
        context->makeCurrent();
    if(useLegacy == true)
    {
        glMatrixMode(GL_MODELVIEW);
        QMatrix4x4 temp = view*model;
        GLdouble mat[16];
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                mat[i*4+j] = temp(j,i);
            }
        }
        glLoadMatrixd(mat);
    }
    else
    {
        if(shaders)
            shaders->setModelMatrix(model);
    }
};

BasicCamera::BasicCamera()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
    yaw = 0.0;
    pitch = 0.0;
    roll = 0.0;
    distance = 0.0;
    mode = CAMERA_DEFAULT;
    matrixHandler = NULL;
};

void BasicCamera::setMatrixHandler(ModelMatrixHandler* handler)
{
    matrixHandler = handler;
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::setCameraMode(unsigned int nmode)
{
    if(nmode < CAMERA_MAX_MODE)
    {
        mode = nmode;
        buildMatrix();

        if(matrixHandler)
            matrixHandler->setView(cameraMatrix);
    }
};

void BasicCamera::setPosition(double nx, double ny, double nz)
{
    x = nx;
    y = ny;
    z = nz;

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::setRotation(double nyaw, double npitch, double nroll)
{
    yaw = fmod(nyaw, 360.0);
    pitch = fmod(npitch, 360.0);
    roll = fmod(nroll, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::setYaw(double nyaw)
{
    yaw = fmod(nyaw, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::setPitch(double npitch)
{
    pitch = fmod(npitch, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::setRoll(double nroll)
{
    roll = fmod(nroll, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::setDistance(double dist)
{
    distance = dist;

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::addPosition(double nx, double ny, double nz)
{
    x += nx;
    y += ny;
    z += nz;
    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::addRotation(double nyaw, double npitch, double nroll)
{
    yaw = fmod(yaw+nyaw, 360.0);
    pitch = fmod(pitch+npitch, 360.0);
    roll = fmod(roll+nroll, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::addYaw(double nyaw)
{
    yaw = fmod(yaw+nyaw, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::addPitch(double npitch)
{
    pitch = fmod(pitch+npitch, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::addRoll(double nroll)
{
    roll = fmod(roll+nroll, 360.0);

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

void BasicCamera::addDistance(double dist)
{
    distance += dist;
    if(distance < 0.0)
        distance = 0.0;

    buildMatrix();
    if(matrixHandler)
        matrixHandler->setView(cameraMatrix);
};

double BasicCamera::getXPosition()
{
    return x;
};

double BasicCamera::getYPosition()
{
    return y;
};

double BasicCamera::getZPosition()
{
    return z;
};

double BasicCamera::getYaw()
{
    return yaw;
};

double BasicCamera::getPitch()
{
    return pitch;
};

double BasicCamera::getRoll()
{
    return roll;
};

double BasicCamera::getDistance()
{
    return distance;
};

unsigned int BasicCamera::getCameraMode()
{
    return mode;
};

void BasicCamera::buildMatrix()
{
    cameraMatrix.setToIdentity();

    switch(mode)
    {
        case CAMERA_DEFAULT:
            cameraMatrix.translate(x,y,z);
            cameraMatrix.rotate(yaw, 0.0, 1.0, 0.0);
            cameraMatrix.rotate(pitch, 1.0, 0.0, 0.0);
            cameraMatrix.rotate(roll, 0.0, 0.0, 1.0);
            break;
        case CAMERA_FOCAL_POINT:
            cameraMatrix.translate(0,0,-distance);
            cameraMatrix.rotate(roll, 0.0, 0.0, 1.0);
            cameraMatrix.rotate(pitch, 1.0, 0.0, 0.0);
            cameraMatrix.rotate(yaw, 0.0, 1.0, 0.0);
            cameraMatrix.translate(x,y,z);
            break;
    }
};

ModelRenderer::ModelRenderer(QGLContext* context, ModelShaders* _shaders, DebugLogger* logger) : QGLFunctions(context)
{
    numTextureGroups = 0;
    hasNonTextured = false;
    groups = NULL;
    iboId = 0;
    vboIds[0] = vboIds[1] = vboIds[2] = vboIds[3] = 0;
    vaoIds[0] = vaoIds[1] = vaoIds[2] = vaoIds[3] = 0;
    shaders = _shaders;

    if(logger)
        log = logger;
    else log = &dummy;
};

ModelRenderer::~ModelRenderer()
{
    cleanup();
};

void ModelRenderer::initializeMissingFunctions()
{
    if(!myGlBindVertexArray || !myGlDeleteVertexArrays || !myGlGenVertexArrays)
    {
        log->Log("Attempting to resolve addresses for missing OpenGL functions.");
        myGlBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        myGlGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        myGlDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
        if(!myGlBindVertexArray || !myGlDeleteVertexArrays || !myGlGenVertexArrays)
        {
            log->Log("WARNING: Could not resolve addresses for glBindVertexArray, glGenVertexAarrays, glDeleteVertexArrays.");
        }
        else log->Log("Addresses resolved successfully!");
    }
};

void ModelRenderer::cleanup()
{
    if(groups)
        delete[] groups;
    groups = NULL;

    hasNonTextured = false;
    numTextureGroups = 0;

    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &iboId);

    if(myGlBindVertexArray)
    myGlBindVertexArray(0);

    for(int i = 0; i < 4; i++)
    {
        if(myGlDeleteVertexArrays)
            myGlDeleteVertexArrays(1, &vaoIds[i]);
        glDeleteBuffers(1, &vboIds[i]);
    }

    iboId = 0;
    vboIds[0] = vboIds[1] = vboIds[2] = vboIds[3] = 0;
    vaoIds[0] = vaoIds[1] = vaoIds[2] = vaoIds[3] = 0;
};

int ModelRenderer::getNumGroups()
{
    return numTextureGroups;
};

int ModelRenderer::getTextureUsed(int idx)
{
    if(idx >= 0 && idx < numTextureGroups)
        return groups[idx].texture;
    return -1;
};

void ModelRenderer::buildRenderData(const DriverModel* model, const DriverModel* reference)
{
    cleanup();
    initializeMissingFunctions();

    if(!model)
    return;

    if(!reference)
    reference = model;

    int numTris[4] = {0, 0, 0, 0};
    hasNonTextured = false;

    for(int i = 0; i < model->getNumFaces(); i++)
    {
        int index = (model->getFace(i).hasAttribute(FACE_NORMAL) || model->getFace(i).hasAttribute(FACE_VERTEX_NORMAL) ? 1 : 0)
                    + (model->getFace(i).hasAttribute(FACE_TEXTURED) ? 2 : 0);
        if(model->getFace(i).hasAttribute(FACE_QUAD))
            numTris[index] += 2;
        else numTris[index]++;
        if(!model->getFace(i).hasAttribute(FACE_TEXTURED))
            hasNonTextured = true;
    }

    int totalTris = numTris[0] + numTris[1] + numTris[2] + numTris[3];

    numTextureGroups = model->getNumTexturesUsed() + (hasNonTextured ? 1 : 0);
    groups = new ModelTextureGroup[numTextureGroups];

    GLushort* indicies = new GLushort[totalTris*3];
    vector<MyGLVertex> vertices;
    vector<MyGLVertex_Norm> vertices_norm;
    vector<MyGLVertex_Tex> vertices_tex;
    vector<MyGLVertex_Norm_Tex> vertices_norm_tex;

    int currentTri[4] = {0, numTris[0], numTris[0]+numTris[1], numTris[0]+numTris[1]+numTris[2]};

    for(int i = 0; i < model->getNumTexturesUsed() + (hasNonTextured ? 1 : 0); i++)
    {
        int texture;

        if(hasNonTextured == false)
            texture = model->getTextureUsed(i);
        else if(i == 0)
            texture = -1;
        else texture = model->getTextureUsed(i-1);

        groups[i].texture = texture;
        for(int j = 0; j < 4; j++)
        {
            groups[i].start[j] = currentTri[j];
        }

        for(int j = 0; j < model->getNumFaces(); j++)
        {
            ModelFace face = model->getFace(j);

            if(face.getTexture() == texture)
            {

                int type = (face.hasAttribute(FACE_NORMAL) || face.hasAttribute(FACE_VERTEX_NORMAL) ? 1 : 0)
                            + (face.hasAttribute(FACE_TEXTURED) ? 2 : 0);
                Vector3f v[4];
                Vector3f n[4];
                Vector2f t[4];
                color_3ub c[4];
                int vertIdx[4];

                v[0] = reference->getVertex(face.getVertexIndex(0));
                v[1] = reference->getVertex(face.getVertexIndex(1));
                v[2] = reference->getVertex(face.getVertexIndex(2));
                if(face.hasAttribute(FACE_QUAD))
                {
                    v[3] = reference->getVertex(face.getVertexIndex(3));
                }

                c[0] = face.getColor(0);
                if(face.hasAttribute(FACE_VERTEX_RGB))
                {
                    c[1] = face.getColor(1);
                    c[2] = face.getColor(2);
                    if(face.hasAttribute(FACE_QUAD))
                    {
                        c[3] = face.getColor(3);
                    }
                }
                else
                {
                    c[1] = c[2] = c[3] = c[0];
                }

                if(face.hasAttribute(FACE_NORMAL))
                {
                    n[0] = model->getNormal(face.getNormalIndex(0));
                    if(face.hasAttribute(FACE_VERTEX_NORMAL))
                    {
                        n[1] = model->getNormal(face.getNormalIndex(1));
                        n[2] = model->getNormal(face.getNormalIndex(2));
                        if(face.hasAttribute(FACE_QUAD))
                        {
                            n[3] = model->getNormal(face.getNormalIndex(3));
                        }
                    }
                    else
                    {
                        n[1] = n[2] = n[3] = n[0];
                    }
                }

                if(face.hasAttribute(FACE_TEXTURED))
                {
                    t[0] = face.getTexCoord(0);
                    t[1] = face.getTexCoord(1);
                    t[2] = face.getTexCoord(2);
                    if(face.hasAttribute(FACE_QUAD))
                    {
                        t[3] = face.getTexCoord(3);
                    }
                }

                MyGLVertex tempVerts[4];
                MyGLVertex_Norm tempVertsNorm[4];
                MyGLVertex_Tex tempVertsTex[4];
                MyGLVertex_Norm_Tex tempVertsNormTex[4];

                switch(type)
                {
                    case 0:
                        memset(tempVerts,0,sizeof(MyGLVertex)*4);
                        for(int k = 0; k < 4; k++)
                        {
                            tempVerts[k].x = v[k].x;
                            tempVerts[k].y = v[k].y;
                            tempVerts[k].z = v[k].z;
                            tempVerts[k].r = c[k].r;
                            tempVerts[k].g = c[k].g;
                            tempVerts[k].b = c[k].b;
                            tempVerts[k].a = 255; //TODO: Adjust alpha for transparent models
                            if(k < 3 || face.hasAttribute(FACE_QUAD))
                            {
                                vertIdx[k] = -1;
                                for(unsigned int l = 0; l < vertices.size(); l++)
                                {
                                    if(memcmp(&vertices[l],&tempVerts[k],sizeof(MyGLVertex)) == 0)
                                    {
                                        vertIdx[k] = l;
                                        break;
                                    }
                                }
                                if(vertIdx[k] < 0)
                                {
                                    vertIdx[k] = vertices.size();
                                    vertices.push_back(tempVerts[k]);
                                }
                            }
                        }
                        break;
                    case 1:
                        memset(tempVertsNorm,0,sizeof(MyGLVertex_Norm)*4);
                        for(int k = 0; k < 4; k++)
                        {
                            tempVertsNorm[k].x = v[k].x;
                            tempVertsNorm[k].y = v[k].y;
                            tempVertsNorm[k].z = v[k].z;
                            tempVertsNorm[k].nx = n[k].x;
                            tempVertsNorm[k].ny = n[k].y;
                            tempVertsNorm[k].nz = n[k].z;
                            tempVertsNorm[k].r = c[k].r;
                            tempVertsNorm[k].g = c[k].g;
                            tempVertsNorm[k].b = c[k].b;
                            tempVertsNorm[k].a = 255; //TODO: Adjust alpha for transparent models
                            if(k < 3 || face.hasAttribute(FACE_QUAD))
                            {
                                vertIdx[k] = -1;
                                for(unsigned int l = 0; l < vertices_norm.size(); l++)
                                {
                                    if(memcmp(&vertices_norm[l],&tempVertsNorm[k],sizeof(MyGLVertex_Norm)) == 0)
                                    {
                                        vertIdx[k] = l;
                                        break;
                                    }
                                }
                                if(vertIdx[k] < 0)
                                {
                                    vertIdx[k] = vertices_norm.size();
                                    vertices_norm.push_back(tempVertsNorm[k]);
                                }
                            }
                        }

                        break;
                    case 2:
                        memset(tempVertsTex,0,sizeof(MyGLVertex_Tex)*4);
                        for(int k = 0; k < 4; k++)
                        {
                            tempVertsTex[k].x = v[k].x;
                            tempVertsTex[k].y = v[k].y;
                            tempVertsTex[k].z = v[k].z;
                            tempVertsTex[k].r = c[k].r;
                            tempVertsTex[k].g = c[k].g;
                            tempVertsTex[k].b = c[k].b;
                            tempVertsTex[k].a = 255; //TODO: Adjust alpha for transparent models
                            tempVertsTex[k].s0 = t[k].x;
                            tempVertsTex[k].t0 = t[k].y;
                            if(k < 3 || face.hasAttribute(FACE_QUAD))
                            {
                                vertIdx[k] = -1;
                                for(unsigned int l = 0; l < vertices_tex.size(); l++)
                                {
                                    if(memcmp(&vertices_tex[l],&tempVertsTex[k],sizeof(MyGLVertex_Tex)) == 0)
                                    {
                                        vertIdx[k] = l;
                                        break;
                                    }
                                }
                                if(vertIdx[k] < 0)
                                {
                                    vertIdx[k] = vertices_tex.size();
                                    vertices_tex.push_back(tempVertsTex[k]);
                                }
                            }
                        }

                        break;
                    case 3:
                        memset(tempVertsNormTex,0,sizeof(MyGLVertex_Norm_Tex)*4);
                        for(int k = 0; k < 4; k++)
                        {
                            tempVertsNormTex[k].x = v[k].x;
                            tempVertsNormTex[k].y = v[k].y;
                            tempVertsNormTex[k].z = v[k].z;
                            tempVertsNormTex[k].nx = n[k].x;
                            tempVertsNormTex[k].ny = n[k].y;
                            tempVertsNormTex[k].nz = n[k].z;
                            tempVertsNormTex[k].r = c[k].r;
                            tempVertsNormTex[k].g = c[k].g;
                            tempVertsNormTex[k].b = c[k].b;
                            tempVertsNormTex[k].a = 255; //TODO: Adjust alpha for transparent models
                            tempVertsNormTex[k].s0 = t[k].x;
                            tempVertsNormTex[k].t0 = t[k].y;
                            if(k < 3 || face.hasAttribute(FACE_QUAD))
                            {
                                vertIdx[k] = -1;
                                for(unsigned int l = 0; l < vertices_norm_tex.size(); l++)
                                {
                                    if(memcmp(&vertices_norm_tex[l],&tempVertsNormTex[k],sizeof(MyGLVertex_Norm_Tex)) == 0)
                                    {
                                        vertIdx[k] = l;
                                        break;
                                    }
                                }
                                if(vertIdx[k] < 0)
                                {
                                    vertIdx[k] = vertices_norm_tex.size();
                                    vertices_norm_tex.push_back(tempVertsNormTex[k]);
                                }
                            }
                        }
                        break;
                }

                indicies[currentTri[type]*3] = vertIdx[0];
                indicies[currentTri[type]*3+1] = vertIdx[1];
                indicies[currentTri[type]*3+2] = vertIdx[2];
                currentTri[type]++;
                if(face.hasAttribute(FACE_QUAD))
                {
                    indicies[currentTri[type]*3] = vertIdx[0];
                    indicies[currentTri[type]*3+1] = vertIdx[2];
                    indicies[currentTri[type]*3+2] = vertIdx[3];
                    currentTri[type]++;
                }
            }
        }
        for(int j = 0; j < 4; j++)
        {
            groups[i].end[j] = currentTri[j];
        }
    }
    //Build VAO, VBO, IBO

    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*totalTris*3, indicies, GL_STATIC_DRAW);

    for(int i = 0; i < 4; i++)
    {
        if(numTris[i] != 0)
        {
            myGlGenVertexArrays(1, &vaoIds[i]);
            myGlBindVertexArray(vaoIds[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

            glGenBuffers(1, &vboIds[i]);
            glBindBuffer(GL_ARRAY_BUFFER, vboIds[i]);

            switch(i)
            {
                case 0:
                    cout<<"V0"<<endl;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(MyGLVertex)*vertices.size(), &vertices[0].x, GL_STATIC_DRAW);

                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyGLVertex), (const void*)0);  //Vertex
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGLVertex), (const void*)12);  //Color
                    break;

                case 1:
                    cout<<"V1"<<endl;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(MyGLVertex_Norm)*vertices_norm.size(), &vertices_norm[0].x, GL_STATIC_DRAW);

                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyGLVertex_Norm), (const void*)0);  //Vertex
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyGLVertex_Norm), (const void*)12);  //Normal
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGLVertex_Norm), (const void*)24);  //Color
                    break;

                case 2:
                    cout<<"V2"<<endl;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(MyGLVertex_Tex)*vertices_tex.size(), &vertices_tex[0].x, GL_STATIC_DRAW);

                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyGLVertex_Tex), (const void*)0);  //Vertex
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGLVertex_Tex), (const void*)12);  //Color
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(3, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGLVertex_Tex), (const void*)16);  //TexCoord
                    break;

                case 3:
                    cout<<"V3"<<endl;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(MyGLVertex_Norm_Tex)*vertices_norm_tex.size(), &vertices_norm_tex[0].x, GL_STATIC_DRAW);

                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyGLVertex_Norm_Tex), (const void*)0);  //Vertex
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyGLVertex_Norm_Tex), (const void*)12);  //Normal
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGLVertex_Norm_Tex), (const void*)24);  //Color
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(3, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGLVertex_Norm_Tex), (const void*)28);  //TexCoord
                    break;
            }
        }
    }

    myGlBindVertexArray(0);

    delete[] indicies;
};

void ModelRenderer::render(int group)
{
    if(group >= 0 && group < numTextureGroups)
    {
        for(int i = 0; i < 4; i++)
        {
            if(vaoIds[i] != 0 && groups[group].end[i]-groups[group].start[i] > 0)
            {
                if(shaders)
                    shaders->bind(i&1, i&2);
                myGlBindVertexArray(vaoIds[i]);
                glDrawElements(GL_TRIANGLES, 3*(groups[group].end[i]-groups[group].start[i]), GL_UNSIGNED_SHORT, (const void*)(3*2*groups[group].start[i]));
            }
        }

    }
};
