#ifndef EVENT_FILTERS_HPP
#define EVENT_FILTERS_HPP
#include <QtWidgets>
#include <iostream>

using namespace std;

class KeypressFilter : public QObject
{
    Q_OBJECT

    public:
        KeypressFilter(QObject* parent = 0);
        bool eventFilter(QObject *obj, QEvent *ev);
    public slots:
        void enableUpperFilter();
        void disableUpperFilter();
        void setUpperFilterEnabled(bool e);
        void enableSpaceFilter();
        void disableSpaceFilter();
        void setSpaceFilterEnabled(bool e);
    protected:
        bool enabled,spaceenabled;
};

class EventFilterDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
        EventFilterDelegate(QObject* parent = 0);
        void setEventFilterToApply(QObject* f);
        virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    protected:
        QObject* filter;
};

#endif
