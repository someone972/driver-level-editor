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

#ifdef DEBUG_ENABLED
    mainLog.Log("Executable is a debug build.");
    setWindowTitle(QApplication::translate("windowtitle", "Driver Car Importer [DEBUG][*]"));
#else
    mainLog.Log("Executable is a release build.");
    setWindowTitle(QApplication::translate("windowtitle", "Driver Car Importer[*]"));
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

    modelViewPanel = new ModelViewPanel(this);
    modelViewPanel->setLevel(&level);
    connect(this, SIGNAL(levelChanged()), modelViewPanel, SLOT(handleLevelChange()));

    definitionEditor = new TextureDefinitionEditor(this); //Don't forget, this needs a share widget when model viewing is done
    definitionEditor->setLevel(&level);
    definitionEditor->setTextureProvider(&levelTextures);
    definitionEditor->overlay()->makeCurrent();

    textureBrowser = new TextureBrowser(this,definitionEditor->overlay());
    textureBrowser->setLevel(&level);
    textureBrowser->setTextureList(&levelTextures);
    textureBrowser->setD3D(&d3d);
    connect(this, SIGNAL(levelChanged()), textureBrowser, SLOT(texturesChanged()));

    definitionEditor->overlay()->makeCurrent();

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
    if(settings.load_ini("settings.ini") != 0)
    mainLog.Log("WARNING: Failed to open settings file! Default settings will be used.");
    else mainLog.Log("Settings loaded successfully.");

    mainLog.setLogPriority(settings.get_int("log_levels","main_log",DEFAULT_PRIORITY));
    levelLog.setLogPriority(settings.get_int("log_levels","level_log",DEFAULT_PRIORITY));
    int levelPriorities[NUMBER_OF_BLOCKS];

    levelPriorities[BLOCK_TEXTURES] = settings.get_int("log_levels","block_textures",levelLog.getLogPriority());
    levelPriorities[BLOCK_MODELS] = settings.get_int("log_levels","block_models",levelLog.getLogPriority());
    levelPriorities[BLOCK_WORLD] = settings.get_int("log_levels","block_world",levelLog.getLogPriority());
    levelPriorities[BLOCK_RANDOM_MODEL_PLACEMENT] = settings.get_int("log_levels","block_random_model_placement",levelLog.getLogPriority());
    levelPriorities[BLOCK_TEXTURE_DEFINITIONS] = settings.get_int("log_levels","block_texture_definitions",levelLog.getLogPriority());
    levelPriorities[BLOCK_ROAD_TABLE] = settings.get_int("log_levels","block_road_table",levelLog.getLogPriority());
    levelPriorities[BLOCK_ROAD_CONNECTIONS] = settings.get_int("log_levels","block_road_connections",levelLog.getLogPriority());
    levelPriorities[BLOCK_INTERSECTIONS] = settings.get_int("log_levels","block_intersections",levelLog.getLogPriority());
    levelPriorities[BLOCK_HEIGHTMAP_TILES] = settings.get_int("log_levels","block_heightmap_tiles",levelLog.getLogPriority());
    levelPriorities[BLOCK_HEIGHTMAP] = settings.get_int("log_levels","block_heightmap",levelLog.getLogPriority());
    levelPriorities[BLOCK_MODEL_NAMES] = settings.get_int("log_levels","block_model_names",levelLog.getLogPriority());
    levelPriorities[BLOCK_EVENT_MODELS] = settings.get_int("log_levels","block_event_models",levelLog.getLogPriority());
    levelPriorities[BLOCK_VISIBILITY] = settings.get_int("log_levels","block_visibility",levelLog.getLogPriority());
    levelPriorities[BLOCK_SECTOR_TEXTURE_USAGE] = settings.get_int("log_levels","block_sector_texture_usage",levelLog.getLogPriority());
    levelPriorities[BLOCK_ROAD_SECTIONS] = settings.get_int("log_levels","block_road_sections",levelLog.getLogPriority());
    levelPriorities[BLOCK_INTERSECTION_POSITIONS] = settings.get_int("log_levels","block_intersection_positions",levelLog.getLogPriority());
    levelPriorities[BLOCK_LAMPS] = settings.get_int("log_levels","block_lamps",levelLog.getLogPriority());
    levelPriorities[BLOCK_CHAIR_PLACEMENT] = settings.get_int("log_levels","block_chair_placement",levelLog.getLogPriority());
    level.setLogPriorities(levelPriorities);

    resize(settings.get_int("main_window","width",800),settings.get_int("main_window","height",600));
    move(settings.get_int("main_window","x",QApplication::desktop()->width()/2-frameSize().width()/2),settings.get_int("main_window","y",QApplication::desktop()->height()/2-frameSize().height()/2));
    if(settings.get_bool("main_window","maximized",false))
    showMaximized();
    else showNormal();
    show();

    customLevelDialog->setLastDirectory(settings.get_string("directories","lastOpenFileDir","C:\\Program Files\\GT Interactive\\Driver\\Levels"));

    if(settings.get_bool(NULL,"firstRun",true))
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
                settings.set_string("directories","rootDir",newRootDir.toLocal8Bit().data());
            }
        }
    }

    rootDir = settings.get_string("directories","rootDir",NULL);

    if(!rootDir.isEmpty())
    setConvenienceActionsEnabled(true);

    textureBrowser->loadSettings(&settings);
};

