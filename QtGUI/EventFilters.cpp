#include "EventFilters.hpp"

KeypressFilter::KeypressFilter(QObject* parent) : QObject(parent)
{
    enabled = true;
    spaceenabled = true;
};

bool KeypressFilter::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(ev);
        if(keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z && ((keyEvent->modifiers() & Qt::ShiftModifier) == false) && enabled)
		{
            QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress,
				keyEvent->key(), keyEvent->modifiers() | Qt::ShiftModifier,
				keyEvent->text().toUpper()));
			QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyRelease,
				keyEvent->key(), keyEvent->modifiers() | Qt::ShiftModifier,
				keyEvent->text().toUpper()));
			return true;
		}
		if(keyEvent->key() == Qt::Key_Space && spaceenabled)
		{
		    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress,
				Qt::Key_Minus, keyEvent->modifiers() | Qt::ShiftModifier,
				QString("_")));
			QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyRelease,
				Qt::Key_Minus, keyEvent->modifiers() | Qt::ShiftModifier,
				QString("_")));
			return true;
		}
    }
    return false;
};

void KeypressFilter::enableUpperFilter()
{
    enabled = true;
};

void KeypressFilter::disableUpperFilter()
{
    enabled = false;
};

void KeypressFilter::setUpperFilterEnabled(bool e)
{
    enabled = e;
};

void KeypressFilter::enableSpaceFilter()
{
    spaceenabled = true;
};

void KeypressFilter::disableSpaceFilter()
{
    spaceenabled = false;
};

void KeypressFilter::setSpaceFilterEnabled(bool e)
{
    spaceenabled = e;
};

EventFilterDelegate::EventFilterDelegate(QObject* parent) : QItemDelegate(parent)
{
    filter = NULL;
};

void EventFilterDelegate::setEventFilterToApply(QObject* f)
{
    filter = f;
};

QWidget * EventFilterDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QWidget* out = QItemDelegate::createEditor(parent,option,index);
    if(filter)
    out->installEventFilter(filter);
    return out;
};
