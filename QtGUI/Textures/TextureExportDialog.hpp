#ifndef TEXTURE_EXPORT_DIALOG_HPP
#define TEXTURE_EXPORT_DIALOG_HPP

#include <QtGUI>
#include <FreeImage.h>
#include "../../Driver_Routines/driver_levels.hpp"
#include "../../Driver_Routines/driver_d3d.hpp"
#include "../../INIParser.hpp"

class FreeImageMemFile
{
    public:
        FreeImageMemFile();
        ~FreeImageMemFile();

        static unsigned __stdcall readProc(void* buffer, unsigned size, unsigned count, fi_handle handle);
        static unsigned __stdcall writeProc(void* buffer, unsigned size, unsigned count, fi_handle handle);
        static int __stdcall seekProc(fi_handle handle, long offset, int origin);
        static long __stdcall tellProc(fi_handle handle);

        unsigned readMemory(void* buffer, unsigned size, unsigned count);
        unsigned writeMemory(void* buffer, unsigned size, unsigned count);
        int seekMemory(long offset, int origin);
        long tellMemory();

        void reset();
        void resetMemory();

        const unsigned char* data();
        int size();

        FreeImageIO* getIOPointer();
        fi_handle getHandle();

    protected:
        void expandMemoryToSize(unsigned int size);

        unsigned int memoryPosition;
        unsigned int memorySize;
        unsigned int maxMemorySize; //max size before buffer needs expanded
        unsigned char* memoryBuffer;
        FreeImageIO ioStruct;
};

class TextureExportDialog : public QDialog
{
    Q_OBJECT

    public:
        TextureExportDialog(QWidget* parent = 0);
        ~TextureExportDialog();
        void setTextureIndex(int idx);
        void setPaletteIndex(int idx);
        void setTextureData(DriverTextures* texs);
        void setD3D(DriverD3D* newD3D);
        void loadSettings(INI* settings);
        void saveSettings(INI* settings);

    public slots:
        int exec();

    protected slots:
        void saveTexture();

        void updateBmpPreview();
        void selectBmpTruecolor();
        void selectBmpPaletted();
        void updateBmpMagicPink(int state);

        void updatePngPreview();
        void selectPngTruecolor();
        void selectPngPaletted();
        void forcePngUpdate();
        void setPng32Bit(int state);

        void updateJpegPreview();
        void updateJpegMagicPink(int state);

        void updateGifPreview();

        void updateTiffPreview();
        void selectTiffTruecolor();
        void selectTiffPaletted();
        void forceTiffUpdate();

        void updateTgaPreview();
        void selectTgaPaletted();
        void selectTgaHighcolor();
        void selectTgaTruecolor24();
        void selectTgaTruecolor32();
        void forceTgaUpdate();

    protected:
        void applyMagicPink(FIBITMAP* bitmap);
        void prepareFreeImageBitmap();
        void setPreviewSize(int size);
        void updatePreviewImage(FIBITMAP* bitmap);
        FIBITMAP* getConvertedTexture(int bpp, bool magicPink);

        void rebuildBmpCachedBitmap(bool paletted = false);
        void disableTransparencyItems();
        void enableTransparencyItems();

        DriverTextures* textureBlock;
        DriverD3D* d3d;

        int textureIndex;
        int paletteIndex;
        bool transEnabled;
        FIBITMAP* textureBitmap;
        FIBITMAP* cachedBitmap; //For storing a copy of the bitmap to be saved in the format to save in. (ex. jpeg saves in 24-bit)
        FreeImageMemFile previewMemFile;

        QStackedLayout* pages;
        QLabel* imagePreview;
        QLabel* imageSize;
        QCheckBox* exportDefinitions;
        QPushButton* editDefintions;

        QWidget* bmpPage;
        QButtonGroup* bmpButtons;
        QRadioButton* bmpTruecolor;
        QRadioButton* bmpPaletted;
        QCheckBox* bmpCompress;
        QCheckBox* bmpMagicPink;

        QWidget* pngPage;
        QButtonGroup* pngButtons;
        QRadioButton* pngTruecolor;
        QRadioButton* pngPaletted;
        QCheckBox* pngExportWithTransparency;
        QComboBox* pngCompression;
        QCheckBox* pngInterlaced;
        QCheckBox* pngMagicPink;

        QWidget* jpegPage;
        QSlider* jpegQualitySlider;
        QSpinBox* jpegQuality;
        QLabel* jpegQualityLabel;
        QCheckBox* jpegProgressive;
        QCheckBox* jpegMagicPink;

        QWidget* gifPage;
        QCheckBox* gifExportMultiPalette;
        QListWidget* gifPaletteList;
        FIMULTIBITMAP* animatedBitmap;

        QWidget* tiffPage;
        QButtonGroup* tiffButtons;
        QRadioButton* tiffPaletted;
        QRadioButton* tiffTruecolor;
        QComboBox* tiffCompression;
        QCheckBox* tiffMagicPink;

        QWidget* tgaPage;
        QButtonGroup* tgaButtons;
        QRadioButton* tgaPaletted;
        QRadioButton* tgaHighcolor15;
        QRadioButton* tgaTruecolor24;
        QRadioButton* tgaTruecolor32;
        QCheckBox* tgaCompress;
        QCheckBox* tgaMagicPink;
        int tgaLastDepth;

        QPushButton* saveButton;
        QPushButton* cancelButton;

        QString lastImageExportDir;
        QString selectedFilter;
        QString filename;
};

#endif
