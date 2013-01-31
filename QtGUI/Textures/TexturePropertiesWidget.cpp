#include "TexturePropertiesWidget.hpp"

TexturePropertiesWidget::TexturePropertiesWidget(QWidget* parent) : QWidget(parent)
{
    flags = 2;

    paletted = new QCheckBox(tr("Paletted (8-bit)"),this);
    hasTransparency = new QCheckBox(tr("Has transparency"),this);
    ramLoad = new QCheckBox(tr("Load to ram"),this);
    carTexture = new QCheckBox(tr("Is player car texture"),this);
    ramLoad->setCheckState(Qt::Checked);
    carnumLabel = new QLabel(tr("Car number:"),this);
    carnumLabel->setEnabled(false);
    carnum = new QSpinBox(this);
    carnum->setMinimum(1);
    carnum->setMaximum(18);
    carnum->setEnabled(false);
    clean = new QRadioButton(tr("Clean"),this);
    clean->setChecked(true);
    clean->setEnabled(false);
    damaged = new QRadioButton(tr("Damaged"),this);
    damaged->setEnabled(false);

    QHBoxLayout* textureTypeLayout = new QHBoxLayout();
    textureTypeLayout->addWidget(clean);
    textureTypeLayout->addWidget(damaged);

    QHBoxLayout* carnumLayout = new QHBoxLayout();
    carnumLayout->addWidget(carnumLabel);
    carnumLayout->addWidget(carnum);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(paletted);
    mainLayout->addWidget(hasTransparency);
    mainLayout->addWidget(ramLoad);
    mainLayout->addWidget(carTexture);
    mainLayout->addLayout(textureTypeLayout);
    mainLayout->addLayout(carnumLayout);
    setLayout(mainLayout);

    connect(paletted, SIGNAL(stateChanged(int)), this, SLOT(handlePalettedState(int)));
    connect(hasTransparency, SIGNAL(stateChanged(int)), this, SLOT(handleTransparencyState(int)));
    connect(ramLoad, SIGNAL(stateChanged(int)), this, SLOT(handleRamLoadState(int)));
    connect(carTexture, SIGNAL(stateChanged(int)), this, SLOT(handleCarTextureState(int)));
    connect(clean, SIGNAL(toggled(bool)), this, SLOT(handleCleanToggle(bool)));
    connect(damaged, SIGNAL(toggled(bool)), this, SLOT(handleDamagedToggle(bool)));
    connect(carnum, SIGNAL(valueChanged(int)), this, SLOT(handleCarnumChange(int)));
};

void TexturePropertiesWidget::handlePalettedState(int state)
{
    if(state == Qt::Checked)
    flags |= TEX_USES_PALETTE;
    else flags &= ~TEX_USES_PALETTE;
    emit propertiesChanged(flags);
};

void TexturePropertiesWidget::handleRamLoadState(int state)
{
    if(state == Qt::Checked)
    flags |= TEX_LOADS_TO_RAM;
    else flags &= ~TEX_LOADS_TO_RAM;
    emit propertiesChanged(flags);
};

void TexturePropertiesWidget::handleCleanToggle(bool checked)
{
    if(checked && (flags&TEX_IS_DAMAGED_TEXTURE))
    {
        flags |= TEX_IS_CLEAN_TEXTURE;
        flags &= ~TEX_IS_DAMAGED_TEXTURE;
        emit propertiesChanged(flags);
    }
};

void TexturePropertiesWidget::handleDamagedToggle(bool checked)
{
    if(checked && (flags&TEX_IS_CLEAN_TEXTURE))
    {
        flags |= TEX_IS_DAMAGED_TEXTURE;
        flags &= ~TEX_IS_CLEAN_TEXTURE;
        emit propertiesChanged(flags);
    }
};

void TexturePropertiesWidget::handleCarnumChange(int value)
{
    emit carNumberChanged(value-1);
};

void TexturePropertiesWidget::handleCarTextureState(int state)
{
    bool enabled = (state == Qt::Checked ? true : false);
    carnumLabel->setEnabled(enabled);
    carnum->setEnabled(enabled);
    clean->setEnabled(enabled);
    damaged->setEnabled(enabled);
    if(enabled)
    {
        if(clean->isChecked())
        flags |= TEX_IS_CLEAN_TEXTURE;
        else flags |= TEX_IS_DAMAGED_TEXTURE;

        emit propertiesChanged(flags);
        emit carNumberChanged(carnum->value()-1);
    }
    else
    {
        flags &= ~(TEX_IS_CLEAN_TEXTURE|TEX_IS_DAMAGED_TEXTURE);

        emit propertiesChanged(flags);
        emit carNumberChanged(-1);
    }
};

