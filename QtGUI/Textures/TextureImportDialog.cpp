#include "TextureImportDialog.hpp"

TextureImportDialog::TextureImportDialog(QWidget* parent) : QDialog(parent)
{
    textureBlock = NULL;
    d3d = NULL;
    textureIndex = -1;
    paletteIndex = -1;
    lastNon32BitState = 0;
    prefered32BitState = 0;
    bitmap = NULL;

    transLabel = new QLabel(tr("For transparency:"),this);
    useDefaultColor = new QRadioButton(tr("Use default color"), this);
    useMagicPink = new QRadioButton(tr("Use magic pink"), this);
    useAlpha = new QRadioButton(tr("Use alpha channel"), this);

    imagePreview = new QLabel(this);
    QPixmap tempImage(256,256);
    tempImage.fill(Qt::black);
    imagePreview->setPixmap(tempImage);

    importButton = new QPushButton(tr("Import"), this);
    importButton->setMaximumWidth(100);

    cancelButton = new QPushButton(tr("Cancel"), this);
    cancelButton->setMaximumWidth(100);

    QVBoxLayout* optionsLayout = new QVBoxLayout();
    optionsLayout->addWidget(transLabel,0,Qt::AlignTop);
    optionsLayout->addWidget(useDefaultColor,0,Qt::AlignTop);
    optionsLayout->addWidget(useMagicPink,0,Qt::AlignTop);
    optionsLayout->addWidget(useAlpha,1,Qt::AlignTop);

    QHBoxLayout* fullLayout = new QHBoxLayout();
    fullLayout->addLayout(optionsLayout);
    fullLayout->addWidget(imagePreview);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(importButton);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(fullLayout);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
    hide();

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(useDefaultColor, SIGNAL(clicked()), this, SLOT(selectDefault()));
    connect(useMagicPink, SIGNAL(clicked()), this, SLOT(selectMagicPink()));
    connect(useAlpha, SIGNAL(clicked()), this, SLOT(selectAlpha()));
    connect(importButton, SIGNAL(clicked()), this, SLOT(importImage()));
};

TextureImportDialog::~TextureImportDialog()
{
    if(bitmap)
    FreeImage_Unload(bitmap);
    bitmap = NULL;
};

void TextureImportDialog::connectChangeHandler(QWidget* handler)
{
    connect(this, SIGNAL(textureChanged(int)), handler, SLOT(handleTextureChange(int)));
    connect(this, SIGNAL(paletteChanged(int)), handler, SLOT(handlePaletteChange(int)));
    connect(this, SIGNAL(d3dChanged()), handler, SLOT(handleD3dChange()));
    connect(this, SIGNAL(d3dEntryChanged(int, int)), handler, SLOT(handleD3dEntryChange(int, int)));
};

void TextureImportDialog::setTextureData(DriverTextures* texs)
{
    textureBlock = texs;
};

void TextureImportDialog::setD3D(DriverD3D* newD3D)
{
    d3d = newD3D;
};

void TextureImportDialog::setTextureIndex(int idx)
{
    textureIndex = idx;
};

void TextureImportDialog::setPaletteIndex(int idx)
{
    paletteIndex = idx;
};

void TextureImportDialog::loadSettings()
{
    QSettings settings;
    selectedFilter = settings.value("TextureImportDialog/selectedFilter").toString();

    lastNon32BitState = settings.value("TextureImportDialog/useMagicPink",0).toInt();
    prefered32BitState = settings.value("TextureImportDialog/preferred32Bit",2).toInt();

    lastImageImportDir = settings.value("directories/lastImageImportDir").toString();
};

void TextureImportDialog::saveSettings()
{
    QSettings settings;
    settings.setValue("TextureImportDialog/selectedFilter", selectedFilter);

    settings.setValue("TextureImportDialog/useMagicPink", lastNon32BitState);
    settings.setValue("TextureImportDialog/preferred32Bit", prefered32BitState);

    settings.setValue("directories/lastImageImportDir", lastImageImportDir);
};

