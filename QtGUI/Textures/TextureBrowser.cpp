#include "TextureBrowser.hpp"

NewTextureDialog::NewTextureDialog(QWidget* parent) : QDialog(parent)
{
    directions = new QLabel(tr("Select the inital flags for the texture:"), this);
    textureProperties = new TexturePropertiesWidget(this);
    doneButton = new QPushButton(tr("&Done"),this);
    doneButton->setFixedWidth(70);
    cancelButton = new QPushButton(tr("&Cancel"),this);
    cancelButton->setFixedWidth(70);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(doneButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(directions);
    mainLayout->addWidget(textureProperties);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setMinimumWidth(250);

    connect(doneButton, SIGNAL(clicked()), this, SLOT(doneButtonPushed()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
};

void NewTextureDialog::doneButtonPushed()
{
    emit newTextureRequested(textureProperties->getFlags(), textureProperties->getCarNumber());
    accept();
};

TextureBrowser::TextureBrowser(QWidget* parent, const QGLWidget * shareWidget, Qt::WindowFlags f) : QWidget(parent)
{
    display = new TextureView(this,shareWidget,f);
    setup();
};

TextureBrowser::TextureBrowser(const QGLFormat& format,QWidget* parent, const QGLWidget * shareWidget, Qt::WindowFlags f) : QWidget(parent)
{
    display = new TextureView(format,this,shareWidget,f);
    setup();
};

void TextureBrowser::setup()
{
    d3d = NULL;
    textureList = NULL;
    level = NULL;

    newTextureDialog = new NewTextureDialog(this);

    pairAdvisoryWidget = new QWidget(this);
    QPalette pal = pairAdvisoryWidget->palette();
    pal.setBrush(QPalette::Background, QColor(255,70,60));
    pairAdvisoryWidget->setAutoFillBackground(true);
    pairAdvisoryWidget->setPalette(pal);
    pairAdvisoryWidget->hide();

    pairAdvisoryText = new QLabel(tr(" *Warning: Paletted textures need paired."),this);
    pairAdvisoryButton = new QPushButton(tr("Pair Now"),this);
    QHBoxLayout* pairLayout = new QHBoxLayout();
    pairLayout->setMargin(0);
    pairLayout->addWidget(pairAdvisoryText, 1, Qt::AlignLeft);
    pairLayout->addWidget(pairAdvisoryButton, 0, Qt::AlignRight);
    pairAdvisoryWidget->setLayout(pairLayout);

    addPaletteDialog = new AddPaletteDialog(this);

    informationFrame = new QFrame(this);
    informationFrame->setMinimumWidth(200);
    informationFrame->setMaximumWidth(200);
    informationFrame->setFrameStyle(QFrame::Sunken | QFrame::Box);
    informationFrame->setLineWidth(1);

    propertiesFrame = new QFrame(this);
    propertiesFrame->setMinimumWidth(200);
    propertiesFrame->setMaximumWidth(200);
    propertiesFrame->setFrameStyle(QFrame::Sunken | QFrame::Box);
    propertiesFrame->setLineWidth(1);
    textureProperties = new TexturePropertiesWidget(this);
    textureProperties->setTextureProperties(NULL);

    textureSizeLabel = new QLabel(tr("Texture Size:"),this);
    textureSizeSelect = new QComboBox(this);
    textureSizeSelect->addItem("64");
    textureSizeSelect->addItem("128");
    textureSizeSelect->addItem(tr("256 (Native)"));
    textureSizeSelect->addItem("512");

    filterLabel = new QLabel(tr("Filter:"),this);
    filterSelect = new QComboBox(this);
    filterSelect->addItem(tr("All Textures"));
    filterSelect->addItem(tr("Car Textures"));
    filterSelect->addItem(tr("Paletted"));
    filterSelect->addItem(tr("15-Bit"));
    filterSelect->addItem(tr("Has Transparency"));

    viewModeLabel = new QLabel(tr("View mode:"),this);
    viewModeSelect = new QComboBox(this);
    viewModeSelect->addItem(tr("Current Palette Only"));
    viewModeSelect->addItem(tr("All Palettes"));
    viewModeSelect->addItem(tr("All Palettes on One Line"));

    textureNumber = new QSpinBox(this);
    textureNumber->setMinimum(0);
    textureNumber->setMaximum(0);

    indexList = new QListWidget(this);
    paletteList = new QListWidget(this);
    paletteList->hide();
    palettesLabel = new QLabel(tr("Palettes:"),this);
    palettesLabel->hide();

    paletteListContext = new QMenu(this);
    removePaletteFromListAction = new QAction(tr("Remove from list"),this);
    addPaletteToListAction = new QAction(tr("Add palette"),this);
    editPaletteAction = new QAction(tr("Edit palette"),this);
    paletteListContext->addAction(removePaletteFromListAction);
    paletteListContext->addAction(editPaletteAction);
    paletteListContext->addAction(addPaletteToListAction);

    exportDialog = new TextureExportDialog(this);
    importDialog = new TextureImportDialog(this);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->addWidget(textureSizeLabel);
    infoLayout->addWidget(textureSizeSelect);
    infoLayout->addWidget(viewModeLabel);
    infoLayout->addWidget(viewModeSelect);
    infoLayout->addWidget(filterLabel);
    infoLayout->addWidget(filterSelect);
    infoLayout->addWidget(textureNumber);
    infoLayout->addWidget(indexList);
    infoLayout->addWidget(palettesLabel);
    infoLayout->addWidget(paletteList);
    informationFrame->setLayout(infoLayout);

    QVBoxLayout* propertiesLayout = new QVBoxLayout();
    propertiesLayout->addWidget(textureProperties);
    propertiesFrame->setLayout(propertiesLayout);

    QVBoxLayout* sideLayout = new QVBoxLayout();
    sideLayout->addWidget(informationFrame);
    sideLayout->addWidget(propertiesFrame);

    QVBoxLayout* displayLayout = new QVBoxLayout();
    displayLayout->addWidget(pairAdvisoryWidget);
    displayLayout->addWidget(display);

    mainLayout = new QHBoxLayout();
    mainLayout->addLayout(displayLayout);
    mainLayout->addLayout(sideLayout);
    setLayout(mainLayout);

    textureDisplayContext = new QMenu(this);
    importAction = new QAction(tr("Import"),this);
    exportAction = new QAction(tr("Export"),this);
    deleteAction = new QAction(tr("Delete"),this);
    moveAction = new QAction(tr("Move"),this);

    editEditPalettesAction = new QAction(tr("Edit Palettes"),this);
    editDeleteUnusedAction = new QAction(tr("Delete Unused Palettes"),this);
    editSeparatorAction = new QAction(this);
    editSeparatorAction->setSeparator(true);
    editAddTextureAction = new QAction(tr("Add New Texture"),this);

    textureDisplayContext->addAction(importAction);
    textureDisplayContext->addAction(exportAction);
    textureDisplayContext->addSeparator();
    textureDisplayContext->addAction(deleteAction);
    textureDisplayContext->addAction(moveAction);

    display->setContextMenuPolicy(Qt::CustomContextMenu);
    paletteList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(pairAdvisoryButton, SIGNAL(clicked()), this, SLOT(pairTextures()));

    connect(display, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doDisplayContext(const QPoint&)));
    connect(importAction, SIGNAL(triggered()), this, SLOT(importTexture()));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportTexture()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteCurrentTexture()));

    connect(paletteList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doPaletteContext(const QPoint&)));
    connect(removePaletteFromListAction, SIGNAL(triggered()), this, SLOT(removePaletteFromList()));
    connect(addPaletteToListAction, SIGNAL(triggered()), this, SLOT(doAddPaletteDialog()));

    connect(editDeleteUnusedAction, SIGNAL(triggered()), this, SLOT(deleteUnusedPalettes()));
    connect(editAddTextureAction, SIGNAL(triggered()), newTextureDialog, SLOT(exec()));
    connect(newTextureDialog, SIGNAL(newTextureRequested(unsigned short, short)), this, SLOT(addNewTexture(unsigned short, short)));

    connect(textureSizeSelect, SIGNAL(activated(int)), this, SLOT( setTextureSize(int)));
    connect(filterSelect, SIGNAL(activated(int)), this, SLOT( applyFilter(int)));
    connect(viewModeSelect, SIGNAL(activated(int)), display->viewer(), SLOT(setViewMode(int)));

    connect(textureProperties, SIGNAL(propertiesChanged(unsigned short)), this, SLOT(handlePropertiesChange(unsigned short)));
    connect(textureProperties, SIGNAL(carNumberChanged(int)), this, SLOT(handleCarNumberChange(int)));

    connect(display->viewer(), SIGNAL(textureSelectionChanged(int)), this, SLOT(syncTextureSelection(int)));
    connect(textureNumber, SIGNAL(valueChanged(int)), this, SLOT(syncTextureSelection(int)));
    connect(indexList, SIGNAL(currentRowChanged(int)), this, SLOT(syncTextureSelection(int)));
    connect(paletteList, SIGNAL(currentRowChanged(int)), this, SLOT(syncPaletteSelection(int)));
    connect(display->viewer(), SIGNAL(paletteSelectionChanged(int)), this, SLOT(syncPaletteSelection(int)));
};

