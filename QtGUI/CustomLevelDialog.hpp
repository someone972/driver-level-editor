#ifndef CUSTOM_LEVEL_DIALOG_HPP
#define CUSTOM_LEVEL_DIALOG_HPP

#include <QtGUI>

class CustomLevelDialog : public QDialog
{
    Q_OBJECT

    public:
        CustomLevelDialog(QWidget* parent = 0);
        QString getLevelString();
        QString getD3DString();
        QString getWheelsString();
        QString getPlayerDenString();
        QString getCivilianDenString();
        QString getPlayerCosString();
        QString getCivilianCosString();
        QString getLastDirectory();
        void setLastDirectory(QString dir);

    protected slots:
        void browseLevel();
        void browseWheels();
        void browsePlayerDen();
        void browseCivilianDen();
        void browseD3D();
        void browsePlayerCos();
        void browseCivilianCos();

    protected:
        QLineEdit* filenames[7];
        QLabel* labels[7];
        QPushButton* buttons[7];
        QPushButton* openButton;
        QPushButton* cancelButton;

        QString level;
        QString d3d;
        QString wheels;
        QString pcarDen;
        QString civcarDen;
        QString pcarCos;
        QString civcarCos;
        QString lastDirectory;
};

#endif
