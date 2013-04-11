#include "ModelDialogs.hpp"

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
