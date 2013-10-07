#ifndef PALETTE_EDITOR_HPP
#define PALETTE_EDITOR_HPP

#include <QtGUI>
#include "PaletteDisplay.hpp"
#include "../../Driver_Routines/driver_levels.hpp"

class PaletteEditor : public QWidget
{
    Q_OBJECT

    public:
        PaletteEditor(QWidget* parent = 0);
        ~PaletteEditor();

    public slots:
        void setPalette(const DriverPalette* pal);
        void setPreviewImage(const unsigned char* image);
        void updateSelectionPreview(const QItemSelection & selected, const QItemSelection & deselected);

    protected:
        PaletteDisplay* display;
        QLabel* imagePreview;
        DriverPalette palette;
        unsigned char imageData[256*256];
};

class PaletteEditorDialog : public QDialog
{
    Q_OBJECT

    public:
        PaletteEditorDialog(QWidget* parent = 0);

    public slots:
        void setPalette(DriverPalette* pal);

    protected:
        PaletteEditor* editor;
};

#endif
