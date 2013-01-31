#ifndef TEXTURE_IMPORT_DIALOG_HPP
#define TEXTURE_IMPORT_DIALOG_HPP

#include <QtGUI>
#include <FreeImage.h>
#include "../../Driver_Routines/driver_levels.hpp"
#include "../../Driver_Routines/driver_d3d.hpp"
#include "../../INIParser.hpp"

class TextureImportDialog : public QDialog
{
    Q_OBJECT

    public:
        TextureImportDialog(QWidget* parent = NULL);
        ~TextureImportDialog();

        int exec();

        void setTextureData(DriverTextures* texs);
        void setD3D(DriverD3D* newD3D);
        void setTextureIndex(int idx);
        void setPaletteIndex(int idx);

        void loadSettings(INI* settings);
        void saveSettings(INI* settings);

        void connectChangeHandler(QWidget* handler);

    protected slots:
        void selectDefault();
        void selectMagicPink();
        void selectAlpha();
        void importImage();

    protected:
        void setPreview(FIBITMAP* dib);

        DriverTextures* textureBlock;
        DriverD3D* d3d;
        int textureIndex;
        int paletteIndex;
        bool lastNon32BitState;
        int prefered32BitState;
        FIBITMAP* bitmap;

        QString filename;
        QString selectedFilter;
        QString lastImageImportDir;

        QLabel* imagePreview;
        QLabel* transLabel;
        QRadioButton* useDefaultColor;
        QRadioButton* useMagicPink;
        QRadioButton* useAlpha;
        QPushButton* importButton;
        QPushButton* cancelButton;
};

#endif
