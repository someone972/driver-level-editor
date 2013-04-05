#include "MainWindow.hpp"

DebugLogger* FreeImageLogger = NULL; //Don't like using globals like this, but can't pass class member function to FreeImage.

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
    if(FreeImageLogger)
    {
        if(fif != FIF_UNKNOWN)
        FreeImageLogger->Log("FreeImage Error (%s): %s",FreeImage_GetFormatFromFIF(fif),message);
        else
        FreeImageLogger->Log("FreeImage Error (Generic): %s",message);
    }
};

MainWindow::MainWindow() : playerCosmetics(18), civilianCosmetics(12)
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QCoreApplication::setOrganizationName("TEMP_ORG"); //TODO: Add a proper organization name.
    QCoreApplication::setOrganizationDomain("TEMP_ORG.com");
    QCoreApplication::setApplicationName("Driver Level Editor");

    levelHasChanged = false;
    d3dHasChanged = false;
    civCosHasChanged = false;
    playerCosHasChanged = false;
    civDentHasChanged = false;
    playerDentHasChanged = false;
    wdfHasChanged = false;

    mainLog.createLogfile("debug.txt");
    levelLog.createLogfile("levelDebug.txt");
    levelLog.setLogPriority(DEBUG_LEVEL_RIDICULOUS);
    level.setLogger(&levelLog);
    levelTextures.setTextureProvider(&level.textures);
    levelTextures.setD3D(&d3d);

    if(QDir("./settings").exists())
    {
        mainLog.Log("Settings folder exists in working directory, settings will be saved portably.");
        if(!QDir("./settings/User").exists())
            QDir().mkdir("./settings/User");
        if(!QDir("./settings/System").exists())
            QDir().mkdir("./settings/System");
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "./settings/User");
        QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, "./settings/System");
    }
    else mainLog.Log("No settings folder found. Saving settings locally.");

#ifdef DEBUG_ENABLED
    mainLog.Log("Executable is a debug build.");
    setWindowTitle(QApplication::translate("windowtitle", "Driver Level Editor [DEBUG][*]"));
#else
    mainLog.Log("Executable is a release build.");
    setWindowTitle(QApplication::translate("windowtitle", "Driver Level Editor[*]"));
#endif

#ifdef FREEIMAGE_LIB
    mainLog.Log("FreeImage is static linked.");
	FreeImage_Initialise();
#endif
    FreeImageLogger = &mainLog;
    FreeImage_SetOutputMessage(FreeImageErrorHandler);
    mainLog.Log("FreeImage version: %s",FreeImage_GetVersion());
    mainLog.Log("%s",FreeImage_GetCopyrightMessage());

    mainLog.Log("Initializing widgets...");

    centralWindow = new QStackedWidget(this);
    startPage = new StartScreen(this);
    customLevelDialog = new CustomLevelDialog(this);
    aboutDialog = new AboutDialog(this);
    //TODO: Enter final about information
    aboutDialog->setDescription(tr("Description goes here!"));
    aboutDialog->setInformation(tr("Information goes here!"));
    aboutDialog->setThanks(tr("Thanks goes here!"));
    aboutDialog->setImage("data/about.png");

    modelViewPanel = new ModelViewPanel(this, NULL, 0, &mainLog);
    modelViewPanel->setLevel(&level);
    modelViewPanel->setTextureProvider(&levelTextures);
    connect(this, SIGNAL(levelChanged()), modelViewPanel, SLOT(handleLevelChange()));

    definitionEditor = new TextureDefinitionEditor(this, modelViewPanel->glViewer()); //Don't forget, this needs a share widget when model viewing is done
    definitionEditor->setLevel(&level);
    definitionEditor->setTextureProvider(&levelTextures);
    definitionEditor->overlay()->makeCurrent();

    textureBrowser = new TextureBrowser(this, modelViewPanel->glViewer());
    textureBrowser->setLevel(&level);
    textureBrowser->setTextureList(&levelTextures);
    textureBrowser->setD3D(&d3d);
    connect(this, SIGNAL(levelChanged()), textureBrowser, SLOT(texturesChanged()));

    levelLoader = new LevelLoadingDialog(this);
    levelLoader->setLevel(&level);
    levelLoader->setD3D(&d3d);
    levelLoader->setWheelDefinitions(&wheels);
    levelLoader->setPlayerCosmetics(&playerCosmetics);
    levelLoader->setCivilianCosmetics(&civilianCosmetics);
    levelLoader->setPlayerDenting(&playerDenting);
    levelLoader->setCivilianDenting(&civilianDenting);
    levelLoader->setLog(&mainLog);

    saveDialog = new SaveAsDialog(this);
    connect(saveDialog, SIGNAL(saveLevel(QString,unsigned int)), this, SLOT(saveLevel(QString,unsigned int)));
    connect(saveDialog, SIGNAL(saveD3D(QString)), this, SLOT(saveD3D(QString)));
    connect(saveDialog, SIGNAL(savePlayerDenting(QString)), this, SLOT(savePlayerDenting(QString)));
    connect(saveDialog, SIGNAL(saveCivilianDenting(QString)), this, SLOT(saveCivilianDenting(QString)));
    connect(saveDialog, SIGNAL(savePlayerCosmetics(QString)), this, SLOT(savePlayerCosmetics(QString)));
    connect(saveDialog, SIGNAL(saveCivilianCosmetics(QString)), this, SLOT(saveCivilianCosmetics(QString)));
    connect(saveDialog, SIGNAL(saveWheelDefinitions(QString)), this, SLOT(saveWheelDefinitions(QString)));

    createActions();
    createMenus();
    textureBrowser->setupEditMenu(editMenu);
    setConvenienceActionsEnabled(false);

    centralWindow->addWidget(startPage);
    centralWindow->addWidget(modelViewPanel);
    centralWindow->addWidget(textureBrowser);
    centralWindow->addWidget(definitionEditor);
    statusBar();
    setCentralWidget(centralWindow);

    mainLog.Log("Finished initializing widgets.");

    loadSettings();
    show();
};

