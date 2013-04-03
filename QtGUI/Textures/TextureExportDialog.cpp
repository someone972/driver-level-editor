#include "TextureExportDialog.hpp"

FreeImageMemFile::FreeImageMemFile()
{
    memoryBuffer = new unsigned char[1024];
    memoryPosition = 0;
    memorySize = 0;
    maxMemorySize = 1024;

    ioStruct.read_proc = &readProc;
    ioStruct.write_proc = &writeProc;
    ioStruct.seek_proc = &seekProc;
    ioStruct.tell_proc = &tellProc;
};

FreeImageMemFile::~FreeImageMemFile()
{
    if(memoryBuffer)
    delete[] memoryBuffer;
};

const unsigned char* FreeImageMemFile::data()
{
    return memoryBuffer;
};

int FreeImageMemFile::size()
{
    return memorySize;
};

unsigned FreeImageMemFile::readProc(void* buffer, unsigned size, unsigned count, fi_handle handle)
{
    return ((FreeImageMemFile*)handle)->readMemory(buffer,size,count);
};

unsigned FreeImageMemFile::writeProc(void* buffer, unsigned size, unsigned count, fi_handle handle)
{
    return ((FreeImageMemFile*)handle)->writeMemory(buffer,size,count);
};

int FreeImageMemFile::seekProc(fi_handle handle, long offset, int origin)
{
    return ((FreeImageMemFile*)handle)->seekMemory(offset,origin);
};

long FreeImageMemFile::tellProc(fi_handle handle)
{
    return ((FreeImageMemFile*)handle)->tellMemory();
};

unsigned FreeImageMemFile::readMemory(void* buffer, unsigned size, unsigned count)
{
    unsigned int sizeToRead = size*count;

    if(sizeToRead > memorySize-memoryPosition)
    sizeToRead = memorySize-memoryPosition;

    memcpy(buffer,memoryBuffer+memoryPosition,sizeToRead);
    memoryPosition += sizeToRead;
    return sizeToRead/size;
};

unsigned FreeImageMemFile::writeMemory(void* buffer, unsigned size, unsigned count)
{
    unsigned int sizeToWrite = size*count;

    if(memoryPosition+sizeToWrite > maxMemorySize)
    expandMemoryToSize(memoryPosition+sizeToWrite);

    memcpy(memoryBuffer+memoryPosition,buffer,sizeToWrite);
    memoryPosition += sizeToWrite;
    if(memoryPosition > memorySize)
    memorySize = memoryPosition;
    return count;
};

int FreeImageMemFile::seekMemory(long offset, int origin)
{
    long int position = 0;

    if(origin == SEEK_SET)
    position = offset;
    else if(origin == SEEK_CUR)
    position = memoryPosition+offset;
    else if(origin == SEEK_END)
    position = memorySize+offset;

    if(position < 0)
    return -1;

    if((unsigned int)position > maxMemorySize)
    expandMemoryToSize(position);

    if((unsigned int)position > memorySize)
    memset(memoryBuffer+memoryPosition,0,position-memorySize);

    memoryPosition = position;
    return 0;
};

long FreeImageMemFile::tellMemory()
{
    return memoryPosition;
};

void FreeImageMemFile::expandMemoryToSize(unsigned int size)
{
    while(maxMemorySize < size)
    maxMemorySize *= 2;

    unsigned char* temp = new unsigned char[maxMemorySize];
    memcpy(temp,memoryBuffer,memorySize);
    delete[] memoryBuffer;
    memoryBuffer = temp;
};

FreeImageIO* FreeImageMemFile::getIOPointer()
{
    return &ioStruct;
};

fi_handle FreeImageMemFile::getHandle()
{
    return (fi_handle)this;
};

void FreeImageMemFile::resetMemory()
{
    if(memoryBuffer)
    delete[] memoryBuffer;
    memoryBuffer = new unsigned char[1024];
    memoryPosition = 0;
    memorySize = 0;
    maxMemorySize = 1024;
};

void FreeImageMemFile::reset()
{
    memoryPosition = 0;
    memorySize = 0;
};

