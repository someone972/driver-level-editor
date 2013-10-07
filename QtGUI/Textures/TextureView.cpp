#include "TextureView.hpp"

TextureViewGL::TexturePositioner::TexturePositioner()
{
    textureIndex = -1;
    paletteIndex = -1;
    gridX = 0;
    gridY = 0;

    startX = 0;
    startY = 0;
    startTime = 0;

    endX = 0;
    endY = 0;
    endTime = 0;
};

void TextureViewGL::TexturePositioner::setTextureIndex(int idx)
{
    textureIndex = idx;
};

void TextureViewGL::TexturePositioner::setPaletteIndex(int idx)
{
    paletteIndex = idx;
};

void TextureViewGL::TexturePositioner::setTransitionTime(int start, int end)
{
    startTime = start;
    endTime = end;
};

int TextureViewGL::TexturePositioner::getTextureIndex() const
{
    return textureIndex;
};

int TextureViewGL::TexturePositioner::getPaletteIndex() const
{
    return paletteIndex;
};

void TextureViewGL::TexturePositioner::setPosition(int x, int y, int _gridX, int _gridY)
{
    startX = x;
    startY = y;
    endX = x;
    endY = y;
    gridX = _gridX;
    gridY = _gridY;
};

void TextureViewGL::TexturePositioner::setTargetPosition(int x, int y, int _gridX, int _gridY, int currentTime)
{
    startX = getXPosition(currentTime);
    startY = getYPosition(currentTime);
    startTime = currentTime;

    endX = x;
    endY = y;
    gridX = _gridX;
    gridY = _gridY;
};

int TextureViewGL::TexturePositioner::getXPosition(int msecTime) const
{
    if(msecTime < startTime)
    return startX;
    if(msecTime > endTime)
    return endX;

    float ratio = (float)(msecTime-startTime)/(float)(endTime-startTime);
    float offset = (float)(endX-startX)*ratio;
    return startX+offset;
};

int TextureViewGL::TexturePositioner::getYPosition(int msecTime) const
{
    if(msecTime < startTime)
    return startY;
    if(msecTime > endTime)
    return endY;

    float ratio = (float)(msecTime-startTime)/(float)(endTime-startTime);
    float offset = (float)(endY-startY)*ratio;
    return startY+offset;
};

int TextureViewGL::TexturePositioner::getGridX() const
{
    return gridX;
};

int TextureViewGL::TexturePositioner::getGridY() const
{
    return gridY;
};

TextureViewGL::TextureViewGL(QWidget* parent, const QGLWidget * shareWidget, Qt::WindowFlags f) : QGLWidget(parent,shareWidget,f)
{
    setup();
};

TextureViewGL::TextureViewGL(const QGLFormat& format,QWidget* parent, const QGLWidget * shareWidget, Qt::WindowFlags f) : QGLWidget(format,parent,shareWidget,f)
{
   setup();
};

TextureViewGL::~TextureViewGL()
{

};

void TextureViewGL::setup()
{
    textureSize = 64;
    viewMode = 0;
    selectedTexture = -1;
    selectedPalette = -1;
    selectedPositioner = -1;
    heldPositioner = -1;
    lastTransition = 0;
    xDisplacement = 0;
    yDisplacement = 0;
    lastGridX = 0;
    lastGridY = 0;
    textures = NULL;
    textureList = NULL;

    timeKeeper = QTime(0,0);
    timeKeeper.start();
    timer.setInterval(16);
    timer.setSingleShot(false);
    for(int i = 0; i < 256; i++)
    {
        textureHidden[i] = false;
        textureColors[i] = -1;
    }

    setFocusPolicy(Qt::ClickFocus);

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkTimer()));
};

//OpenGL and drawing functions

void TextureViewGL::initializeGL()
{

};

void TextureViewGL::resizeGL(int width, int height)
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,width,height);
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
};

void TextureViewGL::drawTexture(int x, int y, GLuint texture)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(x, height()-y);
    glTexCoord2f(1,0);
    glVertex2f(x+textureSize, height()-y);
    glTexCoord2f(1,1);
    glVertex2f(x+textureSize, height()-(y+textureSize));
    glTexCoord2f(0,1);
    glVertex2f(x, height()-(y+textureSize));
    glEnd();
};

