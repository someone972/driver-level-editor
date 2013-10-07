#ifndef MODEL_PROPERTIES_HPP
#define MODEL_PROPERTIES_HPP

#include <QtGUI>

class ModelPropertiesWidget : public QWidget
{
    Q_OBJECT

    public:
        ModelPropertiesWidget(QWidget* parent = 0);
};

class ModelPropertiesDialog : public QDialog
{
    Q_OBJECT

    public:
        ModelPropertiesDialog(QWidget* parent = 0);
};

#endif