TextureExportDialog::TextureExportDialog(QWidget* parent) : QDialog(parent)
{
    level = NULL;
    d3d = NULL;

    textureIndex = -1;
    paletteIndex = -1;

    resize(100,100); //needed to get rid of extra space between layouts, not actually this size when finished.

    bmpPage = new QWidget(this);
    bmpPage->setMaximumWidth(200);
    bmpTruecolor = new QRadioButton(tr("24-bit Truecolor"),this);
    bmpTruecolor->setChecked(true);
    bmpPaletted = new QRadioButton(tr("8-bit Paletted"),this);
    bmpCompress = new QCheckBox(tr("Compress with RLE"),this);
    bmpMagicPink = new QCheckBox(tr("Use Magic Pink for alpha"),this);
    bmpButtons = new QButtonGroup(this);
    bmpButtons->addButton(bmpTruecolor);
    bmpButtons->addButton(bmpPaletted);

    QVBoxLayout* bmpSideLayout = new QVBoxLayout();
    bmpSideLayout->addWidget(bmpTruecolor);
    bmpSideLayout->addWidget(bmpPaletted);
    bmpSideLayout->addWidget(bmpMagicPink);
    bmpSideLayout->addWidget(bmpCompress);
    bmpPage->setLayout(bmpSideLayout);

    pngPage = new QWidget(this);
    pngPage->setMaximumWidth(200);
    pngTruecolor = new QRadioButton(tr("Truecolor"),this);
    pngPaletted = new QRadioButton(tr("Paletted"),this);
    pngExportWithTransparency = new QCheckBox(tr("Export with transparency (32-bit)"),this);
    pngMagicPink = new QCheckBox(tr("Use Magic Pink for alpha"),this);
    pngCompression = new QComboBox(this);
    pngCompression->addItem(tr("Best compression"));
    pngCompression->addItem(tr("Default compression"));
    pngCompression->addItem(tr("Fastest compression"));
    pngCompression->addItem(tr("No compression"));
    pngInterlaced = new QCheckBox(tr("Interlace (For web display)"),this);
    pngButtons = new QButtonGroup(this);
    pngButtons->addButton(pngTruecolor);
    pngButtons->addButton(pngPaletted);

    QVBoxLayout* pngSideLayout = new QVBoxLayout();
    pngSideLayout->addWidget(pngTruecolor);
    pngSideLayout->addWidget(pngPaletted);
    pngSideLayout->addWidget(pngCompression);
    pngSideLayout->addWidget(pngExportWithTransparency);
    pngSideLayout->addWidget(pngMagicPink);
    pngSideLayout->addWidget(pngInterlaced);
    pngPage->setLayout(pngSideLayout);

    jpegPage = new QWidget(this);
    jpegPage->setMaximumWidth(200);
    jpegQuality = new QSpinBox(this);
    jpegQuality->setMinimum(1);
    jpegQuality->setMaximum(100);
    jpegQuality->setValue(100);
    jpegQualitySlider = new QSlider(this);
    jpegQualitySlider->setMinimum(1);
    jpegQualitySlider->setMaximum(100);
    jpegQualitySlider->setValue(100);
    jpegQualitySlider->setOrientation(Qt::Horizontal);
    jpegQualityLabel = new QLabel(tr("Quality:"),this);
    jpegProgressive = new QCheckBox(tr("Progressive (For web display)"),this);
    jpegMagicPink = new QCheckBox(tr("Use Magic Pink for alpha"),this);

    QHBoxLayout* jpegQualityLayout = new QHBoxLayout();
    jpegQualityLayout->addWidget(jpegQualityLabel);
    jpegQualityLayout->addWidget(jpegQuality);

    QVBoxLayout* jpegSideLayout = new QVBoxLayout();
    jpegSideLayout->addLayout(jpegQualityLayout);
    jpegSideLayout->addWidget(jpegQualitySlider);
    jpegSideLayout->addWidget(jpegMagicPink);
    jpegSideLayout->addWidget(jpegProgressive);
    jpegPage->setLayout(jpegSideLayout);

    gifPage = new QWidget(this);
    gifPage->setMaximumWidth(200);
    gifExportMultiPalette = new QCheckBox(tr("Export multiple palettes as animation"),this);
    gifPaletteList = new QListWidget(this);
    animatedBitmap = NULL;

    QVBoxLayout* gifSideLayout = new QVBoxLayout();
    gifSideLayout->addWidget(gifExportMultiPalette);
    gifSideLayout->addWidget(gifPaletteList);
    gifPage->setLayout(gifSideLayout);

    tiffPage = new QWidget(this);
    tiffPage->setMaximumWidth(200);
    tiffTruecolor = new QRadioButton(tr("24-bit Truecolor"),this);
    tiffPaletted = new QRadioButton(tr("8-bit Paletted"),this);
    tiffMagicPink = new QCheckBox(tr("Use Magic Pink for alpha"),this);
    tiffCompression = new QComboBox(this);
    tiffCompression->addItem(tr("Packbits compression"));
    tiffCompression->addItem(tr("Deflate compression"));
    tiffCompression->addItem(tr("Adobe Deflate compression"));
    tiffCompression->addItem(tr("LZW compression"));
    tiffCompression->addItem(tr("No compression"));
    tiffButtons = new QButtonGroup(this);
    tiffButtons->addButton(tiffTruecolor);
    tiffButtons->addButton(tiffPaletted);

    QVBoxLayout* tiffSideLayout = new QVBoxLayout();
    tiffSideLayout->addWidget(tiffTruecolor);
    tiffSideLayout->addWidget(tiffPaletted);
    tiffSideLayout->addWidget(tiffMagicPink);
    tiffSideLayout->addWidget(tiffCompression);
    tiffPage->setLayout(tiffSideLayout);

    tgaPage = new QWidget(this);
    tgaPage->setMaximumWidth(200);
    tgaPaletted = new QRadioButton(tr("8-bit Paletted"),this);
    tgaHighcolor15 = new QRadioButton(tr("15-bit Highcolor"),this);
    tgaTruecolor24 = new QRadioButton(tr("24-bit Truecolor"),this);
    tgaTruecolor32 = new QRadioButton(tr("32-bit Truecolor w/Alpha"),this);
    tgaMagicPink = new QCheckBox(tr("Use Magic Pink for alpha"),this);
    tgaCompress = new QCheckBox(tr("Compress with RLE"),this);
    tgaButtons = new QButtonGroup(this);
    tgaButtons->addButton(tgaPaletted);
    tgaButtons->addButton(tgaHighcolor15);
    tgaButtons->addButton(tgaTruecolor24);
    tgaButtons->addButton(tgaTruecolor32);
    tgaLastDepth = 15;

    QVBoxLayout* tgaSideLayout = new QVBoxLayout();
    tgaSideLayout->addWidget(tgaPaletted);
    tgaSideLayout->addWidget(tgaHighcolor15);
    tgaSideLayout->addWidget(tgaTruecolor24);
    tgaSideLayout->addWidget(tgaTruecolor32);
    tgaSideLayout->addWidget(tgaMagicPink);
    tgaSideLayout->addWidget(tgaCompress);
    tgaPage->setLayout(tgaSideLayout);

    saveButton = new QPushButton(tr("&Save"),this);
    saveButton->setMaximumWidth(100);
    cancelButton = new QPushButton(tr("&Cancel"),this);
    cancelButton->setMaximumWidth(100);

    pages = new QStackedLayout();
    pages->addWidget(bmpPage);
    pages->addWidget(pngPage);
    pages->addWidget(jpegPage);
    pages->addWidget(gifPage);
    pages->addWidget(tiffPage);
    pages->addWidget(tgaPage);

    imagePreview = new QLabel(this);
    imageSize = new QLabel(this);
    QPixmap pixmap(256,256);
    pixmap.fill(Qt::black);
    imagePreview->setPixmap(pixmap);

    exportDefinitions = new QCheckBox(tr("Export definitions"),this);
    editDefintions = new QPushButton(tr("Edit"),this);
    editDefintions->setMaximumWidth(80);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QHBoxLayout* definitionLayout = new QHBoxLayout();
    definitionLayout->addWidget(exportDefinitions);
    definitionLayout->addWidget(editDefintions);

    QVBoxLayout* previewLayout = new QVBoxLayout();
    previewLayout->addWidget(imageSize);
    previewLayout->addWidget(imagePreview);
    previewLayout->addLayout(definitionLayout);

   // QVBoxLayout* optionsLayout = new QVBoxLayout();
   //optionsLayout->addLayout(pages);
   // optionsLayout->addLayout(definitionLayout);

    QHBoxLayout* fullLayout = new QHBoxLayout();
    fullLayout->addLayout(pages);
    fullLayout->addLayout(previewLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(fullLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    textureBitmap = NULL;
    cachedBitmap = NULL;
    transEnabled = true;

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveTexture()));

    connect(bmpCompress, SIGNAL(stateChanged(int)), this, SLOT(updateBmpPreview()));
    connect(bmpTruecolor, SIGNAL(clicked()), this, SLOT(selectBmpTruecolor()));
    connect(bmpPaletted, SIGNAL(clicked()), this, SLOT(selectBmpPaletted()));
    connect(bmpMagicPink, SIGNAL(stateChanged(int)), this, SLOT(updateBmpMagicPink(int)));

    connect(pngCompression, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePngPreview()));
    connect(pngTruecolor, SIGNAL(clicked()), this, SLOT(selectPngTruecolor()));
    connect(pngPaletted, SIGNAL(clicked()), this, SLOT(selectPngPaletted()));
    connect(pngInterlaced, SIGNAL(stateChanged(int)), this, SLOT(updatePngPreview()));
    connect(pngMagicPink, SIGNAL(stateChanged(int)), this, SLOT(forcePngUpdate()));
    connect(pngExportWithTransparency, SIGNAL(stateChanged(int)), this, SLOT(setPng32Bit(int)));

    connect(jpegQuality, SIGNAL(valueChanged(int)), jpegQualitySlider, SLOT(setValue(int)));
    connect(jpegQualitySlider, SIGNAL(valueChanged(int)), jpegQuality, SLOT(setValue(int)));
    connect(jpegQualitySlider, SIGNAL(valueChanged(int)), this, SLOT(updateJpegPreview()));
    connect(jpegProgressive, SIGNAL(stateChanged(int)), this, SLOT(updateJpegPreview()));
    connect(jpegMagicPink,SIGNAL(stateChanged(int)), this, SLOT(updateJpegMagicPink(int)));

    connect(tiffCompression, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTiffPreview()));
    connect(tiffTruecolor, SIGNAL(clicked()), this, SLOT(selectTiffTruecolor()));
    connect(tiffPaletted, SIGNAL(clicked()), this, SLOT(selectTiffPaletted()));
    connect(tiffMagicPink, SIGNAL(stateChanged(int)), this, SLOT(forceTiffUpdate()));

    connect(tgaPaletted, SIGNAL(clicked()), this, SLOT(selectTgaPaletted()));
    connect(tgaHighcolor15, SIGNAL(clicked()), this, SLOT(selectTgaHighcolor()));
    connect(tgaTruecolor24, SIGNAL(clicked()), this, SLOT(selectTgaTruecolor24()));
    connect(tgaTruecolor32, SIGNAL(clicked()), this, SLOT(selectTgaTruecolor32()));
    connect(tgaCompress, SIGNAL(stateChanged(int)), this, SLOT(updateTgaPreview()));
    connect(tgaMagicPink, SIGNAL(stateChanged(int)), this, SLOT(forceTgaUpdate()));
};

