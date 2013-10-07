#include "ModelViewPanel.hpp"

ModelViewPanel::ModelViewPanel(QWidget * parent, const QGLWidget * shareWidget, Qt::WindowFlags f, DebugLogger* logger) : QSplitter(parent)
{
    level = NULL;

    if(logger)
        log = logger;
    else log = &dummy;

    flags1Enabled = false;
    flags2Enabled = false;
    vertsEnabled = false;
    facesEnabled = false;
    boundsEnabled = false;
    modelRefEnabled = false;

    modelView = new ModelView(this, shareWidget, f, log);

    namesListModel = new DriverModelListModel(false,this);
    namesProxyModel = new QSortFilterProxyModel(this);
    namesProxyModel->setSourceModel(namesListModel);
    namesProxyModel->setSortRole(Qt::UserRole);
    namesList = new QTableView(this);
    namesList->setModel(namesProxyModel);
    namesList->setContextMenuPolicy(Qt::CustomContextMenu);
    namesList->setSortingEnabled(true);
    namesList->sortByColumn(0,Qt::AscendingOrder);
    namesList->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    namesList->verticalHeader()->hide();

    eventNamesModel = new DriverModelListModel(true,this);
    eventProxyModel = new QSortFilterProxyModel(this);
    eventProxyModel->setSourceModel(eventNamesModel);
    eventProxyModel->setSortRole(Qt::UserRole);
    eventNamesList = new QTableView(this);
    eventNamesList->setModel(eventProxyModel);
    eventNamesList->setContextMenuPolicy(Qt::CustomContextMenu);
    eventNamesList->setSortingEnabled(true);
    eventNamesList->sortByColumn(0,Qt::AscendingOrder);
    eventNamesList->verticalHeader()->hide();
    eventNamesList->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    setColumnVisibility();
    columnSelector = new ModelListColumnDialog(this);
    connect(columnSelector, SIGNAL(flags1StateChanged(int)), this, SLOT(setFlags1ColumnState(int)));
    connect(columnSelector, SIGNAL(flags2StateChanged(int)), this, SLOT(setFlags2ColumnState(int)));
    connect(columnSelector, SIGNAL(vertsStateChanged(int)), this, SLOT(setVertsColumnState(int)));
    connect(columnSelector, SIGNAL(facesStateChanged(int)), this, SLOT(setFacesColumnState(int)));
    connect(columnSelector, SIGNAL(boundsStateChanged(int)), this, SLOT(setBoundsColumnState(int)));
    connect(columnSelector, SIGNAL(modelRefStateChanged(int)), this, SLOT(setModelRefColumnState(int)));

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

    selectColumns = new QAction(tr("&Select columns"), this);
    connect(selectColumns, SIGNAL(triggered()), columnSelector, SLOT(exec()));

    headerContext = new QMenu(this);
    headerContext->addAction(selectColumns);

    QVBoxLayout* vertical = new QVBoxLayout();
    vertical->addWidget(tabs);
    vertical->addWidget(renderOptions);
    vertical->setContentsMargins(0, 0, 0, 0);

    verticalWidget = new QWidget(this);
    verticalWidget->setContentsMargins(0, 0, 0, 0);
    verticalWidget->setLayout(vertical);
    verticalWidget->setMinimumWidth(200);
    modelView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QSplitterHandle *handlePointer = handle(1);
    QVBoxLayout *layout = new QVBoxLayout(handlePointer);
    layout->setSpacing(0);
    layout->setMargin(2);

    QFrame *line = new QFrame(handlePointer);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    addWidget(modelView);
    addWidget(verticalWidget);

    setStretchFactor(0,1);
    setStretchFactor(1,0);

    modelView->show();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(namesList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doModelsContextMenu(const QPoint&)));
    connect(eventNamesList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doEventModelsContextMenu(const QPoint&)));
    connect(namesList->horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doListHeaderContextMenu(const QPoint&)));
    connect(eventNamesList->horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doListHeaderContextMenu(const QPoint&)));
    connect(namesList->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(modelIndexChanged(const QModelIndex&, const QModelIndex&)));
    connect(eventNamesList->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(eventModelIndexChanged(const QModelIndex&, const QModelIndex&)));
};

