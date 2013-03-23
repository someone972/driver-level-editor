#include "DriverModelView.hpp"

ModelNameList::ModelNameList(DriverLevel* lev,QObject* parent) : QAbstractTableModel(parent)
{
    level = lev;
};

void ModelNameList::setLevel(DriverLevel* lev)
{
    level = lev;
    beginResetModel();
    endResetModel();
};

int ModelNameList::rowCount(const QModelIndex &/*parent*/) const
{
    if(!level)
    return 0;
    else return level->models.getNumModels();
};

int ModelNameList::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
};

QVariant ModelNameList::data(const QModelIndex &index, int role) const
{
    if(role == Qt::SizeHintRole)
    {
        return QSize(10,10);
    }
    else if(role == Qt::BackgroundColorRole && level)
    {
        if(index.row() >= 0 && index.row() < level->models.getNumModels())
        {
            if(level->models.getModel(index.row())->getModelReference() != -1)
            return QColor(255,100,100);
            else return QColor(255,255,255);
        }
    }
    else if((role == Qt::EditRole || role == Qt::DisplayRole) && level)
    {
        switch(index.column())
        {
            case 0:
                return QString(level->modelNames.getName(index.row()));
            case 1:
                return QString("0x%1").arg(level->models.getModel(index.row())->flags2,8,16,QChar('0'));
            default:
                return QVariant();
        }
    }
    return QVariant();
};


QVariant ModelNameList::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
            case 0:
                return QString(tr("Model Name"));
            case 1:
                return QString(tr("Flags"));
            default:
                return QVariant();
        }
    }
    else
        return QString("%1").arg(section);
}

Qt::ItemFlags ModelNameList::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable|  Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool ModelNameList::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        if(level)
        {
            level->modelNames.setName(index.row(),value.toString().toLocal8Bit().data());
        }
    }
    return true;
};

void ModelNameList::updateRow(int row)
{
    emit dataChanged(index(row,0),index(row,columnCount()));
};

bool ModelNameList::insertRow(int row, const QModelIndex & parent)
{
    return insertRows(row,1,parent);
};

bool ModelNameList::insertRows(int row, int count, const QModelIndex & parent)
{
    if(level)
    {
        if(row >= 0 && row < level->modelNames.getMaxNumNames()-1)
        {
            emit beginInsertRows(parent,row,row+count-1);
            for(int i = 0; i < count; i++)
            {
                level->modelNames.insertName(row,"");
            }
            emit endInsertRows();
            return true;
        }
    }
    return false;
};

void ModelNameList::resetList()
{
    beginResetModel();
    endResetModel();
};

EventModelList::EventModelList(DriverLevel* lev,QObject* parent) : QAbstractTableModel(parent)
{
    level = lev;
    beginResetModel();
    endResetModel();
};

void EventModelList::setLevel(DriverLevel* lev)
{
    level = lev;
    beginResetModel();
    endResetModel();
};

int EventModelList::rowCount(const QModelIndex &/*parent*/) const
{
    if(!level)
    return 0;
    else return level->eventModels.getNumModels();
};

int EventModelList::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
};

QVariant EventModelList::data(const QModelIndex &index, int role) const
{
    if(role == Qt::SizeHintRole)
    {
        return QSize(10,10);
    }
    else if(role == Qt::BackgroundColorRole && level)
    {
        if(index.row() >= 0 && index.row() < level->eventModels.getNumModels())
        {
            if(level->eventModels.getModel(index.row())->getModelReference() != -1)
            return QColor(255,100,100);
            else return QColor(255,255,255);
        }
    }
    else if(role == Qt::DisplayRole)
    {
        return QString(tr("Event Model "))+QString::number(index.row());
    }
    return QVariant();
};

QVariant EventModelList::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString(tr("Model Index"));
    else
        return QString("%1").arg(section);
};

void EventModelList::prepareForInsert(int row, int count)
{
    emit beginInsertRows(QModelIndex(),row,row+count-1);
};

void EventModelList::finishInsert()
{
    emit endInsertRows();
};

Qt::ItemFlags EventModelList::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
};

void EventModelList::updateRow(int row)
{
    emit dataChanged(index(row,0),index(row,columnCount()));
};