TextureBrowser::~TextureBrowser()
{
    if(level)
    {
        level->unregisterEventHandler(this);
        level->textures.unregisterEventHandler(this);
        display->setTextureData(NULL);
        exportDialog->setTextureData(NULL);
        importDialog->setTextureData(NULL);
        addPaletteDialog->setTextureData(NULL);
        texturesChanged();
    }
};

void TextureBrowser::doDisplayContext(const QPoint& point)
{
    if(display->textureAt(point,&textureSelection,&paletteSelection))
    textureDisplayContext->exec(QCursor::pos());
};

void TextureBrowser::doPaletteContext(const QPoint& point)
{
    if(paletteList->indexAt(point).row() >= 0)
    {
        editPaletteAction->setEnabled(true);
        removePaletteFromListAction->setEnabled(true);
    }
    else
    {
        editPaletteAction->setEnabled(false);
        removePaletteFromListAction->setEnabled(false);
    }
    paletteListContext->exec(QCursor::pos());
};

void TextureBrowser::importTexture()
{
    importDialog->setTextureIndex(textureSelection);
    importDialog->setPaletteIndex(textureList->getCurrentPalette(textureSelection));
    importDialog->exec();
    display->viewer()->update();
};

void TextureBrowser::exportTexture()
{
    exportDialog->setTextureIndex(textureSelection);
    exportDialog->setPaletteIndex(textureList->getCurrentPalette(textureSelection));
    exportDialog->exec();
};