void TextureViewGL::drawHighlight(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    glDisable(GL_TEXTURE_2D);
    for(int i = 4; i >= 1; i--)
    {
        int red = r-20*i;
        int green = g-20*i;
        int blue = b-20*i;

        if(red < 0)
            red = 0;
        if(green < 0)
            green = 0;
        if(blue < 0)
            blue = 0;
        glColor3ub(red,green,blue);

        glBegin(GL_QUADS);
        glVertex2f((float)(x-i), (float)(height()-(y+textureSize+i)));
        glVertex2f((float)(x+textureSize+i), (float)(height()-(y+textureSize+i)));
        glVertex2f((float)(x+textureSize+i), (float)(height()-(y-i)));
        glVertex2f((float)(x-i), (float)(height()-(y-i)));
        glEnd();
    }
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
};

void TextureViewGL::drawPositioner(int i, int currentTime)
{
    if(i == selectedPositioner)
    {
        drawHighlight(texturePositions[i].getXPosition(currentTime),texturePositions[i].getYPosition(currentTime),0,0,255);
    }
    else if(!(textureColors[texturePositions[i].getTextureIndex()] & 0x80000000))
    {
        int color = textureColors[texturePositions[i].getTextureIndex()];
        drawHighlight(texturePositions[i].getXPosition(currentTime),texturePositions[i].getYPosition(currentTime),
                        (color>>16)&0x0ff,(color>>8)&0x0ff,(color)&0x0ff);
    }
    drawTexture(texturePositions[i].getXPosition(currentTime),texturePositions[i].getYPosition(currentTime),
                textureList->getTexture(texturePositions[i].getTextureIndex(),(texturePositions[i].getPaletteIndex() == -1 ? textureList->getCurrentPalette(texturePositions[i].getTextureIndex()) : texturePositions[i].getPaletteIndex())));
};

void TextureViewGL::paintGL()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-xDisplacement,yDisplacement,0);

    if(textures && textureList)
    {
        int currentTime = getMilliseconds();
        for(unsigned int i = 0; i < texturePositions.size(); i++)
        {
            if(!textureHidden[texturePositions[i].getTextureIndex()] && (int)i != heldPositioner)
            {
                drawPositioner(i,currentTime);
            }
        }
        if(heldPositioner != -1)
        drawPositioner(heldPositioner,currentTime);
    }

    if(!autoBufferSwap())
    swapBuffers();
};

//Overloaded QWidget functions and events

QSize TextureViewGL::minimumSizeHint() const
{
    return QSize(277,277);
};

QSize TextureViewGL::sizeHint() const
{
    return QSize(640,480);
};