ModelViewPanel::~ModelViewPanel()
{
    if(level)
    {
        level->unregisterEventHandler(this);
        level->models.unregisterEventHandler(this);
        level->eventModels.unregisterEventHandler(this);
    }
};

void ModelViewPanel::loadSettings()
{
    QSettings settings;
    flags1Enabled = settings.value("ModelViewPanel/flagsColumnEnabled",false).toBool();
    flags2Enabled = settings.value("ModelViewPanel/additionalFlagsColumnEnabled",false).toBool();
    vertsEnabled = settings.value("ModelViewPanel/vertexColumnEnabled",false).toBool();
    facesEnabled = settings.value("ModelViewPanel/facesColumnEnabled",false).toBool();
    boundsEnabled = settings.value("ModelViewPanel/boundsColumnEnabled",false).toBool();
    modelRefEnabled = settings.value("ModelViewPanel/modelReferenceColumnEnabled",false).toBool();
    setColumnVisibility();

    columnSelector->setFlags1CheckState((flags1Enabled ? Qt::Checked : Qt::Unchecked));
    columnSelector->setFlags2CheckState((flags2Enabled ? Qt::Checked : Qt::Unchecked));
    columnSelector->setVertsCheckState((vertsEnabled ? Qt::Checked : Qt::Unchecked));
    columnSelector->setFacesCheckState((facesEnabled ? Qt::Checked : Qt::Unchecked));
    columnSelector->setBoundsCheckState((boundsEnabled ? Qt::Checked : Qt::Unchecked));
    columnSelector->setModelRefCheckState((modelRefEnabled ? Qt::Checked : Qt::Unchecked));

    restoreState(settings.value("ModelViewPanel/splitterSizes").toByteArray());

    modelView->loadSettings();
};

void ModelViewPanel::saveSettings()
{
    QSettings settings;
    settings.setValue("ModelViewPanel/flagsColumnEnabled",flags1Enabled);
    settings.setValue("ModelViewPanel/additionalFlagsColumnEnabled",flags2Enabled);
    settings.setValue("ModelViewPanel/vertexColumnEnabled",vertsEnabled);
    settings.setValue("ModelViewPanel/facesColumnEnabled",facesEnabled);
    settings.setValue("ModelViewPanel/boundsColumnEnabled",boundsEnabled);
    settings.setValue("ModelViewPanel/modelReferenceColumnEnabled",modelRefEnabled);

    settings.setValue("ModelViewPanel/splitterSizes", saveState());

    modelView->saveSettings();
};

ModelView* ModelViewPanel::glViewer()
{
    return modelView;
};

void ModelViewPanel::setLevel(DriverLevel* lev)
{
    if(level)
    {
        level->unregisterEventHandler(this);
        level->models.unregisterEventHandler(this);
        level->eventModels.unregisterEventHandler(this);
    }
    level = lev;
    namesListModel->setLevel(level);
    eventNamesModel->setLevel(level);
    referenceDialog->setLevel(level);
    modelView->setLevel(level);
    if(level)
    {
        level->registerEventHandler(this);
        level->models.registerEventHandler(this);
        level->eventModels.registerEventHandler(this);
    }
};

void ModelViewPanel::setTextureProvider(TextureList* list)
{
    modelView->setTextureProvider(list);
};

void ModelViewPanel::tabChanged(int newTab)
{
    if(newTab == 0) //Regular models
    {
        modelView->viewEventModels(false);
    }
    else if(newTab == 1)
    {
        modelView->viewEventModels(true);
    }
};

void ModelViewPanel::modelIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    modelView->setModelIndex(namesProxyModel->mapToSource(current).row());
};

void ModelViewPanel::eventModelIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    modelView->setEventModelIndex(eventProxyModel->mapToSource(current).row());
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

