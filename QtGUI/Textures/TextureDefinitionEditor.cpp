#include "TextureDefinitionEditor.hpp"

TextureDefinitionList::TextureDefinitionList(DriverLevel* lev,QObject* parent) : QAbstractTableModel(parent)
{
    level = lev;
    enforceValidData = true;
    resortOnChange = true;
};

TextureDefinitionList::~TextureDefinitionList()
{
    if(level)
        level->unregisterEventHandler(this);
};

void TextureDefinitionList::setLevel(DriverLevel* lev)
{
    if(level)
        level->unregisterEventHandler(this);
    level = lev;
    if(level)
        level->registerEventHandler(this);
    reset();
};

void TextureDefinitionList::levelDestroyed()
{
    level = NULL;
    reset();
};

void TextureDefinitionList::getSettings(INI* settings)
{
    if(settings)
    {
        enforceValidData = settings->get_bool("texture_name_list","enforceValidData",true);
        resortOnChange = settings->get_bool("texture_name_list","resortOnChange",true);
    }
};

int TextureDefinitionList::rowCount(const QModelIndex &parent) const
{
    if(!level)
    return 0;
    else return level->textureDefinitions.getNumTextureDefinitions();
};

int TextureDefinitionList::columnCount(const QModelIndex &parent) const
{
    return 6;
};

bool TextureDefinitionList::removeRows(int row,int count,const QModelIndex& parent)
{
    if(level)
    {
        emit beginRemoveRows(parent,row,row+count);

        for(int i = row; i < row+count; i++)
        {
            if(i >= 0 && i < level->textureDefinitions.getNumTextureDefinitions())
            {
                //next position will end up in row of previously deleted one, so index is same.
                level->textureDefinitions.removeTextureDefinition(row);
            }
        }
        emit endRemoveRows();
        return true;
    }
    return false;
};

void TextureDefinitionList::resetList()
{
    reset();
};

QVariant TextureDefinitionList::data(const QModelIndex &index, int role) const
{
    if(role == Qt::SizeHintRole)
    {
        return QSize(10,10);
    }
    else if((role == Qt::EditRole || role == Qt::DisplayRole) && level)
    {
        TextureDefinition* def = level->textureDefinitions.getTextureDefinition(index.row());
        if(def)
        {
            switch(index.column())
            {
                case 0:
                    return QString(def->getName());
                case 1:
                    return QVariant(def->getTexture());
                case 2:
                    return QVariant(def->getX());
                case 3:
                    return QVariant(def->getY());
                case 4:
                    return QVariant(def->getW());
                case 5:
                    return QVariant(def->getH());
            }
        }
    }
    return QVariant();
};

QVariant TextureDefinitionList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
            case 0:
                return QString(tr("Name"));
            case 1:
                return QString(tr("Texture"));
            case 2:
                return QString("X");
            case 3:
                return QString("Y");
            case 4:
                return QString("W");
            case 5:
                return QString("H");
            default:
                return QVariant();
        }
    }
    return QString("%1").arg(section);
};

Qt::ItemFlags TextureDefinitionList::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
};

bool TextureDefinitionList::insertRow(int row, const QModelIndex & parent)
{
    return insertRows(row,1,parent);
};

bool TextureDefinitionList::insertRows(int row, int count, const QModelIndex & parent)
{
    if(level && count > 0)
    {
        if(row >= 0 && row <= level->textureDefinitions.getNumTextureDefinitions())
        {
            TextureDefinition temp;
            temp.setX(0);
            temp.setY(0);
            temp.setW(16);
            temp.setH(16);
            temp.setTexture(0);
            temp.setName("");
            TextureDefinition* old = level->textureDefinitions.getTextureDefinition(row);
            if(old)
            temp.setTexture(old->getTexture());

            emit beginInsertRows(index(0,0),row,row+count-1);
            level->textureDefinitions.insertTextureDefinitions(row,count,temp);
            emit endInsertRows();
            return true;
        }
    }
    return false;
};