void TextureViewGL::mouseMoveEvent(QMouseEvent* event)
{
    if(heldPositioner != -1)
    {
        int currentTime = getMilliseconds();
        int x = event->x()+xDisplacement-heldPositionX;
        int y = event->y()+yDisplacement-heldPositionY;
        int closestGridX = (x+textureSize/2)/(textureSize+10);
        int closestGridY = (y+textureSize/2)/(textureSize+10);
        int gridX = texturePositions[heldPositioner].getGridX();
        int gridY = texturePositions[heldPositioner].getGridY();
        int numWide = ((width()-10)/(textureSize+10));

        if(closestGridX < 0)
        closestGridX = 0;
        if(closestGridY < 0)
        closestGridY = 0;
        if(closestGridY > lastGridY)
        closestGridY = lastGridY;
        if(viewMode == 2)
        {
            int numPals = textureList->getNumPalettes(texturePositions[heldPositioner].getTextureIndex());
            if(numPals <= 1)
            closestGridX = 0;
            if(closestGridX >= numPals && numPals != 0)
            closestGridX = numPals-1;
        }
        else
        {
            if(closestGridX >= numWide)
            closestGridX = numWide-1;
            if(closestGridY == lastGridY && closestGridX > lastGridX)
            closestGridX = lastGridX;
        }

        if(closestGridX != gridX || closestGridY != gridY)
        {
            if(viewMode == 2)
            {
                int minY,maxY;
                if(closestGridY > gridY)
                {
                    minY = gridY+1;
                    maxY = closestGridY;
                }
                else
                {
                    minY = closestGridY;
                    maxY = gridY-1;
                }

                int minX,maxX;
                if(closestGridX > gridX)
                {
                    minX = gridX+1;
                    maxX = closestGridX;
                }
                else
                {
                    minX = closestGridX;
                    maxX = gridX-1;
                }
                for(unsigned int i = 0; i < texturePositions.size(); i++)
                {
                    if((int)i != heldPositioner)
                    {
                        int newGridX = texturePositions[i].getGridX();
                        int newGridY = texturePositions[i].getGridY();
                        if(texturePositions[i].getTextureIndex() == texturePositions[heldPositioner].getTextureIndex())
                        {
                            if(gridY != closestGridY)
                            newGridY = closestGridY;
                        }
                        else if(texturePositions[i].getGridY() >= minY && texturePositions[i].getGridY() <= maxY)
                        {
                            newGridY += (gridY > closestGridY ? 1 : -1);
                        }
                        if(texturePositions[i].getGridX() >= minX && texturePositions[i].getGridX() <= maxX
                           && texturePositions[i].getTextureIndex() == texturePositions[heldPositioner].getTextureIndex())
                        {
                            newGridX += (gridX > closestGridX ? 1 : -1);
                        }
                        if(texturePositions[i].getGridX() != newGridX || texturePositions[i].getGridY() != newGridY)
                        {
                            texturePositions[i].setTargetPosition(10+newGridX*(textureSize+10),10+newGridY*(textureSize+10),newGridX,newGridY,currentTime);
                            texturePositions[i].setTransitionTime(currentTime,currentTime+200);
                            lastTransition = currentTime+300;
                            if(!timer.isActive())
                            timer.start();
                        }
                    }
                }
            }
            else
            {
                int start = gridY*numWide+gridX;
                int end = closestGridY*numWide+closestGridX;
                int min,max;
                if(start > end)
                {
                    min = end;
                    max = start-1;
                }
                else
                {
                    min = start+1;
                    max = end;
                }

                for(unsigned int i = 0; i < texturePositions.size(); i++)
                {
                    int position = texturePositions[i].getGridX()+texturePositions[i].getGridY()*numWide;
                    if(position >= min && position <= max)
                    {
                        int newGridX = (position+(start > end ? 1 : -1))%numWide;
                        int newGridY = (position+(start > end ? 1 : -1))/numWide;
                        while(newGridX < 0)
                        {
                            newGridY--;
                            newGridX += numWide;
                        }
                        while(newGridX >= numWide)
                        {
                            newGridY++;
                            newGridX -= numWide;
                        }
                        texturePositions[i].setTargetPosition(10+newGridX*(textureSize+10),10+newGridY*(textureSize+10),newGridX,newGridY,currentTime);
                        texturePositions[i].setTransitionTime(currentTime,currentTime+200);
                        lastTransition = currentTime+300;
                        if(!timer.isActive())
                        timer.start();
                    }
                }
            }
        }
        texturePositions[heldPositioner].setPosition(x,y,closestGridX,closestGridY);
        update();
    }
};

void TextureViewGL::mousePressEvent(QMouseEvent* event)
{
    if(event->x()+xDisplacement > 10 && event->y()+yDisplacement > 10)
    {
        int x = event->x()+xDisplacement-10;
        int y = event->y()+yDisplacement-10;
        if(x%(textureSize+10) < textureSize && y%(textureSize+10) < textureSize)
        {
            int gridX = x/(textureSize+10);
            int gridY = y/(textureSize+10);
            int currentTime = getMilliseconds();
            for(unsigned int i = 0; i < texturePositions.size(); i++)
            {
                if(texturePositions[i].getGridX() == gridX && texturePositions[i].getGridY() == gridY)
                {

                    if(event->button() == Qt::LeftButton)
                    {
                        heldPositioner = i;
                        heldPositionX = event->x()+xDisplacement-texturePositions[i].getXPosition(currentTime);
                        heldPositionY = event->y()+yDisplacement-texturePositions[i].getYPosition(currentTime);
                    }
                    int oldTexture = selectedTexture;
                    int oldPalette = selectedPalette;
                    if(selectedTexture != texturePositions[i].getTextureIndex())
                    {
                        selectedTexture = texturePositions[i].getTextureIndex();
                    }
                    int palette = (texturePositions[i].getPaletteIndex() == -1 ? textureList->getCurrentPalette(texturePositions[i].getTextureIndex()) : texturePositions[i].getPaletteIndex());
                    if(selectedPalette !=  palette && palette != -1)
                    {
                        selectedPalette = palette;
                    }
                    if(selectedTexture != oldTexture)
                    {
                        emit textureSelectionChanged(selectedTexture);
                        update();
                    }
                    if(selectedPalette != oldPalette)
                    {
                        emit paletteSelectionChanged(selectedPalette);
                        update();
                    }
                    findSelectedPositioner();
                    break;
                }
            }
        }
    }
};