TextureExportDialog::~TextureExportDialog()
{
    if(textureBitmap)
    FreeImage_Unload(textureBitmap);
    textureBitmap = NULL;

    if(cachedBitmap)
    FreeImage_Unload(cachedBitmap);
    cachedBitmap = NULL;
};

void TextureExportDialog::disableTransparencyItems()
{
    bmpMagicPink->hide();
    pngExportWithTransparency->hide();
    pngMagicPink->hide();
    jpegMagicPink->hide();
    tiffMagicPink->hide();
    tgaMagicPink->hide();
    tgaTruecolor32->hide();
    transEnabled = false;
};

void TextureExportDialog::enableTransparencyItems()
{
    bmpMagicPink->show();
    pngExportWithTransparency->show();
    pngMagicPink->show();
    jpegMagicPink->show();
    tiffMagicPink->show();
    tgaMagicPink->show();
    tgaTruecolor32->show();
    transEnabled = true;
};

void TextureExportDialog::setLevel(DriverLevel* lev)
{
    level = lev;
};

void TextureExportDialog::setD3D(DriverD3D* newD3D)
{
    d3d = newD3D;
};

void TextureExportDialog::setTextureIndex(int idx)
{
    textureIndex = idx;
};

void TextureExportDialog::setPaletteIndex(int idx)
{
    paletteIndex = idx;
};