void TextureBrowser::applyFilter(int idx)
{
    if(level)
    {
        for(int i = 0; i < level->textures.getNumTextures(); i++)
        {
            switch(idx)
            {
                case FILTER_ALL_TEXTURES:
                    indexList->setRowHidden(i,false);
                    display->viewer()->setTextureHidden(i,false);
                    break;
                case FILTER_CAR_TEXTURES:
                    if(level->textures.getTexture(i)->getCarNumber() != -1 && (level->textures.getTexture(i)->isCleanTexture() || level->textures.getTexture(i)->isDamageTexture()))
                    {
                        indexList->setRowHidden(i,false);
                        display->viewer()->setTextureHidden(i,false);
                    }
                    else
                    {
                        indexList->setRowHidden(i,true);
                        display->viewer()->setTextureHidden(i,true);
                    }
                    break;
                case FILTER_PALETTED_TEXTURES:
                    if(level->textures.getTexture(i)->usesPalette())
                    {
                        indexList->setRowHidden(i,false);
                        display->viewer()->setTextureHidden(i,false);
                    }
                    else
                    {
                        indexList->setRowHidden(i,true);
                        display->viewer()->setTextureHidden(i,true);
                    }
                    break;
                case FILTER_15_BIT_TEXTURES:
                    if(level->textures.getTexture(i)->usesPalette())
                    {
                        indexList->setRowHidden(i,true);
                        display->viewer()->setTextureHidden(i,true);
                    }
                    else
                    {
                        indexList->setRowHidden(i,false);
                        display->viewer()->setTextureHidden(i,false);
                    }
                    break;
                case FILTER_TRANSPARENT_TEXTURES:
                    if(level->textures.getTexture(i)->hasTransparency())
                    {
                        indexList->setRowHidden(i,false);
                        display->viewer()->setTextureHidden(i,false);
                    }
                    else
                    {
                        indexList->setRowHidden(i,true);
                        display->viewer()->setTextureHidden(i,true);
                    }
                    break;
                default:
                    break;
            }
        }
    }
};

void TextureBrowser::syncTextureSelection(int s)
{
    display->setSelectedTexture(s);
    textureNumber->setValue(s);
    indexList->setCurrentItem(indexList->item(s));
    if(level)
    {
        const DriverTexture* tex = level->textures.getTexture(s);
        textureProperties->setTextureProperties(tex);
        if(tex)
        {
            if(tex->usesPalette() && d3d)
            {
                rebuildPaletteList();
                paletteList->show();
                palettesLabel->show();
            }
            else
            {
                palettesLabel->hide();
                paletteList->hide();
            }
        }
    }
};

void TextureBrowser::syncPaletteSelection(int index)
{
    paletteList->setCurrentRow(index);
    if(textureList)
    textureList->setCurrentPalette(textureNumber->value(), paletteList->currentRow());
    display->setSelectedPalette(index);
    display->viewer()->update();
};