void TextureViewGL::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(heldPositioner != -1)
        {
            int newGridX = texturePositions[heldPositioner].getGridX();
            int newGridY = texturePositions[heldPositioner].getGridY();
            int newTextureIndex = 0;
            int newPaletteIndex = 0;
            int numWide = ((width()-10)/(textureSize+10));

            texturePositions[heldPositioner].setPosition(10+newGridX*(textureSize+10),10+newGridY*(textureSize+10),newGridX,newGridY);

            int gridX = 0;
            int gridY = 0;
            int num = 0;

            for(int i = 0; i < textures->getNumTextures(); i++)
            {
                if(!textureHidden[i])
                {
                    switch(viewMode)
                    {
                        case 0:
                            if(gridX == newGridX && gridY == newGridY)
                            {
                                newTextureIndex = i;
                                newPaletteIndex = -1;
                                break;
                            }
                            gridX++;
                            if(gridX >= numWide)
                            {
                                gridX = 0;
                                gridY++;
                            }
                            break;
                        case 1:
                            num = textureList->getNumPalettes(i);
                            if(num == 0)
                            num = 1;
                            for(int j = 0; j < num; j++)
                            {

                                if(gridX == newGridX && gridY == newGridY)
                                {
                                    newTextureIndex = i;
                                    newPaletteIndex = j;
                                    break;
                                }
                                gridX++;
                                if(gridX >= numWide)
                                {
                                    gridX = 0;
                                    gridY++;
                                }
                            }
                            break;
                        case 2:
                            num = textureList->getNumPalettes(i);
                            if(num == 0)
                            num = 1;
                            for(int j = 0; j < num; j++)
                            {

                                if(gridX == newGridX && gridY == newGridY)
                                {
                                    newTextureIndex = i;
                                    newPaletteIndex = j;
                                    break;
                                }
                                gridX++;
                            }
                            gridX = 0;
                            gridY++;
                            break;
                    }
                }
            }

            //moveTexture(texturePositions[heldPositioner].getTextureIndex(),
            update();
        }
        heldPositioner = -1;
    }
};

void TextureViewGL::resizeEvent(QResizeEvent* event)
{
    rebuildView();
    QGLWidget::resizeEvent(event);
};

void TextureViewGL::keyPressEvent(QKeyEvent* event)
{
    if(!(event->modifiers() & Qt::ControlModifier))
    {
        int gridX = 0;
        int gridY = 0;
        int numWide = (width()-10)/(textureSize+10);
        if(selectedPositioner != -1)
        {
            gridX = texturePositions[selectedPositioner].getGridX();
            gridY = texturePositions[selectedPositioner].getGridY();

            bool press = false;
            switch(event->key())
            {
                case Qt::Key_Up:
                    gridY--;
                    press = true;
                    break;
                case Qt::Key_Down:
                    gridY++;
                    press = true;
                    break;
                case Qt::Key_Left:
                    gridX--;
                    if(gridX < 0)
                    {
                        gridY--;
                        gridX = numWide-1;
                    }
                    press = true;
                    break;
                case Qt::Key_Right:
                    gridX++;
                    if(gridX >= numWide)
                    {
                        gridY++;
                        gridX = 0;
                    }
                    press = true;
                    break;
            }
            if(!press)
                return;

            int found = -1;
            for(unsigned int i = 0; i < texturePositions.size(); i++)
            {
                if(texturePositions[i].getGridX() == gridX && texturePositions[i].getGridY() == gridY)
                {
                    found = i;
                    break;
                }
            }
            if(found != -1)
            {
                selectedPositioner = found;
                if(texturePositions[found].getTextureIndex() != selectedTexture)
                {
                    selectedTexture = texturePositions[found].getTextureIndex();
                    update();
                    emit textureSelectionChanged(selectedTexture);
                }
                if(texturePositions[found].getPaletteIndex() != selectedPalette && texturePositions[found].getPaletteIndex() != -1)
                {
                    selectedPalette = texturePositions[found].getPaletteIndex();
                    update();
                    emit paletteSelectionChanged(selectedPalette);
                }
            }
        }
    }
};

//Misc.

