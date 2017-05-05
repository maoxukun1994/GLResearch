#include "glwindow.h"

GLWindow::GLWindow(QWindow * parent) : QWindow(parent),GLBase()
{
    //set surface type for openGL drawing
    this->setSurfaceType(SurfaceType::OpenGLSurface);

    //set initvalues for virables
    m_pContext = Q_NULLPTR;
    m_pScene = Q_NULLPTR;

    m_frameCount= 0;
    m_isGLInitialized = false;
    m_isFullScreen = false;
    m_update_pending = false;
    m_mouseX = m_mouseY = 0;
    m_isMousePressed = false;
}

GLWindow::~GLWindow()
{
    if(m_pScene != Q_NULLPTR)
    {
        delete m_pScene;
    }
    //Note the context should always be deleted last.
    if(m_pContext != Q_NULLPTR)
    {
        //we can't delete it instantly,cause the context will be used by GLBase deconstructor to delete function pointers.
        m_pContext->deleteLater();
    }
    qDebug("GLWindow : %d frames drawed.",m_frameCount);
}

void GLWindow::exposeEvent(QExposeEvent * pEvent)
{
    Q_UNUSED(pEvent);

    //initGL
    if(!this->m_isGLInitialized)
    {
        this->initializeGL();

#ifdef PLATFORM_GL_ES
        this->showFullScreen();
        this->m_isFullScreen = true;
#endif

    }
    else
    {
        this->paintGLLater();
    }
}

bool GLWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        this->paintGL();
        return true;
    default:
        return QWindow::event(event);
    }
}


void GLWindow::initializeGL()
{
    if(this->m_isGLInitialized)
    {
        return;
    }
    if (m_pContext == Q_NULLPTR)
    {
        //create a context
        m_pContext = new QOpenGLContext(this);

        m_pContext->setFormat(this->requestedFormat());

        if(m_pContext->create() && m_pContext->makeCurrent( this ))
        {
            qDebug("Context created.");
        }
        else
        {
            qFatal("GLWindow : Context failed to create.");

            return;
        }
    }
    //do check
    if(m_pContext->isValid())
    {
        //if we can get the valid openGL context,do the base init and check
        this->checkAndInit();

        //connect resize event to resizeGL() call
        connect(this,SIGNAL(heightChanged(int)),this,SLOT(resized()));
        connect(this,SIGNAL(widthChanged(int)),this,SLOT(resized()));

        //after all initialization work is done
        this->m_isGLInitialized = true;

        //additional info about GL context
        qDebug("Context depth buffer size : %d",m_pContext->format().depthBufferSize());
        qDebug("Context sample size : %d",m_pContext->format().samples());
        qDebug("Context vsync status : %d",m_pContext->format().swapInterval());
        qDebug("Context is GLES : %d",m_pContext->isOpenGLES());
        qDebug("Context GL version : %d.%d",m_pContext->format().version().first,m_pContext->format().version().second);

        //print additional message about GL
#ifndef PLATFORM_GL_ES
        GLint max_texunit;
        gl_ptr->glGetIntegerv(GL_MAX_TEXTURE_UNITS,&max_texunit);
        qDebug("GL Max texture units : %d",(int)max_texunit);
#endif
        qDebug("GL Vendor : %s",(const char *)gl_ptr->glGetString(GL_VENDOR));
        qDebug("GL Renderer : %s",(const char *)gl_ptr->glGetString(GL_RENDERER));
        qDebug("GL driver version : %s",(const char *)gl_ptr->glGetString(GL_VERSION));
    }
    else
    {
        qFatal("GLWindow : GL context not valid,initialization failed.");
    }

    //if initialized,then call resizeGL to set viewport and paintGL to paintfirst frame
    if(this->m_isGLInitialized)
    {
        this->setHeight(480);

        this->setWidth(800);

        //set GL status
        gl_ptr->glClearColor(0.1,0.1,0.15,1.0);
        gl_ptr->glDepthFunc(GL_LEQUAL);
        gl_ptr->glEnable(GL_DEPTH_TEST);
        gl_ptr->glCullFace(GL_BACK);
        gl_ptr->glEnable(GL_CULL_FACE);

        //initialize scene
        m_pScene = new GLScene();
        m_pScene->sceneInit();
        m_pScene->changeDrawProperty(this->width(),this->height());

        //paint first frame
        this->paintGL();
    }
}

