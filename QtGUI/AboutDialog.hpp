#ifndef ABOUT_DIALOG_HPP
#define ABOUT_DIALOG_HPP

#include <QtWidgets>

class AboutDialog : public QDialog
{
    Q_OBJECT

    public:
        AboutDialog(QWidget* parent = 0);

        void setImage(const QString& filename);
        void setImage(const QPixmap& pixmap);
        void setDescription(const QString& text);
        void setInformation(const QString& text);
        void setThanks(const QString& text);

    protected:
        QLabel* image;
        QFrame* separator;
        QTabWidget* tabs;
        QTextEdit* description;
        QTextEdit* information;
        QTextEdit* thanks;
        QPushButton* closeButton;
        QVBoxLayout* layout;
};

#endif
