#include "SaveAsDialog.hpp"

LevelSaveOptions::LevelSaveOptions(QWidget* parent) : QDialog(parent)
{
    QString labelList[] = {"Textures","Models","World","Unused","Random Placements","Texture Definitions","Unused","Road Table","Road Connections","Intersectinos",
                           "Heightmap Tiles","Heightmaps","Model Names","Event Models","Visibility","Sector Texture Usage","Road Sections","Intersection Positions",
                           "Unused","Lamps","Chair Placement"};

    for(int i = 0; i < 21; i++)
    {
        labels[i] = new QLabel(labelList[i],this);
        checkBoxes[i] = new QCheckBox(this);
    }
    selectAll();

    doneButton = new QPushButton(tr("&Done"),this);
    doneButton->setFixedWidth(60);
    selectAllButton = new QPushButton(tr("&Select All"),this);
    selectAllButton->setFixedWidth(100);
    selectNoneButton = new QPushButton(tr("D&eselect All"),this);
    selectNoneButton->setFixedWidth(100);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(selectAllButton,0,0,1,2,Qt::AlignCenter);
    layout->addWidget(selectNoneButton,0,2,1,2,Qt::AlignHCenter);
    layout->addWidget(doneButton,12,0,1,-1,Qt::AlignHCenter);
    for(int i = 0; i < 11; i++)
    {
        layout->addWidget(checkBoxes[i],i+1,0);
        layout->addWidget(labels[i],i+1,1);
    }
    for(int i = 0; i < 10; i++)
    {
        layout->addWidget(checkBoxes[i+11],i+1,2);
        layout->addWidget(labels[i+11],i+1,3);
    }
    setLayout(layout);

    connect(selectAllButton,SIGNAL(clicked()),this,SLOT(selectAll()));
    connect(selectNoneButton,SIGNAL(clicked()),this,SLOT(deselectAll()));
    connect(doneButton,SIGNAL(clicked()),this,SLOT(accept()));
};

void LevelSaveOptions::selectAll()
{
    for(int i = 0; i < 21; i++)
    {
        checkBoxes[i]->setCheckState(Qt::Checked);
    }
};

void LevelSaveOptions::deselectAll()
{
    for(int i = 0; i < 21; i++)
    {
        checkBoxes[i]->setCheckState(Qt::Unchecked);
    }
};

unsigned int LevelSaveOptions::getBitfield()
{
    unsigned int temp = 0;
    for(int i = 0; i < 21; i++)
    {
        if(checkBoxes[i]->checkState() == Qt::Checked)
        temp |= (1<<i);
    }
    return temp;
};

void LevelSaveOptions::setBitfield(unsigned int saveWhat)
{
    for(int i = 0; i < 21; i++)
    {
        checkBoxes[i]->setCheckState(((saveWhat&(1<<i)) != 0 ? Qt::Checked : Qt::Unchecked));
    }
};