void TextureExportDialog::loadSettings()
{
    QSettings settings;
    selectedFilter = settings.value("TextureExportDialog/selectedFilter","").toString();
    exportDefinitions->setCheckState((settings.value("TextureExportDialog/exportDefinitions",false).toBool() ? Qt::Checked : Qt::Unchecked));
    lastImageExportDir = settings.value("directories/lastImageExportDir").toString();

    bmpCompress->setCheckState((settings.value("TextureExportDialog/BMP/compressRLE",false).toBool() ? Qt::Checked : Qt::Unchecked));
    bmpMagicPink->setCheckState((settings.value("TextureExportDialog/BMP/magicPink",false).toBool() ? Qt::Checked : Qt::Unchecked));

    pngExportWithTransparency->setCheckState((settings.value("TextureExportDialog/PNG/exportTransparency",false).toBool() ? Qt::Checked : Qt::Unchecked));
    pngMagicPink->setCheckState((settings.value("TextureExportDialog/PNG/magicPink",false).toBool() ? Qt::Checked : Qt::Unchecked));
    pngInterlaced->setCheckState((settings.value("TextureExportDialog/PNG/interlace",false).toBool() ? Qt::Checked : Qt::Unchecked));
    pngCompression->setCurrentIndex(settings.value("TextureExportDialog/PNG/compression",0).toInt());

    jpegQualitySlider->setValue(settings.value("TextureExportDialog/JPEG/quality",100).toInt());
    jpegQuality->setValue(settings.value("TextureExportDialog/JPEG/quality",100).toInt());
    jpegProgressive->setCheckState((settings.value("TextureExportDialog/JPEG/progressive",false).toBool() ? Qt::Checked : Qt::Unchecked));
    jpegMagicPink->setCheckState((settings.value("TextureExportDialog/JPEG/magicPink",false).toBool() ? Qt::Checked : Qt::Unchecked));

    gifExportMultiPalette->setCheckState((settings.value("TextureExportDialog/GIF/multiPalette",false).toBool() ? Qt::Checked : Qt::Unchecked));

    tiffCompression->setCurrentIndex(settings.value("TextureExportDialog/TIFF/compression",0).toInt());
    tiffMagicPink->setCheckState((settings.value("TextureExportDialog/TIFF/magicPink",false).toBool() ? Qt::Checked : Qt::Unchecked));

    tgaCompress->setCheckState((settings.value("TextureExportDialog/TGA/compressRLE",false).toBool() ? Qt::Checked : Qt::Unchecked));
    tgaMagicPink->setCheckState((settings.value("TextureExportDialog/TGA/magicPink",false).toBool() ? Qt::Checked : Qt::Unchecked));
    tgaLastDepth = settings.value("TextureExportDialog/TGA/lastDepth",15).toInt();
};