MainWindow::~MainWindow()
{
    saveSettings();

#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
};

void MainWindow::loadSettings()
{
    mainLog.Log("Loading settings...");
    QSettings settings;
    if(settings.allKeys().size() == 0)
    {
        mainLog.Log("Settings is empty. Default values will be used.");
    }
    else mainLog.Log("Loading settings from %s.", settings.fileName().toLocal8Bit().data());


    QPoint defaultPosition;
    QSize defaultSize(800,600);
    if(!settings.contains("MainWindow/size") || !settings.contains("MainWindow/pos"))
    {
        int frameWidth = 0;
        int frameTop = 0;
        int frameBottom = 0;
        bool screenHack = true; //If we want to get frame geometry (which is apparently a pain in qt).
        if(screenHack)
        {
            //The following from a post on stackoverflow by TonyK

            // BIG PAIN: We want to get the dialog box to caluclate its own size. But there is
            // no simple way to do this. The following seems to work, but only if processEvents
            // is called at least twice. God knows why:
            setAttribute (Qt::WA_DontShowOnScreen, true) ; // Prevent screen flicker
            show() ;

            QEventLoop EventLoop (this) ;
            for (int i = 0 ; i < 10 ; i++)
              if (!EventLoop.processEvents()) break ;

            hide() ;
            setAttribute (Qt::WA_DontShowOnScreen, false) ;
            //End code section from TonyK

            frameWidth = frameGeometry().width()-geometry().width();
            frameTop = abs(geometry().top()-frameGeometry().top());
            frameBottom = abs(frameGeometry().bottom()-geometry().bottom());
        }

        //Ensure sane positioning for first run (i.e. not split across multiple monitors
        QDesktopWidget* desk = QApplication::desktop();
        int usedScreen = 0;
        const QRect available = desk->availableGeometry(usedScreen);
        if((available.x()+available.width())/2 >= defaultSize.width()/2+frameWidth/2)
        {
            defaultPosition.setX((available.x()+available.width())/2-(defaultSize.width()/2+frameWidth/2));
        }
        else
        {
            defaultPosition.setX(available.x());
            defaultSize.setWidth(available.width()-frameWidth);
        }
        if((available.y()+available.height())/2 >= defaultSize.height()/2 + (frameTop+frameBottom)/2)
        {
            defaultPosition.setY((available.y()+available.height())/2-defaultSize.height()/2);
        }
        else
        {
            defaultPosition.setY(available.y());
            defaultSize.setHeight(available.height()-(frameTop+frameBottom));
        }
        mainLog.Log("Positioning window at (%d, %d) with size (%d, %d) on screen %d/%d",defaultPosition.x(),defaultPosition.y(),defaultSize.width(),defaultSize.height(),usedScreen,desk->screenCount());
    }

    mainLog.setLogPriority(settings.value("LoggingLevels/mainLog",DEFAULT_PRIORITY).toInt());
    levelLog.setLogPriority(settings.value("LoggingLevels/levelLog",DEFAULT_PRIORITY).toInt());
    int levelPriorities[NUMBER_OF_BLOCKS];

    levelPriorities[BLOCK_TEXTURES] = settings.value("LoggingLevels/blockTextures",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_MODELS] = settings.value("LoggingLevels/blockModels",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_WORLD] = settings.value("LoggingLevels/blockWorld",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_RANDOM_MODEL_PLACEMENT] = settings.value("LoggingLevels/blockRandomModelPlacement",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_TEXTURE_DEFINITIONS] = settings.value("LoggingLevels/blockTextureDefinitions",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_ROAD_TABLE] = settings.value("LoggingLevels/blockRoadTable",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_ROAD_CONNECTIONS] = settings.value("LoggingLevels/blockRoadConnections",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_INTERSECTIONS] = settings.value("LoggingLevels/blockIntersections",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_HEIGHTMAP_TILES] = settings.value("LoggingLevels/blockHeightmapTiles",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_HEIGHTMAP] = settings.value("LoggingLevels/blockHeightmap",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_MODEL_NAMES] = settings.value("LoggingLevels/blockModelNames",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_EVENT_MODELS] = settings.value("LoggingLevels/blockEventModels",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_VISIBILITY] = settings.value("LoggingLevels/blockVisibility",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_SECTOR_TEXTURE_USAGE] = settings.value("LoggingLevels/blockSectorTextureUsage",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_ROAD_SECTIONS] = settings.value("LoggingLevels/blockRoadSections",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_INTERSECTION_POSITIONS] = settings.value("LoggingLevels/blockIntersectionPositions",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_LAMPS] = settings.value("LoggingLevels/blockLamps",levelLog.getLogPriority()).toInt();
    levelPriorities[BLOCK_CHAIR_PLACEMENT] = settings.value("LoggingLevels/blockChairPlacement",levelLog.getLogPriority()).toInt();
    level.setLogPriorities(levelPriorities);

    resize(settings.value("MainWindow/size",defaultSize).toSize());
    move(settings.value("MainWindow/pos", defaultPosition).toPoint());
    if(settings.value("MainWindow/maximized",false).toBool())
    showMaximized();
    else showNormal();
    show();

    customLevelDialog->setLastDirectory(settings.value("directories/lastOpenFileDir","C:\\Program Files\\GT Interactive\\Driver\\Levels").toString());

    if(settings.value("firstRun",true).toBool())
    {
        int ret = QMessageBox::No;
        QMessageBox msgBox(centralWindow);
        msgBox.setText(tr("<b>It looks like this is the first run.</b>"));
        msgBox.setInformativeText(tr("It is recommended that you set the default Driver directory. Would you like to do this now? The directory can be changed later in the settings menu. ")+rootDir);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        ret = msgBox.exec();
        if(ret == QMessageBox::Yes)
        {
            QString newRootDir = QFileDialog::getExistingDirectory(centralWindow,tr("Choose the root Driver directory."),"C:\\Program Files\\GT Interactive\\Driver");
            if(!newRootDir.isEmpty())
            {
                settings.setValue("directories/rootDir",newRootDir);
            }
        }
    }

    rootDir = QDir::toNativeSeparators(settings.value("directories/rootDir").toString());

    if(!rootDir.isEmpty())
    setConvenienceActionsEnabled(true);

    textureBrowser->loadSettings();
    mainLog.Log("Finished loading settings.");
};

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("firstRun",false);
    settings.setValue("directories/rootDir",QDir::fromNativeSeparators(rootDir));

    settings.setValue("MainWindow/maximized",isMaximized());
    showNormal();
    settings.setValue("MainWindow/size",size());
    settings.setValue("MainWindow/pos",pos());

    textureBrowser->saveSettings();
};

