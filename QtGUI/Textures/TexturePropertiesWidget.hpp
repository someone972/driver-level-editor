#ifndef TEXTURE_PROPERTIES_WIDGET_HPP
#define TEXTURE_PROPERTIES_WIDGET_HPP

#include <QtWidgets>
#include "../../Driver_Routines/driver_levels.hpp"

class TexturePropertiesWidget : public QWidget
{
    Q_OBJECT

    public:
        TexturePropertiesWidget(QWidget* parent = 0);
        void setTextureProperties(const DriverTexture* tex);
        unsigned short getFlags();
        short getCarNumber();

    signals:
        void propertiesChanged(unsigned short);
        void carNumberChanged(int);

    protected slots:
        void handlePalettedState(int state);
        void handleRamLoadState(int state);
        void handleCarnumChange(int value);
        void handleCleanToggle(bool checked);
        void handleDamagedToggle(bool checked);
        void handleCarTextureState(int state);
        void handleTransparencyState(int state);
        void disable();
        void enable();

    protected:
        QCheckBox* paletted;
        QCheckBox* carTexture;
        QLabel* carnumLabel;
        QSpinBox* carnum;
        QRadioButton* clean;
        QRadioButton* damaged;
        QCheckBox* hasTransparency;
        QCheckBox* ramLoad;

        unsigned short flags;
};

#endif
