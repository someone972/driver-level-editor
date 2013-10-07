#ifndef MODEL_VIEW_PANEL_HPP
#define MODEL_VIEW_PANEL_HPP

#include <QtWidgets>
#include "ModelView.hpp"
#include "DriverModelListModel.hpp"
#include "ModelDialogs.hpp"
#include "../../Driver_Routines/driver_levels.hpp"
#include "../../Log_Routines/debug_logger.hpp"

//TODO: Add level events to ModelViewPanel and update table size on open.

class ModelViewPanel : public QSplitter, IDriverModelEvents, IDriverLevelEvents
{
    Q_OBJECT

    public:
        ModelViewPanel(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0, DebugLogger* logger = NULL);
        ~ModelViewPanel();
        ModelView* glViewer();

        void modelsDestroyed(ModelContainer* container);
        void modelsReset(ModelContainer* container, bool aboutToBe);
        void modelsOpened(ModelContainer* container);
        void modelsSaved(ModelContainer* container, bool aboutToBe);
        void modelInserted(ModelContainer* container, int idx);
        void levelDestroyed();

    signals:
        void modelChanged(int idx);
        void eventModelChanged(int idx);
        void heightmapTileChanged(int idx);
        void worldSectorChanged(int idx);
        void modelInserted(int idx);
        void eventModelInserted(int idx);

    public slots:
        void loadSettings();
        void saveSettings();

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
        void setTextureProvider(TextureList* list);
        void insertModel(int index);
        void insertEventModel(int index);

        void modelIndexChanged(const QModelIndex& current, const QModelIndex& previous);
        void eventModelIndexChanged(const QModelIndex& current, const QModelIndex& previous);

        void setFlags1ColumnState(int state);
        void setFlags2ColumnState(int state);
        void setVertsColumnState(int state);
        void setFacesColumnState(int state);
        void setBoundsColumnState(int state);
        void setModelRefColumnState(int state);

    protected slots:
        void doListHeaderContextMenu(const QPoint& pos);
        void doModelsContextMenu(const QPoint& pos);
        void doEventModelsContextMenu(const QPoint& pos);
        void tabChanged(int newTab);

    protected:
        void setColumnVisibility();

        ModelView* modelView;
        DriverModelListModel* namesListModel;
        QSortFilterProxyModel* namesProxyModel;
        QTableView* namesList;
        ModelListColumnDialog* columnSelector;

        DriverModelListModel* eventNamesModel;
        QSortFilterProxyModel* eventProxyModel;
        QTableView* eventNamesList;

        QTabWidget* tabs;
        RenderOptionsWidget* renderOptions;
        QWidget* verticalWidget;
        ModelReferenceDialog* referenceDialog;

        DebugLogger dummy;
        DebugLogger* log;
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

        QMenu* headerContext;
        QAction* selectColumns;

        QModelIndex savedIndex;
        DriverLevel* level;

        bool flags1Enabled;
        bool flags2Enabled;
        bool vertsEnabled;
        bool facesEnabled;
        bool boundsEnabled;
        bool modelRefEnabled;
};

#endif
