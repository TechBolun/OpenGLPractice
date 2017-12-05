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
const uint NUM_FLOATS_PER_VERTICE = 15;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

GLuint programID;
GLuint CubeMapPorgramID;
GLuint reflectCubemapProgramID;
GLuint planeTextureProgramID;
GLuint shadowProgramID;

GLuint cubeNumIndices;
GLuint sphereNumIndices;
GLuint planeNumIndices;

Camera camera;
Camera LightCamera; //move the camera to the light dir in order to get light map

GLuint cubeVertexArrayObjectID;
GLuint sphereVertexArrayObjectID;
GLuint planeVertexArrayObjectID;

GLuint cubeSizeOfVertexs;
GLuint sphereSizeOfVertexs;
GLuint planeSizeOfVertexs;

glm::vec3 lightPosition(0.0f, 3.0f, 0.0f);

GLfloat fresnelScale = 0.5f;

const char* MeGlWindow::CubeMapFile[] = { "Textures/CloudsWaterRight.png","Textures/CloudsWaterLeft.png","Textures/CloudsWaterDown.png",
										"Textures/CloudsWaterUp.png","Textures/CloudsWaterBack.png","Textures/CloudsWaterFront.png" };

void MeGlWindow::sendDataToOpenGL()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData sphere = ShapeGenerator::makeSphere(20);
	ShapeData plane = ShapeGenerator::makePlane(20);

	GLuint theBufferID;

	glGenBuffers(1, &theBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glBufferData(GL_ARRAY_BUFFER, 
		cube.vertexBufferSize() + cube.indexBufferSize() +
		sphere.vertexBufferSize() + sphere.indexBufferSize() +
		plane.vertexBufferSize() + plane.indexBufferSize(), 0, GL_STATIC_DRAW);
	GLsizeiptr currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.vertexBufferSize(), cube.vertices);
	currentOffset += cube.vertexBufferSize();
	cubeSizeOfVertexs = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.indexBufferSize(), cube.indices);
	currentOffset += cube.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	sphereSizeOfVertexs = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
	currentOffset += sphere.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.vertexBufferSize(), plane.vertices);
	currentOffset += plane.vertexBufferSize();
	planeSizeOfVertexs = currentOffset;
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
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 9));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 11));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	glBindVertexArray(sphereVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint sphereByteOffset = cube.vertexBufferSize() + cube.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)sphereByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 6));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 9));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 11));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	glBindVertexArray(planeVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	GLuint planeByteOffset = sphereByteOffset + sphere.vertexBufferSize() + sphere.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)planeByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 6));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 9));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 11));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

	cube.cleanup();
	sphere.cleanup();
	plane.cleanup();
}

void MeGlWindow::DrawObjects(Camera &camera) {

	mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	mat4 viewToProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 150.0f);
	mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;
	GLuint fullTransformationUniformLocation = glGetUniformLocation(programID, "modelToProjectionMatrix");;
	GLuint modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");

	glUseProgram(programID);

	//ambient light
	//vec3 ambientLight(0.3f, 0.3f, 0.3f);
	vec3 ambientLight(1.0f, 1.0f, 1.0f);
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	//point light
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPosition");
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	//fresnel
	GLint fresnelScaleUniformLocation = glGetUniformLocation(programID, "fresnelScale");
	glUniform1f(fresnelScaleUniformLocation, fresnelScale);

	//get camera position 
	GLint cameraPositionUniformLocation = glGetUniformLocation(programID, "cameraPosition");
	glm::vec3 cameraPosition = camera.getPosition();
	glUniform3fv(cameraPositionUniformLocation, 1, &cameraPosition[0]);

	//Diffuse initial
	GLint diffuseMap = glGetUniformLocation(programID, "myTexture");
	glUniform1i(diffuseMap, 0);
	//Normal initial
	GLint normalMap = glGetUniformLocation(programID, "meNormal");
	glUniform1i(normalMap, 1);

	GLint CubeMap = glGetUniformLocation(programID, "meCubeMap");
	glUniform1i(CubeMap, 2);

	//cube 1
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cube1ModelToWorldMatrix =
		glm::translate(vec3(-5.0f, 0.0f, -2.0f));
	mat4 modelToProjectionMatrix = worldToProjectionMatrix * cube1ModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&cube1ModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeOfVertexs);

	//cube 2
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cube2ModelToWorldMatrix =
		glm::translate(vec3(3.0f, 1.0f, -0.75f)) *
		glm::rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
	modelToProjectionMatrix = worldToProjectionMatrix * cube2ModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&cube2ModelToWorldMatrix[0][0]); 
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeOfVertexs);

	// Sphere
	glBindVertexArray(sphereVertexArrayObjectID);
	mat4 sphereModelToWorldMatrix = glm::translate(-3.0f, 3.5f, 0.0f) * glm::scale(0.5f, 0.5f, 0.5f);
	modelToProjectionMatrix = worldToProjectionMatrix * sphereModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&sphereModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereSizeOfVertexs);

	// Plane
	glBindVertexArray(planeVertexArrayObjectID);
	mat4 planeModelToWorldMatrix = glm::mat4();
	modelToProjectionMatrix = worldToProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&planeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeSizeOfVertexs);

	//CubeMap Cube
	glUseProgram(CubeMapPorgramID);

	//CubeMap initial
	CubeMap = glGetUniformLocation(CubeMapPorgramID, "meCubeMap");
	glUniform1i(CubeMap, 2);

	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cubemapModelToWorldMatrix = glm::scale(70.0f, 70.0f, 70.0f);

	worldToViewMatrix[3][0] = 0.0;
	worldToViewMatrix[3][1] = 0.0;
	worldToViewMatrix[3][2] = 0.0;
	GLuint SkyboxTransformMatrixUniformLocation = glGetUniformLocation(CubeMapPorgramID, "skyboxTransformMatrix");
	mat4 skyboxTransformMatrix = viewToProjectionMatrix * worldToViewMatrix * cubemapModelToWorldMatrix;
	glUniformMatrix4fv(SkyboxTransformMatrixUniformLocation, 1, GL_FALSE, &skyboxTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeOfVertexs);

	//reflect cubemap 
	glUseProgram(reflectCubemapProgramID);

	GLuint reflectTransformMatrixUniformLocation = glGetUniformLocation(reflectCubemapProgramID, "reflectionTransformMatrix");
	GLuint reflectModelToWorldMatrixUniformLocation = glGetUniformLocation(reflectCubemapProgramID, "reflectionModelToWorldMatrix");

	//get camera position 
	cameraPositionUniformLocation = glGetUniformLocation(reflectCubemapProgramID, "eyePosition");
	cameraPosition = camera.getPosition();
	glUniform3fv(cameraPositionUniformLocation, 1, &cameraPosition[0]);

	CubeMap = glGetUniformLocation(reflectCubemapProgramID, "meCubeMap");
	glUniform1i(CubeMap, 2);

	//cube 3 Light Placeholder
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 LightHolderModelToWorldMatrix =
		glm::translate(lightPosition) *
		glm::scale(0.05f, 0.05f, 0.05f);

	mat4 reflectionTransformMatrix = worldToProjectionMatrix * LightHolderModelToWorldMatrix;
	glUniformMatrix4fv(reflectTransformMatrixUniformLocation, 1, GL_FALSE, &reflectionTransformMatrix[0][0]);
	glUniformMatrix4fv(reflectModelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&LightHolderModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeOfVertexs);

	// Sphere
	glBindVertexArray(sphereVertexArrayObjectID);
	mat4 ReflectSphereModelToWorldMatrix = 
			glm::translate(0.0f, 5.0f, 0.0f) * 
			glm::scale(0.5f, 0.5f, 0.5f);
	reflectionTransformMatrix = worldToProjectionMatrix * ReflectSphereModelToWorldMatrix;
	glUniformMatrix4fv(reflectTransformMatrixUniformLocation, 1, GL_FALSE, &reflectionTransformMatrix[0][0]);
	glUniformMatrix4fv(reflectModelToWorldMatrixUniformLocation, 1, GL_FALSE,
		&ReflectSphereModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereSizeOfVertexs);

}