void TextureViewGL::setTextureData(DriverTextures* newTexData)
{
    if(textures)
        textures->unregisterEventHandler(this);

    textures = newTexData;

    if(textures)
        textures->registerEventHandler(this);

    rebuildView();
};

void TextureViewGL::setGLTextureProvider(TextureList* newTexList)
{
    textureList = newTexList;
};

int TextureViewGL::getHorizontalScrollStep()
{
    return textureSize/4;
};

int TextureViewGL::getVerticalScrollStep()
{
    return textureSize/4;
};

int TextureViewGL::getMilliseconds()
{
    return timeKeeper.elapsed();
};

void TextureViewGL::checkTimer()
{
    if(getMilliseconds() > lastTransition)
    timer.stop();
};

void TextureViewGL::findSelectedPositioner()
{
    selectedPositioner = -1;
    for(unsigned int i = 0; i < texturePositions.size(); i++)
    {
        if(texturePositions[i].getTextureIndex() == selectedTexture && (texturePositions[i].getPaletteIndex() == selectedPalette || texturePositions[i].getPaletteIndex() == -1))
        {
            selectedPositioner = i;
            break;
        }
    }
};

void TextureViewGL::rebuildView()
{
    if(!textures || !textureList)
    {
        texturePositions.clear();
        return;
    }

    int numWide = (width()-10)/(textureSize+10);
    int gridX = 0;
    int gridY = 0;
    TexturePositioner tempPos;
    unsigned int currentPosition = 0;
    int num = 0;

    for(int i = 0; i < textures->getNumTextures(); i++)
    {
        if(!textureHidden[i])
        {
            switch(viewMode)
            {
                case 0:
                    tempPos.setTextureIndex(i);
                    tempPos.setPaletteIndex(-1);
                    tempPos.setPosition(10+gridX*(textureSize+10),10+gridY*(textureSize+10),gridX,gridY);

                    //Used to keep users from dropping textures in grid positions past the end of the textures.
                    lastGridX = gridX;
                    lastGridY = gridY;
                    gridX++;
                    if(gridX >= numWide)
                    {
                        gridX = 0;
                        gridY++;
                    }

                    if(currentPosition < texturePositions.size())
                        texturePositions[currentPosition] = tempPos;
                    else texturePositions.push_back(tempPos);
                    currentPosition++;
                    break;
                case 1:
                    num = textureList->getNumPalettes(i);
                    if(num == 0)
                    num = 1;
                    for(int j = 0; j < num; j++)
                    {
                        tempPos.setTextureIndex(i);
                        if(textureList->getNumPalettes(i) > 0)
                        tempPos.setPaletteIndex(j);
                        else tempPos.setPaletteIndex(-1);
                        tempPos.setPosition(10+gridX*(textureSize+10),10+gridY*(textureSize+10),gridX,gridY);

                        //Used to keep users from dropping textures in grid positions past the end of the textures.
                        lastGridX = gridX;
                        lastGridY = gridY;
                        gridX++;
                        if(gridX >= numWide)
                        {
                            gridX = 0;
                            gridY++;
                        }

                        if(currentPosition < texturePositions.size())
                            texturePositions[currentPosition] = tempPos;
                        else texturePositions.push_back(tempPos);
                        currentPosition++;
                    }
                    break;
                case 2:
                    num = textureList->getNumPalettes(i);
                    if(num == 0)
                    num = 1;
                    for(int j = 0; j < num; j++)
                    {
                        tempPos.setTextureIndex(i);
                        if(textureList->getNumPalettes(i) > 0)
                        tempPos.setPaletteIndex(j);
                        else tempPos.setPaletteIndex(-1);
                        tempPos.setPosition(10+gridX*(textureSize+10),10+gridY*(textureSize+10),gridX,gridY);

                        //Used to keep users from dropping textures in grid positions past the end of the textures.
                        lastGridX = gridX;
                        lastGridY = gridY;
                        gridX++;
                        if(currentPosition < texturePositions.size())
                            texturePositions[currentPosition] = tempPos;
                        else texturePositions.push_back(tempPos);
                        currentPosition++;
                    }
                    gridX = 0;
                    gridY++;
                    break;
            }
        }
    }

    for(unsigned int i = texturePositions.size(); i > currentPosition; i--)
    {
        texturePositions.pop_back();
    }
    findSelectedPositioner();
    emit sizeHasChanged();
};

