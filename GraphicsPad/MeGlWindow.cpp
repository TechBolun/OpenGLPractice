#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <Primitives\Vertex.h>
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include "Camera.h"
using namespace std;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 6;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

GLuint programID;

float rotateValue;

GLuint numIndices;

glm::vec3 position = glm::vec3(+0.0f, +0.0f, -3.0f);
glm::vec3 scaleValue = glm::vec3(+2.0f, +2.0f, +2.0f);

glm::vec3 rotation = glm::vec3(+0.0f, +0.0f, +0.0f);

Camera camera;

GLuint vertexBufferID;

void sendDataToOpenGL()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData arrow = ShapeGenerator::makeArrow();
	ShapeData plane = ShapeGenerator::makePlane();

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize() + cube.indexBufferSize() 
		+ arrow.vertexBufferSize() + arrow.indexBufferSize()
		+ plane.vertexBufferSize() + plane.indexBufferSize(), 0, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));

	GLuint indexArrayBufferID;
	glGenBuffers(1, &indexArrayBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize(), cube.indices, GL_STATIC_DRAW);
	numIndices = cube.numIndices;
	cube.cleanup();
}


void MeGlWindow::paintGL()
{
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	//transformObj();
	glm::mat4 rotateObjX = glm::rotate(rotation.x, 1.0f, 0.0f, 0.0f);
	glm::mat4 rotateObjY = glm::rotate(rotation.y, 0.0f, 1.0f, 0.0f);
	glm::mat4 rotateObjZ = glm::rotate(rotation.z, 0.0f, 0.0f, 1.0f);
	glm::mat4 translateObj = glm::translate(position);
	glm::mat4 scaleObj = glm::scale(glm::mat4(1.0f), scaleValue);

	glm::mat4 projectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 20.0f);

	glm::mat4 transform = projectionMatrix * translateObj * rotateObjX * rotateObjY * rotateObjZ;

	GLint uniformLocation =
		glGetUniformLocation(programID, "transform");

	glUniformMatrix4fv(uniformLocation, 1,
		GL_FALSE, &transform[0][0]);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
}

void MeGlWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		position.y += 0.05f;
		break;
	case Qt::Key::Key_S:
		position.y -= 0.05f;
		break;
	case Qt::Key::Key_A:
		position.x -= 0.05f;
		break;
	case Qt::Key::Key_D:
		position.x += 0.05f;
		break;
	case Qt::Key::Key_J:
		rotation.x += 5;
		break;
	case Qt::Key::Key_L:
		rotation.x -= 5;
		break;
	case Qt::Key::Key_I:
		rotation.y += 5;
		break;
	case Qt::Key::Key_K:
		rotation.y -= 5;
		break;
	case Qt::Key::Key_U:
		rotation.z += 5;
		break;
	case Qt::Key::Key_O:
		rotation.z -= 5;
		break;
	case Qt::Key::Key_Z:
		scaleValue *= 2;
		break;
	case Qt::Key::Key_X:
		scaleValue /= 2;
		break;
	case Qt::Key::Key_Escape:
		exit(0);
		break;

	}
	repaint();
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if ( ! meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if( ! checkShaderStatus(vertexShaderID) || ! checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if ( ! checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void MeGlWindow::initializeGL()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);

	sendDataToOpenGL();
	installShaders();

}