bool TextureDefinitionList::setData(const QModelIndex & idx, const QVariant & value, int role)
{
    if(role == Qt::EditRole)
    {
        if(level && idx.row() >= 0 && idx.row() < level->textureDefinitions.getNumTextureDefinitions())
        {
            TextureDefinition temp;
            if(level->textureDefinitions.getTextureDefinition(idx.row(),&temp))
            {
                if(enforceValidData)
                {
                    QMessageBox msgbox;
                    msgbox.setIcon(QMessageBox::Warning);
                    int ret;

                    switch(idx.column())
                    {
                        case 0:
                            ret = level->textureDefinitions.findTextureDefByName(value.toString().toLocal8Bit().data());
                            if(ret != -1 && ret != idx.row())
                            {
                                msgbox.setText(tr("<b>Texture name already used by another entry!</b>"));
                                msgbox.setInformativeText(tr("Please enter a unique texture name. Name was taken by entry: ")+QString::number(ret));
                                msgbox.exec();
                                return false;
                            }
                            break;
                        case 1:
                            if(value.toInt() < 0 || value.toInt() >= level->textures.getNumTextures())
                            {
                                msgbox.setText(tr("<b>Texture idx is not valid!</b>"));
                                msgbox.setInformativeText(tr("Index must be between 0 and ")+QString::number(level->textures.getNumTextures())+".");
                                msgbox.exec();
                                return false;
                            }
                            break;
                    }
                }
                switch(idx.column())
                {
                    case 0:
                        temp.setName(value.toString().toLocal8Bit().data());
                        break;
                    case 1:
                        temp.texture = value.toInt();
                        break;
                    case 2:
                        temp.x = value.toInt();
                        break;
                    case 3:
                        temp.y = value.toInt();
                        break;
                    case 4:
                        temp.w = value.toInt();
                        break;
                    case 5:
                        temp.h = value.toInt();
                        break;
                }

                if(enforceValidData)
                {
                    if(temp.getX()+temp.getW() > 255)
                    temp.setW(255-temp.getX());
                    if(temp.getY()+temp.getH() > 255)
                    temp.setH(255-temp.getY());
                }

                level->textureDefinitions.setTextureDefinition(idx.row(),temp);
                int position = idx.row();
                if(resortOnChange)
                {
                    position = level->textureDefinitions.getSortedPosition(idx.row());
                    if(position != idx.row())
                    {
                        emit beginMoveRows(idx,idx.row(),idx.row(),idx,position);
                        level->textureDefinitions.resortTextureDefinition(idx.row());
                        emit endMoveRows();
                    }
                }
                emit dataChanged(index(position,idx.column()),index(position,idx.column()));
                return true;
            }
        }
    }
    return false;
};

int TextureDefinitionList::insertDefinition(int position,TextureDefinition def)
{
    if(level)
    {
        if(position >= 0 && position <= level->textureDefinitions.getNumTextureDefinitions())
        {
            level->textureDefinitions.insertTextureDefinition(position,def);
            emit dataChanged(index(position,0),index(position,5));
        }
    }
    return position;
};

OverlayedTexture::OverlayedTexture(const QGLFormat& glform,QWidget* parent,const QGLWidget*  shareWidget, Qt::WindowFlags f) : QGLWidget(glform,parent,shareWidget,f)
{
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    moving = false;
    resizing = false;
    state = NO_ACTION;
    hoverState = NO_ACTION;
    texidx = -1;
    texlist = NULL;
    enabled = false;
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAutoBufferSwap(false);
};

void OverlayedTexture::setTextureProvider(TextureList* list)
{
    texlist = list;
    updateGL();
};

void OverlayedTexture::setTexture(int idx)
{
    texidx = idx;
    updateGL();
};

void OverlayedTexture::setOverlayRect(int nx,int ny,int nw,int nh)
{
    x = nx;
    y = ny;
    w = nw;
    h = nh;
    updateGL();
};

void OverlayedTexture::resizeGL(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,width,height);
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    updateGL();
};

void OverlayedTexture::initializeGL()
{

};

