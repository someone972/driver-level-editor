#ifndef PALETTE_DISPLAY_HPP
#define PALETTE_DISPLAY_HPP

#include <QtGUI>
#include "../../Driver_Routines/driver_levels.hpp"

struct color_index_info
{
    int hue;
    int saturation;
    int lightness;
    int index;
};

class PaletteViewModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        PaletteViewModel(QWidget* parent = 0);
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;

        void setRemappedIndicies(int mapping[256]);
        void setRemappedIndex(int idx, int remapped);
        void setGridSize(int pixels);
        void setNumColumns(int c);
        void setDefaultMapping();

    public slots:
        void setPalette(const DriverPalette* pal);
        void sortByHue();
        void sortBySaturation();
        void sortByLightness();
        void sortHueByLightness();
        void sortHueBySaturation();

    protected:
        DriverPalette palette;
        int gridSize;
        int numColumns;
        int remappedIndicies[256];
};

class PaletteDisplay : public QWidget
{
    Q_OBJECT

    public:
        PaletteDisplay(QWidget* parent = 0);

    public slots:
        void setPalette(const DriverPalette* pal);
        void setGridSize(int pixels);
        void setNumColumns(int c);

    protected:
        void repairSizes();

        QTableView* paletteView;
        PaletteViewModel* paletteModel;
        int gridSize;
        int numColumns;
};

class PalettedImage : public QWidget
{
    Q_OBJECT

    public:
        PalettedImage(QWidget* parent = 0);
        ~PalettedImage();
        void setTextureData(const DriverTexture* tex);
        void setPalette(DriverPalette* pal);

    protected:
        QLabel* image;
        QImage* intermediateImage;
        QVector<QRgb> palette;
};

#endif
