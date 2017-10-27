#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <QtGui\qmouseevent>
#include <QtGui\qkeyevent>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>
#include "Camera.h"
using namespace std;
using glm::vec3;
using glm::mat4;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
GLuint programID;
GLuint textureID;
GLuint cubeNumIndices;
GLuint sphereNumIndices;
GLuint planeNumIndices;
Camera camera;
GLuint fullTransformationUniformLocation;

GLuint theBufferID;

GLuint cubeVertexArrayObjectID;
GLuint sphereVertexArrayObjectID;
GLuint planeVertexArrayObjectID;
GLuint cubeIndexByteOffset;
GLuint sphereIndexByteOffset;
GLuint planeIndexByteOffset;

void MeGlWindow::sendDataToOpenGL()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData sphere = ShapeGenerator::makeSphere(20);
	ShapeData plane = ShapeGenerator::makePlane(20);

	glGenBuffers(1, &theBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glBufferData(GL_ARRAY_BUFFER, 
		cube.vertexBufferSize() + cube.indexBufferSize() +
		sphere.vertexBufferSize() + sphere.indexBufferSize() +
		plane.vertexBufferSize() + plane.indexBufferSize(), 0, GL_STATIC_DRAW);
	GLsizeiptr currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.vertexBufferSize(), cube.vertices);
	currentOffset += cube.vertexBufferSize();
	cubeIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.indexBufferSize(), cube.indices);
	currentOffset += cube.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	sphereIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
	currentOffset += sphere.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.vertexBufferSize(), plane.vertices);
	currentOffset += plane.vertexBufferSize();
	planeIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.indexBufferSize(), plane.indices);
	currentOffset += plane.indexBufferSize();

	cubeNumIndices = cube.numIndices;
	sphereNumIndices = sphere.numIndices;
	planeNumIndices = plane.numIndices;

	glGenVertexArrays(1, &cubeVertexArrayObjectID);
	glGenVertexArrays(1, &sphereVertexArrayObjectID);
	glGenVertexArrays(1, &planeVertexArrayObjectID);

	glBindVertexArray(cubeVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 9));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	glBindVertexArray(sphereVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint arrowByteOffset = cube.vertexBufferSize() + cube.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)arrowByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(arrowByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(arrowByteOffset + sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	glBindVertexArray(planeVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint planeByteOffset = arrowByteOffset + sphere.vertexBufferSize() + sphere.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)planeByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	cube.cleanup();
	sphere.cleanup();
	plane.cleanup();
}

void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	mat4 modelToProjectionMatrix;
	mat4 viewToProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 20.0f);
	mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;

	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;

	//ambient light
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	vec3 ambientLight(0.3f, 0.3f, 0.3f);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	//point light
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPosition");
	glm::vec3 lightPosition(0.0f, 5.0f, 0.0f);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	//get camera postion 
	GLint cameraPositionUniformLocation = glGetUniformLocation(programID, "cameraPosition");
	glm::vec3 cameraPosition = camera.getPosition();
	glUniform3fv(cameraPositionUniformLocation, 1, &cameraPosition[0]);

	//named the matrixs
	fullTransformationUniformLocation = glGetUniformLocation(programID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");


	//cube 1
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cube1ModelToWorldMatrix =
		glm::translate(vec3(-5.0f, 0.0f, -2.0f)) *
		glm::rotate(-65.0f, vec3(1.0f, 3.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * cube1ModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&cube1ModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeIndexByteOffset);


	//cube 2
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cube2ModelToWorldMatrix =
		glm::translate(vec3(3.0f, 1.0f, -0.75f)) *
		glm::rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * cube2ModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&cube2ModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeIndexByteOffset);

	// Sphere
	glBindVertexArray(sphereVertexArrayObjectID);
	mat4 sphereModelToWorldMatrix = glm::translate(2.0f, 1.5f, -3.0f);
	modelToProjectionMatrix = worldToProjectionMatrix * sphereModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&sphereModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);

	// Plane
	glBindVertexArray(planeVertexArrayObjectID);
	mat4 planeModelToWorldMatrix = glm::mat4();
	modelToProjectionMatrix = worldToProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&planeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexByteOffset);
}

void MeGlWindow::mouseMoveEvent(QMouseEvent* e)
{
	camera.mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void MeGlWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		camera.moveForward();
		break;
	case Qt::Key::Key_S:
		camera.moveBackward();
		break;
	case Qt::Key::Key_A:
		camera.strafeLeft();
		break;
	case Qt::Key::Key_D:
		camera.strafeRight();
		break;
	case Qt::Key::Key_R:
		camera.moveUp();
		break;
	case Qt::Key::Key_F:
		camera.moveDown();
		break;
	}
	repaint();
}

bool MeGlWindow::checkStatus(
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

bool MeGlWindow::checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool MeGlWindow::checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string MeGlWindow::readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
}

void MeGlWindow::initialTexture() {

	glEnable(GL_TEXTURE_2D);
	
	QImage myTexture = QGLWidget::convertToGLFormat(QImage("texture_pinky.png", "png"));
		// meImage.width(), meImage.height(), meImage.bits()

	GLint pinkyDiffuse = glGetUniformLocation(programID, "meTexture");


	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, myTexture.width(), myTexture.width(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, myTexture.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUniform1i(pinkyDiffuse, GL_TEXTURE0);

	glDisable(GL_TEXTURE_2D);
}

void MeGlWindow::installShaders()
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

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void MeGlWindow::initializeGL()
{
	setMouseTracking(true);
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	sendDataToOpenGL();
	initialTexture();
	installShaders();
	fullTransformationUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
}

MeGlWindow::~MeGlWindow()
{
	glDeleteBuffers(1, &theBufferID);
	glUseProgram(0);
	glDeleteProgram(programID);
}