void EventModelList::resetList()
{
    beginResetModel();
    endResetModel();
};

ModelView::ModelView(QWidget * parent, const QGLWidget * shareWidget, Qt::WindowFlags f, DebugLogger* logger) : QGLWidget(parent, shareWidget, f)
{
    if(logger)
        log = logger;
    else log = &dummy;

    level = NULL;
    textures = NULL;
    render = NULL;
    dragging = false;
    modelIndex = -1;
    eventModelIndex = -1;
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

void ModelView::setLevel(DriverLevel* lev)
{
    level = lev;
    if(level)
    {
        if(level->models.getNumModels() > 0)
        {
            if(render)
            render->buildRenderData(level->models.getModel(90), level->models.getReferencedModel(level->models.getModel(90)));
            updateGL();
        cout<<"YOLO"<<endl;
        }
    }
};

void ModelView::setModelIndex(int idx)
{
    modelIndex = idx;
    if(render)
    {
        if(modelIndex >= 0 && modelIndex < level->models.getNumModels())
        {
            const DriverModel* model = level->models.getModel(modelIndex);
            if(model)
            {
                const DriverModel* referencedModel = level->models.getReferencedModel(model);
                Vector3f center = referencedModel->getCenter();
                camera.setPosition(center.x, center.y, center.z);
                camera.setDistance(referencedModel->getBoundingSphereRadius()*3.0f);
                render->buildRenderData(model, referencedModel);
            }
            else render->cleanup();
        }
    }
    else render->cleanup();
    updateGL();
};

void ModelView::setEventModelIndex(int idx)
{
    eventModelIndex = idx;

    updateGL();
};

void ModelView::setTextureProvider(TextureList* list)
{
    textures = list;
};

void ModelView::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        lastPoint = event->pos();
        dragging = true;
    }
};

void ModelView::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragging = false;
    }
};

void ModelView::mouseMoveEvent(QMouseEvent* event)
{
    if(dragging)
    {
        QPoint delta = lastPoint-event->pos();
        camera.addYaw(delta.x());
        camera.addPitch(delta.y());
        lastPoint = event->pos();
        updateGL();
    }
};

