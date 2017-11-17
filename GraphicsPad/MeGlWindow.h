#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <gl\glew.h>
#include <QtOpenGL\qglwidget>
#include <string>

class MeGlWindow : public QGLWidget
{

protected:
	void sendDataToOpenGL();
	std::string readShaderCode(const char* fileName);
	void installShaders();
	void MeGlWindow::initialTexture();
	void MeGlWindow::initialCubeMap();
	void initializeGL();
	void paintGL();
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
	static const char* CubeMapFile[];
	void renderCamera(Camera &camera);
public:
};

#endif