void TextureBrowser::setTextureSize(int idx)
{
    if(idx >= 0 && idx < 4)
    {
        int sizes[4] = {64,128,256,512};
        display->viewer()->setTextureSize(sizes[idx]);
    }
};

void TextureBrowser::setTextureList(LevelTextures* texs)
{
    display->setGLTextureProvider(texs);
    textureList = texs;
};

void TextureBrowser::setD3D(DriverD3D* newd3d)
{
    exportDialog->setD3D(newd3d);
    importDialog->setD3D(newd3d);
    d3d = newd3d;
};

void TextureBrowser::loadSettings()
{
    exportDialog->loadSettings();
    importDialog->loadSettings();

    QSettings settings;
    setTextureSize(settings.value("TextureBrowser/textureSize",0).toInt());
};

void TextureBrowser::saveSettings()
{
    exportDialog->saveSettings();
    importDialog->saveSettings();

    QSettings settings;
    settings.setValue("TextureBrowser/textureSize",textureSizeSelect->currentIndex());
};

void TextureBrowser::texturesChanged()
{
    display->viewer()->update();
    applyFilter(filterSelect->currentIndex());
    if(level)
    {
        if(level->textures.getNumTextures() > 0)
        textureNumber->setMaximum(level->textures.getNumTextures()-1);
        else textureNumber->setMaximum(0);
    }
    refreshIndexList();
};

void TextureBrowser::refreshIndexList()
{
    if(level)
    {
        if(level->textures.getNumTextures() < indexList->count())
        {
            for(int i = indexList->count()-1; i > level->textures.getNumTextures()-1; i--)
            delete indexList->takeItem(i);
        }
        else if(level->textures.getNumTextures() > indexList->count())
        {
            for(int i = indexList->count(); i < level->textures.getNumTextures(); i++)
            {
                QString item = tr("Texture ");
                item += QString::number(i);
                indexList->addItem(item);
            }
        }
    }
    else indexList->clear();
};

void TextureBrowser::setLevel(DriverLevel* lev)
{
    if(level)
    {
        level->unregisterEventHandler(this);
        level->textures.unregisterEventHandler(this);
    }
    level = lev;
    DriverTextures* textures = NULL;
    if(level)
    textures = &level->textures;

    display->setTextureData(textures);
    exportDialog->setTextureData(textures);
    importDialog->setTextureData(textures);
    addPaletteDialog->setTextureData(textures);
    texturesChanged();
    if(level)
    {
        level->registerEventHandler(this);
        level->textures.registerEventHandler(this);
    }
};

void TextureBrowser::hideEvent(QHideEvent* event)
{
    if(!event->spontaneous())
    hideMenuActions();
};

void TextureBrowser::showEvent(QShowEvent* event)
{
    if(!event->spontaneous())
    showMenuActions();
};

void TextureBrowser::hideMenuActions()
{
    editDeleteUnusedAction->setVisible(false);
    editEditPalettesAction->setVisible(false);
    editSeparatorAction->setVisible(false);
    editAddTextureAction->setVisible(false);
};

void TextureBrowser::showMenuActions()
{
    editDeleteUnusedAction->setVisible(true);
    editEditPalettesAction->setVisible(true);
    editSeparatorAction->setVisible(true);
    editAddTextureAction->setVisible(true);
};

void TextureBrowser::setupEditMenu(QMenu* editMenu)
{
    editMenu->addAction(editEditPalettesAction);
    editMenu->addAction(editDeleteUnusedAction);
    editMenu->addAction(editSeparatorAction);
    editMenu->addAction(editAddTextureAction);
};

void TextureBrowser::handleCarNumberChange(int car)
{
    if(level)
    {
        //I don't like how inefficient a deep copy is here.
        DriverTexture newTex = *level->textures.getTexture(indexList->currentRow());
        newTex.setCarNumber(car);
        level->textures.setTexture(indexList->currentRow(),&newTex);
    }
};

