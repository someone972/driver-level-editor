#include "PaletteDisplay.hpp"

PaletteViewModel::PaletteViewModel(QWidget* parent) : QAbstractTableModel(parent)
{
    gridSize = 15;
    numColumns = 32;
    for(int i = 0; i < 256; i++)
    {
        palette.colors[i].r = i;
        palette.colors[i].g = i;
        palette.colors[i].b = i;
    }
    setDefaultMapping();
};

int PaletteViewModel::rowCount(const QModelIndex &parent) const
{
    return 256/numColumns;
};

int PaletteViewModel::columnCount(const QModelIndex &parent) const
{
    return numColumns;
};

void PaletteViewModel::setNumColumns(int c)
{
    numColumns = c;
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::setGridSize(int pixels)
{
    if(pixels > 0)
    gridSize = pixels;
    beginResetModel();
    endResetModel();
};

QVariant PaletteViewModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::SizeHintRole)
    {
        return QSize(gridSize,gridSize);
    }
    else if(role == Qt::BackgroundColorRole)
    {
        if(index.row() >= 0 && index.row() < rowCount() && index.column() >= 0 && index.column() < columnCount())
        {
            int palIndex = remappedIndicies[index.row()*columnCount()+index.column()];
            return QColor(palette.colors[palIndex].r,palette.colors[palIndex].g,palette.colors[palIndex].b);
        }
    }
    return QVariant();
};

QVariant PaletteViewModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    return QVariant();
};

Qt::ItemFlags PaletteViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
};

void PaletteViewModel::setPalette(const DriverPalette* pal)
{
    if(pal)
    palette = *pal;
    beginResetModel();
    endResetModel();
};

int compareHue(const void* elem1, const void* elem2)
{
    if(((const color_index_info*)elem1)->hue > ((const color_index_info*)elem2)->hue)
    return 1;
    else if(((const color_index_info*)elem1)->hue == ((const color_index_info*)elem2)->hue)
    return 0;
    return -1;
};

int compareSaturation(const void* elem1, const void* elem2)
{
    if(((const color_index_info*)elem1)->saturation > ((const color_index_info*)elem2)->saturation)
    return 1;
    else if(((const color_index_info*)elem1)->saturation == ((const color_index_info*)elem2)->saturation)
    return 0;
    return -1;
};

int compareLightness(const void* elem1, const void* elem2)
{
    if(((const color_index_info*)elem1)->lightness > ((const color_index_info*)elem2)->lightness)
    return 1;
    else if(((const color_index_info*)elem1)->lightness == ((const color_index_info*)elem2)->lightness)
    return 0;
    return -1;
};