int TextureImportDialog::exec()
{
    if(textureBlock)
    {
        QString allFilesFilter = "All Files (*.*)";
        QString imageFilter = "Images (*.bmp *.png *.gif *.jpeg *.jpg *.tiff *.tif *.tga *.dds)";
        QString iniFilter = "Definition files (*.ini)";

        filename = QFileDialog::getOpenFileName(this,QString("Import texture..."),lastImageImportDir,QString(allFilesFilter+";;"+imageFilter+";;"+iniFilter),&selectedFilter);

        if(!filename.isEmpty())
        {
            QFileInfo fileInfo(filename);
            lastImageImportDir = fileInfo.absolutePath();

            const DriverTexture* tex = textureBlock->getTexture(textureIndex);
            if(tex)
            {
                if(bitmap)
                FreeImage_Unload(bitmap);
                bitmap = NULL;

                FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
                fif = FreeImage_GetFileType(filename.toLocal8Bit().data(), 0);
                if(fif == FIF_UNKNOWN)
                fif = FreeImage_GetFIFFromFilename(filename.toLocal8Bit().data());

                if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
                bitmap = FreeImage_Load(fif, filename.toLocal8Bit().data(), (fif == FIF_JPEG ? JPEG_ACCURATE : 0));

                if(bitmap)
                {
                    if(FreeImage_GetWidth(bitmap) != 256 || FreeImage_GetHeight(bitmap) != 256)
                    {
                        QMessageBox msgBox(this);
                        msgBox.setText(tr("Incorrect image dimensions!"));
                        msgBox.setInformativeText(tr("Images must be exactly 256x256 pixels to be imported."));
                        msgBox.setIcon(QMessageBox::Warning);
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.exec();
                        return QDialog::Rejected;
                    }
                    if(tex->hasTransparency() && !tex->usesPalette())
                    {
                        setPreview(bitmap);

                        if(FreeImage_GetBPP(bitmap) != 32)
                        {
                            useAlpha->setEnabled(false);
                            if(lastNon32BitState == 0)
                            useDefaultColor->setChecked(true);
                            else if(lastNon32BitState == 1)
                            useMagicPink->setChecked(true);
                        }
                        else
                        {
                            useAlpha->setEnabled(true);
                            if(prefered32BitState == 0)
                            useDefaultColor->setChecked(true);
                            else if(prefered32BitState == 1)
                            useMagicPink->setChecked(true);
                            else if(prefered32BitState == 2)
                            useAlpha->setChecked(true);
                        }
                        return QDialog::exec();
                    }
                    else importImage();
                }
                else
                {
                    QMessageBox msgBox(this);
                    msgBox.setText(tr("Failed to load image!"));
                    msgBox.setInformativeText(tr("An error occured while trying to load the image ") + filename + tr(". Check the main log file for details."));
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.exec();
                    return QDialog::Rejected;
                }
            }
        }
    }
    return QDialog::Rejected;
};

void TextureImportDialog::setPreview(FIBITMAP* dib)
{
    QPixmap tempPixmap;
    FIBITMAP* bmp = dib;
    if(FreeImage_GetBPP(bmp) != 32)
    bmp = FreeImage_ConvertTo32Bits(dib);

    QImage tempImage(FreeImage_GetBits(bmp),FreeImage_GetWidth(bmp),FreeImage_GetHeight(bmp),FreeImage_GetPitch(bmp),QImage::Format_ARGB32);
    tempPixmap.convertFromImage(tempImage.mirrored(false,true));
    imagePreview->setPixmap(tempPixmap);

    if(bmp != dib)
    FreeImage_Unload(bmp);
};

void TextureImportDialog::selectDefault()
{
    if(FreeImage_GetBPP(bitmap) == 32)
    prefered32BitState = 0;
    else lastNon32BitState = 0;
};

void TextureImportDialog::selectMagicPink()
{
    if(FreeImage_GetBPP(bitmap) == 32)
    prefered32BitState = 1;
    else lastNon32BitState = 1;
};

void TextureImportDialog::selectAlpha()
{
    prefered32BitState = 2;
};

