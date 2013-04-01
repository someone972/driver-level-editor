#ifndef TEXTURE_DEFINITION_EDITOR_HPP
#define TEXTURE_DEFINITION_EDITOR_HPP

#include <QtGUI>
#include <QtOpengl>
#include "../../Driver_Routines/driver_levels.hpp"
#include "../EventFilters.hpp"
#include "../TextureList.hpp"

class TextureDefinitionList : public QAbstractTableModel, IDriverLevelEvents
{
    Q_OBJECT

    public:
        TextureDefinitionList(DriverLevel* lev = NULL,QObject *parent = 0);
        ~TextureDefinitionList();
        void setLevel(DriverLevel* lev);
        void levelDestroyed();

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        bool removeRows(int row,int count,const QModelIndex& parent = QModelIndex());
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
        bool insertRow(int row, const QModelIndex & parent = QModelIndex());
        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());

        int insertDefinition(int position,TextureDefinition def);

    public slots:
        void resetList();
        void loadSettings();

    protected:
        bool enforceValidData;
        bool resortOnChange;
        DriverLevel* level;
};

class OverlayedTexture : public QGLWidget
{
    Q_OBJECT

    public:
        OverlayedTexture(const QGLFormat& format = QGLFormat(),QWidget* parent = 0,const QGLWidget*  shareWidget = 0, Qt::WindowFlags f = 0);
        void setTextureProvider(TextureList* list);

    signals:
        void overlayRectChanged(int x,int y,int w,int h);

    public slots:
        void setOverlayRect(int x,int y,int w,int h);
        void setTexture(int idx);
        void setOverlayEnabled(bool en);

    protected:
        static const int NO_ACTION = 0;
        static const int DRAG_ACTION = 1;
        static const int RESIZE_LEFT = 2;
        static const int RESIZE_RIGHT = 4;
        static const int RESIZE_TOP = 8;
        static const int RESIZE_BOTTOM = 16;

        void keyPressEvent(QKeyEvent* event);
        void keyReleaseEvent(QKeyEvent* event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent *event);

        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        TextureList* texlist;
        int texidx;
        int state;
        int hoverState;
        int offsetX,offsetY;
        int x,y,w,h;
        bool enabled;
        bool resizing;
        bool moving;
};

class TextureDefinitionEditor : public QWidget, IDriverTexDefEvents
{
    Q_OBJECT

    public:
        TextureDefinitionEditor(QWidget* parent = 0,const QGLWidget*  shareWidget = 0, Qt::WindowFlags f = 0);
        ~TextureDefinitionEditor();
        void setLevel(DriverLevel* lev);
        void setTextureProvider(TextureList* list);
        OverlayedTexture* overlay();

        //Event handlers
        void definitionsReset(bool aboutToBe);
        void definitionsOpened();
        void definitionsDestroyed();
        void definitionChanged(int idx);

    public slots:
        void loadSettings();
        void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
        void updateTexture(const QModelIndex&,const QModelIndex&);
        void doViewContext(const QPoint& point);
        void deletePositionEntry();
        void insertPositionEntry();
        void positionChanged(int x,int y, int w,int h);

    protected:
        int savedIndex;
        int selection;

        QTableView* texDefsView;
        TextureDefinitionList* texDefsModel;
        OverlayedTexture* overlayedTex;

        DriverLevel* level;
        TextureList* texList;

        KeypressFilter* keyFilter;
        EventFilterDelegate* filterDelegate;

        QMenu* contextmenu;
        QAction* deleteAction;
        QAction* insertAction;
};

#endif