void TextureViewGL::setTextureColor(int idx, unsigned char r, unsigned char g, unsigned char b)
{
    if(idx >= 0 && idx < 256)
    {
        textureColors[idx] &= 0x80000000;
        textureColors[idx] |= r<<16;
        textureColors[idx] |= g<<8;
        textureColors[idx] |= b;
    }
};

void TextureViewGL::enableTextureHighlight(int idx, bool enabled)
{
    if(idx >= 0 && idx < 256)
    {
        textureColors[idx] &= 0x7fffffff;
        if(enabled == false)
        textureColors[idx] |= 0x80000000;
    }
};

void TextureViewGL::setTextureHidden(int idx, bool hidden, bool refresh)
{
    if(idx >= 0 && idx < 256)
        textureHidden[idx] = hidden;

    if(refresh)
    {
        rebuildView();
        update();
    }
};

QSize TextureViewGL::getVirtualDimensions()
{
    int maxX = 0;
    int maxY = 0;
    for(unsigned int i = 0; i < texturePositions.size(); i++)
    {
        if(texturePositions[i].getGridX() > maxX)
        maxX = texturePositions[i].getGridX();
        if(texturePositions[i].getGridY() > maxY)
        maxY = texturePositions[i].getGridY();
    }
    maxX++;
    maxY++;
    return QSize(10+maxX*(textureSize+10),10+maxY*(textureSize+10));
};

void TextureViewGL::setDisplacement(int x, int y)
{
    xDisplacement = x;
    yDisplacement = y;
    update();
};

void TextureViewGL::setSelectedTexture(int idx)
{
    selectedTexture = idx;
    findSelectedPositioner();
    update();
};

void TextureViewGL::setSelectedPalette(int idx)
{
    selectedPalette = idx;
    findSelectedPositioner();
    update();
};

void TextureViewGL::setTextureSize(int size)
{
    textureSize = size;
    rebuildView();
    update();
};

int TextureViewGL::getSelectedPalette() const
{
    return selectedPalette;
};

int TextureViewGL::getSelectedTexture() const
{
    return selectedTexture;
};

void TextureViewGL::setViewMode(int mode)
{
    viewMode = mode;
    rebuildView();
    update();
};

QRect TextureViewGL::getSelectionRect()
{
    for(unsigned int i = 0; i < texturePositions.size(); i++)
    {
        if(texturePositions[i].getTextureIndex() == selectedTexture && (texturePositions[i].getPaletteIndex() == selectedPalette || texturePositions[i].getPaletteIndex() == -1))
        {
            return QRect(10+texturePositions[i].getGridX()*(textureSize+10),10+texturePositions[i].getGridY()*(textureSize+10),textureSize, textureSize);
        }
    }
    return QRect(10,10,0,0);
};

bool TextureViewGL::textureAt(const QPoint& point, int* texture, int* palette) const
{
    if(texture)
    *texture = 0;
    if(palette)
    *palette = 0;

    int x = point.x()+xDisplacement;
    int y = point.y()+yDisplacement;

    if(x < 10 || y < 10)
    return false;

    x -= 10;
    y -= 10;
    if(x%(textureSize+10)<textureSize && y%(textureSize+10)<textureSize)
    {
        int gridX = x/(textureSize+10);
        int gridY = y/(textureSize+10);
        for(unsigned int i = 0; i < texturePositions.size(); i++)
        {
            if(texturePositions[i].getGridX() == gridX && texturePositions[i].getGridY() == gridY)
            {
                if(texture)
                *texture = texturePositions[i].getTextureIndex();
                if(palette)
                *palette = texturePositions[i].getPaletteIndex();
                return true;
            }
        }
    }
    return false;
};

//Level event handlers

void TextureViewGL::textureRemoved(int idx)
{
    rebuildView();
};

void TextureViewGL::textureChanged(int /*idx*/)
{
    //Number of textures has not changed, only appearance, so update screen.
    update();
};

void TextureViewGL::textureMoved(int /*from*/, int /*to*/)
{
    rebuildView();
};

void TextureViewGL::textureInserted(int idx)
{
    rebuildView();
};

void TextureViewGL::texturesDestroyed()
{
    if(textures)
        textures->unregisterEventHandler(this);
    textures = NULL;
    rebuildView();
};

void TextureViewGL::texturesReset(bool aboutToBe)
{
    if(!aboutToBe)
    rebuildView();
};

