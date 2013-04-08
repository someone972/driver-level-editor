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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    shaders = new ModelShaders(context(), log);
    matrixHandler = new ModelMatrixHandler(context(), shaders);
    render = new ModelRenderer(context(), shaders, log);
    camera.setMatrixHandler(matrixHandler);
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

            render->render(i);
        }
    }
};

RenderOptionsWidget::RenderOptionsWidget(QWidget* parent) : QWidget(parent)
{

};

ModelReferenceDialog::ModelReferenceDialog(QWidget* parent) : QDialog(parent)
{
    level = NULL;
    eventModel = false;
    modelIndex = -1;

    okButton = new QPushButton(tr("Finish"),this);
    cancelButton = new QPushButton(tr("Cancel"),this);
    autoButton = new QPushButton(tr("Auto-detect"),this);
    modelIndexBox = new QSpinBox(this);
    modelIndexBox->setMinimum(0);
    modelIndexBox->setMaximum(0);
    indexLabel = new QLabel(tr("Model Index: "),this);

    QHBoxLayout* indexLayout = new QHBoxLayout();
    indexLayout->addWidget(indexLabel);
    indexLayout->addWidget(modelIndexBox);
    indexLayout->addWidget(autoButton);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* masterLayout = new QVBoxLayout();
    masterLayout->addLayout(indexLayout);
    masterLayout->addLayout(buttonLayout);
    setLayout(masterLayout);

    connect(autoButton, SIGNAL(clicked()), this, SLOT(autoDetect()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(makeReference()));
};

void ModelReferenceDialog::setLevel(DriverLevel* lev)
{
    level = lev;
};

void ModelReferenceDialog::autoDetect()
{
    if(level)
    {
        if(modelIndex >= 0 && modelIndex < (eventModel ? level->eventModels.getNumModels() : level->models.getNumModels()))
        {
            DriverModel* original = (eventModel ? level->eventModels.getModel(modelIndex) : level->models.getModel(modelIndex));
            if(original)
            {
                int numModels = (eventModel ? level->eventModels.getNumModels() : level->models.getNumModels());
                for(int i = 0; i < numModels; i++)
                {
                    if(i != modelIndex)
                    {
                        DriverModel* reference = (eventModel ? level->eventModels.getModel(i) : level->models.getModel(i));
                        if(reference->getNumVertices() == original->getNumVertices() && reference->getModelReference() == -1)
                        {
                            bool match = true;
                            for(int j = 0; j < reference->getNumVertices(); j++)
                            {
                                if(reference->getVertex(j) != original->getVertex(j))
                                {
                                    match = false;
                                    break;
                                }
                            }
                            if(match)
                            {
                                modelIndexBox->setValue(i);
                                return;
                            }
                        }
                    }
                }
            }
        }
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("No suitable reference model found!"));
        msgBox.exec();
    }
};

void ModelReferenceDialog::makeReference()
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(tr("<b>Could not create model reference!</b>"));

    if(level)
    {
        if(modelIndex >= 0 && modelIndex < (eventModel ? level->eventModels.getNumModels() : level->models.getNumModels()))
        {
            if(modelIndexBox->value() >= 0 && modelIndexBox->value() < (eventModel ? level->eventModels.getNumModels() : level->models.getNumModels()))
            {
                DriverModel* original = (eventModel ? level->eventModels.getModel(modelIndex) : level->models.getModel(modelIndex));
                DriverModel* reference = (eventModel ? level->eventModels.getModel(modelIndexBox->value()) : level->models.getModel(modelIndexBox->value()));
                if(original && reference)
                {
                    if(reference->getNumVertices() == original->getNumVertices())
                    {
                        bool match = true;
                        DriverModel* mr = (eventModel ? level->eventModels.getReferencedModel(reference) : level->models.getReferencedModel(reference));
                        if(mr)
                        {
                            for(int j = 0; j < mr->getNumVertices(); j++)
                            {
                                if(mr->getVertex(j) != original->getVertex(j))
                                {
                                    match = false;
                                    break;
                                }
                            }
                        }
                        else match = false;
                        if(match)
                        {
                            int ret = original->createReferenceToModel(modelIndexBox->value(),reference);
                            if(ret != 0)
                            {
                                switch(ret)
                                {
                                    case 1:
                                        msgBox.setInformativeText(tr("Model reference is NULL!"));
                                        break;
                                    case 2:
                                        msgBox.setInformativeText(tr("Number of vertices do not match!"));
                                        break;
                                    case 3:
                                        msgBox.setInformativeText(tr("Model reference index is invalid!"));
                                        break;
                                }
                                msgBox.exec();
                            }
                            else
                            {
                                if(eventModel)
                                emit eventModelChanged(modelIndex);
                                else emit modelChanged(modelIndex);
                            }
                            hide();
                            return;
                        }
                    }
                    msgBox.setInformativeText(tr("Vertices do not match!"));
                    msgBox.exec();
                    hide();
                    return;
                }
            }
        }
    }
    msgBox.setInformativeText(tr("Model index is invalid!"));
    msgBox.exec();
    hide();
};