void OverlayedTexture::paintGL()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if(texlist)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1,1,1);
        glBindTexture(GL_TEXTURE_2D,texlist->getTexture(texidx));

        float min = width();
        if(height() < min)
        min = height();

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex2f(width()/2-min/2,height()/2-min/2);
        glTexCoord2f(1,1);
        glVertex2f(width()/2+min/2,height()/2-min/2);
        glTexCoord2f(1,0);
        glVertex2f(width()/2+min/2,height()/2+min/2);
        glTexCoord2f(0,0);
        glVertex2f(width()/2-min/2,height()/2+min/2);
        glEnd();
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        glDisable(GL_TEXTURE_2D);
        if(enabled)
        {
            glColor3f(0.7,0.7,1.0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+0.5,height()/2+min/2-((float)(y)/256.0)*min-0.5);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+((float)(w+1)/256.0)*min-0.5,height()/2+min/2-((float)(y)/256.0)*min-0.5);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+((float)(w+1)/256.0)*min-0.5,height()/2+min/2-((float)(y)/256.0)*min-((float)(h+1)/256.0)*min+0.5);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+0.5,height()/2+min/2-((float)(y)/256.0)*min-((float)(h+1)/256.0)*min+0.5);
            glEnd();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0,1.0,0,0.5);
            glBegin(GL_QUADS);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+1,height()/2+min/2-((float)(y)/256.0)*min-1);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+((float)(w+1)/256.0)*min-1,height()/2+min/2-((float)(y)/256.0)*min-1);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+((float)(w+1)/256.0)*min-1,height()/2+min/2-((float)(y)/256.0)*min-((float)(h+1)/256.0)*min+1);
            glVertex2f(width()/2-min/2+((float)x/256.0)*min+1,height()/2+min/2-((float)(y)/256.0)*min-((float)(h+1)/256.0)*min);
            glEnd();
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    swapBuffers();
};

void OverlayedTexture::setOverlayEnabled(bool en)
{
    enabled = en;
    updateGL();
};