void MainWindow::createActions()
{
    openCustom = new QAction(tr("&Choose Files..."), this);
    connect(openCustom, SIGNAL(triggered()), this, SLOT(loadCustomFiles()));

    openMiami = new QAction(tr("&Miami"), this);
    connect(openMiami,SIGNAL(triggered()),this,SLOT(loadMiamiFiles()));

    openFrisco = new QAction(tr("San &Francisco"), this);
    connect(openFrisco,SIGNAL(triggered()),this,SLOT(loadFriscoFiles()));

    openLosAngeles = new QAction(tr("&Los Angeles"), this);
    connect(openLosAngeles,SIGNAL(triggered()),this,SLOT(loadLosAngelesFiles()));

    openNewYork = new QAction(tr("New &York"), this);
    connect(openNewYork,SIGNAL(triggered()),this,SLOT(loadNewYorkFiles()));

    openTrain = new QAction(tr("&Desert"), this);
    connect(openTrain,SIGNAL(triggered()),this,SLOT(loadTrainFiles()));

    openIview = new QAction(tr("Car &Park"), this);
    connect(openIview,SIGNAL(triggered()),this,SLOT(loadIviewFiles()));

    openNewcastle = new QAction(tr("&Newcastle"), this);
    connect(openNewcastle,SIGNAL(triggered()),this,SLOT(loadNewcastleFiles()));

    saveAs = new QAction(tr("Save &As"),this);
    connect(saveAs, SIGNAL(triggered()), saveDialog, SLOT(exec()));

    exitAction = new QAction(tr("&Exit"),this);
    connect(exitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

    launchGameAction = new QAction(tr("&Run Game"),this);
    connect(launchGameAction, SIGNAL(triggered()), this, SLOT(launchGame()));

    openGameDirAction = new QAction(tr("&Open Root Directory"),this);
    connect(openGameDirAction, SIGNAL(triggered()), this, SLOT(openGameDirectory()));

    installPatchAction = new QAction(tr("&Install Vertex Patch"),this);
    connect(installPatchAction, SIGNAL(triggered()), this, SLOT(installPatch()));

    helpTopics = new QAction(tr("Help &Topics"),this);

    aboutAction = new QAction(tr("&About"),this);
    connect(aboutAction, SIGNAL(triggered()), aboutDialog, SLOT(exec()));

    viewModelsAction = new QAction(tr("&Models"),this);
    connect(viewModelsAction, SIGNAL(triggered()), this, SLOT(viewModels()));

    viewTextureBrowserAction = new QAction(tr("&Textures"),this);
    connect(viewTextureBrowserAction, SIGNAL(triggered()), this, SLOT(viewTextureBrowser()));

    viewDefinitionEditorAction = new QAction(tr("Texture &Definitions"),this);
    connect(viewDefinitionEditorAction, SIGNAL(triggered()), this, SLOT(viewDefinitionEditor()));
};

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
        openMenu = fileMenu->addMenu(tr("&Open"));
            openMenu->addAction(openCustom);
            openMenu->addSeparator();
            openMenu->addAction(openMiami);
            openMenu->addAction(openFrisco);
            openMenu->addAction(openLosAngeles);
            openMenu->addAction(openNewYork);
            openMenu->addAction(openNewcastle);
            openMenu->addAction(openTrain);
            openMenu->addAction(openIview);
        fileMenu->addAction(saveAs);
        fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));

    viewMenu = menuBar()->addMenu(tr("&View"));
        viewMenu->addAction(viewModelsAction);
        viewMenu->addAction(viewTextureBrowserAction);
        viewMenu->addAction(viewDefinitionEditorAction);

    driverMenu = menuBar()->addMenu(tr("&Driver"));
        driverMenu->addAction(launchGameAction);
        driverMenu->addAction(openGameDirAction);
        driverMenu->addAction(installPatchAction);

    settingsMenu = menuBar()->addMenu(tr("&Settings"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
        helpMenu->addAction(helpTopics);
        helpMenu->addSeparator();
        helpMenu->addAction(aboutAction);
};

void MainWindow::viewModels()
{
    centralWindow->setCurrentWidget(modelViewPanel);
};

void MainWindow::viewDefinitionEditor()
{
    centralWindow->setCurrentWidget(definitionEditor);
};

void MainWindow::viewTextureBrowser()
{
    centralWindow->setCurrentWidget(textureBrowser);
};

void MainWindow::setConvenienceActionsEnabled(bool enabled)
{
    openMiami->setEnabled(enabled);
    openFrisco->setEnabled(enabled);
    openLosAngeles->setEnabled(enabled);
    openNewYork->setEnabled(enabled);
    openTrain->setEnabled(enabled);
    openIview->setEnabled(enabled);
    openNewcastle->setEnabled(enabled);
    launchGameAction->setEnabled(enabled);
    openGameDirAction->setEnabled(enabled);
};

void MainWindow::openLevel(QString levFile,QString d3dFile,QString pcarDenFile,QString civcarDenFile,QString pcarCosFile,QString civcarCosFile,QString wheelFile)
{
    levelString = levFile;
    d3dString = d3dFile;
    playerDenString = pcarDenFile;
    civilianCosString = civcarDenFile;
    playerCosString = pcarCosFile;
    civilianCosString = civcarCosFile;
    wheelsString = wheelFile;

    bool success = levelLoader->load(levFile, d3dFile, wheelFile, pcarCosFile, civcarCosFile, pcarDenFile, civcarDenFile);
    if(success && centralWindow->currentWidget() == startPage)
        centralWindow->setCurrentWidget(textureBrowser);

    levelTextures.rebuildAllTextures();
    modelViewPanel->setLevel(&level);
    saveDialog->setLevelFilename(levelString);
    saveDialog->setD3DFilename(d3dString);
};

void MainWindow::loadCustomFiles()
{
    if(customLevelDialog->exec() == QDialog::Accepted)
    {
        openLevel(customLevelDialog->getLevelString(),customLevelDialog->getD3DString(),customLevelDialog->getPlayerDenString(),customLevelDialog->getCivilianDenString(),
                   customLevelDialog->getPlayerCosString(),customLevelDialog->getCivilianCosString(),customLevelDialog->getWheelsString());
        lastOpenDirectory = customLevelDialog->getLastDirectory();
    }
};

void MainWindow::loadMiamiFiles()
{
    openLevel(rootDir+"\\Levels\\miami_01.lev",rootDir+"\\Levels\\miami.d3d",rootDir+"\\Levels\\pcar_miami.den",rootDir+"\\Levels\\miami_01.den",
              rootDir+"\\Levels\\pcar_miami.cos",rootDir+"\\Levels\\Miami_01.cos",rootDir+"\\Levels\\miami_wheels.wdf");
};

void MainWindow::loadFriscoFiles()
{
    openLevel(rootDir+"\\Levels\\frisco_01.lev",rootDir+"\\Levels\\frisco.d3d",rootDir+"\\Levels\\pcar_frisco.den",rootDir+"\\Levels\\frisco_01.den",
              rootDir+"\\Levels\\pcar_frisco.cos",rootDir+"\\Levels\\frisco_01.cos",rootDir+"\\Levels\\frisco_wheels.wdf");
};

void MainWindow::loadLosAngelesFiles()
{
    openLevel(rootDir+"\\Levels\\LA_01.lev",rootDir+"\\Levels\\LA.d3d",rootDir+"\\Levels\\pcar_LA.den",rootDir+"\\Levels\\LA_01.den",
              rootDir+"\\Levels\\pcar_LA.cos",rootDir+"\\Levels\\LA_01.cos",rootDir+"\\Levels\\LA_wheels.wdf");
};

void MainWindow::loadNewYorkFiles()
{
    openLevel(rootDir+"\\Levels\\ny_01.lev",rootDir+"\\Levels\\newyork.d3d",rootDir+"\\Levels\\pcar_ny.den",rootDir+"\\Levels\\ny_01.den",
              rootDir+"\\Levels\\pcar_ny.cos",rootDir+"\\Levels\\ny_01.cos",rootDir+"\\Levels\\ny_wheels.wdf");
};

void MainWindow::loadTrainFiles()
{
    openLevel(rootDir+"\\Levels\\train.lev",rootDir+"\\Levels\\train.d3d",rootDir+"\\Levels\\pcar_train.den",rootDir+"\\Levels\\train.den",
              rootDir+"\\Levels\\pcar_train.cos",rootDir+"\\Levels\\train.cos",rootDir+"\\Levels\\train_wheels.wdf");
};

void MainWindow::loadIviewFiles()
{
    openLevel(rootDir+"\\Levels\\iview.lev",rootDir+"\\Levels\\iview.d3d",rootDir+"\\Levels\\pcar_iview.den",rootDir+"\\Levels\\iview.den",
              rootDir+"\\Levels\\pcar_iview.cos",rootDir+"\\Levels\\iview.cos",rootDir+"\\Levels\\iview_wheels.wdf");
};

void MainWindow::loadNewcastleFiles()
{
    openLevel(rootDir+"\\Levels\\nc_01.lev",rootDir+"\\Levels\\nc_01.d3d",rootDir+"\\Levels\\pcar_nc.den",rootDir+"\\Levels\\nc_01.den",
              rootDir+"\\Levels\\pcar_nc.cos",rootDir+"\\Levels\\nc_01.cos",rootDir+"\\Levels\\nc_wheels.wdf");
};

void MainWindow::launchGame()
{
    TCHAR workingdir[4096];
    GetCurrentDirectory(4096,workingdir);
    SetCurrentDirectoryA(rootDir.toLocal8Bit().data());
    QString path = rootDir+"\\game.exe";
    int ret = (int)ShellExecuteA(GetDesktopWindow(), "open", path.toLocal8Bit().data(), NULL, NULL, SW_SHOWNORMAL);
    SetCurrentDirectory(workingdir);
    if(ret <= 32)
    {
        QString error;
        switch(ret)
        {
            case ERROR_FILE_NOT_FOUND:
                error = tr("Could not find Game.exe to run!");
                break;
            case ERROR_PATH_NOT_FOUND:
                error = tr("Path to Game.exe invalid!");
                break;
            case SE_ERR_ACCESSDENIED:
                error = tr("Access to Game.exe denied!");
                break;
            default:
                error = tr("An error occured while trying to run Game.exe: ")+QString::number(ret);
                break;
        }
        QMessageBox msgBox(centralWindow);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(error);
        msgBox.exec();
    }
};

void MainWindow::openGameDirectory()
{
    int ret = (int)ShellExecuteA(GetDesktopWindow(), "open", rootDir.toLocal8Bit().data(), NULL, NULL, SW_SHOWNORMAL);

    if(ret <= 32)
    {
        QString error;
        switch(ret)
        {
            case ERROR_FILE_NOT_FOUND:
                error = tr("Could not find game folder!");
                break;
            case ERROR_PATH_NOT_FOUND:
                error = tr("Path to folder invalid!");
                break;
            case SE_ERR_ACCESSDENIED:
                error = tr("Access to folder denied!");
                break;
            default:
                error = tr("An error occured while trying to open the folder: ")+QString::number(ret);
                break;
        }
        QMessageBox msgBox(centralWindow);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(error);
        msgBox.exec();
    }
};

int MainWindow::copyFile(QString from,QString to)
{
    FILE* in = fopen(from.toLocal8Bit().data(),"rb");
    if(!in)
    return 1;
    FILE* out = fopen(to.toLocal8Bit().data(),"wb");
    if(!out)
    {
        fclose(in);
        return 2;
    }

    fseek(in,0,SEEK_END);
    unsigned int length = ftell(in);
    fseek(in,0,SEEK_SET);

    unsigned char* buffer;
    if(length <= 0x100000)
    buffer = new unsigned char[length];
    else buffer = new unsigned char[0x100000];

    if(!buffer)
    {
        fclose(out);
        fclose(in);
        return 3;
    }

    unsigned int offset = 0;
    while(offset < length)
    {
        if(offset+0x100000 <= length)
        {
            fread(buffer,1,0x100000,in);
            fwrite(buffer,1,0x100000,out);
        }
        else
        {
            fread(buffer,1,length-offset,in);
            fwrite(buffer,1,length-offset,out);
        }
        offset += 0x100000;
    }
    delete[] buffer;
    fclose(out);
    fclose(in);
    return 0;
};

void MainWindow::installPatch()
{
    int ret = QMessageBox::No;
    QString directory;
    if(!rootDir.isEmpty())
    {
        QMessageBox msgBox(centralWindow);
        msgBox.setText(tr("<b>Do you want to use the default Driver directory?</b>"));
        msgBox.setInformativeText(tr("Select 'No' to choose a different directory, or 'Yes' to install to: ")+rootDir);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        ret = msgBox.exec();
    }

    if(ret == QMessageBox::Yes)
    {
        directory = rootDir;
    }
    else if(ret == QMessageBox::No)
    {
        directory = QFileDialog::getExistingDirectory(this,QString(tr("Please select the 'Driver' root folder that you wish to install the patch to.")),QString("C:\\Program Files\\GT Interactive\\Driver\\Levels"));
    }
    else
    {
        return;
    }

    if(directory.isEmpty())
    return;

    directory = QDir::toNativeSeparators(directory);

    mainLog.Log("Preparing to install vertex patch.");

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("<b>Would you like to make a backup the files being replaced?</b>"));
    msgBox.setInformativeText(tr("The files will be saved under DCI_Backup in the driver directory ")+rootDir);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    ret = msgBox.exec();

    QString src,dest;

    if(ret == QMessageBox::Yes)
    {

        mkdir(QDir::toNativeSeparators(directory+"\\DCI_Backup").toLocal8Bit().data());
        mkdir(QDir::toNativeSeparators(directory+"\\DCI_Backup\\Levels").toLocal8Bit().data());

        const char* backup_files[] = {"\\Game.exe","\\Levels\\Miami_01.den","\\Levels\\NC_01.den"};
        for(int i = 0; i < 3; i++)
        {
            src = QDir::toNativeSeparators(directory+backup_files[i]);
            dest = QDir::toNativeSeparators(directory+"\\DCI_Backup"+backup_files[i]);
            mainLog.Log("Backing up file %s to %s.",src.toLocal8Bit().data(),dest.toLocal8Bit().data());
            ret = copyFile(src,dest);
            if(ret != 0)
            {
                mainLog.Log("Failed to backup file!");
                msgBox.setText(tr("<b>Failed to backup file! Aborting installation!</b>"));
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setInformativeText(tr("The file ")+src+tr(" failed to copy to ")+dest);
                msgBox.exec();
                return;
            }
        }
    }

    const char* patch_files[] = {"Vertex_Patch\\Game.exe","Vertex_Patch\\Levels\\pcar_miami.den","Vertex_Patch\\Levels\\pcar_frisco.den","Vertex_Patch\\Levels\\pcar_la.den",
                                 "Vertex_Patch\\Levels\\pcar_ny.den","Vertex_Patch\\Levels\\pcar_train.den","Vertex_Patch\\Levels\\pcar_iview.den","Vertex_Patch\\Levels\\pcar_nc.den",
                                 "Vertex_Patch\\Levels\\miami_01.den","Vertex_Patch\\Levels\\frisco_01.den","Vertex_Patch\\Levels\\la_01.den","Vertex_Patch\\Levels\\ny_01.den",
                                 "Vertex_Patch\\Levels\\iview.den","Vertex_Patch\\Levels\\train.den","Vertex_Patch\\Levels\\nc_01.den"};
    const char* dest_files[] = {"\\Game.exe","\\Levels\\pcar_miami.den","\\Levels\\pcar_frisco.den","\\Levels\\pcar_la.den",
                                "\\Levels\\pcar_ny.den","\\Levels\\pcar_train.den","\\Levels\\pcar_iview.den","\\Levels\\pcar_nc.den",
                                "\\Levels\\miami_01.den","\\Levels\\frisco_01.den","\\Levels\\la_01.den","\\Levels\\ny_01.den",
                                "\\Levels\\iview.den","\\Levels\\train.den","\\Levels\\nc_01.den"};
    mainLog.Log("Installing vertex patch to %s.",directory.toLocal8Bit().data());
    for(int i = 0; i < 15; i++)
    {
        mainLog.Log("Copying file %s.",patch_files[i]);
        src = QDir::toNativeSeparators(patch_files[i]);
        dest = QDir::toNativeSeparators(directory+dest_files[i]);
        ret = copyFile(src,dest);
        if(ret != 0)
        {
            mainLog.Log("File failed to copy!");
            msgBox.setText(tr("<b>Failed to copy file!</b>"));
            msgBox.setInformativeText(tr("The file ")+src+tr(" failed to copy to ")+dest);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
    }

    QMessageBox msgBox2(centralWindow);
    msgBox2.setText(tr("Vertex patch installed successfully!"));
    msgBox2.exec();
    mainLog.Log("Patch installed successfully.");
};

void MainWindow::saveAll()
{
//TODO: Don't forget to change save locations when using "Save As".
};

void MainWindow::saveLevel(QString filename, unsigned int bitfield)
{
    mainLog.Log("Saving level as %s with options %d...", filename.toLocal8Bit().data(), bitfield);

    //prepare message box for level saving failures
    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Level saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\temp.lev","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for level saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for level saving.");
        return;
    }

    int ret = level.saveToFile(file,bitfield);
    fclose(file);

    if(ret != 0)
    {
        switch(ret)
        {
            case 1:
                msgBox.setInformativeText(tr("Invalid I/O handle!"));
                break;
            default:
                msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
                break;
        }
        msgBox.exec();
        mainLog.Log("ERROR: Level saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old level file.");
            return;
        }
    }
    success = QFile::rename("temp\\temp.lev",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp level file.");
        return;
    }
    mainLog.Log("Finished saving level successfully.");
};