void GLWindow::resizeGL(int w, int h)
{
    //resize opengl viewport

    if(!m_isGLInitialized)
    {
        qWarning("resizeGL called when GL is not initialized.");
        return;
    }

    //change glViewPort by Aspect ratio
    int pw,ph;
    if((float)w / (float)h > DRAW_ASPECT_RATIO)
    {
        pw = (int)( (float)h * DRAW_ASPECT_RATIO );
        ph = h;
        gl_ptr->glViewport((w-pw)/2,0,pw,ph);
    }
    else
    {
        pw = w;
        ph = (int)( (float)w / DRAW_ASPECT_RATIO );
        gl_ptr->glViewport(0,(h-ph)/2,pw,ph);
    }

    //pass changes to Scene
    m_pScene->changeDrawProperty(w,h);

    qDebug("GLWindow : GL viewport resized to : %d x %d",w,h);
}

void GLWindow::paintGL()
{
    if(!m_isGLInitialized) return;

    m_pContext->makeCurrent(this);

    gl_ptr->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_pScene->sceneUpdate();

    m_pContext->swapBuffers(this);

    m_frameCount++;

    this->paintGLLater();
}

void GLWindow::paintGLLater()
{
    if (!m_update_pending)
    {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

void GLWindow::resized()
{
    this->resizeGL(this->width(),this->height());
}

void GLWindow::keyPressEvent(QKeyEvent * pEvent)
{
    switch ( pEvent->key() )
    {
    case Qt::Key_F11:
        //toggle full screen
        if(this->m_isFullScreen)
        {
            this->showNormal();
            this->m_isFullScreen = false;
        }
        else
        {
            this->showFullScreen();
            this->m_isFullScreen = true;
        }
        break;
    case Qt::Key_W:
        m_pScene->cameramovingF = true;
        break;
    case Qt::Key_S:
        m_pScene->cameramovingB = true;
        break;
    case Qt::Key_A:
        m_pScene->cameramovingL = true;
        break;
    case Qt::Key_D:
        m_pScene->cameramovingR = true;
        break;
    case Qt::Key_Shift:
        m_pScene->cameramovingU = true;
        break;
    case Qt::Key_Control:
        m_pScene->cameramovingD = true;
        break;
    }
}

void GLWindow::keyReleaseEvent(QKeyEvent * pEvent)
{
    switch ( pEvent->key() )
    {
    case Qt::Key_W:
        m_pScene->cameramovingF = false;
        break;
    case Qt::Key_S:
        m_pScene->cameramovingB = false;
        break;
    case Qt::Key_A:
        m_pScene->cameramovingL = false;
        break;
    case Qt::Key_D:
        m_pScene->cameramovingR = false;
        break;
    case Qt::Key_Shift:
        m_pScene->cameramovingU = false;
        break;
    case Qt::Key_Control:
        m_pScene->cameramovingD = false;
        break;
    }
}

void GLWindow::mousePressEvent(QMouseEvent * pEvent)
{
    Q_UNUSED(pEvent);
    this->m_isMousePressed = true;

    int x = pEvent->x();
    int y = pEvent->y();

    if(x<50 && y <50) m_pScene->cameramovingU = true;
    if(x<50 && y > this->height()-50) m_pScene->cameramovingD = true;
    if(x > this->width()-50 && y < 50) m_pScene->cameramovingF = true;
    if(x > this->width()-50 && y > this->height()-50) m_pScene->cameramovingB =true;

}

void GLWindow::mouseReleaseEvent(QMouseEvent * pEvent)
{
    Q_UNUSED(pEvent);
    this->m_isMousePressed = false;

    int x = pEvent->x();
    int y = pEvent->y();
    if(x<50 && y <50) m_pScene->cameramovingU = false;
    if(x<50 && y > this->height()-50) m_pScene->cameramovingD = false;
    if(x > this->width()-50 && y < 50) m_pScene->cameramovingF = false;
    if(x > this->width()-50 && y > this->height()-50) m_pScene->cameramovingB =false;
}

void GLWindow::mouseMoveEvent(QMouseEvent * pEvent)
{
    int mx,my,dx,dy;
    mx = pEvent->x();
    my = pEvent->y();

    dx = mx - m_mouseX;
    dy = my -m_mouseY;

    if( m_pScene != Q_NULLPTR && m_isMousePressed)
    {
        m_pScene->camera.rotateHV(-0.2*dx,0.2*dy);
    }

    m_mouseX = mx;
    m_mouseY = my;
}
