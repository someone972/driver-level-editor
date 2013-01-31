#include "PaletteEditor.hpp"

PaletteEditor::PaletteEditor(QWidget* parent) : QWidget(parent)
{
    display = new PaletteDisplay(this);
    imagePreview = new QLabel(this);
    memset(imageData,0,256*256);
};

void PaletteEditor::setPalette(const DriverPalette* pal)
{
    palette = *pal;
    display->setPalette(&palette);
};

void PaletteEditor::setPreviewImage(const unsigned char* image)
{
    if(image)
    memcpy(imageData, image, 256*256);
};

void PaletteEditor::updateSelectionPreview(const QItemSelection & selected, const QItemSelection & deselected)
{

};

PaletteEditor::~PaletteEditor()
{

};

PaletteEditorDialog::PaletteEditorDialog(QWidget* parent) : QDialog(parent)
{
    editor = new PaletteEditor(this);
};

void PaletteEditorDialog::setPalette(DriverPalette* pal)
{
    editor->setPalette(pal);
};
