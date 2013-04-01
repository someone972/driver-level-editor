#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGUI>
#include <windows.h>
#include <dir.h>
#include <FreeImage.h>

#include "Driver_Routines/driver_levels.hpp"
#include "Driver_Routines/driver_d3d.hpp"
#include "Driver_Routines/driver_cos.hpp"
#include "Driver_Routines/driver_den.hpp"
#include "Driver_Routines/driver_wdf.hpp"

#include "QtGUI/AboutDialog.hpp"
#include "QtGUI/CustomLevelDialog.hpp"
#include "QtGUI/Models/DriverModelView.hpp"
#include "QtGUI/SaveAsDialog.hpp"
#include "QtGUI/Textures/TextureDefinitionEditor.hpp"
#include "QtGUI/Textures/TextureBrowser.hpp"
#include "QtGUI/StartScreen.hpp"

#include "Log_Routines/debug_logger.hpp"
#include "Log_Routines/default_loggers.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

    signals:
        void levelChanged();

        //Signals emitted after change has been handled.
        void updateTexture(int);
        void updateTextures();
        void updatePalette(int);

        void updateD3D();

    public slots:
        void cleanupLevelData();
        void setConvenienceActionsEnabled(bool enabled);

        void loadCustomFiles();
        void loadMiamiFiles();
        void loadFriscoFiles();
        void loadLosAngelesFiles();
        void loadNewYorkFiles();
        void loadTrainFiles();
        void loadIviewFiles();
        void loadNewcastleFiles();

        void launchGame();
        void openGameDirectory();
        void installPatch();

        void viewModels();
        void viewDefinitionEditor();
        void viewTextureBrowser();

        void saveAll();
        void saveLevel(QString filename, unsigned int bitfield);
        void saveD3D(QString filename);
        void savePlayerDenting(QString filename);
        void saveCivilianDenting(QString filename);
        void savePlayerCosmetics(QString filename);
        void saveCivilianCosmetics(QString filename);
        void saveWheelDefinitions(QString filename);

    protected:
        int copyFile(QString from, QString to);
        void openLevel(QString levFile,QString d3dFile,QString pcarDenFile,QString civcarDenFile,QString pcarCosFile,QString civcarCosFile,QString wheelFile);

        void createMenus();
        void createActions();

        void saveSettings();
        void loadSettings();

        void setChangeIndicator();

        FileLogger mainLog;
        FileLogger importLog;
        FileLogger levelLog;

        bool levelHasChanged,d3dHasChanged,civCosHasChanged,playerCosHasChanged,civDentHasChanged,playerDentHasChanged,wdfHasChanged;

        DriverLevel level;
        LevelTextures levelTextures;
        CosmeticsContainer playerCosmetics;
        CosmeticsContainer civilianCosmetics;
        DriverDenting denting;
        DriverD3D d3d;
        DriverWheelDefinitions wheels;

        QString rootDir;
        QString lastOpenDirectory;
        QString levelString,d3dString,playerDenString,civilianDenString,playerCosString,civilianCosString,wheelsString;

        QMenu* fileMenu;
        QMenu* openMenu;
        QAction* openCustom;
        QAction* openMiami;
        QAction* openFrisco;
        QAction* openLosAngeles;
        QAction* openNewYork;
        QAction* openTrain;
        QAction* openIview;
        QAction* openNewcastle;
        QAction* exitAction;
        QAction* saveAs;

        QMenu* editMenu;

        QMenu* viewMenu;
        QAction* viewModelsAction;
        QAction* viewTextureBrowserAction;
        QAction* viewDefinitionEditorAction;

        QMenu* driverMenu;
        QAction* launchGameAction;
        QAction* openGameDirAction;
        QAction* installPatchAction;

        QMenu* settingsMenu;

        QMenu* helpMenu;
        QAction* helpTopics;
        QAction* aboutAction;

        StartScreen* startPage;
        QStackedWidget* centralWindow;
        CustomLevelDialog* customLevelDialog;
        AboutDialog* aboutDialog;
        ModelViewPanel* modelViewPanel;
        SaveAsDialog* saveDialog;
        TextureDefinitionEditor* definitionEditor;
        TextureBrowser* textureBrowser;
};

#endif