void TextureBrowser::handlePropertiesChange(unsigned short properties)
{
    if(level)
    {
        const DriverTexture* tex = level->textures.getTexture(indexList->currentRow());
        if(tex)
        {
            DriverTexture newTex = *tex;

            if(((properties&TEX_USES_PALETTE) != 0) != tex->usesPalette())
            {
                QMessageBox msg(this);
                msg.setText(tr("Are you sure you want to change this texture's color depth?"));
                msg.setInformativeText(tr("Converting this texture may cause loss of color information or incompatibility with the game."));
                msg.setIcon(QMessageBox::Warning);
                msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
                int ret = msg.exec();

                if(ret == QMessageBox::Yes)
                {
                    newTex.setFlags(properties);
                    if(tex->usesPalette())
                    {
                        const unsigned char *oldScanline;
                        unsigned char* scanline = new unsigned char[256*2];

                        DriverPalette palette;

                        D3DEntry* entry = d3d->getTextureEntry(indexList->currentRow());
                        if(entry)
                        {
                            if(entry->getNumPaletteIndicies() > 0)
                            {
                                DriverPalette* entryPalette = level->textures.getPalette(entry->getPaletteIndex(0));
                                if(entryPalette)
                                    palette = *entryPalette;
                            }
                        }

                        for(int i = 0; i < 256; i++)
                        {
                            oldScanline = tex->getScanLine(i);
                            for(int j = 0; j < 256; j++)
                            {
                                color_4ub color = palette.colors[oldScanline[j]];
                                *(unsigned short*)(scanline+j*2) = ((color.r>>3)<<10) + ((color.g>>3)<<5) + (color.b>>3);
                            }
                            newTex.setScanLine(i, scanline);
                        }
                        delete[] scanline;
                        d3d->removeTextureEntry(indexList->currentRow());
                    }
                    else
                    {
                        FIBITMAP* dib = FreeImage_Allocate(256, 256, 24);
                        if(dib)
                        {
                            const unsigned char *textureData = tex->getData();
                            for(int y = 0; y < 256; y++)
                            {
                                BYTE *bits = FreeImage_GetScanLine(dib, y);
                                for(int x = 0; x < 256; x++)
                                {
                                    unsigned short color = *(const unsigned short*)(textureData+x*2+y*512);
                                    bits[FI_RGBA_RED] = ((color&0x7C00)>>10)<<3;
                                    bits[FI_RGBA_GREEN] = ((color&0x03E0)>>5)<<3;
                                    bits[FI_RGBA_BLUE] = ((color&0x001F)<<3);
                                    bits += 3;
                                }
                            }
                            FIBITMAP* converted = FreeImage_ColorQuantize(dib,FIQ_NNQUANT);
                            FreeImage_Unload(dib);

                            RGBQUAD *fiPal = FreeImage_GetPalette(converted);
                            DriverPalette palette;

                            for (int i = 0; i < 256; i++)
                            {
                                palette.colors[i].r = fiPal[i].rgbRed;
                                palette.colors[i].g = fiPal[i].rgbGreen;
                                palette.colors[i].b = fiPal[i].rgbBlue;
                            }

                            for(int i = 0; i < 256; i++)
                            {
                                newTex.setScanLine(i,FreeImage_GetScanLine(converted,i));
                            }
                            FreeImage_Unload(converted);

                            int paletteSlot = level->textures.getNextOpenSlot();
                            palette.paletteNumber = paletteSlot;
                            level->textures.setPaletteIndexed(&palette);
                            d3d->addEntry(indexList->currentRow());
                            D3DEntry* entry = d3d->getTextureEntry(indexList->currentRow());
                            if(entry)
                            {
                                entry->addPaletteIndex(paletteSlot);
                            }
                        }
                    }
                }
                else
                {
                    properties &= ~TEX_USES_PALETTE;
                    if(tex->usesPalette())
                        properties |= TEX_USES_PALETTE;
                    newTex.setFlags(properties);
                    textureProperties->setTextureProperties(&newTex);
                }
            }
            else
            {
                newTex.setFlags(properties);
            }
            level->textures.setTexture(indexList->currentRow(),&newTex);
        }
    }
};

void TextureBrowser::rebuildPaletteList()
{
    paletteList->blockSignals(true);
    paletteList->clear();
    D3DEntry* entry = d3d->getTextureEntry(indexList->currentRow());
    if(entry)
    {
        if(entry->getNumPaletteIndicies() == 0)
        {
            paletteList->addItem(tr("No palettes set."));
        }
        else
        {
            for(int i = 0; i < entry->getNumPaletteIndicies(); i++)
            {
                QString text = QString::number(i)+": "+tr("Slot ")+QString::number(entry->getPaletteIndex(i));
                paletteList->addItem(text);
            }
        }
    }
    else
    {
        paletteList->addItem(tr("No palette entry."));
    }
    paletteList->blockSignals(false);
    paletteList->setCurrentRow(display->viewer()->getSelectedPalette());
};