void ModelReferenceDialog::doDialog(int modelIdx,bool event)
{
    eventModel = event;
    modelIndex = modelIdx;
    if(level)
    modelIndexBox->setMaximum((eventModel ? level->eventModels.getNumModels() : level->models.getNumModels())-1);
    else modelIndexBox->setMaximum(0);
    exec();
};

ModelListColumnDialog::ModelListColumnDialog(QWidget* parent) : QDialog(parent)
{
    flags1CheckBox = new QCheckBox(tr("Flags"),this);
    flags2CheckBox = new QCheckBox(tr("Additional Flags"),this);
    vertsCheckBox = new QCheckBox(tr("Vertex Count"),this);
    facesCheckBox = new QCheckBox(tr("Face Count"),this);
    boundsCheckBox = new QCheckBox(tr("Collision Bound Count"),this);
    modelRefCheckBox = new QCheckBox(tr("Model Reference"),this);

    doneButton = new QPushButton(tr("Done"),this);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(flags1CheckBox, 0, 0);
    layout->addWidget(flags2CheckBox, 1, 0);
    layout->addWidget(modelRefCheckBox, 2, 0);
    layout->addWidget(vertsCheckBox, 0, 1);
    layout->addWidget(facesCheckBox, 1, 1);
    layout->addWidget(boundsCheckBox, 2, 1);
    layout->addWidget(doneButton, 3, 0, 1, -1, Qt::AlignHCenter);
    setLayout(layout);

    connect(flags1CheckBox, SIGNAL(stateChanged(int)), this, SLOT(emitFlags1Signal(int)));
    connect(flags2CheckBox, SIGNAL(stateChanged(int)), this, SLOT(emitFlags2Signal(int)));
    connect(vertsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(emitVertsSignal(int)));
    connect(facesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(emitFacesSignal(int)));
    connect(boundsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(emitBoundsSignal(int)));
    connect(modelRefCheckBox, SIGNAL(stateChanged(int)), this, SLOT(emitModelRefSignal(int)));
    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept()));
};

void ModelListColumnDialog::setFlags1CheckState(Qt::CheckState state)
{
    flags1CheckBox->setCheckState(state);
};

void ModelListColumnDialog::setFlags2CheckState(Qt::CheckState state)
{
    flags2CheckBox->setCheckState(state);
};

void ModelListColumnDialog::setVertsCheckState(Qt::CheckState state)
{
    vertsCheckBox->setCheckState(state);
};

void ModelListColumnDialog::setFacesCheckState(Qt::CheckState state)
{
    facesCheckBox->setCheckState(state);
};

void ModelListColumnDialog::setBoundsCheckState(Qt::CheckState state)
{
    boundsCheckBox->setCheckState(state);
};

void ModelListColumnDialog::setModelRefCheckState(Qt::CheckState state)
{
    modelRefCheckBox->setCheckState(state);
};

void ModelListColumnDialog::emitFlags1Signal(int state)
{
    emit flags1StateChanged(state);
};

void ModelListColumnDialog::emitFlags2Signal(int state)
{
    emit flags2StateChanged(state);
};

void ModelListColumnDialog::emitVertsSignal(int state)
{
    emit vertsStateChanged(state);
};

void ModelListColumnDialog::emitFacesSignal(int state)
{
    emit facesStateChanged(state);
};

void ModelListColumnDialog::emitBoundsSignal(int state)
{
    emit boundsStateChanged(state);
};

void ModelListColumnDialog::emitModelRefSignal(int state)
{
    emit modelRefStateChanged(state);
};
