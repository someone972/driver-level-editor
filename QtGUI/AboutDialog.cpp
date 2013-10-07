#include "AboutDialog.hpp"

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
{
    image = new QLabel(this);

    separator = new QFrame(this);
    separator->setLineWidth(0);
    separator->setMidLineWidth(1);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    description = new QTextEdit(this);
    description->setReadOnly(true);

    information = new QTextEdit(this);
    information->setReadOnly(true);

    thanks = new QTextEdit(this);
    thanks->setReadOnly(true);

    tabs = new QTabWidget(this);
    tabs->addTab(description,tr("&Description"));
    tabs->addTab(information,tr("&Information"));
    tabs->addTab(thanks,tr("&Thanks to..."));

    closeButton = new QPushButton(tr("&Close"),this);
    closeButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    layout = new QVBoxLayout();
    layout->addWidget(image,0,Qt::AlignHCenter);
    layout->addWidget(separator);
    layout->addWidget(tabs);
    layout->addWidget(closeButton,0,Qt::AlignHCenter);
    setLayout(layout);

    resize(width(),400);

    connect(closeButton,SIGNAL(clicked()), this, SLOT(hide()));
};

void AboutDialog::setImage(const QString& filename)
{
    QPixmap aboutPixmap(filename);
    image->setPixmap(aboutPixmap);
};

void AboutDialog::setImage(const QPixmap& pixmap)
{
    image->setPixmap(pixmap);
};

void AboutDialog::setDescription(const QString& text)
{
    description->setText(text);
};

void AboutDialog::setInformation(const QString& text)
{
    information->setText(text);
};

void AboutDialog::setThanks(const QString& text)
{
    thanks->setText(text);
};
