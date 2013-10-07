#ifndef ADD_PALETTE_DIALOG_HPP
#define ADD_PALETTE_DIALOG_HPP

#include <QtGUI>
#include "PaletteDisplay.hpp"
#include "../../Driver_Routines/driver_levels.hpp"

class AddPaletteDialog : public QDialog, IDriverTextureEvents
{
    Q_OBJECT

    public:
        AddPaletteDialog(QWidget* parent = 0);
        ~AddPaletteDialog();
        void setTextureData(DriverTextures* texs);
        int getSelection();

        void texturesReset(bool aboutToBe);
        void texturesOpened();
        void texturesDestroyed();
        void paletteChanged(int idx);
        void paletteRemoved(int idx);
        void paletteInserted(int idx);

    public slots:
        void setTexture(int index);

    protected slots:
        void setSelection(int row);

    protected:
        void refreshList();

        DriverTextures* textureBlock;
        int currentTexture;

        QFrame* previewFrame;
        PaletteDisplay* palDisplay;
        PalettedImage* preview;

        QFrame* listFrame;
        QLabel* palettesLabel;
        QListWidget* paletteList;
        QPushButton* editPalettes;

        QPushButton* addButton;
        QPushButton* cancelButton;
};

#endif
