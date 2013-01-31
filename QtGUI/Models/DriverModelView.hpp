#ifndef DRIVER_MODEL_VIEW_HPP
#define DRIVER_MODEL_VIEW_HPP
#include <QtOpenGL>
#include <QtGUI>
#include "../../Driver_Routines/driver_levels.hpp"

class ModelNameList : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ModelNameList(DriverLevel* lev = NULL,QObject *parent = 0);
        void setLevel(DriverLevel* lev);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    public slots:
        void resetList();
        void updateRow(int row);
        bool insertRow(int row, const QModelIndex & parent = QModelIndex());
        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());

    private:
        DriverLevel* level;
};

class EventModelList : public QAbstractTableModel
{
    Q_OBJECT

    public:
        EventModelList(DriverLevel* lev = NULL,QObject *parent = 0);
        void setLevel(DriverLevel* lev);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;

    public slots:
        void resetList();
        void updateRow(int row);
        void prepareForInsert(int row, int count);
        void finishInsert();

    private:
        DriverLevel* level;
};

class ModelView : public QWidget
{
    Q_OBJECT

    public:
        ModelView(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
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

class ModelViewPanel : public QWidget
{
    Q_OBJECT

    public:
        ModelViewPanel(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);

    signals:
        void modelChanged(int idx);
        void eventModelChanged(int idx);
        void heightmapTileChanged(int idx);
        void worldSectorChanged(int idx);
        void modelInserted(int idx);
        void eventModelInserted(int idx);

    public slots:
        void renameCurrentModel();
        void dereferenceCurrentModel();
        void currentModelMakeReference();
        void deleteCurrentModel();
        void insertModelBeforeCurrent();
        void insertModelAfterCurrent();
        void exportCurrentModel();
        void importCurrentModel();
        void openCurrentModelProperties();
        void setLevel(DriverLevel* lev);
        void insertModel(int index);
        void insertEventModel(int index);

    protected slots:
        void doModelsContextMenu(const QPoint& pos);
        void doEventModelsContextMenu(const QPoint& pos);

        void handleLevelChange();

    protected:
        ModelView* modelView;
        ModelNameList* namesListModel;
        QTableView* namesList;
        EventModelList* eventNamesModel;
        QTableView* eventNamesList;
        QTabWidget* tabs;
        RenderOptionsWidget* renderOptions;
        ModelReferenceDialog* referenceDialog;

        QMenu* listContext;
        QAction* deleteModel;
        QAction* insertModelBefore;
        QAction* insertModelAfter;
        QAction* importModel;
        QAction* exportModel;
        QAction* rename;
        QAction* makeReference;
        QAction* dereference;
        QAction* modelProperties;

        QModelIndex savedIndex;
        DriverLevel* level;
};

#endif
