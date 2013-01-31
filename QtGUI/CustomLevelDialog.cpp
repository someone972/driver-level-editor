#include "CustomLevelDialog.hpp"

QString getPath(QString& in)
{
    for(int i = in.length()-1; i >= 0; i--)
    {
        if(in.at(i) == QChar('\\') || in.at(i) == QChar('/'))
        {
            return in.left(i);
        }
    }
    return NULL;
};

CustomLevelDialog::CustomLevelDialog(QWidget* parent) : QDialog(parent)
{
    setMinimumWidth(400);
    for(int i = 0; i < 7; i++)
    {
        filenames[i] = new QLineEdit(this);
        buttons[i] = new QPushButton(tr("Browse"),this);
    }

    labels[0] = new QLabel(tr("Level:"),this);
    labels[1] = new QLabel(tr("D3D:"),this);
    labels[2] = new QLabel(tr("Player Denting:"),this);
    labels[3] = new QLabel(tr("Civilian Denting:"),this);
    labels[4] = new QLabel(tr("Player Cosmetics:"),this);
    labels[5] = new QLabel(tr("Civilian Cosmetics:"),this);
    labels[6] = new QLabel(tr("Wheel Definitions:"),this);

    openButton = new QPushButton(tr("Open"),this);
    cancelButton = new QPushButton(tr("Cancel"),this);

    QGridLayout* layout = new QGridLayout();
    for(int i = 0; i < 7; i++)
    {
        layout->addWidget(labels[i],i,0);
        layout->addWidget(filenames[i],i,1);
        layout->addWidget(buttons[i],i,2);
    }

    QWidget* spacer = new QWidget(this);
    spacer->setMinimumHeight(10);
    layout->addWidget(spacer,7,0,1,-1);
    layout->addWidget(openButton,8,0,1,-1,Qt::AlignLeft);
    layout->addWidget(cancelButton,8,0,1,-1,Qt::AlignRight);
    setLayout(layout);

    connect(buttons[0], SIGNAL(clicked()),this, SLOT(browseLevel()));
    connect(buttons[1], SIGNAL(clicked()),this, SLOT(browseD3D()));
    connect(buttons[2], SIGNAL(clicked()),this, SLOT(browsePlayerDen()));
    connect(buttons[3], SIGNAL(clicked()),this, SLOT(browseCivilianDen()));
    connect(buttons[4], SIGNAL(clicked()),this, SLOT(browsePlayerCos()));
    connect(buttons[5], SIGNAL(clicked()),this, SLOT(browseCivilianCos()));
    connect(buttons[6], SIGNAL(clicked()),this, SLOT(browseWheels()));
    connect(openButton, SIGNAL(clicked()),this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),this, SLOT(reject()));
};

QString CustomLevelDialog::getLevelString()
{
    return level;
};

QString CustomLevelDialog::getD3DString()
{
    return d3d;
};

QString CustomLevelDialog::getWheelsString()
{
    return wheels;
};

QString CustomLevelDialog::getPlayerDenString()
{
    return pcarDen;
};

QString CustomLevelDialog::getCivilianDenString()
{
    return civcarDen;
};

QString CustomLevelDialog::getPlayerCosString()
{
    return pcarCos;
};

QString CustomLevelDialog::getCivilianCosString()
{
    return civcarCos;
};

QString CustomLevelDialog::getLastDirectory()
{
    return lastDirectory;
};

void CustomLevelDialog::setLastDirectory(QString dir)
{
    lastDirectory = dir;
};

void CustomLevelDialog::browseLevel()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate level file to open.")),lastDirectory,QString(tr("Level Files (*.lev);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[0]->setText(filename);
        level = filename;
    }
};

void CustomLevelDialog::browseD3D()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate D3D file to open.")),lastDirectory,QString(tr("D3D File (*.d3d);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[1]->setText(filename);
        d3d = filename;
    }
};

void CustomLevelDialog::browsePlayerDen()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate playercar denting file to open.")),lastDirectory,QString(tr("Denting Files (*.den);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[2]->setText(filename);
        pcarDen = filename;
    }
};

void CustomLevelDialog::browseCivilianDen()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate civ-car denting file to open.")),lastDirectory,QString(tr("Denting Files (*.den);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[3]->setText(filename);
        civcarDen = filename;
    }
};

void CustomLevelDialog::browsePlayerCos()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate playercar cosmetics file to open.")),lastDirectory,QString(tr("Cosmetics File (*.cos);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[4]->setText(filename);
        pcarCos = filename;
    }
};

void CustomLevelDialog::browseCivilianCos()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate civ-car cosmetics file to open.")),lastDirectory,QString(tr("Cosmetics File (*.cos);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[5]->setText(filename);
        civcarCos = filename;
    }
};

void CustomLevelDialog::browseWheels()
{
    QString filename = QFileDialog::getOpenFileName(this,QString(tr("Locate wheel definition file to open.")),lastDirectory,QString(tr("Wheel Definition Files (*.wdf);;All Files (*.*)")));
    if(!filename.isNull())
    {
        if(!filename.isEmpty())
        lastDirectory = getPath(filename);
        filenames[6]->setText(filename);
        wheels = filename;
    }
};