void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	DrawObjects(camera);
}



void MeGlWindow::initialTexture() {

	//diffuse
	QImage myDiffuseTexture = QGLWidget::convertToGLFormat(QImage("texture_pinky.png", "png"));

	glActiveTexture(GL_TEXTURE0);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, myDiffuseTexture.width(), myDiffuseTexture.width(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, myDiffuseTexture.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//create normal
	QImage myNormalTexture = QGLWidget::convertToGLFormat(QImage("Shapes.png", "png"));

	glActiveTexture(GL_TEXTURE1);
	GLuint normalID;
	glGenTextures(1, &normalID);
	glBindTexture(GL_TEXTURE_2D, normalID);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, myNormalTexture.width(), myNormalTexture.width(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, myNormalTexture.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void MeGlWindow::initialCubeMap() {
	glActiveTexture(GL_TEXTURE2);
	GLuint CubeMapID;
	glGenTextures(1, &CubeMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapID);
	for (int i = 0; i < 6; i++) {
		QImage CubeMapTexture = QGLWidget::convertToGLFormat(QImage(CubeMapFile[i], "PNG"));

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, CubeMapTexture.width(), CubeMapTexture.height(),
			0, GL_RGBA, GL_UNSIGNED_BYTE, CubeMapTexture.bits());
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void MeGlWindow::installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	//Simple Shader
	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);


	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// CubeMap Shader
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	temp = readShaderCode("CubeMapVertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("CubeMapFragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	CubeMapPorgramID = glCreateProgram();
	glAttachShader(CubeMapPorgramID, vertexShaderID);
	glAttachShader(CubeMapPorgramID, fragmentShaderID);

	glLinkProgram(CubeMapPorgramID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//reflection Shader
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	temp = readShaderCode("ReflectionVertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("ReflectionFragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	reflectCubemapProgramID = glCreateProgram();
	glAttachShader(reflectCubemapProgramID, vertexShaderID);
	glAttachShader(reflectCubemapProgramID, fragmentShaderID);

	glLinkProgram(reflectCubemapProgramID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

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
	case Qt::Key::Key_Escape:
		exit(1);
		break;
	case Qt::Key::Key_I:
		lightPosition += glm::vec3(0, -0.2, 0);
		break;
	case Qt::Key::Key_K:
		lightPosition += glm::vec3(0, 0.2, 0);
		break;
	case Qt::Key::Key_Left:
		lightPosition += glm::vec3(-0.2, 0, 0.0);
		break;
	case Qt::Key::Key_Right:
		lightPosition += glm::vec3(0.2, 0, 0.0);
		break;
	case Qt::Key::Key_Up:
		lightPosition += glm::vec3(0, 0, 0.2);
		break;
	case Qt::Key::Key_Down:
		lightPosition += glm::vec3(0, 0, -0.2);
	}
	repaint();
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

void MeGlWindow::initializeGL()
{
	setMouseTracking(true);
	glewInit();
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	sendDataToOpenGL();
	installShaders();
	initialTexture();
	initialCubeMap();
	
	//fullTransformationUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
}