void ModelViewPanel::doListHeaderContextMenu(const QPoint& point)
{
    headerContext->exec(QCursor::pos());
};

void ModelViewPanel::setColumnVisibility()
{
    namesList->setColumnHidden(2,!flags1Enabled);
    namesList->setColumnHidden(3,!flags2Enabled);
    namesList->setColumnHidden(4,!vertsEnabled);
    namesList->setColumnHidden(5,!facesEnabled);
    namesList->setColumnHidden(6,!boundsEnabled);
    namesList->setColumnHidden(7,!modelRefEnabled);

    eventNamesList->setColumnHidden(1,!flags1Enabled);
    eventNamesList->setColumnHidden(2,!flags2Enabled);
    eventNamesList->setColumnHidden(3,!vertsEnabled);
    eventNamesList->setColumnHidden(4,!facesEnabled);
    eventNamesList->setColumnHidden(5,!boundsEnabled);
    eventNamesList->setColumnHidden(6,!modelRefEnabled);

    namesList->resizeColumnsToContents();
    namesList->resizeRowsToContents();

    eventNamesList->resizeColumnsToContents();
    eventNamesList->resizeRowsToContents();
};

void ModelViewPanel::setFlags1ColumnState(int state)
{
    if(state == Qt::Checked)
        flags1Enabled = true;
    else flags1Enabled = false;
    setColumnVisibility();
};

void ModelViewPanel::setFlags2ColumnState(int state)
{
    if(state == Qt::Checked)
        flags2Enabled = true;
    else flags2Enabled = false;
    setColumnVisibility();
};

void ModelViewPanel::setVertsColumnState(int state)
{
    if(state == Qt::Checked)
        vertsEnabled = true;
    else vertsEnabled = false;
    setColumnVisibility();
};

void ModelViewPanel::setFacesColumnState(int state)
{
    if(state == Qt::Checked)
        facesEnabled = true;
    else facesEnabled = false;
    setColumnVisibility();
};

void ModelViewPanel::setBoundsColumnState(int state)
{
    if(state == Qt::Checked)
        boundsEnabled = true;
    else boundsEnabled = false;
    setColumnVisibility();
};

void ModelViewPanel::setModelRefColumnState(int state)
{
    if(state == Qt::Checked)
        modelRefEnabled = true;
    else modelRefEnabled = false;
    setColumnVisibility();
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
    level->eventModels.insertModel(index);
    eventNamesModel->insertRow(index);
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

void ModelViewPanel::modelsDestroyed(ModelContainer* container)
{
    if(container == &level->models)
    {
        level->models.unregisterEventHandler(this);
    }
    else
    {
        level->eventModels.unregisterEventHandler(this);
    }
};

void ModelViewPanel::modelsReset(ModelContainer* container, bool aboutToBe)
{
    if(!aboutToBe)
    {
        if(container == &level->models)
        {
            namesListModel->resetList();
        }
        else
        {
            eventNamesModel->resetList();
        }
    }
};

void ModelViewPanel::modelsOpened(ModelContainer* container)
{
    if(container == &level->models)
    {
        namesListModel->resetList();
        namesList->resizeColumnsToContents();
        namesList->resizeRowsToContents();
    }
    else
    {
        eventNamesModel->resetList();
        eventNamesList->resizeColumnsToContents();
        eventNamesList->resizeRowsToContents();
    }
};

void ModelViewPanel::modelsSaved(ModelContainer* container, bool aboutToBe)
{
    if(container == &level->models)
    {

    }
    else
    {

    }
};

void ModelViewPanel::modelInserted(ModelContainer* container, int idx)
{
    if(container == &level->models)
    {
        namesListModel->resetList();
    }
    else
    {
        eventNamesModel->resetList();
    }
};

void ModelViewPanel::levelDestroyed()
{
    level->models.unregisterEventHandler(this);
    level->eventModels.unregisterEventHandler(this);
    level = NULL;
};

