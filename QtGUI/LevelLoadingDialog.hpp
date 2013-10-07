#ifndef LEVEL_LOADING_DIALOG_HPP
#define LEVEL_LOADING_DIALOG_HPP

#include <QtWidgets>
#include "../Driver_Routines/driver_levels.hpp"
#include "../Driver_Routines/driver_cos.hpp"
#include "../Driver_Routines/driver_den.hpp"
#include "../Driver_Routines/driver_wdf.hpp"
#include "../Driver_Routines/driver_d3d.hpp"

class LevelLoadingDialog : public QDialog
{
    Q_OBJECT

    public:
        LevelLoadingDialog(QWidget* parent = NULL);
        void setLevel(DriverLevel* _level);
        void setD3D(DriverD3D* _d3d);
        void setWheelDefinitions(DriverWheelDefinitions* _wheels);
        void setPlayerCosmetics(CosmeticsContainer* _playerCos);
        void setCivilianCosmetics(CosmeticsContainer* _civilianCos);
        void setPlayerDenting(DriverDenting* _playerDen);
        void setCivilianDenting(DriverDenting* _civilianDen);
        void setLog(DebugLogger* newLog);

        bool levelLoaded();
        bool d3dLoaded();
        bool wheelDefinitionsLoaded();
        bool playerCosmeticsLoaded();
        bool playerDentingLoaded();
        bool civilianCosmeticsLoaded();
        bool civilianDentingLoaded();

        bool load(QString levelFilename, QString d3dFilename, QString wheelsFilename, QString playerCosFilename, QString civilianCosFilename, QString playerDenFilename, QString civilianDenFilename);
        int exec();

    protected:
        void buildTable();
        void cleanupLevelData();
        DriverLevel* level;
        DriverD3D* d3d;
        DriverWheelDefinitions* wheels;
        CosmeticsContainer* playerCos;
        CosmeticsContainer* civilianCos;
        DriverDenting* playerDen;
        DriverDenting* civilianDen;

        QString filenames[7];
        QString errorStrings[7];
        int errorCodes[7];

        DebugLogger dummyLog;
        DebugLogger* mainLog;

        QLabel* warningLabel;
        QLabel* questionLabel;
        QTableWidget* resultTable;
        QPushButton* continueButton;
        QPushButton* abortButton;
};

#endif