void MainWindow::saveD3D(QString filename)
{
    mainLog.Log("Saving d3d as %s...",filename.toLocal8Bit().data());

    //prepare message box for d3d saving failures
    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("D3D saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\temp.d3d","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for D3D saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for level saving.");
        return;
    }

    int ret = d3d.saveToFile(file);
    fclose(file);

    if(ret < 0)
    {
        switch(-ret)
        {
            default:
                msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
                break;
        }
        msgBox.exec();
        mainLog.Log("ERROR: D3D saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old d3d file.");
            return;
        }
    }
    success = QFile::rename("temp\\temp.d3d",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp d3d file.");
        return;
    }
    mainLog.Log("Finished saving d3d successfully.");
};

void MainWindow::savePlayerDenting(QString filename)
{
    mainLog.Log("Saving player denting as %s...",filename.toLocal8Bit().data());

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Player denting saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\player.den","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for player denting saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for player denting saving.");
        return;
    }

    int ret = playerDenting.savePlayerDentingToFile(file);
    fclose(file);

    if(ret < 0)
    {
        msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
        msgBox.exec();
        mainLog.Log("ERROR: Player denting saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old player denting file.");
            return;
        }
    }
    success = QFile::rename("temp\\player.den",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp player denting file.");
        return;
    }
    mainLog.Log("Finished saving player denting successfully.");
};

