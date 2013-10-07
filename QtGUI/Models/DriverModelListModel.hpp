#ifndef DRIVER_MODEL_LIST_MODEL
#define DRIVER_MODEL_LIST_MODEL

#include <QtWidgets>
#include "../../Driver_Routines/driver_levels.hpp"

class DriverModelListModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        DriverModelListModel(bool event, QObject *parent = 0);
        void setLevel(DriverLevel* lev);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    public slots:
        void resetList();
        void updateRow(int row);
        bool insertRow(int row, const QModelIndex & parent = QModelIndex());
        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());

    protected:
        DriverLevel* level;
        bool isEventList;
};

#endif