void TextureBrowser::removePaletteFromList()
{
    if(d3d)
    {
        D3DEntry* entry = d3d->getTextureEntry(indexList->currentRow());
        if(entry)
        {
            int oldRow = paletteList->currentRow();
            entry->removePaletteIndex(paletteList->currentRow());

            rebuildPaletteList();
            if(oldRow < paletteList->model()->rowCount())
            syncPaletteSelection(oldRow);
            else if(paletteList->model()->rowCount() > 0)
            syncPaletteSelection(paletteList->model()->rowCount()-1);
        }
    }
};

void TextureBrowser::deleteUnusedPalettes()
{
    QMessageBox msg(this);
    msg.setText(tr("Are you sure you want to remove unused palettes?"));
    msg.setInformativeText(tr("Palettes will be permanently deleted."));
    msg.setIcon(QMessageBox::Warning);
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    int ret = msg.exec();

    if(ret == QMessageBox::Yes)
    {
        int numRemoved = 0;
        int highestIndex = 0;
        for(int i = 0; i < level->textures.getNumPalettes(); i++)
        {
            if(level->textures.getPalette(i)->paletteNumber > highestIndex)
            highestIndex = level->textures.getPalette(i)->paletteNumber;
        }
        if(highestIndex)
        {
            bool* isUsed = new bool[highestIndex];
            for(int i = 0; i < highestIndex; i++)
            {
                isUsed[i] = false;
            }
            for(int i = 0; i < d3d->getNumEntries(); i++)
            {
                D3DEntry* entry = d3d->getEntry(i);
                for(int j = 0; j < entry->getNumPaletteIndicies(); j++)
                {
                    int slot = entry->getPaletteIndex(j);
                    if(slot >= 0 && slot < highestIndex)
                        isUsed[slot] = true;
                }
            }
            for(int i = 0; i < highestIndex; i++)
            {
                if(!isUsed[i])
                {
                    if(level->textures.getIndexedPalette(i))
                    {
                        level->textures.removeIndexedPalette(i);
                        numRemoved++;
                    }
                }
            }
        }

        msg.setText(tr("Finished removing palettes."));
        msg.setInformativeText(tr("Removed %1 palettes.").arg(numRemoved));
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
};

void TextureBrowser::checkForPairs()
{
    if(level)
    {
        bool expectingPaletted = false;
        bool mismatch = false;
        for(int i = 0; i < level->textures.getNumTextures(); i++)
        {
            display->viewer()->enableTextureHighlight(i,false);
            if(expectingPaletted)
            {
                if(level->textures.getTexture(i)->usesPalette())
                {
                    expectingPaletted = false;
                }
                else
                {
                    display->viewer()->setTextureColor(i-1,200,0,0);
                    display->viewer()->enableTextureHighlight(i-1,true);
                    expectingPaletted = false;
                    mismatch = true;
                }
            }
            else if(level->textures.getTexture(i)->usesPalette())
            {
                expectingPaletted = true;
            }
        }

        if(mismatch)
            pairAdvisoryWidget->show();
        else pairAdvisoryWidget->hide();
    }
};

void TextureBrowser::doAddPaletteDialog()
{
    addPaletteDialog->setTexture(indexList->currentRow());
    if(addPaletteDialog->exec() == QDialog::Accepted && level && d3d)
    {
        DriverPalette* pal = level->textures.getPalette(addPaletteDialog->getSelection());
        D3DEntry* entry = d3d->getTextureEntry(indexList->currentRow());
        if(!entry)
        {
            d3d->addEntry(indexList->currentRow());
            entry = d3d->getTextureEntry(indexList->currentRow());
        }
        if(entry && pal)
        {
            bool exists = false;
            for(int i = 0; i < entry->getNumPaletteIndicies(); i++)
            {
                if(entry->getPaletteIndex(i) == pal->paletteNumber)
                {
                    exists = true;
                    break;
                }
            }
            if(!exists)
            {
                entry->addPaletteIndex(pal->paletteNumber);
                rebuildPaletteList();
                if(paletteList->model()->rowCount() == 1)
                syncPaletteSelection(0);
            }
            else
            {
                QMessageBox msgBox(this);
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(tr("Palette already added for this texture."));
                msgBox.exec();
            }
        }
    }
};

void TextureBrowser::deleteCurrentTexture()
{
    int idx = indexList->currentRow();

    QMessageBox msgBox(this);
    msgBox.setText(tr("Delete texture %1?").arg(idx));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes)
    {
        if(level)
        {
            level->textures.removeTexture(idx);
            for(int i = 0; i < level->models.getNumModels(); i++)
            {
                DriverModel* model = level->models.getModel(i);
                for(int j = 0; j < model->getNumFaces(); j++)
                {
                    ModelFace face = model->getFace(j);
                    if(face.getTexture() == idx)
                    face.setTexture(-1);
                    else if(face.getTexture() > idx)
                    face.setTexture(face.getTexture()-1);
                    model->setFace(j,face);
                }
                model->recalculateTexturesUsed();
            }
            for(int i = 0; i < level->world.getNumSectors(); i++)
            {
                SectorTextureList* list = level->sectorTextures.getTextureList(i);
                int indexToRemove = -1;
                for(int j = 0; j < list->getNumTexturesUsed(); j++)
                {

                    if(list->getTexture(j) == idx)
                    indexToRemove = j;
                    else if(list->getTexture(j) > idx)
                    list->setTexture(j,list->getTexture(j)-1);
                }
                if(indexToRemove != -1)
                list->removeTexture(indexToRemove);
            }
            for(int i = 0; i < level->textureDefinitions.getNumTextureDefinitions(); i++)
            {
                TextureDefinition* def = level->textureDefinitions.getTextureDefinition(i);
                if(def->getTexture() == idx)
                {
                    level->textureDefinitions.removeTextureDefinition(i);
                    i--; //Keep index at same location.
                }
                else if(def->getTexture() > idx)
                {
                    def->setTexture(def->getTexture()-1);
                }
            }
        }
        if(d3d)
        {
            D3DEntry* entry = d3d->getTextureEntry(idx);
            if(entry)
                d3d->removeTextureEntry(idx);
            for(int i = 0; i < d3d->getNumEntries(); i++)
            {
                if(d3d->getEntry(i)->getTexture() > idx)
                {
                    d3d->getEntry(i)->setTexture(d3d->getEntry(i)->getTexture()-1);
                }
            }
        }
        rebuildPaletteList();
    }
};