void MainWindow::saveCivilianDenting(QString filename)
{
    mainLog.Log("Saving civilian denting as %s...",filename.toLocal8Bit().data());

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Civilian denting saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\civilian.den","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for civilian denting saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for civilian denting saving.");
        return;
    }

    int ret = civilianDenting.saveCivilianDentingToFile(file);
    fclose(file);

    if(ret != 0)
    {
        msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
        msgBox.exec();
        mainLog.Log("ERROR: Civilian denting saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old civilian denting file.");
            return;
        }
    }
    success = QFile::rename("temp\\civilian.den",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp civilian denting file.");
        return;
    }
    mainLog.Log("Finished saving civilian denting successfully.");
};

void MainWindow::savePlayerCosmetics(QString filename)
{
    mainLog.Log("Saving player cosmetics as %s...", filename.toLocal8Bit().data());

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Player cosmetics saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\player.cos","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for player cosmetics saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for player cosmetics saving.");
        return;
    }

    int ret = playerCosmetics.saveCosmeticsToFile(file);
    fclose(file);

    if(ret != 0)
    {
        switch(ret)
        {
            default:
                msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
                break;
        }
        msgBox.exec();
        mainLog.Log("ERROR: Player cosmetics saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old player cosmetics file.");
            return;
        }
    }
    success = QFile::rename("temp\\player.cos",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp player cosmetics file.");
        return;
    }
    mainLog.Log("Finished saving player cosmetics successfully.");
};

