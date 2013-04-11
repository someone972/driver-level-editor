#include "ModelView.hpp"

ModelView::ModelView(QWidget * parent, const QGLWidget * shareWidget, Qt::WindowFlags f, DebugLogger* logger) : QGLWidget(parent, shareWidget, f)
{
    if(logger)
        log = logger;
    else log = &dummy;

    level = NULL;
    textures = NULL;
    render = NULL;
    matrixHandler = NULL;
    shaders = NULL;

    modelIndex = -1;
    eventModelIndex = -1;
    viewingEvent = false;

    wheelSensitivity = 10.0;
    mouseSensitivity = 1.0;
    zoomSensitivity = 10.0;

    camera.setCameraMode(CAMERA_FOCAL_POINT);
    camera.setDistance(2000);
    camera.setYaw(45.0);
    camera.setPitch(45.0);
    camera.setRoll(180.0);
};

ModelView::~ModelView()
{
    if(render)
        delete render;
    if(shaders)
        delete shaders;
    if(matrixHandler)
        delete matrixHandler;
};

void ModelView::loadSettings()
{
    QSettings settings;
    wheelSensitivity = settings.value("ModelView/wheelSensitivity", 10.0).toDouble();
    mouseSensitivity = settings.value("ModelView/mouseSensitivity", 1.0).toDouble();
    zoomSensitivity = settings.value("ModelView/zoomSensitivity", 10.0).toDouble();
};

void ModelView::saveSettings()
{
    QSettings settings;
    settings.setValue("ModelView/wheelSensitivity", wheelSensitivity);
    settings.setValue("ModelView/mouseSensitivity", mouseSensitivity);
    settings.setValue("ModelView/zoomSensitivity", zoomSensitivity);
    settings.setValue("legacyRendering", legacyRendering);
};

void ModelView::setLevel(DriverLevel* lev)
{
    level = lev;
    if(level)
    {

    }
};

void ModelView::setModelIndex(int idx)
{
    modelIndex = idx;
    rebuildModelRenderer();
    updateGL();
};

void ModelView::setEventModelIndex(int idx)
{
    eventModelIndex = idx;
    rebuildModelRenderer();
    updateGL();
};

void ModelView::viewEventModels(bool view)
{
    viewingEvent = view;
    rebuildModelRenderer();
    updateGL();
};

void ModelView::rebuildModelRenderer()
{
    if(level)
    {
        ModelContainer* container;
        int index;
        if(viewingEvent)
        {
            container = &level->eventModels;
            index = eventModelIndex;
        }
        else
        {
            container = &level->models;
            index = modelIndex;
        }
        if(render)
        {
            if(index >= 0 && index < container->getNumModels())
            {
                const DriverModel* model = container->getModel(index);
                if(model)
                {
                    const DriverModel* referencedModel = container->getReferencedModel(model);
                    Vector3f center = referencedModel->getCenter();
                    camera.setPosition(-center.x, -center.y, -center.z);
                    camera.setDistance(referencedModel->getBoundingSphereRadius()*3.0f);
                    render->buildRenderData(model, referencedModel);
                }
                else render->cleanup();
            }
        }
        else render->cleanup();
    }
    updateGL();
};

void ModelView::setTextureProvider(TextureList* list)
{
    textures = list;
};

void ModelView::mousePressEvent(QMouseEvent* event)
{
    //need to set up last position before creating delta.
    lastPoint = event->pos();
};

void ModelView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = lastPoint-event->pos();

    if(event->buttons() & Qt::LeftButton)
    {
        camera.addYaw(mouseSensitivity*delta.x());
        camera.addPitch(mouseSensitivity*delta.y());
    }
    else if(event->buttons() & Qt::RightButton)
    {
        camera.addDistance(zoomSensitivity*delta.y());
    }
    lastPoint = event->pos();
    updateGL();
};

void ModelView::wheelEvent(QWheelEvent* event)
{
    camera.addDistance(-wheelSensitivity*event->delta());
    updateGL();
};

void ModelView::initializeGL()
{
    //Ensure that we get settings before initializing.
    if(log)
        log->Log("Setting up model view...");
    QSettings settings;
    legacyRendering = settings.value("legacyRendering", false).toBool();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    shaders = new ModelShaders(context(), log);
    if(!legacyRendering)
    {
        if(log)
            log->Log("Checking for compatiblity with modern OpenGL.");
        if(shaders->initializeShaders() != 0 || ModelRenderer::hasMissingFunctions() == false)
        {
            legacyRendering = true;
            settings.setValue("legacyRendering", legacyRendering);
            if(log)
                log->Log("Failed to meet minimum requirements for modern OpenGL, reverting to legacy rendering.");
        }
        else if(log)
        {
            log->Log("Passed compatibility test.");
        }
    }
    else if(log)
    {
        log->Log("Using legacy rendering.");
    }

    if(log)
        log->Log("Setting up matrix handler.");
    matrixHandler = new ModelMatrixHandler(context(), legacyRendering, shaders);
    if(log)
        log->Log("Setting up model renderer.");
    render = new ModelRenderer(context(), legacyRendering, shaders, log);
    if(log)
        log->Log("Setting up camera.");
    camera.setMatrixHandler(matrixHandler);
    if(log)
        log->Log("Finished setting up model view.");
};

void ModelView::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    matrixHandler->setPerspective(45.0,(double)width/(double)height, 1.0, 100000.0);
};

void ModelView::paintGL()
{
    glClearColor(0.5,0.7,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(render)
    {
        for(int i = 0; i < render->getNumGroups(); i++)
        {
            if(render->getTextureUsed(i) == -1)
            {
                glDisable(GL_TEXTURE_2D);
            }
            else
            {
                if(textures)
                {
                    const DriverTexture* tex = level->textures.getTexture(render->getTextureUsed(i));
                    if(tex)
                    {
                        if(tex->getFlags()&TEX_HAS_TRANSPARENCY)
                        {
                            glEnable(GL_ALPHA_TEST);
                            glAlphaFunc(GL_GREATER, 0.5);
                            //TODO: disable alpha test
                        }
                    }
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textures->getTexture(render->getTextureUsed(i),textures->getCurrentPalette(render->getTextureUsed(i))));
                }
            }
            glCullFace(GL_BACK);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(-1,-100);
            render->render(i);
            glPolygonMode(GL_BACK,GL_LINE);
            glCullFace(GL_FRONT);
            glDisable(GL_POLYGON_OFFSET_FILL);
            render->render(i);
            glCullFace(GL_BACK);
        }
    }
};

RenderOptionsWidget::RenderOptionsWidget(QWidget* parent) : QWidget(parent)
{

};
