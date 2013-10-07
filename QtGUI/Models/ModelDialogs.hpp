#ifndef MODEL_DIALOGS_HPP
#define MODEL_DIALOGS_HPP

#include <QtWidgets>
#include "../../Driver_Routines/driver_levels.hpp"

class ModelReferenceDialog : public QDialog
{
    Q_OBJECT

    public:
        ModelReferenceDialog(QWidget* parent = 0);
        void setLevel(DriverLevel* lev);

    signals:
        void modelChanged(int idx);
        void eventModelChanged(int idx);

    public slots:
        void doDialog(int modelIdx,bool event);

    protected slots:
        void makeReference();
        void autoDetect();

    protected:
        DriverLevel* level;
        bool eventModel;
        int modelIndex;

        QPushButton* okButton;
        QPushButton* cancelButton;
        QSpinBox* modelIndexBox;
        QPushButton* autoButton;
        QLabel* indexLabel;
};

class ModelListColumnDialog : public QDialog
{
    Q_OBJECT

    public:
        ModelListColumnDialog(QWidget* parent);

    signals:
        void flags1StateChanged(int state);
        void flags2StateChanged(int state);
        void vertsStateChanged(int state);
        void facesStateChanged(int state);
        void boundsStateChanged(int state);
        void modelRefStateChanged(int state);

    public slots:
        void setFlags1CheckState(Qt::CheckState state);
        void setFlags2CheckState(Qt::CheckState state);
        void setVertsCheckState(Qt::CheckState state);
        void setFacesCheckState(Qt::CheckState state);
        void setBoundsCheckState(Qt::CheckState state);
        void setModelRefCheckState(Qt::CheckState state);

    protected slots:
        void emitFlags1Signal(int state);
        void emitFlags2Signal(int state);
        void emitVertsSignal(int state);
        void emitFacesSignal(int state);
        void emitBoundsSignal(int state);
        void emitModelRefSignal(int state);

    protected:
        QCheckBox* flags1CheckBox;
        QCheckBox* flags2CheckBox;
        QCheckBox* vertsCheckBox;
        QCheckBox* facesCheckBox;
        QCheckBox* boundsCheckBox;
        QCheckBox* modelRefCheckBox;
        QPushButton* doneButton;
};

#endif