void TextureExportDialog::saveSettings()
{
    QSettings settings;
    settings.setValue("TextureExportDialog/selectedFilter",selectedFilter);
    settings.setValue("directories/lastImageExportDir",lastImageExportDir);
    settings.setValue("TextureExportDialog/exportDefinitions",(exportDefinitions->checkState() == Qt::Checked ? true : false));

    settings.setValue("TextureExportDialog/BMP/compressRLE",(bmpCompress->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/BMP/magicPink",(bmpMagicPink->checkState() == Qt::Checked ? true : false));

    settings.setValue("TextureExportDialog/PNG/exportTransparency",(pngExportWithTransparency->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/PNG/magicPink",(pngMagicPink->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/PNG/interlace",(pngInterlaced->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/PNG/compression",pngCompression->currentIndex());

    settings.setValue("TextureExportDialog/JPEG/quality",jpegQuality->value());
    settings.setValue("TextureExportDialog/JPEG/progressive",(jpegProgressive->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/JPEG/magicPink",(jpegMagicPink->checkState() == Qt::Checked ? true : false));

    settings.setValue("TextureExportDialog/GIF/multiPalette",(gifExportMultiPalette->checkState() == Qt::Checked ? true : false));

    settings.setValue("TextureExportDialog/TIFF/compression",tiffCompression->currentIndex());
    settings.setValue("TextureExportDialog/TIFF/magicPink",(tiffMagicPink->checkState() == Qt::Checked ? true : false));

    settings.setValue("TextureExportDialog/TGA/compressRLE",(tgaCompress->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/TGA/magicPink",(tgaMagicPink->checkState() == Qt::Checked ? true : false));
    settings.setValue("TextureExportDialog/TGA/lastDepth",tgaLastDepth);
};

int TextureExportDialog::exec()
{
    QString bitmapFilter = "Bitmap (*.bmp)";
    QString pngFilter = "Portable Network Graphics (*.png)";
    QString jpegFilter = "JPEG (*.jpg *.jpeg)";
    QString gifFilter = "Graphics Interchange Format (*.gif)";
    QString tiffFilter = "TIFF (*.tiff *.tif)";
    QString targaFilter = "TARGA (*.tga *.targa)";
    filename = QFileDialog::getSaveFileName(this,QString("Save texture as..."),lastImageExportDir,QString(bitmapFilter+";;"+pngFilter+";;"+jpegFilter+";;"+gifFilter+";;"+tiffFilter+";;"+targaFilter),&selectedFilter);

    if(!filename.isEmpty())
    {
        QFileInfo fileInfo(filename);
        lastImageExportDir = fileInfo.absolutePath();

        prepareFreeImageBitmap();

        if(cachedBitmap)
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;

        if(selectedFilter == bitmapFilter)
        {
            pages->setCurrentWidget(bmpPage);
            if(FreeImage_GetBPP(textureBitmap) == 8)
            {
                bmpPaletted->setChecked(true);
                selectBmpPaletted();
            }
            else
            {
                bmpTruecolor->setChecked(true);
                selectBmpTruecolor();
            }
        }
        else if(selectedFilter == pngFilter)
        {
            if(FreeImage_GetBPP(textureBitmap) == 8)
            {
                pngPaletted->setChecked(true);
                selectPngPaletted();
            }
            else
            {
                pngTruecolor->setChecked(true);
                selectPngTruecolor();
            }
            pages->setCurrentWidget(pngPage);
        }
        else if(selectedFilter == jpegFilter)
        {
            pages->setCurrentWidget(jpegPage);
            cachedBitmap = FreeImage_ConvertTo24Bits(textureBitmap);
            if(jpegMagicPink->checkState() == Qt::Checked && transEnabled)
            updateJpegMagicPink(Qt::Checked);
            else updateJpegPreview(); //updateJpegMagicPink updates the preview, so we only need to do it if not using magic pink.
        }
        else if(selectedFilter == gifFilter)
        {
            if(FreeImage_GetBPP(textureBitmap) == 8)
            gifExportMultiPalette->setEnabled(true);
            else gifExportMultiPalette->setEnabled(false);

            pages->setCurrentWidget(gifPage);
        }
        else if(selectedFilter == tiffFilter)
        {
            if(FreeImage_GetBPP(textureBitmap) == 8)
            {
                tiffPaletted->setChecked(true);
                selectTiffPaletted();
            }
            else
            {
                tiffTruecolor->setChecked(true);
                selectTiffTruecolor();
            }
            pages->setCurrentWidget(tiffPage);
        }
        else if(selectedFilter == targaFilter)
        {
            if(FreeImage_GetBPP(textureBitmap) == 8)
            {
                tgaPaletted->setChecked(true);
                selectTgaPaletted();
            }
            else if(tgaLastDepth == 24)
            {
                tgaTruecolor24->setChecked(true);
                selectTgaTruecolor24();
            }
            else if(tgaLastDepth == 32)
            {
                tgaTruecolor32->setChecked(true);
                selectTgaTruecolor32();
            }
            else
            {
                tgaHighcolor15->setChecked(true);
                selectTgaHighcolor();
            }
            pages->setCurrentWidget(pngPage);
            pages->setCurrentWidget(tgaPage);
        }
        return QDialog::exec();
    }
    return QDialog::Rejected;
};

void TextureExportDialog::saveTexture()
{
    QString bitmapFilter = "Bitmap (*.bmp)";
    QString pngFilter = "Portable Network Graphics (*.png)";
    QString jpegFilter = "JPEG (*.jpg *.jpeg)";
    QString gifFilter = "Graphics Interchange Format (*.gif)";
    QString tiffFilter = "TIFF (*.tiff *.tif)";
    QString targaFilter = "TARGA (*.tga *.targa)";

    bool success = false;

    if(selectedFilter == bitmapFilter)
    {
        success = FreeImage_Save(FIF_BMP, cachedBitmap, filename.toLocal8Bit().data(), (bmpCompress->checkState() == Qt::Checked ? BMP_SAVE_RLE : 0));
    }
    else if(selectedFilter == pngFilter)
    {
        int compression = 0;
        switch(pngCompression->currentIndex())
        {
            case 0:
                compression = PNG_Z_BEST_COMPRESSION;
                break;
            case 1:
                compression = PNG_Z_DEFAULT_COMPRESSION;
                break;
            case 2:
                compression = PNG_Z_BEST_SPEED;
                break;
            case 3:
                compression = PNG_Z_NO_COMPRESSION;
                break;
        }
        success = FreeImage_Save(FIF_PNG, cachedBitmap, filename.toLocal8Bit().data(), (pngInterlaced->checkState() == Qt::Checked ? PNG_INTERLACED : 0)+compression);
    }
    else if(selectedFilter == jpegFilter)
    {
        success = FreeImage_Save(FIF_JPEG, cachedBitmap, filename.toLocal8Bit().data(), jpegQuality->value() | JPEG_OPTIMIZE | (jpegProgressive->checkState() == Qt::Checked ? JPEG_PROGRESSIVE : 0));
    }
    else if(selectedFilter == gifFilter)
    {

    }
    else if(selectedFilter == tiffFilter)
    {

        int compression = 0;
        switch(tiffCompression->currentIndex())
        {
            case 0:
                compression = TIFF_PACKBITS;
                break;
            case 1:
                compression = TIFF_DEFLATE;
                break;
            case 2:
                compression = TIFF_ADOBE_DEFLATE;
                break;
            case 3:
                compression = TIFF_LZW;
                break;
            case 4:
                compression = TIFF_NONE;
                break;
        }
        success = FreeImage_Save(FIF_TIFF, cachedBitmap, filename.toLocal8Bit().data(), compression);
    }
    else if(selectedFilter == targaFilter)
    {
        success = FreeImage_Save(FIF_TARGA, cachedBitmap, filename.toLocal8Bit().data(), (tgaCompress->checkState() == Qt::Checked ? TARGA_SAVE_RLE : 0));
    }

    if(!success)
    {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Failed to save texture!"));
        msgBox.setInformativeText(tr("An error occured while trying to save texture as ") + filename + tr(". Check the main log file for details."));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

    emit accept();
};

void TextureExportDialog::prepareFreeImageBitmap()
{
    if(level)
    {
        disableTransparencyItems();
        const DriverTexture* tex = level->textures.getTexture(textureIndex);
        if(tex)
        {
            if(tex->usesPalette())
            {
                if(textureBitmap)
                {
                    if(FreeImage_GetBPP(textureBitmap) != 8)
                    {
                        FreeImage_Unload(textureBitmap);
                        textureBitmap = NULL;
                    }
                }
                if(!textureBitmap)
                textureBitmap = FreeImage_Allocate(256,256,8);

                for(int i = 0; i < 256; i++)
                {
                    unsigned char* data = (unsigned char*)FreeImage_GetScanLine(textureBitmap,255-i);
                    memcpy(data,tex->getScanLine(i),sizeof(unsigned char)*256);
                }

                RGBQUAD *pal = FreeImage_GetPalette(textureBitmap);

                int palette = -1;
                if(d3d)
                {
                    int d3dTexIndex = d3d->findTexture(textureIndex);
                    D3DEntry* entry = d3d->getEntry(d3dTexIndex);
                    if(entry)
                    {
                        palette = entry->getPaletteIndex(paletteIndex);
                    }
                }

                DriverPalette* paletteData = NULL;
                if(palette != -1)
                paletteData = level->textures.getIndexedPalette(palette);

                if(paletteData)
                {
                    for (int i = 0; i < 256; i++)
                    {
                        pal[i].rgbRed = paletteData->colors[i].r;
                        pal[i].rgbGreen = paletteData->colors[i].g;
                        pal[i].rgbBlue = paletteData->colors[i].b;
                    }
                }
                else
                {
                    for (int i = 0; i < 256; i++)
                    {
                        pal[i].rgbRed = i;
                        pal[i].rgbGreen = i;
                        pal[i].rgbBlue = i;
                    }
                }
            }
            else
            {
                if(textureBitmap)
                {
                    if(FreeImage_GetBPP(textureBitmap) != 16)
                    {
                        FreeImage_Unload(textureBitmap);
                        textureBitmap = NULL;
                    }
                }
                if(!textureBitmap)
                textureBitmap = FreeImage_Allocate(256,256,16,0x7C00, 0x03E0, 0x001F);

                for(int i = 0; i < 256; i++)
                {
                    unsigned short* data = (unsigned short*)FreeImage_GetScanLine(textureBitmap,255-i);
                    memcpy(data,tex->getScanLine(i),sizeof(unsigned short)*256);
                }

                if(tex->hasTransparency())
                enableTransparencyItems();
            }
        }
    }
};

void TextureExportDialog::setPreviewSize(int size)
{
    if(size < 0)
    {
        imageSize->setText(QString());
    }
    else if(size < 1024)
    {
        imageSize->setText(tr("Preview size: ") + QString::number(size) + tr(" Bytes"));
    }
    else if(size < 1024*1024)
    {
        QString temp;
        temp.setNum((float)size/1024,'f',2);
        imageSize->setText(tr("Preview size: ") + temp + tr(" KB"));
    }
    else
    {
        QString temp;
        temp.setNum((float)size/(1024*1024),'f',2);
        imageSize->setText(tr("Preview size: ") + temp + tr(" MB"));
    }
};

void TextureExportDialog::applyMagicPink(FIBITMAP* bitmap)
{
    if(bitmap)
    {
        if(FreeImage_GetBPP(bitmap) == 24)
        {
            for(int y = 0; y < 256; y++)
            {
                unsigned char* bits = FreeImage_GetScanLine(bitmap,255-y);
                for(int x = 0; x < 256; x++)
                {
                    if(bits[FI_RGBA_RED] == 8 && bits[FI_RGBA_GREEN] == 8 && bits[FI_RGBA_BLUE] == 8)
                    {
                        bits[FI_RGBA_RED] = 255;
                        bits[FI_RGBA_GREEN] = 0;
                        bits[FI_RGBA_BLUE] = 255;
                    }
                    bits += 3;
                }
            }
        }
    }
};

void TextureExportDialog::updatePreviewImage(FIBITMAP* bitmap)
{
    QImage* tempImage;
    QPixmap tempPixmap;
    RGBQUAD *pal;

    switch(FreeImage_GetBPP(bitmap))
    {
        case 8:
            tempImage = new QImage(FreeImage_GetBits(bitmap),FreeImage_GetWidth(bitmap),FreeImage_GetHeight(bitmap),FreeImage_GetPitch(bitmap),QImage::Format_Indexed8);
            pal = FreeImage_GetPalette(bitmap);
            for (int i = 0; i < 256; i++)
            {
                tempImage->setColor(i,qRgb(pal[i].rgbRed,pal[i].rgbGreen,pal[i].rgbBlue));
            }
            tempPixmap.convertFromImage(tempImage->mirrored(false,true));
            imagePreview->setPixmap(tempPixmap);
            delete tempImage;
            break;
        case 16:
            tempImage = new QImage(FreeImage_GetBits(bitmap),FreeImage_GetWidth(bitmap),FreeImage_GetHeight(bitmap),FreeImage_GetPitch(bitmap),QImage::Format_RGB555);
            tempPixmap.convertFromImage(tempImage->mirrored(false,true));
            imagePreview->setPixmap(tempPixmap);
            delete tempImage;
            break;
        case 24:
            tempImage = new QImage(FreeImage_GetBits(bitmap),FreeImage_GetWidth(bitmap),FreeImage_GetHeight(bitmap),FreeImage_GetPitch(bitmap),QImage::Format_RGB888);
            tempPixmap.convertFromImage(tempImage->rgbSwapped().mirrored(false,true));
            imagePreview->setPixmap(tempPixmap);
            delete tempImage;
            break;
        case 32:
            tempImage = new QImage(FreeImage_GetBits(bitmap),FreeImage_GetWidth(bitmap),FreeImage_GetHeight(bitmap),FreeImage_GetPitch(bitmap),QImage::Format_ARGB32);
            tempPixmap.convertFromImage(tempImage->mirrored(false,true));
            imagePreview->setPixmap(tempPixmap);
            delete tempImage;
            break;
    };
};

FIBITMAP* TextureExportDialog::getConvertedTexture(int bpp, bool magicPink)
{
    FIBITMAP* temp = NULL;
    if(textureBitmap)
    {
        if(bpp == 8)
        {
            if(FreeImage_GetBPP(textureBitmap) == 8)
            {
                temp = FreeImage_Clone(textureBitmap);
            }
            else
            {
                FIBITMAP* temp24 = FreeImage_ConvertTo24Bits(textureBitmap);

                if(magicPink)
                applyMagicPink(temp24);

                temp = FreeImage_ColorQuantize(temp24,FIQ_NNQUANT);
                FreeImage_Unload(temp24);
            }
        }
        else if(bpp == 15)
        {
            FIBITMAP* temp24 = FreeImage_ConvertTo24Bits(textureBitmap);

            if(magicPink)
            applyMagicPink(temp24);

            temp = FreeImage_ConvertTo16Bits555(temp24);
            FreeImage_Unload(temp24);
        }
        else if(bpp == 24)
        {
            temp = FreeImage_ConvertTo24Bits(textureBitmap);
            if(magicPink)
            applyMagicPink(temp);
        }
        else if(bpp == 32)
        {
            temp = FreeImage_ConvertTo32Bits(textureBitmap);

            if(magicPink)
            {
                FIBITMAP* temp24 = FreeImage_ConvertTo24Bits(textureBitmap);
                for(int y = 0; y < 256; y++)
                {
                    unsigned char* bits24 = FreeImage_GetScanLine(temp24,255-y);
                    unsigned char* bits32 = FreeImage_GetScanLine(temp,255-y);
                    for(int x = 0; x < 256; x++)
                    {
                        if(bits24[FI_RGBA_RED] == 8 && bits24[FI_RGBA_GREEN] == 8 && bits24[FI_RGBA_BLUE] == 8)
                        {
                            bits32[FI_RGBA_ALPHA] = 0;
                        }
                        else bits32[FI_RGBA_ALPHA] = 255;

                        bits24 += 3;
                        bits32 += 4;
                    }
                }
                FreeImage_Unload(temp24);
            }
        }
    }
    return temp;
};

//BMP

void TextureExportDialog::updateBmpPreview()
{
    if(cachedBitmap)
    {
        previewMemFile.reset();
        int flags = 0;

        if(bmpCompress->checkState() == Qt::Checked && bmpPaletted->isChecked())
        flags = BMP_SAVE_RLE;

        if(FreeImage_SaveToHandle(FIF_BMP, cachedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), flags))
        {
            previewMemFile.seekMemory(0,SEEK_SET);
            FIBITMAP* preview = FreeImage_LoadFromHandle(FIF_BMP, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0);

            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());

            updatePreviewImage(preview);

            FreeImage_Unload(preview);
        }
    }
};

void TextureExportDialog::selectBmpTruecolor()
{
    bmpCompress->setEnabled(false);

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 24)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(24, (bmpMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);

    updateBmpPreview();
};

void TextureExportDialog::selectBmpPaletted()
{
    bmpCompress->setEnabled(true);

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 8)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(8, (bmpMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updateBmpPreview();
};

void TextureExportDialog::updateBmpMagicPink(int state)
{
    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }

    if(bmpTruecolor->isChecked())
    cachedBitmap = getConvertedTexture(24, (state == Qt::Checked ? true : false));
    else cachedBitmap = getConvertedTexture(8, (state == Qt::Checked ? true : false));

    updateBmpPreview();
};

//PNG

void TextureExportDialog::updatePngPreview()
{
    if(cachedBitmap)
    {
        int compression = 0;
        switch(pngCompression->currentIndex())
        {
            case 0:
                compression = PNG_Z_BEST_COMPRESSION;
                break;
            case 1:
                compression = PNG_Z_DEFAULT_COMPRESSION;
                break;
            case 2:
                compression = PNG_Z_BEST_SPEED;
                break;
            case 3:
                compression = PNG_Z_NO_COMPRESSION;
                break;
        }

        previewMemFile.reset();
        if(FreeImage_SaveToHandle(FIF_PNG, cachedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), (pngInterlaced->checkState() == Qt::Checked ? PNG_INTERLACED : 0)+compression))
        {
            previewMemFile.seekMemory(0,SEEK_SET);
            FIBITMAP* preview = FreeImage_LoadFromHandle(FIF_PNG, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0);

            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());

            updatePreviewImage(preview);
            FreeImage_Unload(preview);
        }
    }
};

void TextureExportDialog::selectPngPaletted()
{
    pngExportWithTransparency->setEnabled(false);
    pngMagicPink->setEnabled(true);

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 8)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(8, (pngMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updatePngPreview();
};

void TextureExportDialog::selectPngTruecolor()
{
    pngExportWithTransparency->setEnabled(true);

    if(cachedBitmap && ((FreeImage_GetBPP(cachedBitmap) == 24 && pngExportWithTransparency->checkState() != Qt::Checked) || (FreeImage_GetBPP(cachedBitmap) == 32 && pngExportWithTransparency->checkState() == Qt::Checked)))
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    if(transEnabled)
    {
        if(pngExportWithTransparency->checkState() == Qt::Checked)
        {
            pngMagicPink->setEnabled(false);
            cachedBitmap = getConvertedTexture(32, true);
        }
        else
        {
            pngMagicPink->setEnabled(true);
            cachedBitmap = getConvertedTexture(24, (pngMagicPink->checkState() == Qt::Checked ? true : false));
        }
    }
    else
    {
        cachedBitmap = getConvertedTexture(24, false);
    }
    updatePngPreview();
};

void TextureExportDialog::forcePngUpdate()
{
    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    if(pngTruecolor->isChecked())
    selectPngTruecolor();
    else selectPngPaletted();
};

void TextureExportDialog::setPng32Bit(int state)
{
    if(state == Qt::Checked)
    pngMagicPink->setEnabled(false);
    else pngMagicPink->setEnabled(true);
    forcePngUpdate();
};

//JPEG

void TextureExportDialog::updateJpegMagicPink(int state)
{
    if(state == Qt::Checked && cachedBitmap)
    {
        applyMagicPink(cachedBitmap);
    }
    else
    {
        if(cachedBitmap)
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = getConvertedTexture(24,false);
    }
    updateJpegPreview();
};

void TextureExportDialog::updateJpegPreview()
{
    if(cachedBitmap)
    {
        int quality = jpegQuality->value();
        previewMemFile.reset();
        if(FreeImage_SaveToHandle(FIF_JPEG, cachedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), quality | (jpegProgressive->checkState() == Qt::Checked ? JPEG_PROGRESSIVE : 0)))
        {
            previewMemFile.seekMemory(0,SEEK_SET);
            FIBITMAP* preview = FreeImage_LoadFromHandle(FIF_JPEG, previewMemFile.getIOPointer(), previewMemFile.getHandle(), JPEG_ACCURATE);

            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());

            updatePreviewImage(preview);
            FreeImage_Unload(preview);
        }
    }
};

//GIF

void TextureExportDialog::updateGifPreview()
{
    /*previewMemFile.reset();
    if()
    {
        if(FreeImage_SaveToHandle(FIF_GIF, cachedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0))
        {
            previewMemFile.seekMemory(0,SEEK_SET);
            FIBITMAP* preview = FreeImage_LoadFromHandle(FIF_GIF, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0);

            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());

            updatePreviewImage(preview);
            FreeImage_Unload(preview);
        }
    }
    else
    {
        if(FreeImage_SaveMultiBitmapToHandle(FIF_GIF,animatedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0))
        {
            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());
            QByteArray temp(previewMemFile.data(),previewMemFile.size());
            QBuffer buffer(&temp);
            QMovie movie(buffer);
            preview->setMovie(movie);
        }
    }*/
};

//TIFF

void TextureExportDialog::updateTiffPreview()
{
    if(cachedBitmap)
    {
        int compression = 0;
        switch(tiffCompression->currentIndex())
        {
            case 0:
                compression = TIFF_PACKBITS;
                break;
            case 1:
                compression = TIFF_DEFLATE;
                break;
            case 2:
                compression = TIFF_ADOBE_DEFLATE;
                break;
            case 3:
                compression = TIFF_LZW;
                break;
            case 4:
                compression = TIFF_NONE;
                break;
        }

        previewMemFile.reset();
        if(FreeImage_SaveToHandle(FIF_TIFF, cachedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), compression))
        {
            previewMemFile.seekMemory(0,SEEK_SET);
            FIBITMAP* preview = FreeImage_LoadFromHandle(FIF_TIFF, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0);

            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());

            updatePreviewImage(preview);
            FreeImage_Unload(preview);
        }
    }
};

void TextureExportDialog::selectTiffPaletted()
{
    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 8)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(8, (tiffMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updateTiffPreview();
};

void TextureExportDialog::selectTiffTruecolor()
{
    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 24)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(24, (tiffMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updateTiffPreview();
};

void TextureExportDialog::forceTiffUpdate()
{
    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    if(tiffTruecolor->isChecked())
    selectTiffTruecolor();
    else selectTiffPaletted();
};

//TGA

void TextureExportDialog::selectTgaPaletted()
{
    tgaMagicPink->setEnabled(true);

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 8)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(8, (tgaMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updateTgaPreview();
};

void TextureExportDialog::selectTgaHighcolor()
{
    tgaMagicPink->setEnabled(true);
    tgaLastDepth = 15;

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 16)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(15, (tgaMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updateTgaPreview();
};

void TextureExportDialog::selectTgaTruecolor24()
{
    tgaMagicPink->setEnabled(true);
    tgaLastDepth = 24;

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 24)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(24, (tgaMagicPink->checkState() == Qt::Checked ? true : false) && transEnabled);
    updateTgaPreview();
};

void TextureExportDialog::selectTgaTruecolor32()
{
    tgaMagicPink->setEnabled(false);
    tgaLastDepth = 32;

    if(cachedBitmap && FreeImage_GetBPP(cachedBitmap) == 32)
    return;

    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }
    cachedBitmap = getConvertedTexture(32, transEnabled);
    updateTgaPreview();
};

void TextureExportDialog::updateTgaPreview()
{
    if(cachedBitmap)
    {
        previewMemFile.reset();
        if(FreeImage_SaveToHandle(FIF_TARGA, cachedBitmap, previewMemFile.getIOPointer(), previewMemFile.getHandle(), (tgaCompress->checkState() == Qt::Checked ? TARGA_SAVE_RLE : 0)))
        {
            previewMemFile.seekMemory(0,SEEK_SET);
            FIBITMAP* preview = FreeImage_LoadFromHandle(FIF_TARGA, previewMemFile.getIOPointer(), previewMemFile.getHandle(), 0);

            previewMemFile.seekMemory(0,SEEK_END);
            setPreviewSize(previewMemFile.tellMemory());

            updatePreviewImage(preview);
            FreeImage_Unload(preview);
        }
    }
};

void TextureExportDialog::forceTgaUpdate()
{
    if(cachedBitmap)
    {
        FreeImage_Unload(cachedBitmap);
        cachedBitmap = NULL;
    }

    if(tgaHighcolor15->isChecked())
    selectTgaHighcolor();
    else if(tgaTruecolor24->isChecked())
    selectTgaTruecolor24();
    else if(tgaTruecolor32->isChecked())
    selectTgaTruecolor32();
    else selectTgaPaletted();
};
