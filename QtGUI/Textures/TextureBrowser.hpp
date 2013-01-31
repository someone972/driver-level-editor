#ifndef TEXTURE_BROWSER_HPP
#define TEXTURE_BROWSER_HPP

#include <QtGUI>
#include <QGLWidget>
#include <FreeImage.h>
#include "../../Driver_Routines/driver_levels.hpp"
#include "../Palettes/AddPaletteDialog.hpp"
#include "../TextureList.hpp"
#include "TextureView.hpp"
#include "TexturePropertiesWidget.hpp"
#include "TextureExportDialog.hpp"
#include "TextureImportDialog.hpp"

const int FILTER_ALL_TEXTURES = 0;
const int FILTER_CAR_TEXTURES = 1;
const int FILTER_PALETTED_TEXTURES = 2;
const int FILTER_15_BIT_TEXTURES = 3;
const int FILTER_TRANSPARENT_TEXTURES = 4;

class NewTextureDialog : public QDialog
{
    Q_OBJECT

    public:
        NewTextureDialog(QWidget* parent = 0);

    signals:
        void newTextureRequested(unsigned short flags, short carnum);

    protected slots:
        void doneButtonPushed();

    protected:
        QLabel* directions;
        QPushButton* doneButton;
        QPushButton* cancelButton;
        TexturePropertiesWidget* textureProperties;
};

class TextureBrowser : public QWidget, IDriverTextureEvents, IDriverLevelEvents
{
    Q_OBJECT

    public:
        TextureBrowser(QWidget* parent = NULL, const QGLWidget * shareWidget = NULL, Qt::WindowFlags f = 0);
        TextureBrowser(const QGLFormat& format,QWidget* parent = NULL, const QGLWidget * shareWidget = NULL, Qt::WindowFlags f = 0);
        ~TextureBrowser();
        void setTextureList(LevelTextures* texs);
        void setLevel(DriverLevel* lev);
        void setD3D(DriverD3D* newd3d);
        void setupEditMenu(QMenu* editMenu);
        void loadSettings(INI* settings);
        void saveSettings(INI* settings);

        //Event handlers
        void textureRemoved(int idx);
        void textureChanged(int idx);
        void textureInserted(int idx);
        void textureMoved(int from, int to);
        void levelSaved(bool aboutToBe);
        void levelDestroyed();
        void levelOpened();

    public slots:
        void texturesChanged();
        void setTextureSize(int idx);

    protected slots:
        void syncTextureSelection(int s);
        void syncPaletteSelection(int s);
        void applyFilter(int idx);
        void doDisplayContext(const QPoint& point);
        void doPaletteContext(const QPoint& point);
        void doAddPaletteDialog();
        void removePaletteFromList();
        void importTexture();
        void exportTexture();
        void rebuildPaletteList();
        void deleteUnusedPalettes();
        void deleteCurrentTexture();
        void pairTextures();
        void handleCarNumberChange(int car);
        void handlePropertiesChange(unsigned short properties);
        void addNewTexture(unsigned short flags, short carnum);

    protected:
        void setup();
        void refreshIndexList();
        void moveTexture(int from, int to);
        void insertTexture(int idx, const DriverTexture* tex);
        void hideEvent(QHideEvent* event);
        void showEvent(QShowEvent* event);
        void hideMenuActions();
        void showMenuActions();
        void checkForPairs();

        DriverLevel* level;
        DriverD3D* d3d;
        LevelTextures* textureList;

        int textureSelection, paletteSelection;
        QString lastImageImportDir;

        NewTextureDialog* newTextureDialog;

        TextureView* display;
        QHBoxLayout* mainLayout;

        QFrame* informationFrame;
        QLabel* textureSizeLabel;
        QLabel* filterLabel;
        QLabel* viewModeLabel;
        QComboBox* textureSizeSelect;
        QComboBox* filterSelect;
        QComboBox* viewModeSelect;
        QListWidget* indexList;
        QListWidget* paletteList;
        QSpinBox* textureNumber;

        QMenu* textureDisplayContext;
        QAction* exportAction;
        QAction* importAction;
        QAction* deleteAction;
        QAction* moveAction;

        QMenu* paletteListContext;
        QAction* removePaletteFromListAction;
        QAction* addPaletteToListAction;
        QAction* editPaletteAction;
        QLabel* palettesLabel;

        QAction* editAddTextureAction;
        QAction* editEditPalettesAction;
        QAction* editDeleteUnusedAction;
        QAction* editSeparatorAction;

        QFrame* propertiesFrame;
        TexturePropertiesWidget* textureProperties;
        AddPaletteDialog* addPaletteDialog;

        QWidget* pairAdvisoryWidget;
        QLabel* pairAdvisoryText;
        QPushButton* pairAdvisoryButton;

        TextureExportDialog* exportDialog;
        TextureImportDialog* importDialog;
};

#endif
