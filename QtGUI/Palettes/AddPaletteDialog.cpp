#include "AddPaletteDialog.hpp"

AddPaletteDialog::AddPaletteDialog(QWidget* parent) : QDialog(parent)
{
    textureBlock = NULL;
    currentTexture = -1;

    palDisplay = new PaletteDisplay(this);
    palDisplay->setGridSize(8);
    preview = new PalettedImage(this);

    previewFrame = new QFrame(this);
    previewFrame->setFrameStyle(QFrame::Sunken | QFrame::Box);
    previewFrame->setLineWidth(1);

    QVBoxLayout* previewLayout = new QVBoxLayout();
    previewLayout->addWidget(palDisplay);
    previewLayout->addWidget(preview);
    previewFrame->setLayout(previewLayout);

    palettesLabel = new QLabel(tr("Palettes:"),this);
    paletteList = new QListWidget(this);
    editPalettes = new QPushButton(tr("Edit Palettes"),this);

    listFrame = new QFrame(this);
    listFrame->setFrameStyle(QFrame::Sunken | QFrame::Box);
    listFrame->setLineWidth(1);
    listFrame->setMaximumWidth(200);

    QVBoxLayout* listLayout = new QVBoxLayout();
    listLayout->addWidget(palettesLabel);
    listLayout->addWidget(paletteList);
    listLayout->addWidget(editPalettes);
    listFrame->setLayout(listLayout);

    addButton = new QPushButton(tr("Add"),this);
    addButton->setMaximumWidth(100);
    cancelButton = new QPushButton(tr("Cancel"),this);
    cancelButton->setMaximumWidth(100);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(cancelButton);

    QHBoxLayout* frameLayout = new QHBoxLayout();
    frameLayout->addWidget(listFrame);
    frameLayout->addWidget(previewFrame);

    QVBoxLayout* masterLayout = new QVBoxLayout();
    masterLayout->addLayout(frameLayout);
    masterLayout->addLayout(buttonsLayout);
    setLayout(masterLayout);

    setFixedHeight(400);
    setFixedWidth(500);

    connect(paletteList, SIGNAL(currentRowChanged(int)), this, SLOT(setSelection(int)));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(accept()));
};

AddPaletteDialog::~AddPaletteDialog()
{
    if(textureBlock)
        textureBlock->unregisterEventHandler(this);
};

void AddPaletteDialog::setTextureData(DriverTextures* texs)
{
    if(textureBlock)
        textureBlock->unregisterEventHandler(this);
    textureBlock = texs;
    if(textureBlock)
        textureBlock->registerEventHandler(this);
    refreshList();
};

void AddPaletteDialog::setTexture(int index)
{
    if(textureBlock)
    {
        if(index >= 0 && index < textureBlock->getNumTextures())
        {
            preview->setTextureData(textureBlock->getTexture(index));
        }
    }
};

void AddPaletteDialog::refreshList()
{
    paletteList->clear();
    if(textureBlock)
    {
        for(int i = 0; i < textureBlock->getNumPalettes(); i++)
        {
            paletteList->addItem(QString("%1: Slot %2").arg(i).arg(textureBlock->getPalette(i)->paletteNumber));
        }
    }
};

void AddPaletteDialog::setSelection(int row)
{
    if(textureBlock)
    {
        if(row >= 0 && row < textureBlock->getNumPalettes())
        {
            palDisplay->setPalette(textureBlock->getPalette(row));
            preview->setPalette(textureBlock->getPalette(row));
        }
    }
};

void AddPaletteDialog::texturesReset(bool aboutToBe)
{
    if(!aboutToBe)
        refreshList();
};

void AddPaletteDialog::texturesOpened()
{
    refreshList();
};

void AddPaletteDialog::paletteChanged(int idx)
{
    if(idx == paletteList->currentRow())
    {
        //Update widgets using palette.
        setSelection(idx);
    }
};

void AddPaletteDialog::paletteRemoved(int /*idx*/)
{
    refreshList();
};

void AddPaletteDialog::paletteInserted(int /*idx*/)
{
    refreshList();
};

void AddPaletteDialog::texturesDestroyed()
{
    textureBlock = NULL;
    refreshList();
};

int AddPaletteDialog::getSelection()
{
    return paletteList->currentRow();
};