void PaletteViewModel::sortByHue()
{
    color_index_info colorInfo[256];

    for(int i = 0; i < 256; i++)
    {
        QColor color(palette.colors[i].r,palette.colors[i].g,palette.colors[i].b);
        color.getHsl(&colorInfo[i].hue,&colorInfo[i].saturation,&colorInfo[i].lightness);
        colorInfo[i].index = i;
    }

    qsort((void*)colorInfo,256,sizeof(color_index_info),&compareHue);

    for(int i = 0; i < 256; i++)
    {
        remappedIndicies[i] = colorInfo[i].index;
    }
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::sortBySaturation()
{
    color_index_info colorInfo[256];

    for(int i = 0; i < 256; i++)
    {
        QColor color(palette.colors[i].r,palette.colors[i].g,palette.colors[i].b);
        color.getHsl(&colorInfo[i].hue,&colorInfo[i].saturation,&colorInfo[i].lightness);
        colorInfo[i].index = i;
    }

    qsort((void*)colorInfo,256,sizeof(color_index_info),&compareSaturation);

    for(int i = 0; i < 256; i++)
    {
        remappedIndicies[i] = colorInfo[i].index;
    }
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::sortByLightness()
{
    color_index_info colorInfo[256];

    for(int i = 0; i < 256; i++)
    {
        QColor color(palette.colors[i].r,palette.colors[i].g,palette.colors[i].b);
        color.getHsl(&colorInfo[i].hue,&colorInfo[i].saturation,&colorInfo[i].lightness);
        colorInfo[i].index = i;
    }

    qsort((void*)colorInfo,256,sizeof(color_index_info),&compareLightness);

    for(int i = 0; i < 256; i++)
    {
        remappedIndicies[i] = colorInfo[i].index;
    }
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::sortHueByLightness()
{
    color_index_info colorInfo[256];
    int numDark = 0;
    int numMedium = 0;

    for(int i = 0; i < 256; i++)
    {
        QColor color(palette.colors[i].r,palette.colors[i].g,palette.colors[i].b);
        color.getHsl(&colorInfo[i].hue,&colorInfo[i].saturation,&colorInfo[i].lightness);
        colorInfo[i].index = i;
    }

    qsort((void*)colorInfo,256,sizeof(color_index_info),&compareLightness);

    for(int i = 0; i < 256; i++)
    {
        if(colorInfo[i].lightness < 50)
        numDark++;
        else if(colorInfo[i].lightness < 205)
        numMedium++;
        else break;
    }

    qsort((void*)colorInfo,numDark,sizeof(color_index_info),&compareHue);
    qsort((void*)&colorInfo[numDark],numMedium,sizeof(color_index_info),&compareHue);
    qsort((void*)&colorInfo[numDark+numMedium],256-(numMedium+numDark),sizeof(color_index_info),&compareHue);

    for(int i = 0; i < 256; i++)
    {
        remappedIndicies[i] = colorInfo[i].index;
    }
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::sortHueBySaturation()
{
    color_index_info colorInfo[256];
    int numPale = 0;

    for(int i = 0; i < 256; i++)
    {
        QColor color(palette.colors[i].r,palette.colors[i].g,palette.colors[i].b);
        color.getHsl(&colorInfo[i].hue,&colorInfo[i].saturation,&colorInfo[i].lightness);
        colorInfo[i].index = i;
    }

    qsort((void*)colorInfo,256,sizeof(color_index_info),&compareSaturation);

    for(int i = 0; i < 256; i++)
    {
        if(colorInfo[i].saturation < 50)
        numPale++;
        else break;
    }

    qsort((void*)colorInfo,numPale,sizeof(color_index_info),&compareHue);
    qsort((void*)&colorInfo[numPale],(256-numPale),sizeof(color_index_info),&compareHue);

    for(int i = 0; i < 256; i++)
    {
        remappedIndicies[i] = colorInfo[i].index;
    }
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::setRemappedIndex(int idx, int remapped)
{
    if(idx >= 0 && idx < 256 && remapped >= 0 && remapped < 256)
    {
        remappedIndicies[idx] = remapped;
        emit dataChanged(index(idx/columnCount(),idx%columnCount()),index(idx/columnCount(),idx%columnCount()));
    }
};

void PaletteViewModel::setRemappedIndicies(int mapped[256])
{
    for(int i = 0; i < 256; i++)
    remappedIndicies[i] = mapped[i]%256;
    beginResetModel();
    endResetModel();
};

void PaletteViewModel::setDefaultMapping()
{
    for(int i = 0; i < 256; i++)
    remappedIndicies[i] = i;
    beginResetModel();
    endResetModel();
};

PaletteDisplay::PaletteDisplay(QWidget* parent) : QWidget(parent)
{
    paletteModel = new PaletteViewModel(this);
    paletteView = new QTableView(this);
    paletteView->setModel(paletteModel);
    paletteView->verticalHeader()->hide();
    paletteView->horizontalHeader()->hide();
    numColumns = 32;
    paletteModel->setNumColumns(numColumns);
    gridSize = 15;
    paletteModel->setGridSize(gridSize);
    repairSizes();
};

void PaletteDisplay::repairSizes()
{
    setMinimumWidth(numColumns*gridSize+2);
    setMinimumHeight(256/numColumns*gridSize+2);
    setMaximumWidth(numColumns*gridSize+2);
    setMaximumHeight(256/numColumns*gridSize+2);
    paletteView->setMinimumWidth(numColumns*gridSize+2);
    paletteView->setMinimumHeight(256/numColumns*gridSize+2);
    for(int i = 0; i < paletteModel->rowCount(paletteModel->index(0,0)); i++)
    paletteView->setRowHeight(i,gridSize);

    for(int i = 0; i < paletteModel->columnCount(paletteModel->index(0,0)); i++)
    paletteView->setColumnWidth(i,gridSize);
    resize(numColumns*gridSize+6,256/numColumns*gridSize+2);
};

void PaletteDisplay::setPalette(const DriverPalette* pal)
{
    paletteModel->setPalette(pal);
   // paletteModel->sortHueBySaturation();
};

void PaletteDisplay::setGridSize(int pixels)
{
    gridSize = pixels;
    paletteModel->setGridSize(gridSize);
    repairSizes();
};

void PaletteDisplay::setNumColumns(int c)
{
    numColumns = c;
    paletteModel->setNumColumns(numColumns);
    repairSizes();
};

PalettedImage::PalettedImage(QWidget* parent) : QWidget(parent)
{
    QPixmap pixmap(256,256);
    image = new QLabel(this);
    image->setPixmap(pixmap);
    setMinimumHeight(256+2);
    setMinimumWidth(256+2);
    intermediateImage = NULL;
    palette.resize(256);
    for(int i = 0; i < 256; i++)
    {
        palette[i] = qRgb(i,i,i);
    }
};

PalettedImage::~PalettedImage()
{
    if(intermediateImage)
        delete intermediateImage;
};

void PalettedImage::setPalette(DriverPalette* pal)
{
    if(pal)
    {
        for(int i = 0; i < 256; i++)
        {
            palette[i] = qRgb(pal->colors[i].r,pal->colors[i].g,pal->colors[i].b);
        }
        if(intermediateImage)
        {
            intermediateImage->setColorTable(palette);
            image->setPixmap(QPixmap::fromImage(*intermediateImage));
        }
    }
};

void PalettedImage::setTextureData(const DriverTexture* tex)
{
    if(tex)
    {
        if(tex->usesPalette())
        {
            if(intermediateImage)
                delete intermediateImage;
            intermediateImage = new QImage(tex->getData(),256,256,QImage::Format_Indexed8);
            intermediateImage->setColorTable(palette);
            image->setPixmap(QPixmap::fromImage(*intermediateImage));
        }
    }
};
