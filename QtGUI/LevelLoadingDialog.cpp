#include "LevelLoadingDialog.hpp"

LevelLoadingDialog::LevelLoadingDialog(QWidget* parent) : QDialog(parent)
{
    resultTable = new QTableWidget(this);
    resultTable->setRowCount(7);
    resultTable->setColumnCount(4);
    resultTable->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("File")));
    resultTable->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Status")));
    resultTable->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Code")));
    resultTable->setHorizontalHeaderItem(3,new QTableWidgetItem(tr("Filename")));
    resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    resultTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    resultTable->verticalHeader()->hide();

    warningLabel = new QLabel(tr("One or more files failed to load:"), this);
    questionLabel = new QLabel(tr("Do you want to continue on to the editor?"), this);
    continueButton = new QPushButton(tr("Continue Anyway"), this);
    connect(continueButton, SIGNAL(clicked()), this, SLOT(accept()));
    abortButton = new QPushButton(tr("Abort"), this);
    connect(abortButton, SIGNAL(clicked()), this, SLOT(reject()));

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(warningLabel, 0, 0, 1, -1, Qt::AlignHCenter);
    layout->addWidget(resultTable, 1, 0, 1, -1);
    layout->addWidget(questionLabel, 2, 0, 1, -1, Qt::AlignHCenter);
    layout->addWidget(continueButton,3,0,1,1,Qt::AlignHCenter);
    layout->addWidget(abortButton,3,1,1,1,Qt::AlignHCenter);
    setLayout(layout);

    level = NULL;
    d3d = NULL;
    wheels = NULL;
    playerCos = NULL;
    civilianCos = NULL;
    playerDen = NULL;
    civilianDen = NULL;

    mainLog = &dummyLog;

    for(int i = 0; i < 7; i++)
        errorCodes[i] = 0;
};

void LevelLoadingDialog::setLog(DebugLogger* newLog)
{
    if(newLog)
        mainLog = newLog;
    else mainLog = &dummyLog;
};

void LevelLoadingDialog::setLevel(DriverLevel* _level)
{
    level = _level;
};

void LevelLoadingDialog::setD3D(DriverD3D* _d3d)
{
    d3d = _d3d;
};

void LevelLoadingDialog::setWheelDefinitions(DriverWheelDefinitions* _wheels)
{
    wheels = _wheels;
};

void LevelLoadingDialog::setPlayerCosmetics(CosmeticsContainer* _playerCos)
{
    playerCos = _playerCos;
};

void LevelLoadingDialog::setCivilianCosmetics(CosmeticsContainer* _civilianCos)
{
    civilianCos = _civilianCos;
};

void LevelLoadingDialog::setPlayerDenting(DriverDenting* _playerDen)
{
    playerDen = _playerDen;
};

void LevelLoadingDialog::setCivilianDenting(DriverDenting* _civilianDen)
{
    civilianDen = _civilianDen;
};

bool LevelLoadingDialog::levelLoaded()
{
    return errorCodes[0] >= 0;
};

bool LevelLoadingDialog::d3dLoaded()
{
    return errorCodes[1] >= 0;
};

bool LevelLoadingDialog::wheelDefinitionsLoaded()
{
    return errorCodes[6] >= 0;
};

bool LevelLoadingDialog::playerCosmeticsLoaded()
{
    return errorCodes[4] >= 0;
};

bool LevelLoadingDialog::playerDentingLoaded()
{
    return errorCodes[2] >= 0;
};

bool LevelLoadingDialog::civilianCosmeticsLoaded()
{
    return errorCodes[5] >= 0;
};

bool LevelLoadingDialog::civilianDentingLoaded()
{
    return errorCodes[3] >= 0;
};

