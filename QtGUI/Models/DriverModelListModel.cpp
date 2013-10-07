#include "DriverModelListModel.hpp"

DriverModelListModel::DriverModelListModel(bool event, QObject* parent) : QAbstractTableModel(parent)
{
    level = NULL;
    isEventList = event;
};

void DriverModelListModel::setLevel(DriverLevel* lev)
{
    level = lev;
    beginResetModel();
    endResetModel();
};

int DriverModelListModel::rowCount(const QModelIndex &/*parent*/) const
{
    if(!level)
    return 0;

    if(isEventList)
        return level->eventModels.getNumModels();
    return level->models.getNumModels();
};

int DriverModelListModel::columnCount(const QModelIndex &/*parent*/) const
{
    if(isEventList)
        return 7;
    return 8;
};

QVariant DriverModelListModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::SizeHintRole)
    {
        return QSize(10,10);
    }
    if(level)
    {
        if(index.row() >= 0 && index.row() < (isEventList ? level->eventModels.getNumModels() : level->models.getNumModels()))
        {
            const DriverModel* model;
            int adjustedColumn;

            if(isEventList)
            {
                adjustedColumn = index.column()+1;
                model = level->eventModels.getModel(index.row());
            }
            else
            {
                adjustedColumn = index.column();
                model = level->models.getModel(index.row());
            }

            if(model)
            {
                if(role == Qt::BackgroundColorRole)
                {
                    if(model->getModelReference() != -1)
                        return QColor(255,100,100);
                    return QColor(255,255,255);
                }
                else if(role == Qt::EditRole || role == Qt::DisplayRole)
                {
                    switch(adjustedColumn)
                    {
                        case 0:
                            return index.row();
                        case 1:
                            if(isEventList)
                                return QString(tr("Event Model "))+QString::number(index.row());
                            return QString(level->modelNames.getName(index.row()));
                        case 2:
                            return QString("0x%1").arg(model->flags1,8,16,QChar('0'));
                        case 3:
                            return QString("0x%1").arg(model->flags2,8,16,QChar('0'));
                        case 4:
                            return model->getNumVertices();
                        case 5:
                            return model->getNumFaces();
                        case 6:
                            return model->getNumCollisionBounds();
                        case 7:
                            return model->getModelReference();
                        default:
                            return QVariant();
                    }
                }
                else if(role == Qt::UserRole)
                {
                    switch(adjustedColumn)
                    {
                        case 0:
                            return index.row();
                        case 1:
                            if(isEventList)
                                return index.row();
                            return QString(level->modelNames.getName(index.row()));
                        case 2:
                            return model->flags1;
                        case 3:
                            return model->flags2;
                        case 4:
                            return model->getNumVertices();
                        case 5:
                            return model->getNumFaces();
                        case 6:
                            return model->getNumCollisionBounds();
                        case 7:
                            return model->getModelReference();
                        default:
                            return QVariant();
                    }
                }
            }
        }
    }
    return QVariant();
};

QVariant DriverModelListModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(isEventList)
            section++;

        switch(section)
        {
            case 0:
                return QString();
            case 1:
                return QString(tr("Model Name"));
            case 2:
                return QString(tr(" Flags "));
            case 3:
                return QString(tr("Flags 2"));
            case 4:
                return QString(tr("Vertices"));
            case 5:
                return QString(tr("Faces"));
            case 6:
                return QString(tr("BBoxes"));
            case 7:
                return QString(tr("Model Ref"));
            default:
                return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags DriverModelListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if(index.column() == 1 && !isEventList)
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool DriverModelListModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        if(level && !isEventList)
        {
            level->modelNames.setName(index.row(),value.toString().toLocal8Bit().data());
        }
    }
    return true;
};

void DriverModelListModel::updateRow(int row)
{
    emit dataChanged(index(row,0),index(row,columnCount()));
};

bool DriverModelListModel::insertRow(int row, const QModelIndex & parent)
{
    return insertRows(row,1,parent);
};

bool DriverModelListModel::insertRows(int row, int count, const QModelIndex & parent)
{
    if(level)
    {
        if(!isEventList)
        {
            if(row >= 0 && row < level->modelNames.getMaxNumNames()-1)
            {
                emit beginInsertRows(parent,row,row+count-1);
                for(int i = 0; i < count; i++)
                {
                    level->modelNames.insertName(row,"");
                }
                emit endInsertRows();
                return true;
            }
        }
    }
    return false;
};

void DriverModelListModel::resetList()
{
    beginResetModel();
    endResetModel();
};