void TextureBrowser::pairTextures()
{
    bool expectingPaletted = false;

    DriverTexture emptyTex(TEX_USES_PALETTE | TEX_LOADS_TO_RAM, -1);
    unsigned char scanline[256];
    memset(scanline,0,sizeof(unsigned char)*256);
    for(int i = 0; i < 256; i++)
    emptyTex.setScanLine(i,scanline);

    if(level)
    {
        for(int i = 0; i < level->textures.getNumTextures(); i++)
        {
            if(level->textures.getTexture(i)->usesPalette())
            {
                if(expectingPaletted == false)
                {
                    expectingPaletted = true;
                }
                else
                {
                    expectingPaletted = false;
                }
            }
            else
            {
                if(expectingPaletted)
                {
                    for(int j = i+1; j < level->textures.getNumTextures(); j++)
                    {
                        if(level->textures.getTexture(j)->usesPalette())
                        {
                            moveTexture(j,i);
                            expectingPaletted = false;
                            break;
                        }
                    }
                    if(expectingPaletted) //We could not find another paletted, so insert a texture.
                    {
                        insertTexture(i,&emptyTex);
                        expectingPaletted = false;
                        break; //could not find a paletted texture so there are no more to check.
                    }
                }
            }
        }
        if(expectingPaletted) //Ran out of textures before finding second paletted
        {
            insertTexture(level->textures.getNumTextures(), &emptyTex);
        }
    }
    pairAdvisoryWidget->hide();
};

void TextureBrowser::insertTexture(int idx, const DriverTexture* tex)
{
    if(idx >= 0 && idx <= level->textures.getNumTextures())
    {
        if(level)
        {
            level->textures.insertTexture(idx,tex);
            for(int i = 0; i < level->models.getNumModels(); i++)
            {
                DriverModel* model = level->models.getModel(i);
                for(int j = 0; j < model->getNumFaces(); j++)
                {
                    ModelFace face = model->getFace(j);

                    if(face.getTexture() >= idx)
                    face.setTexture(face.getTexture()+1);

                    model->setFace(j,face);
                }
                model->recalculateTexturesUsed();
            }
            for(int i = 0; i < level->world.getNumSectors(); i++)
            {
                SectorTextureList* list = level->sectorTextures.getTextureList(i);
                for(int j = 0; j < list->getNumTexturesUsed(); j++)
                {
                    if(list->getTexture(j) >= idx)
                    list->setTexture(j,list->getTexture(j)+1);
                }
            }
            for(int i = 0; i < level->textureDefinitions.getNumTextureDefinitions(); i++)
            {
                TextureDefinition* def = level->textureDefinitions.getTextureDefinition(i);
                if(def->getTexture() >= idx)
                def->setTexture(def->getTexture()+1);
            }
        }
        if(d3d)
        {
            for(int i = 0; i < d3d->getNumEntries(); i++)
            {
                if(d3d->getEntry(i)->getTexture() >= idx)
                d3d->getEntry(i)->setTexture(d3d->getEntry(i)->getTexture()+1);
            }
        }
    }
};