void TextureImportDialog::importImage()
{
    if(textureBlock)
    {
        const DriverTexture* origTex = textureBlock->getTexture(textureIndex);
        if(origTex)
        {
            DriverTexture tex = *origTex;
            if(tex.usesPalette())
            {
                int paletteSlot = textureBlock->getNextOpenSlot();
                if(d3d)
                {
                    D3DEntry* entry = d3d->getTextureEntry(textureIndex);
                    if(!entry)
                    {
                        d3d->addEntry(textureIndex);
                        entry = d3d->getTextureEntry(textureIndex);
                    }

                    int currentPalette = entry->getPaletteIndex(paletteIndex);

                    if(currentPalette > -1)
                    {
                        int numReferences = 0;
                        for(int i = 0; i < d3d->getNumEntries(); i++)
                        {
                            D3DEntry* tempEntry = d3d->getEntry(i);
                            if(tempEntry)
                            {
                                for(int j = 0; j < tempEntry->getNumPaletteIndicies(); j++)
                                {
                                    if(tempEntry->getPaletteIndex(j) == currentPalette)
                                    numReferences++;
                                }
                            }
                        }
                        if(numReferences == 1) //no other textures are using this palette slot
                        paletteSlot = currentPalette;
                    }
                    entry->setPaletteIndex(paletteIndex, paletteSlot);
                }

                DriverPalette palette;
                FIBITMAP* temp8 = NULL;
                if(FreeImage_GetBPP(bitmap) == 8)
                {
                    temp8 = FreeImage_Clone(bitmap);
                }
                else
                {
                    FIBITMAP* temp24 = FreeImage_ConvertTo24Bits(bitmap);
                    temp8 = FreeImage_ColorQuantize(temp24,FIQ_NNQUANT);
                    FreeImage_Unload(temp24);
                }

                RGBQUAD *pal = FreeImage_GetPalette(temp8);
                for (int i = 0; i < 256; i++)
                {
                    palette.colors[i].r = pal[i].rgbRed;
                    palette.colors[i].g = pal[i].rgbGreen;
                    palette.colors[i].b = pal[i].rgbBlue;
                }
                palette.paletteNumber = paletteSlot;
                textureBlock->setPaletteIndexed(&palette);

                for(int i = 0; i < 256; i++)
                {
                    tex.setScanLine(i,FreeImage_GetScanLine(temp8,(255-i)));
                }

                textureBlock->setTexture(textureIndex, &tex);
                emit accept();
            }
            else
            {
                FIBITMAP* temp = FreeImage_ConvertTo16Bits555(bitmap);

                if(temp)
                {
                    FIBITMAP* alpha = NULL;
                    if(tex.hasTransparency())
                    alpha = FreeImage_ConvertTo32Bits(bitmap);

                    for(int i = 0; i < 256; i++)
                    {
                        unsigned char* scanLine = FreeImage_GetScanLine(temp,(255-i));
                        if(tex.hasTransparency())
                        {
                            unsigned char* scanLineAlpha = FreeImage_GetScanLine(alpha,(255-i));
                            for(int j = 0; j < 256; j++)
                            {
                                if(useDefaultColor->isChecked())
                                {
                                    if(scanLineAlpha[FI_RGBA_RED] == 8 && scanLineAlpha[FI_RGBA_GREEN] == 8 && scanLineAlpha[FI_RGBA_BLUE] == 8)
                                    *(unsigned short*)(scanLine+j*2) = 0b1000010000100001;
                                }
                                else if(useMagicPink->isChecked())
                                {
                                    if(scanLineAlpha[FI_RGBA_RED] == 255 && scanLineAlpha[FI_RGBA_GREEN] == 0 && scanLineAlpha[FI_RGBA_BLUE] == 255)
                                    *(unsigned short*)(scanLine+j*2) = 0b1000010000100001;
                                }
                                else
                                {
                                    if(scanLineAlpha[FI_RGBA_ALPHA] != 255)
                                    *(unsigned short*)(scanLine+j*2) = 0b1000010000100001;
                                }
                                scanLineAlpha += 4;
                            }
                        }
                        tex.setScanLine(i,scanLine);
                    }
                    if(alpha)
                        FreeImage_Unload(alpha);
                    FreeImage_Unload(temp);

                    textureBlock->setTexture(textureIndex, &tex);
                    emit accept();
                }
            }
        }
    }
};