void MainWindow::saveCivilianCosmetics(QString filename)
{
    mainLog.Log("Saving civilian cosmetics as %s...", filename.toLocal8Bit().data());

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Civilian cosmetics saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\civilian.cos","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for civilian cosmetics saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for civilian cosmetics saving.");
        return;
    }

    int ret = civilianCosmetics.saveCosmeticsToFile(file);
    fclose(file);

    if(ret != 0)
    {
        switch(ret)
        {
            default:
                msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
                break;
        }
        msgBox.exec();
        mainLog.Log("ERROR: Civilian cosmetics saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old civilian cosmetics file.");
            return;
        }
    }
    success = QFile::rename("temp\\civilian.cos",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp civilian cosmetics file.");
        return;
    }
    mainLog.Log("Finished saving civilian cosmetics successfully.");
};

void MainWindow::saveWheelDefinitions(QString filename)
{
    mainLog.Log("Saving wheel definition file as %s...", filename.toLocal8Bit().data());

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Wheel definition file saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\wheels.wdf","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for wheel definition file saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for wheel definition file saving.");
        return;
    }

    int ret = wheels.saveToFile(file);
    fclose(file);

    if(ret != 0)
    {
        switch(ret)
        {
            default:
                msgBox.setInformativeText(tr("Unknown error: ")+QString::number(ret));
                break;
        }
        msgBox.exec();
        mainLog.Log("ERROR: Wheel definition file saving returned failure code %d.",ret);
        return;
    }
    if(QFile::exists(filename))
    {
        success = QFile::remove(filename);
        if(!success)
        {
            msgBox.setInformativeText(tr("Failed to remove old file. (Do you need admin rights?)"));
            msgBox.exec();
            mainLog.Log("ERROR: Failed to remove old wheel definition file file.");
            return;
        }
    }
    success = QFile::rename("temp\\wheels.wdf",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp wheel definition file.");
        return;
    }
    mainLog.Log("Finished saving wheel definition file successfully.");
};
