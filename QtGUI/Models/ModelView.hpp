#ifndef DRIVER_MODEL_VIEW_HPP
#define DRIVER_MODEL_VIEW_HPP

#include <QtOpenGL>
#include <QtWidgets>
#include "../TextureList.hpp"
#include "../../Driver_Routines/driver_levels.hpp"
#include "ModelRenderer.hpp"

class ModelView : public QGLWidget
{
    //TODO: Better camera control (right click zoom, sensitivity etc)
    //TODO: Set event models to render
    Q_OBJECT

    public:
        ModelView(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0, DebugLogger* logger = NULL);
        ~ModelView();

        void setLevel(DriverLevel* lev);
        void setTextureProvider(TextureList* list);

    public slots:
        void setModelIndex(int idx);
        void setEventModelIndex(int idx);
        void viewEventModels(bool view);
        void loadSettings();
        void saveSettings();

    protected:
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);
        void rebuildModelRenderer();

        DebugLogger dummy;
        DebugLogger* log;
        DriverLevel* level;
        TextureList* textures;
        ModelRenderer* render;
        ModelShaders* shaders;
        ModelMatrixHandler* matrixHandler;
        BasicCamera camera;

        int modelIndex;
        int eventModelIndex;
        bool viewingEvent;
        QPoint lastPoint;
        double wheelSensitivity;
        double mouseSensitivity;
        double zoomSensitivity;
};

class RenderOptionsWidget : public QWidget
{
    Q_OBJECT

    public:
        RenderOptionsWidget(QWidget* parent = 0);
};

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