void ModelView::wheelEvent(QWheelEvent* event)
{
    camera.addDistance(-10*event->delta());
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
            cout<<i<<" "<<render->getTextureUsed(i)<<endl;
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
                    glBindTexture(GL_TEXTURE_2D, textures->getTexture(render->getTextureUsed(i)));
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

ModelViewPanel::ModelViewPanel(QWidget * parent, const QGLWidget * shareWidget, Qt::WindowFlags f, DebugLogger* logger) : QWidget(parent)
{
    level = NULL;

    if(logger)
        log = logger;
    else log = &dummy;

    modelView = new ModelView(this, shareWidget, f, log);

    namesListModel = new ModelNameList(NULL,this);
    namesList = new QTableView(this);
    namesList->setModel(namesListModel);
    namesList->setContextMenuPolicy(Qt::CustomContextMenu);

    eventNamesModel = new EventModelList(NULL,this);
    eventNamesList = new QTableView(this);
    eventNamesList->setModel(eventNamesModel);
    eventNamesList->setContextMenuPolicy(Qt::CustomContextMenu);

    tabs = new QTabWidget(this);
    tabs->addTab(namesList,tr("&Models"));
    tabs->addTab(eventNamesList,tr("&Event Models"));

    renderOptions = new RenderOptionsWidget(this);
//    renderOptions->connectModelView(modelView);
    referenceDialog = new ModelReferenceDialog(this);
    connect(referenceDialog, SIGNAL(modelChanged(int)), namesListModel, SLOT(updateRow(int)));
    connect(referenceDialog, SIGNAL(eventModelChanged(int)), eventNamesModel, SLOT(updateRow(int)));

    rename = new QAction(tr("&Rename"),this);
    connect(rename,SIGNAL(triggered()),this,SLOT(renameCurrentModel()));
    deleteModel = new QAction(tr("&Delete"),this);
    connect(deleteModel,SIGNAL(triggered()),this,SLOT(deleteCurrentModel()));
    insertModelBefore = new QAction(tr("Insert model &before"),this);
    connect(insertModelBefore,SIGNAL(triggered()),this,SLOT(insertModelBeforeCurrent()));
    insertModelAfter = new QAction(tr("Insert model &after"),this);
    connect(insertModelAfter,SIGNAL(triggered()),this,SLOT(insertModelAfterCurrent()));
    importModel = new QAction(tr("&Import model here"),this);
    connect(importModel,SIGNAL(triggered()),this,SLOT(importCurrentModel()));
    exportModel = new QAction(tr("&Export"),this);
    connect(exportModel,SIGNAL(triggered()),this,SLOT(exportCurrentModel()));
    makeReference = new QAction(tr("Make re&ference"),this);
    connect(makeReference,SIGNAL(triggered()),this,SLOT(currentModelMakeReference()));
    dereference = new QAction(tr("Dere&ference"),this);
    connect(dereference,SIGNAL(triggered()),this,SLOT(dereferenceCurrentModel()));
    modelProperties = new QAction(tr("&Properties"),this);
    connect(modelProperties,SIGNAL(triggered()),this,SLOT(openCurrentModelProperties()));

    listContext = new QMenu(this);
    listContext->addAction(rename);
    listContext->addSeparator();
    listContext->addAction(deleteModel);
    listContext->addAction(insertModelBefore);
    listContext->addAction(insertModelAfter);
    listContext->addSeparator();
    listContext->addAction(importModel);
    listContext->addAction(exportModel);
    listContext->addSeparator();
    listContext->addAction(makeReference);
    listContext->addAction(dereference);
    listContext->addAction(modelProperties);

    QVBoxLayout* vertical = new QVBoxLayout();
    vertical->addWidget(tabs);
    vertical->addWidget(renderOptions);

    QHBoxLayout* horizontal = new QHBoxLayout();
    horizontal->addWidget(modelView);
    horizontal->addLayout(vertical);

    setLayout(horizontal);
    modelView->show();
    modelView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(namesList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doModelsContextMenu(const QPoint&)));
    connect(eventNamesList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doEventModelsContextMenu(const QPoint&)));
    connect(namesList->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(modelIndexChanged(const QModelIndex&, const QModelIndex&)));
    connect(eventNamesList->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(eventModelIndexChanged(const QModelIndex&, const QModelIndex&)));
};

ModelView* ModelViewPanel::glViewer()
{
    return modelView;
}

void ModelViewPanel::setLevel(DriverLevel* lev)
{
    level = lev;
    namesListModel->setLevel(level);
    eventNamesModel->setLevel(level);
    referenceDialog->setLevel(level);
    modelView->setLevel(level);
};

void ModelViewPanel::setTextureProvider(TextureList* list)
{
    modelView->setTextureProvider(list);
};

void ModelViewPanel::handleLevelChange()
{
    namesListModel->resetList();
    eventNamesModel->resetList();
};

void ModelViewPanel::modelIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    modelView->setModelIndex(current.row());
};

void ModelViewPanel::eventModelIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    modelView->setEventModelIndex(current.row());
};

void ModelViewPanel::doModelsContextMenu(const QPoint& point)
{
    if(level)
    {
        rename->setVisible(true);
        savedIndex = namesList->indexAt(point);
        int idx = savedIndex.row();
        if(idx >= 0 && idx < level->models.getNumModels())
        {
            if(level->models.getModel(idx)->getModelReference() == -1)
            {
                makeReference->setVisible(true);
                dereference->setVisible(false);
            }
            else
            {
                makeReference->setVisible(false);
                dereference->setVisible(true);
            }
            listContext->exec(QCursor::pos());
        }
    }
};

void ModelViewPanel::doEventModelsContextMenu(const QPoint& point)
{
    if(level)
    {
        rename->setVisible(false);
        savedIndex = namesList->indexAt(point);
        int idx = savedIndex.row();
        if(idx >= 0 && idx < level->eventModels.getNumModels())
        {
            if(level->eventModels.getModel(idx)->getModelReference() == -1)
            {
                makeReference->setVisible(true);
                dereference->setVisible(false);
            }
            else
            {
                makeReference->setVisible(false);
                dereference->setVisible(true);
            }
            listContext->exec(QCursor::pos());
        }
    }
};