void TexturePropertiesWidget::handleTransparencyState(int state)
{
    bool enabled = (state == Qt::Checked ? false : true);
    ramLoad->setEnabled(enabled);

    if(state == Qt::Checked)
    flags |= TEX_HAS_TRANSPARENCY;
    else flags &= ~TEX_HAS_TRANSPARENCY;

    if(enabled && ramLoad->isChecked())
    flags |= TEX_LOADS_TO_RAM;
    else if(flags&TEX_LOADS_TO_RAM)
    flags &= ~TEX_LOADS_TO_RAM;

    emit propertiesChanged(flags);
};

void TexturePropertiesWidget::enable()
{
    paletted->setEnabled(true);
    hasTransparency->setEnabled(true);
    carTexture->setEnabled(true);
    ramLoad->setEnabled(true);
    carnumLabel->setEnabled(true);
    carnum->setEnabled(true);
    clean->setEnabled(true);
    damaged->setEnabled(true);
    //handleCarTextureState(Qt::Checked);
};

void TexturePropertiesWidget::disable()
{
    paletted->setEnabled(false);
    hasTransparency->setEnabled(false);
    carTexture->setEnabled(false);
    ramLoad->setEnabled(false);
    carnumLabel->setEnabled(false);
    carnum->setEnabled(false);
    clean->setEnabled(false);
    damaged->setEnabled(false);
    //handleCarTextureState(Qt::Unchecked);
};

void TexturePropertiesWidget::setTextureProperties(const DriverTexture* tex)
{
    //disable signals so that propertiesChanged can't be emitted while we are setting them.
    bool palsig = paletted->signalsBlocked();
    bool texsig = carTexture->signalsBlocked();
    bool numsig = carnum->signalsBlocked();
    bool cleansig = clean->signalsBlocked();
    bool damsig = damaged->signalsBlocked();
    bool transsig = hasTransparency->signalsBlocked();
    bool ramsig = ramLoad->signalsBlocked();

    paletted->blockSignals(true);
    carTexture->blockSignals(true);
    carnum->blockSignals(true);
    clean->blockSignals(true);
    damaged->blockSignals(true);
    hasTransparency->blockSignals(true);
    ramLoad->blockSignals(true);
    if(!tex)
    {
        disable();
    }
    else
    {
        enable();
        if(tex->usesPalette())
        paletted->setCheckState(Qt::Checked);
        else paletted->setCheckState(Qt::Unchecked);

        if(tex->loadsToRam())
        ramLoad->setCheckState(Qt::Checked);
        else ramLoad->setCheckState(Qt::Unchecked);

        if(tex->hasTransparency())
        {
            hasTransparency->setCheckState(Qt::Checked);
            ramLoad->setEnabled(false);
        }
        else
        {
            hasTransparency->setCheckState(Qt::Unchecked);
            ramLoad->setEnabled(true);
        }

        bool enabled = false;
        if(tex->isCleanTexture() || tex->isDamageTexture())
        {
            if(tex->isDamageTexture())
            damaged->setChecked(true);
            else clean->setChecked(true);
            carTexture->setCheckState(Qt::Checked);
            carnum->setValue(tex->getCarNumber()+1);
            enabled = true;
        }
        else
        {
            carTexture->setCheckState(Qt::Unchecked);
            enabled = false;
        }

        carnumLabel->setEnabled(enabled);
        carnum->setEnabled(enabled);
        clean->setEnabled(enabled);
        damaged->setEnabled(enabled);

        flags = tex->getFlags();
    }
    paletted->blockSignals(palsig);
    carTexture->blockSignals(texsig);
    carnum->blockSignals(numsig);
    clean->blockSignals(cleansig);
    damaged->blockSignals(damsig);
    hasTransparency->blockSignals(transsig);
    ramLoad->blockSignals(ramsig);
};

unsigned short TexturePropertiesWidget::getFlags()
{
    return flags;
};

short TexturePropertiesWidget::getCarNumber()
{
    if(carTexture->checkState() == Qt::Checked)
        return carnum->value()-1;
    return -1;
};