void OverlayedTexture::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case Qt::Key_Control:
            moving = true;
            break;
        case Qt::Key_Shift:
            resizing = true;
            break;
        case Qt::Key_Up:
            if(enabled)
            {
                if(moving && y > 0)
                {
                    y--;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
                else if(resizing && h > 0)
                {
                    h--;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
            }
            break;
        case Qt::Key_Down:
            if(enabled)
            {
                if(moving && y+h < 255)
                {
                    y++;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
                else if(resizing && y+h < 255)
                {
                    h++;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
            }
            break;
        case Qt::Key_Right:
            if(enabled)
            {
                if(moving && x+w < 255)
                {
                    x++;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
                else if(resizing && x+w < 255)
                {
                    w++;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
            }
            break;
        case Qt::Key_Left:
            if(enabled)
            {
                if(moving && x > 0)
                {
                    x--;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
                else if(resizing && w > 0)
                {
                    w--;
                    update();
                    emit overlayRectChanged(x,y,w,h);
                }
            }
            break;
    }
};

void OverlayedTexture::keyReleaseEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case Qt::Key_Control:
            moving = false;
            break;
        case Qt::Key_Shift:
            resizing = false;
            break;
    }
};

void OverlayedTexture::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        state = hoverState;
    }
};

void OverlayedTexture::mouseReleaseEvent(QMouseEvent* event)
{
    state = NO_ACTION;
    emit overlayRectChanged(x,y,w,h);
};

void OverlayedTexture::mouseMoveEvent(QMouseEvent *event)
{
    float min = width();
    if(height() < min)
    min = height();
    int xpos = (event->x()-(width()/2-min/2))/(min/256.0);
    int ypos = (event->y()-(height()/2-min/2))/(min/256.0);

    if(enabled)
    {
        if(state == NO_ACTION)
        {
            hoverState = NO_ACTION;
            if(xpos == x)
            hoverState |= RESIZE_LEFT;
            else if(xpos == x+w)
            hoverState |= RESIZE_RIGHT;

            if(ypos == y)
            hoverState |= RESIZE_TOP;
            else if(ypos == y+h)
            hoverState |= RESIZE_BOTTOM;

            if(xpos >= x+1 && xpos < x+w-1 && ypos >= y+1 && ypos < y+h-1)
            {
                hoverState = DRAG_ACTION;
                offsetX = xpos-x;
                offsetY = ypos-y;
                setCursor(Qt::SizeAllCursor);
            }
            else if((xpos == x || xpos == x+w) && ypos >= y+1 && ypos < y+h-1)
            setCursor(Qt::SizeHorCursor);
            else if((ypos == y || ypos == y+h) && xpos >= x+1 && xpos < x+w-1)
            setCursor(Qt::SizeVerCursor);
            else if((ypos == y && xpos == x) || (ypos == y+h && xpos == x+w))
            setCursor(Qt::SizeFDiagCursor);
            else if((ypos == y+h && xpos == x) || (ypos == y && xpos == x+w))
            setCursor(Qt::SizeBDiagCursor);
            else setCursor(Qt::ArrowCursor);
        }
        else if(state == DRAG_ACTION)
        {
            x = xpos-offsetX;
            y = ypos-offsetY;
            if(x+w > 255)
            x = 255-w;
            if(y+h > 255)
            y = 255-h;
            if(x < 0)
            x = 0;
            if(y < 0)
            y = 0;
            update();
            emit overlayRectChanged(x,y,w,h);
        }
        else
        {
            if(state & RESIZE_LEFT)
            {
                int temp = x+w;
                if(temp-xpos >= 0)
                {
                    x = xpos;
                    if(x < 0)
                    x = 0;
                    w = temp-x;
                }
                else
                {
                    x = temp+1;
                    if(x > 255)
                    x = 255;
                    w = xpos-(temp+1);
                    if(x+w > 255)
                    w = 255-x;
                    if(x < 255)
                    {
                        state &= ~RESIZE_LEFT;
                        state |= RESIZE_RIGHT;
                    }
                }
            }
            else if(state& RESIZE_RIGHT)
            {
                if(xpos-x >= 0)
                {
                    w = xpos-x;
                    if(x+w > 255)
                    w = 255-x;
                }
                else
                {
                    if(xpos >= 0)
                    {
                        w = x-xpos-1;
                        x = xpos;
                    }
                    else
                    {
                        w = x-1;
                        x = 0;
                    }
                    if(w < 0)
                    w = 0;
                    if(x > 0)
                    {
                        state |= RESIZE_LEFT;
                        state &= ~RESIZE_RIGHT;
                    }
                }
            }

            if(state & RESIZE_TOP)
            {
                int temp = y+h;
                if(temp-ypos >= 0)
                {
                    y = ypos;
                    if(y < 0)
                    y = 0;
                    h = temp-y;
                }
                else
                {
                    y = temp+1;
                    if(y > 255)
                    y = 255;
                    h = ypos-(temp+1);
                    if(y+h > 255)
                    h = 255-y;
                    if(y < 255)
                    {
                        state &= ~RESIZE_TOP;
                        state |= RESIZE_BOTTOM;
                    }
                }
            }
            else if(state& RESIZE_BOTTOM)
            {
                if(ypos-y >= 0)
                {
                    h = ypos-y;
                    if(y+h > 255)
                    h = 255-y;
                }
                else
                {
                    if(ypos >= 0)
                    {
                        h = y-ypos-1;
                        y = ypos;
                    }
                    else
                    {
                        h = y-1;
                        y = 0;
                    }
                    if(h < 0)
                    h = 0;
                    if(y > 0)
                    {
                        state |= RESIZE_TOP;
                        state &= ~RESIZE_BOTTOM;
                    }
                }
            }
            update();
            emit overlayRectChanged(x,y,w,h);
        }
    }
    else setCursor(Qt::ArrowCursor);
};

TextureDefinitionEditor::TextureDefinitionEditor(QWidget* parent,const QGLWidget*  shareWidget, Qt::WindowFlags f) : QWidget(parent)
{
    level = NULL;
    texList = NULL;
    texDefsView = new QTableView(this);
    texDefsModel = new TextureDefinitionList(NULL,this);
    texDefsView->setModel(texDefsModel);
    texDefsView->setColumnWidth(0,80);
    texDefsView->setColumnWidth(1,60);
    texDefsView->setColumnWidth(2,50);
    texDefsView->setColumnWidth(3,50);
    texDefsView->setColumnWidth(4,50);
    texDefsView->setColumnWidth(5,50);
    texDefsView->setMaximumWidth(400);
    texDefsView->setContextMenuPolicy(Qt::CustomContextMenu);
    texDefsView->setSelectionMode(QAbstractItemView::SingleSelection);

    selection = -1;

    deleteAction = new QAction(tr("Delete entry"),this);
    insertAction = new QAction(tr("Insert new entry"),this);
    contextmenu = new QMenu(this);
    contextmenu->addAction(deleteAction);
    contextmenu->addAction(insertAction);

    QGLFormat form;
    form.setDoubleBuffer(true);
    overlayedTex = new OverlayedTexture(form,this,shareWidget,f);
    overlayedTex->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(overlayedTex,0,0);
    layout->addWidget(texDefsView,0,1);
    setLayout(layout);

    keyFilter = new KeypressFilter(this);
    filterDelegate = new EventFilterDelegate(this);
    filterDelegate->setEventFilterToApply(keyFilter);
    texDefsView->installEventFilter(keyFilter);
    texDefsView->setItemDelegate(filterDelegate);

    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deletePositionEntry()));
    connect(insertAction, SIGNAL(triggered()), this, SLOT(insertPositionEntry()));
    connect(overlayedTex, SIGNAL(overlayRectChanged(int,int,int,int)),this, SLOT(positionChanged(int,int,int,int)));
    connect(texDefsView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(doViewContext(const QPoint&)));
    connect(texDefsView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(texDefsModel,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(updateTexture(const QModelIndex&,const QModelIndex&)));
};

