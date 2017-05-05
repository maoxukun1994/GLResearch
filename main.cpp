#include <QApplication>
#include <QSurfaceFormat>

#include "glwindow.h"

int main(int argc, char *argv[])
{
    //Request a format
    QSurfaceFormat glFormat;
    glFormat.setDepthBufferSize(24);
    glFormat.setStencilBufferSize(8);
    glFormat.setSamples(4);
    glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    glFormat.setSwapInterval(1);
    glFormat.setMajorVersion(3);
    glFormat.setMinorVersion(3);
    glFormat.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
    glFormat.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    glFormat.setVersion(3,3);
    QSurfaceFormat::setDefaultFormat(glFormat);

    QApplication a(argc, argv);

    GLWindow w;

    w.show();

    return a.exec();
}