void LevelLoadingDialog::buildTable()
{
    QString names[7] = {tr("Level"), tr("D3D"), tr("Player denting"), tr("Civilian denting"), tr("Player cosmetics"), tr("Civilian cosmetics"), tr("Wheel definitions")};
    QTableWidgetItem* namesItem, * errorItem, *codeItem, *filenameItem;
    for(int i = 0; i < 7; i++)
    {
        //File
        namesItem = new QTableWidgetItem(names[i]);
        errorItem = new QTableWidgetItem(errorStrings[i]);
        if(errorCodes[i] < 0)
        codeItem = new QTableWidgetItem(QString::number(errorCodes[i]));
        else codeItem = new QTableWidgetItem("0");
        filenameItem = new QTableWidgetItem(filenames[i]);

        if(errorCodes[i] < 0)
        {
            namesItem->setBackground(QBrush(QColor(255,70,70)));
            errorItem->setBackground(QBrush(QColor(255,70,70)));
            codeItem->setBackground(QBrush(QColor(255,70,70)));
            filenameItem->setBackground(QBrush(QColor(255,70,70)));
        }

        resultTable->setItem(i, 0, namesItem);
        //Status
        resultTable->setItem(i, 1, errorItem);
        //Code
        resultTable->setItem(i, 2, codeItem);
        //Filename
        resultTable->setItem(i, 3, filenameItem);
    }
    resultTable->resizeRowsToContents();
    //Resize table.
	int tableWidth = 2 + resultTable->verticalHeader()->width();
	for(int i = 0; i < resultTable->columnCount(); i++)
    {
	    tableWidth += resultTable->columnWidth(i);
	}
    resultTable->setMinimumWidth(tableWidth);
    resultTable->setMaximumWidth(tableWidth);

	int tableHeight = 2 + resultTable->horizontalHeader()->height();
	for(int i = 0; i < resultTable->rowCount(); i++)
    {
	    tableHeight += resultTable->rowHeight(i);
	}
	resultTable->setMinimumHeight(tableHeight);
	resultTable->setMaximumHeight(tableHeight);
};

int LevelLoadingDialog::exec()
{
    buildTable();
    return QDialog::exec();
};

void LevelLoadingDialog::cleanupLevelData()
{
    if(level)
    {
        mainLog->Log("Cleaning up level data...");
        level->cleanup();
        mainLog->Log("Level data cleaned up successfully.");
    }

    if(d3d)
    {
        mainLog->Log("Cleaning up D3D data...");
        d3d->cleanup();
        mainLog->Log("D3D data cleaned up successfully.");
    }

    if(playerCos)
    {
        mainLog->Log("Cleaning up player cosmetics data...");
        playerCos->reset();
        mainLog->Log("Player cosmetics data cleaned up successfully.");
    }

    if(civilianCos)
    {
        mainLog->Log("Cleaning up civilian cosmetics data...");
        civilianCos->reset();
        mainLog->Log("Civilian cosmetics data cleaned up successfully.");
    }

    if(playerDen)
    {
        mainLog->Log("Cleaning up player denting data...");
        playerDen->cleanup();
        mainLog->Log("Denting data cleaned up successfully.");
    }

    if(civilianDen)
    {
        mainLog->Log("Cleaning up civilian denting data...");
        civilianDen->cleanup();
        mainLog->Log("Denting data cleaned up successfully.");
    }

    if(wheels)
    {
        mainLog->Log("Cleaning up wheel definition data...");
        wheels->reset();
        mainLog->Log("Wheel definition data cleaned up successfully.");
    }
};

