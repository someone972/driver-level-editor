#ifndef SAVE_AS_DIALOG_HPP
#define SAVE_AS_DIALOG_HPP

#include <QtGUI>

class LevelSaveOptions : public QDialog
{
    Q_OBJECT

    public:
        LevelSaveOptions(QWidget* parent = 0);
        unsigned int getBitfield();
        void setBitfield(unsigned int saveWhat);

    public slots:
        void selectAll();
        void deselectAll();

    protected:
        QLabel* labels[21];
        QCheckBox* checkBoxes[21];
        QPushButton* doneButton;
        QPushButton* selectAllButton;
        QPushButton* selectNoneButton;
};

class SaveAsDialog : public QDialog
{
    Q_OBJECT

    public:
        SaveAsDialog(QWidget* parent = 0);

    signals:
        void saveLevel(QString filename,unsigned int saveWhat);
        void saveD3D(QString filename);
        void savePlayerCosmetics(QString filename);
        void saveCivilianCosmetics(QString filename);
        void savePlayerDenting(QString filename);
        void saveCivilianDenting(QString filename);
        void saveWheelDefinitions(QString filename);

    public slots:
        void setLevelFilename(QString filename);
        void setD3DFilename(QString filename);
        void setPlayerCosmeticsFilename(QString filename);
        void setCivilianCosmeticsFilename(QString filename);
        void setPlayerDentingFilename(QString filename);
        void setCivilianDentingFilename(QString filename);
        void setWheelDefinitionFilename(QString filename);

    protected slots:
        void browseLevel();
        void browseWheels();
        void browsePlayerDen();
        void browseCivilianDen();
        void browseD3D();
        void browsePlayerCos();
        void browseCivilianCos();
        void doLevelSaveOptions();
        void sendSaveSignals();

    protected:
        QLineEdit* filenames[7];
        QLabel* labels[7];
        QPushButton* buttons[7];
        QCheckBox* checkBoxes[7];
        QPushButton* levelOptionButton;
        QPushButton* saveSelected;
        QPushButton* cancelButton;
/*
        QString level;
        QString d3d;
        QString wheels;
        QString pcarDen;
        QString civcarDen;
        QString pcarCos;
        QString civcarCos;
*/
        LevelSaveOptions* levOptions;
};

#endif