void TextureViewGL::texturesOpened()
{
    rebuildView();
};

TextureView::TextureView(QWidget* parent, const QGLWidget * shareWidget, Qt::WindowFlags f) : QAbstractScrollArea(parent)
{
    glView = new TextureViewGL(this,shareWidget,f);
    setup();
};

TextureView::TextureView(const QGLFormat& format,QWidget* parent, const QGLWidget * shareWidget, Qt::WindowFlags f) : QAbstractScrollArea(parent)
{
    glView = new TextureViewGL(format,this,shareWidget,f);
    setup();
};

TextureView::~TextureView()
{

};

void TextureView::setup()
{
    glView->show();
    glView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //Required to allow for control-scroll resizing in texture browser. Does not affect scrolling or clicking.
    setFocusPolicy(Qt::NoFocus);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(glView);
    layout->setContentsMargins(0,0,0,0);

    viewportWidget = new QWidget(this);
    viewportWidget->setContentsMargins(0,0,0,0);
    viewportWidget->setLayout(layout);
    locked = false;

    setViewport(viewportWidget);
    QSize minGL = glView->minimumSizeHint();
    setMinimumSize(QSize(minGL.width()+contentsMargins().left()+contentsMargins().right()+verticalScrollBar()->height(),minGL.height()+contentsMargins().top()+contentsMargins().bottom()));

    connect(glView, SIGNAL(sizeHasChanged()), this, SLOT(reconfigureScrollbars()));
    connect(glView, SIGNAL(textureSelectionChanged(int)), this, SLOT(ensureSelectionVisible()));
    connect(glView, SIGNAL(paletteSelectionChanged(int)), this, SLOT(ensureSelectionVisible()));
    connect(glView, SIGNAL(sizeHasChanged()), this, SLOT(ensureSelectionVisible()));
};

void TextureView::scrollContentsBy(int /*dx*/, int /*dy*/)
{
    updateViewerPosition();
};

void TextureView::updateStep()
{
    verticalScrollBar()->setSingleStep(glView->getVerticalScrollStep());
    horizontalScrollBar()->setSingleStep(glView->getHorizontalScrollStep());
};

void TextureView::updateViewerPosition()
{
    glView->setDisplacement(horizontalScrollBar()->value(),verticalScrollBar()->value());
};

void TextureView::setLocked(bool lock)
{
    locked = lock;
};

void TextureView::wheelEvent(QWheelEvent* event)
{
    //Intercept wheel event, and if locked, do not pass on to scrollbars.
    if(!locked)
        QAbstractScrollArea::wheelEvent(event);
};

void TextureView::reconfigureScrollbars()
{
    QSize areaSize = glView->size();
    QSize widgetSize = glView->getVirtualDimensions();

    verticalScrollBar()->setPageStep(areaSize.height());
    horizontalScrollBar()->setPageStep(areaSize.width());
    verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
    horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
    updateStep();
    updateViewerPosition();
};

TextureViewGL* TextureView::viewer()
{
    return glView;
};

void TextureView::ensureSelectionVisible()
{
    QRect r = glView->getSelectionRect();
    if(r.right() > horizontalScrollBar()->value()+horizontalScrollBar()->pageStep())
    horizontalScrollBar()->setValue(r.right()-horizontalScrollBar()->pageStep()+10);
    if(r.left() < horizontalScrollBar()->value())
    horizontalScrollBar()->setValue(r.left()-10);

    if(r.bottom() > verticalScrollBar()->value()+verticalScrollBar()->pageStep())
    verticalScrollBar()->setValue(r.bottom()-verticalScrollBar()->pageStep()+10);
    if(r.top() < verticalScrollBar()->value())
    verticalScrollBar()->setValue(r.top()-10);
};

void TextureView::setSelectedTexture(int idx)
{
    glView->setSelectedTexture(idx);
    ensureSelectionVisible();
};

void TextureView::setSelectedPalette(int idx)
{
    glView->setSelectedPalette(idx);
    ensureSelectionVisible();
};

void TextureView::setTextureData(DriverTextures* newTexData)
{
    glView->setTextureData(newTexData);
};

void TextureView::setGLTextureProvider(TextureList* newTexList)
{
    glView->setGLTextureProvider(newTexList);
};

bool TextureView::textureAt(const QPoint& point, int* texture, int* palette) const
{
    return glView->textureAt(point, texture, palette);
};