void MainWindow::saveSettings()
{
    settings.set_bool(NULL,"firstRun",false);
    settings.set_string("directories","rootDir",rootDir.toLocal8Bit().data());

    settings.set_bool("main_window","maximized",isMaximized());
    showNormal();
    settings.set_int("main_window","width",width());
    settings.set_int("main_window","height",height());
    settings.set_int("main_window","x",pos().x());
    settings.set_int("main_window","y",pos().y());

    textureBrowser->saveSettings(&settings);

    settings.save_ini("settings.ini");
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

void MainWindow::cleanupLevelData()
{
    level.cleanup();
    d3d.cleanup();
//    playerCosmetics.reset();
//    civilianCosmetics.reset();
    wheels.reset();
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
    cleanupLevelData();

    levelString = levFile;
    d3dString = d3dFile;
    playerDenString = pcarDenFile;
    civilianCosString = civcarDenFile;
    playerCosString = pcarCosFile;
    civilianCosString = civcarCosFile;
    wheelsString = wheelFile;

    QMessageBox msgBox(centralWindow);
    msgBox.setIcon(QMessageBox::Warning);

    int ret = level.loadFromFile(levFile.toLocal8Bit().data(),LEV_ALL);
    emit levelChanged();
    if(ret != 0)
    {
        if(ret == 1)
        {
            msgBox.setText(tr("Failed to open level file!"));
        }
        else if(ret == 2)
        {
            level.cleanup();
            msgBox.setText(tr("Level is corrupt! Could not load!"));
        }
        else
        {
            msgBox.setText(tr("Unknown error occured when loading level!"));
        }

        msgBox.exec();
        return;
    }

    ret = d3d.loadFromFile(d3dFile.toLocal8Bit().data());
    if(ret < 0)
    {
        if(ret == -1)
        {
            msgBox.setText(tr("Failed to open d3d file!"));
        }
        else if(ret == -2)
        {
            level.cleanup();
            msgBox.setText(tr("D3D is corrupt! Could not load!"));
        }
        else
        {
            msgBox.setText(tr("Unknown error occured when loading d3d!"));
        }

        msgBox.exec();
        return;
    }
    levelTextures.rebuildAllTextures();
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
        msgBox.setInformativeText(tr("Select 'No' to choose a different directory, or 'yes' to install to: ")+rootDir);
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

    mainLog.Log("Preparing to install vertex patch.");

    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("<b>Would you like to make a backup the files being replaced?</b>"));
    msgBox.setInformativeText(tr("The files will be saved under DCI_Backup in the driver directory.")+rootDir);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    ret = msgBox.exec();
    if(ret == QMessageBox::Yes)
    {

        mkdir(QString(directory+"\\DCI_Backup").toLocal8Bit().data());
        mkdir(QString(directory+"\\DCI_Backup\\Levels").toLocal8Bit().data());

        const char* backup_files[] = {"\\Game.exe","\\Levels\\Miami_01.den","\\Levels\\NC_01.den"};
        for(int i = 0; i < 3; i++)
        {
            mainLog.Log("Backing up file %s to %s.",QString(directory+backup_files[i]).toLocal8Bit().data(),QString(directory+"\\DCI_Backup"+backup_files[i]).toLocal8Bit().data());
            ret = copyFile(directory+backup_files[i],directory+"\\DCI_Backup"+backup_files[i]);
            if(ret != 0)
            {
                mainLog.Log("Failed to backup file!");
                msgBox.setText(tr("<b>Failed to backup file! Aborting installation!</b>"));
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setInformativeText(tr("The file ")+backup_files[i]+tr(" failed to copy to ")+directory+"\\DCI_Backup"+backup_files[i]);
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
        ret = copyFile(QString(patch_files[i]),directory+dest_files[i]);
        if(ret != 0)
        {
            mainLog.Log("File failed to copy!");
            msgBox.setText(tr("<b>Failed to copy file!</b>"));
            msgBox.setInformativeText(tr("The file ")+patch_files[i]+tr(" failed to copy to ")+directory+dest_files[i]);
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
            //TODO: fill with actual error messages
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

    //prepare message box for denting saving failures
    QMessageBox msgBox(centralWindow);
    msgBox.setText(tr("Playercar denting saving failed!"));
    msgBox.setIcon(QMessageBox::Warning);
    bool success;

    FILE* file = fopen("temp\\player.den","wb");
    if(!file)
    {
        msgBox.setInformativeText(tr("Unable to create temporary file for denting saving!"));
        msgBox.exec();
        mainLog.Log("ERROR: Unable to create temporary file for denting saving.");
        return;
    }
    //TODO: insert function for saving denting
    int ret = 0;//playerDenting.saveToFile(file);
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
            mainLog.Log("ERROR: Failed to remove old denting file.");
            return;
        }
    }
    success = QFile::rename("temp\\temp.d3d",filename);
    if(!success)
    {
        msgBox.setInformativeText(tr("Failed to rename and move temp file."));
        msgBox.exec();
        mainLog.Log("ERROR: Failed to rename/move temp denting file.");
        return;
    }
    mainLog.Log("Finished saving player denting successfully.");
};

void MainWindow::saveCivilianDenting(QString filename)
{
    //TODO: Implement civilian denting saving
};

void MainWindow::savePlayerCosmetics(QString filename)
{
    //TODO: Implement player cosmetics saving
};

void MainWindow::saveCivilianCosmetics(QString filename)
{
    //TODO: Implement civilian cosmetics saving
};

void MainWindow::saveWheelDefinitions(QString filename)
{
    //TODO: Implement wheel definition saving
};

void MainWindow::handleTextureChange(int tex)
{
    //FIXME: Deprecated. Remove.
    levelHasChanged = true;
    setWindowModified(true);
    levelTextures.rebuildTexture(tex);
    emit updateTexture(tex);
};

void MainWindow::handlePaletteChange(int pal)
{
    //FIXME: Deprecated. Remove.
    levelHasChanged = true;
    setWindowModified(true);
    for(int i = 0; i < d3d.getNumEntries(); i++)
    {
        D3DEntry* entry = d3d.getEntry(i);
        for(int j = 0; j < entry->getNumPaletteIndicies(); j++)
        {
            if(entry->getPaletteIndex(j) == pal)
            {
                levelTextures.rebuildTexture(entry->getTexture());
                emit updateTexture(entry->getTexture());
                break;
            }
        }
    }
    emit updatePalette(pal);
};

void MainWindow::handleD3dChange()
{
    //FIXME: Deprecated. Remove.
    d3dHasChanged = true;
    setWindowModified(true);
    levelTextures.rebuildAllTextures();
    emit updateTextures();
    emit updateD3D();
};

void MainWindow::handleD3dEntryChange(int tex, int pal)
{
    //FIXME: Deprecated. Remove.
    d3dHasChanged = true;
    setWindowModified(true);
    levelTextures.rebuildTexture(tex);
    emit updateTexture(tex);
    emit updateD3D();
};