void TextureBrowser::addNewTexture(unsigned short flags, short carnum)
{
    if(level)
    {
        if(level->textures.getNumTextures() < 255)
        {
            level->textures.addTexture(flags,carnum);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Failed to add texture!"));
            msgBox.setInformativeText(tr("Levels cannot contain more than 255 textures."));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
    }
};

void TextureBrowser::moveTexture(int from, int to)
{
    if(from >= 0 && from < level->textures.getNumTextures() && to >= 0 && to < level->textures.getNumTextures() && from != to)
    {
        int min,max,dir;

        if(from > to)
        {
            min = to;
            max = from-1;
            dir = 1;
        }
        else
        {
            min = from+1;
            max = to;
            dir = -1;
        }

        if(level)
        {
            level->textures.moveTexture(from,to);

            for(int i = 0; i < level->models.getNumModels(); i++)
            {
                DriverModel* model = level->models.getModel(i);
                for(int j = 0; j < model->getNumFaces(); j++)
                {
                    ModelFace face = model->getFace(j);

                    if(face.getTexture() == from)
                    face.setTexture(to);
                    else if(face.getTexture() >= min && face.getTexture() <= max)
                    face.setTexture(face.getTexture()+dir);

                    model->setFace(j,face);
                }
                model->recalculateTexturesUsed();
            }
            for(int i = 0; i < level->world.getNumSectors(); i++)
            {
                SectorTextureList* list = level->sectorTextures.getTextureList(i);
                for(int j = 0; j < list->getNumTexturesUsed(); j++)
                {
                    if(list->getTexture(j) == from)
                    list->setTexture(j,to);
                    else if(list->getTexture(j) >= min && list->getTexture(j) <= max)
                    list->setTexture(j,list->getTexture(j)+dir);
                }
            }
            for(int i = 0; i < level->textureDefinitions.getNumTextureDefinitions(); i++)
            {
                TextureDefinition* def = level->textureDefinitions.getTextureDefinition(i);
                if(def->getTexture() == from)
                def->setTexture(to);
                else if(def->getTexture() >= min && def->getTexture() <= max)
                def->setTexture(def->getTexture()+dir);
            }
            for(int i = 0; i < level->textureDefinitions.getNumTextureDefinitions(); i++)
            {
                //TODO: Is there a more efficient way to do this?
                if(level->textureDefinitions.resortTextureDefinition(i) > i)
                i--; //If it is moved to after its original position then a new definition is in its place
            }
        }
        if(d3d)
        {
            for(int i = 0; i < d3d->getNumEntries(); i++)
            {
                if(d3d->getEntry(i)->getTexture() == from)
                d3d->getEntry(i)->setTexture(to);
                else if(d3d->getEntry(i)->getTexture() >= min && d3d->getEntry(i)->getTexture() <= max)
                d3d->getEntry(i)->setTexture(d3d->getEntry(i)->getTexture()+dir);
            }
        }
    }
};

void TextureBrowser::textureRemoved(int /*idx*/)
{
    refreshIndexList();
    rebuildPaletteList();
    checkForPairs();
};

void TextureBrowser::textureChanged(int idx)
{
    const DriverTexture* tex = level->textures.getTexture(idx);
    textureProperties->setTextureProperties(tex);
    if(tex)
    {
        if(tex->usesPalette() && d3d)
        {
            rebuildPaletteList();
            paletteList->show();
            palettesLabel->show();
        }
        else
        {
            palettesLabel->hide();
            paletteList->hide();
        }
    }
    checkForPairs();
};

void TextureBrowser::textureMoved(int /*from*/, int /*to*/)
{
    checkForPairs();
};

void TextureBrowser::textureInserted(int /*idx*/)
{
    //TODO: implement me
    refreshIndexList();
    checkForPairs();
};

void TextureBrowser::levelSaved(bool aboutToBe)
{
    if(aboutToBe)
    {
        pairTextures();
    }
};

void TextureBrowser::levelDestroyed()
{
    level = NULL;
    display->setTextureData(NULL);
    exportDialog->setTextureData(NULL);
    importDialog->setTextureData(NULL);
    addPaletteDialog->setTextureData(NULL);
    texturesChanged();
};

void TextureBrowser::levelOpened()
{
    const DriverTexture* tex = level->textures.getTexture(0);
    textureProperties->setTextureProperties(tex);
};