bool LevelLoadingDialog::load(QString levelFilename, QString d3dFilename, QString wheelsFilename, QString playerCosFilename, QString civilianCosFilename, QString playerDenFilename, QString civilianDenFilename)
{
    int ret;
    for(int i = 0; i < 7; i++)
    {
        errorStrings[i].clear();
        errorCodes[i] = 0;
    }
    cleanupLevelData();

    //Load level
    filenames[0] = levelFilename;
    if(!levelFilename.isEmpty())
    {
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load level %s.",levelFilename.toLocal8Bit().data());
        if(level)
        {
            ret = level->loadFromFile(levelFilename.toLocal8Bit().data(),LEV_ALL);
            errorCodes[0] = ret;
            if(ret < 0)
            {
                if(ret == -1)
                {
                    errorStrings[0] = tr("Failed to open file!");
                    mainLog->Log("ERROR: Failed to open level file!");
                }
                else if(ret == -2)
                {
                    level->cleanup();
                    errorStrings[0] = tr("Level is corrupt! Could not load!");
                    mainLog->Log("ERROR: Level is corrupt!.");
                }
                else if(ret == -3)
                {
                    level->cleanup();
                    errorStrings[0] = tr("Level is corrupt! Block load failed.");
                    mainLog->Log("ERROR: Level is corrupt!. Block load failed.");
                }
                else
                {
                    errorStrings[0] = tr("Unknown error occurred!");
                    mainLog->Log("ERROR: Unknown error occurred when loading level: %d", ret);
                }
            }
            else
            {
                errorStrings[0] = tr("Loaded successfully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Level loaded successfully.");
            }
        }
        else
        {
            errorStrings[0] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "Level pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[0] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Level filename is empty, nothing to be done.");
    }

    //Load D3D
    filenames[1] = d3dFilename;
    if(!d3dFilename.isEmpty())
    {
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load D3D %s.",d3dFilename.toLocal8Bit().data());
        if(d3d)
        {
            ret = d3d->loadFromFile(d3dFilename.toLocal8Bit().data());
            errorCodes[1] = ret;
            if(ret < 0)
            {
                if(ret == -1)
                {
                    errorStrings[1] = tr("Failed to open file!");
                    mainLog->Log("ERROR: Failed to open D3D file!");
                }
                else if(ret == -2)
                {
                    errorStrings[1] = tr("D3D is corrupt! Could not load!");
                    mainLog->Log("ERROR: D3D is corrupt!");
                }
                else
                {
                    errorStrings[1] = tr("Unknown error occurred!");
                    mainLog->Log("ERROR: Unknown error occurred when loading D3D: %d", ret);
                }
            }
            else
            {
                errorStrings[1] = tr("Loaded successfully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "D3D loaded successfully.");
            }
        }
        else
        {
            errorStrings[1] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "D3D pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[1] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "D3D filename is empty, nothing to be done.");
    }

    //Load player denting
    filenames[2] = playerDenFilename;
    if(!playerDenFilename.isEmpty())
    {
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load player denting %s.",playerDenFilename.toLocal8Bit().data());
        if(playerDen)
        {
            ret = playerDen->loadPlayerDentingFromFile(playerDenFilename.toLocal8Bit().data());
            errorCodes[2] = ret;
            if(ret < 0)
            {
                if(ret == -1)
                {
                    errorStrings[2] = tr("Failed to open file!");
                    mainLog->Log("ERROR: Failed to open player denting file!");
                }
                else
                {
                    errorStrings[2] = tr("Unknown error occurred!");
                    mainLog->Log("ERROR: Unknown error occurred when loading player denting: %d", ret);
                }
            }
            else
            {
                errorStrings[2] = tr("Loaded successfully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Player denting loaded successfully.");
            }
        }
        else
        {
            errorStrings[2] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "Player denting pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[2] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Player denting filename is empty, nothing to be done.");
    }

    //Load civilian denting
    filenames[3] = civilianDenFilename;
    if(!civilianDenFilename.isEmpty())
    {
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load civilian denting %s.",civilianDenFilename.toLocal8Bit().data());
        if(civilianDen)
        {
            ret = civilianDen->loadCivilianDentingFromFile(civilianDenFilename.toLocal8Bit().data());
            errorCodes[3] = ret;

            if(ret < 0)
            {
                if(ret == -1)
                {
                    errorStrings[3] = tr("Failed to open file!");
                    mainLog->Log("ERROR: Failed to open civilian denting file!");
                }
                else if(ret == -2)
                {
                    errorStrings[3] = tr("Non-'vertex patch' version encountered!");
                    mainLog->Log("ERROR: Failed to open civilian denting file!");
                }
                else
                {
                    errorStrings[3] = tr("Unknown error occurred!");
                    mainLog->Log("ERROR: Unknown error occurred when loading civilian denting: %d", ret);
                }
            }
            else
            {
                errorStrings[3] = tr("Loaded successfully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Civilian denting loaded successfully.");
            }
        }
        else
        {
            errorStrings[3] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "Civilian denting pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[3] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Civilian denting filename is empty, nothing to be done.");
    }

    //Load player cosmetics
    filenames[4] = playerCosFilename;
    if(!playerCosFilename.isEmpty())
    {
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load player cosmetics %s.",playerCosFilename.toLocal8Bit().data());
        if(playerCos)
        {
            ret = playerCos->loadCosmeticsFromFile(playerCosFilename.toLocal8Bit().data());
            errorCodes[4] = ret;

            if(ret < 0)
            {
                if(ret == -1)
                {
                    errorStrings[4] = tr("Failed to open file!");
                    mainLog->Log("ERROR: Failed to open player cosmetics file!");
                }
                else
                {
                    errorStrings[4] = tr("Unknown error occurred!");
                    mainLog->Log("ERROR: Unknown error occurred when loading player cosmetics: %d", ret);
                }
            }
            else
            {
                errorStrings[4] = tr("Player cosmetics loaded successfully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Player cosmetics loaded successfully.");
            }
        }
        else
        {
            errorStrings[4] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "Player cosmetics pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[4] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Player cosmetics filename is empty, nothing to be done.");
    }

    //Load civilian cosmetics
    filenames[5] = civilianCosFilename;
    if(!civilianCosFilename.isEmpty())
    {
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load civilian cosmetics %s.",civilianCosFilename.toLocal8Bit().data());
        if(civilianCos)
        {
            ret = civilianCos->loadCosmeticsFromFile(civilianCosFilename.toLocal8Bit().data());
            errorCodes[5] = ret;

            if(ret < 0)
            {
                if(ret == -1)
                {
                    errorStrings[5] = tr("Failed to open file!");
                    mainLog->Log("ERROR: Failed to open civilian cosmetics file!");
                }
                else
                {
                    errorStrings[5] = tr("Unknown error occurred!");
                    mainLog->Log("ERROR: Unknown error occurred when loading civilian cosmetics: %d", ret);
                }
            }
            else
            {
                errorStrings[5] = tr("Loaded successfully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Civilian cosmetics loaded successfully.");
            }
        }
        else
        {
            errorStrings[5] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "Civilian cosmetics pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[5] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Civilian cosmetics filename is empty, nothing to be done.");
    }

    //Load wheels file
    filenames[6] = wheelsFilename;
    if(!wheelsFilename.isEmpty())
    {
        if(wheels)
        {
            if(QFile::exists(wheelsFilename))
            {
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Preparing to load wheel definition file %s.", wheelsFilename.toLocal8Bit().data());
                ret = wheels->loadFromFile(wheelsFilename.toLocal8Bit().data());
                errorCodes[6] = ret;

                if(ret < 0)
                {
                    if(ret == -1)
                    {
                        errorStrings[6] = tr("Failed to open file!");
                        mainLog->Log("ERROR: Failed to open wheel definition file!");
                    }
                    else if(ret == -2)
                    {
                        errorStrings[6] = tr("WDF corrupt: file is too small!");
                        mainLog->Log("ERROR: Failed to open wheel definition file!");
                    }
                    else
                    {
                        errorStrings[6] = tr("Unknown error occurred!");
                        mainLog->Log("ERROR: Unknown error occurred when loading wheel definition file: %d", ret);
                    }
                }
                else
                {
                    errorStrings[6] = tr("Loaded successfully.");
                    mainLog->Log(DEBUG_LEVEL_NORMAL, "Wheel definition file loaded successfully.");
                }
            }
            else
            {
                errorStrings[6] = tr("File not found, skipped gracefully.");
                mainLog->Log(DEBUG_LEVEL_NORMAL, "Wheel definition file does not exist, skipping.");
            }
        }
        else
        {
            errorStrings[6] = tr("Pointer not set! Bug the programmer!");
            mainLog->Log(DEBUG_LEVEL_NORMAL, "Wheel definition pointer not set, programmer error.");
        }
    }
    else
    {
        errorStrings[6] = tr("No filename specified, nothing to be done.");
        mainLog->Log(DEBUG_LEVEL_NORMAL, "Wheel definition filename is empty, nothing to be done.");
    }

    if(!(levelLoaded() && d3dLoaded() && wheelDefinitionsLoaded() && playerCosmeticsLoaded() && playerDentingLoaded() && civilianCosmeticsLoaded() && civilianDentingLoaded()))
    {
        if(exec() == QDialog::Accepted)
        return true;

        //The user chose to abort, so cleanup anything that was loaded.
        cleanupLevelData();
        return false;
    }
    return true;
};