void ModelViewPanel::renameCurrentModel()
{
    namesList->setCurrentIndex(savedIndex);
    namesList->edit(savedIndex);
};

void ModelViewPanel::dereferenceCurrentModel()
{
    if(level)
    {
        int ret = (tabs->currentIndex() == 0 ? level->models.dereferenceModel(savedIndex.row()) : level->eventModels.dereferenceModel(savedIndex.row()));
        if(ret != 0)
        {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(tr("<b>Failed to dereference model!</b>"));

            switch(ret)
            {
                case 1:
                    msgBox.setInformativeText(tr("Model referenced was passed as NULL!"));
                    break;
                case 2:
                    msgBox.setInformativeText(tr("Number of vertices for referenced model do not match current model!"));
                    break;
                case 3:
                    msgBox.setInformativeText(tr("Model is not referencing another model!"));
                    break;
                case 4:
                    msgBox.setInformativeText(tr("Current model was passed as NULL!"));
                    break;
                case 5:
                    msgBox.setInformativeText(tr("Model index is invalid!"));
                    break;
            }
            msgBox.exec();
            return;
        }
        if(tabs->currentIndex() == 0)
        {
            namesListModel->updateRow(savedIndex.row());
            emit modelChanged(savedIndex.row());
        }
        else
        {
            eventNamesModel->updateRow(savedIndex.row());
            emit eventModelChanged(savedIndex.row());
        }
    }
};

void ModelViewPanel::currentModelMakeReference()
{
    if(tabs->currentIndex() == 0)
    referenceDialog->doDialog(savedIndex.row(),false);
    else referenceDialog->doDialog(savedIndex.row(),true);
};

void ModelViewPanel::deleteCurrentModel()
{
    if(tabs->currentIndex() == 0)
    {

    }
    else
    {

    }
};

void ModelViewPanel::insertModelBeforeCurrent()
{
    if(tabs->currentIndex() == 0)
    insertModel(savedIndex.row());
    else insertEventModel(savedIndex.row());
};

void ModelViewPanel::insertModelAfterCurrent()
{
    if(tabs->currentIndex() == 0)
    insertModel(savedIndex.row()+1);
    else insertEventModel(savedIndex.row()+1);

};

void ModelViewPanel::insertEventModel(int index)
{
    eventNamesModel->prepareForInsert(index,1);
    level->eventModels.insertModel(index);
    eventNamesModel->finishInsert();
    emit eventModelInserted(index);
};

void ModelViewPanel::insertModel(int index)
{
    if(level)
    {
        level->models.insertModel(index);
        namesListModel->insertRow(index);
        for(int i = 0; i < level->heightmapTiles.getNumTiles(); i++)
        {
            int modelIndex = level->heightmapTiles.getTile(i)->getModelIndex();
            if(modelIndex >= index)
            {
                level->heightmapTiles.getTile(i)->setModelIndex(modelIndex+1);
                emit heightmapTileChanged(i);
            }
        }

        bool changed = false;
        for(int i = 0; i < level->world.getNumBridgedDefs(); i++)
        {
            WorldModelDef* def = level->world.getBridgedDef(i);
            if(def->getModelIndex() >= index)
            {
                def->setModelIndex(def->getModelIndex()+1);
                changed = true;
            }
        }
        if(changed)
        emit worldSectorChanged(-1);
        changed = false;

        for(int i = 0; i < level->world.getNumSectors(); i++)
        {
            WorldSector* sector = level->world.getSector(i);

            bool changed = false;
            for(int j = 0; j < sector->getNumModelDefs(); j++)
            {
                WorldModelDef* def = sector->getModelDef(j);
                if(def->getModelIndex() >= index)
                {
                    def->setModelIndex(def->getModelIndex()+1);
                    changed = true;
                }
            }
            if(changed)
            emit worldSectorChanged(i);
        }
        emit modelInserted(index);
    }
};

void ModelViewPanel::exportCurrentModel()
{

};

void ModelViewPanel::importCurrentModel()
{

};

void ModelViewPanel::openCurrentModelProperties()
{

};