SaveAsDialog::SaveAsDialog(QWidget* parent) : QDialog(parent)
{
    QString labelNames[] = {tr("Level: "), tr("D3D: "), tr("Player Denting: "), tr("Civilian Denting: "), tr("Player Cosmetics"), tr("Civilian Cosmetics: "), tr("Wheel Definitions: ")};

    QGridLayout* layout = new QGridLayout();
    for(int i = 0; i < 7; i++)
    {
        labels[i] = new QLabel(labelNames[i],this);
        buttons[i] = new QPushButton(tr("Browse"),this);
        filenames[i] = new QLineEdit(this);
        checkBoxes[i] = new QCheckBox(this);
        checkBoxes[i]->setCheckState(Qt::Checked);

        layout->addWidget(checkBoxes[i],i,0);
        layout->addWidget(labels[i],i,1);
        layout->addWidget(filenames[i],i,2);
        layout->addWidget(buttons[i],i,3);
    }
    levelOptionButton = new QPushButton(tr("Options"),this);
    saveSelected = new QPushButton(tr("&Save Selected"),this);
    saveSelected->setFixedWidth(100);
    cancelButton = new QPushButton(tr("Cancel"),this);
    cancelButton->setFixedWidth(100);
    layout->addWidget(levelOptionButton,0,4);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveSelected);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout,7,0,1,-1);

    levOptions = new LevelSaveOptions(this);

    setLayout(layout);

    connect(buttons[0], SIGNAL(clicked()), this, SLOT(browseLevel()));
    connect(buttons[1], SIGNAL(clicked()), this, SLOT(browseD3D()));
    connect(buttons[2], SIGNAL(clicked()), this, SLOT(browsePlayerDen()));
    connect(buttons[3], SIGNAL(clicked()), this, SLOT(browseCivilianDen()));
    connect(buttons[4], SIGNAL(clicked()), this, SLOT(browsePlayerCos()));
    connect(buttons[5], SIGNAL(clicked()), this, SLOT(browseCivilianCos()));
    connect(buttons[6], SIGNAL(clicked()), this, SLOT(browseWheels()));
    connect(levelOptionButton, SIGNAL(clicked()), this, SLOT(doLevelSaveOptions()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(saveSelected, SIGNAL(clicked()), this, SLOT(sendSaveSignals()));
};

void SaveAsDialog::doLevelSaveOptions()
{
    levOptions->exec();
};

void SaveAsDialog::sendSaveSignals()
{
    if(checkBoxes[0]->checkState() == Qt::Checked && !filenames[0]->text().isEmpty())
    emit saveLevel(filenames[0]->text(),levOptions->getBitfield());

    if(checkBoxes[1]->checkState() == Qt::Checked && !filenames[1]->text().isEmpty())
    emit saveD3D(filenames[1]->text());

    if(checkBoxes[2]->checkState() == Qt::Checked && !filenames[2]->text().isEmpty())
    emit savePlayerDenting(filenames[2]->text());

    if(checkBoxes[3]->checkState() == Qt::Checked && !filenames[3]->text().isEmpty())
    emit saveCivilianDenting(filenames[3]->text());

    if(checkBoxes[4]->checkState() == Qt::Checked && !filenames[4]->text().isEmpty())
    emit savePlayerCosmetics(filenames[4]->text());

    if(checkBoxes[5]->checkState() == Qt::Checked && !filenames[5]->text().isEmpty())
    emit saveCivilianCosmetics(filenames[5]->text());

    if(checkBoxes[6]->checkState() == Qt::Checked && !filenames[6]->text().isEmpty())
    emit saveWheelDefinitions(filenames[6]->text());

    accept();
};

void SaveAsDialog::browseLevel()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save level as..."), filenames[0]->text(), tr("Level Files (*.lev);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[0]->setText(temp);
};

void SaveAsDialog::browseD3D()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save D3D as..."), filenames[1]->text(), tr("D3D Files (*.d3d);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[1]->setText(temp);
};

void SaveAsDialog::browsePlayerDen()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save player denting as..."), filenames[2]->text(), tr("Denting Files (*.den);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[2]->setText(temp);
};

void SaveAsDialog::browseCivilianDen()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save civilian denting as..."), filenames[3]->text(), tr("Denting Files (*.den);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[3]->setText(temp);
};

void SaveAsDialog::browsePlayerCos()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save player cosmetics as..."), filenames[4]->text(), tr("Cosmetic Files (*.cos);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[4]->setText(temp);
};

void SaveAsDialog::browseCivilianCos()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save civilian cosmetics as..."), filenames[5]->text(), tr("Cosmetic Files (*.cos);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[5]->setText(temp);
};

void SaveAsDialog::browseWheels()
{
    QString temp = QFileDialog::getSaveFileName(this, tr("Save wheel definitions as..."), filenames[6]->text(), tr("Wheel Definition Files (*.wdf);;All Files (*.*)"));
    if(!temp.isEmpty())
    filenames[6]->setText(temp);
};

void SaveAsDialog::setLevelFilename(QString filename)
{
    filenames[0]->setText(filename);
};

void SaveAsDialog::setD3DFilename(QString filename)
{
    filenames[1]->setText(filename);
};

void SaveAsDialog::setPlayerDentingFilename(QString filename)
{
    filenames[2]->setText(filename);
};

void SaveAsDialog::setCivilianDentingFilename(QString filename)
{
    filenames[3]->setText(filename);
};

void SaveAsDialog::setPlayerCosmeticsFilename(QString filename)
{
    filenames[4]->setText(filename);
};

void SaveAsDialog::setCivilianCosmeticsFilename(QString filename)
{
    filenames[5]->setText(filename);
};

void SaveAsDialog::setWheelDefinitionFilename(QString filename)
{
    filenames[6]->setText(filename);
};