TextureDefinitionEditor::~TextureDefinitionEditor()
{
    if(level)
        level->textureDefinitions.unregisterEventHandler(this);
};

void TextureDefinitionEditor::positionChanged(int x,int y,int w,int h)
{
    if(level)
    {
        TextureDefinition* name = level->textureDefinitions.getTextureDefinition(selection);
        if(name)
        {
            name->x = x;
            name->y = y;
            name->w = w;
            name->h = h;
            //using set data causes massive issues, so we use this to refresh the view instead.
            texDefsView->hideRow(selection);
            texDefsView->showRow(selection);
        }
    }
};

void TextureDefinitionEditor::doViewContext(const QPoint& point)
{
    savedIndex = texDefsView->indexAt(point).row();
    contextmenu->exec(texDefsView->mapToGlobal(point)+QPoint(texDefsView->verticalHeader()->width(),texDefsView->horizontalHeader()->height()));
};

void TextureDefinitionEditor::deletePositionEntry()
{
    texDefsModel->removeRow(savedIndex);
};

void TextureDefinitionEditor::insertPositionEntry()
{
    TextureDefinition temp;
    if(level)
    {
        TextureDefinition* highlighted = level->textureDefinitions.getTextureDefinition(savedIndex);
        if(highlighted)
        temp.texture = highlighted->texture;
        temp.w = 32;
        temp.h = 32;

        int ret = texDefsModel->insertDefinition(savedIndex,temp);
        if(ret != -1)
        {
            texDefsView->selectionModel()->select(texDefsModel->index(ret,0),QItemSelectionModel::SelectCurrent);
            texDefsView->scrollTo(texDefsModel->index(ret,0));
        }
    }
};

void TextureDefinitionEditor::getSettings(INI* settings)
{
    texDefsModel->getSettings(settings);
    keyFilter->setUpperFilterEnabled(!settings->get_bool("texture_names","allow_lowercase",false));
    keyFilter->setSpaceFilterEnabled(!settings->get_bool("texture_names","allow_spaces",false));
};

void TextureDefinitionEditor::updateTexture(const QModelIndex& index1,const QModelIndex& index2)
{
    if(level)
    {
        TextureDefinition* name = level->textureDefinitions.getTextureDefinition(index1.row());
        if(name)
        {
            overlayedTex->setTexture(name->texture);
            overlayedTex->setOverlayRect(name->x,name->y,name->w,name->h);
            overlayedTex->setOverlayEnabled(true);
        }
    }
};

void TextureDefinitionEditor::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    if(level)
    {
        selection = selected.indexes().first().row();
        TextureDefinition* name = level->textureDefinitions.getTextureDefinition(selected.indexes().first().row());
        if(name)
        {
            overlayedTex->setTexture(name->texture);
            overlayedTex->setOverlayRect(name->x,name->y,name->w,name->h);
            overlayedTex->setOverlayEnabled(true);
        }
    }
};

void TextureDefinitionEditor::setLevel(DriverLevel* lev)
{
    if(level)
        level->textureDefinitions.unregisterEventHandler(this);

    texDefsModel->setLevel(lev);
    level = lev;
    if(level)
        level->textureDefinitions.registerEventHandler(this);

    overlayedTex->setOverlayEnabled(false);
    texDefsView->reset();
    overlayedTex->updateGL();
};

OverlayedTexture* TextureDefinitionEditor::overlay()
{
    return overlayedTex;
};

void TextureDefinitionEditor::setTextureProvider(TextureList* list)
{
    overlayedTex->setTextureProvider(list);
    texList = list;
};

void TextureDefinitionEditor::definitionsReset(bool aboutToBe)
{
    if(!aboutToBe)
    texDefsModel->resetList();
};

void TextureDefinitionEditor::definitionsOpened()
{
    texDefsModel->resetList();
};

void TextureDefinitionEditor::definitionChanged(int idx)
{
    //TODO: Implement me
};

void TextureDefinitionEditor::definitionsDestroyed()
{
    level = NULL;
    overlayedTex->setOverlayEnabled(false);
    texDefsView->reset();
    overlayedTex->updateGL();
};
