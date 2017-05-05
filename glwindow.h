#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "glbase.h"
#include "glscene.h"

#include <QSize>
#include <QDebug>
#include <QWindow>
#include <QKeyEvent>
#include <QBasicTimer>
#include <QMouseEvent>
#include <QSurfaceFormat>
#include <QCoreApplication>


class GLWindow : public QWindow,protected GLBase
{
    Q_OBJECT

public:
    //public virables

private:
    //private virables

    //pointer type
    QOpenGLContext * m_pContext;

    GLScene * m_pScene;

    //non-pointer type
    int m_frameCount,m_mouseX,m_mouseY;

    bool m_isGLInitialized;

    bool m_isFullScreen;

    bool m_update_pending;

    bool m_isMousePressed;

public:
    //public functions

    GLWindow(QWindow * parent = Q_NULLPTR);

    ~GLWindow();

protected:
    //protected functions

    bool event(QEvent *event) Q_DECL_OVERRIDE;

    void exposeEvent(QExposeEvent * pEvent) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent * pEvent) Q_DECL_OVERRIDE;

    void keyReleaseEvent(QKeyEvent * pEvent) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent * pEvent) Q_DECL_OVERRIDE;

    void mouseMoveEvent(QMouseEvent * pEvent) Q_DECL_OVERRIDE;

    void mouseReleaseEvent(QMouseEvent * pEvent) Q_DECL_OVERRIDE;


private:
    //private functions

    void initializeGL();

    void resizeGL(int w,int h);

    void paintGL();

    void paintGLLater();

private slots:
    //private slots (callback functions)

    void resized();

};

#endif // GLWINDOW_H
