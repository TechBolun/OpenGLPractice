#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>
#include <QtGui\qkeyevent>

class MeGlWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void MeGlWindow::keyPressEvent(QKeyEvent* e);

public:
};

#endif
