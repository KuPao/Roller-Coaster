#pragma once
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL/QtOpenGL> 
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QFileInfo>
#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>
#include <QString>
#include "ObjLoader.h"

class MovingObject {
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector2D> uvs;
	QVector<QVector3D> normals;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	QOpenGLBuffer nvbo;

	float time = 0;
	float scale = 1;

	std::vector<glm::vec3> objVertices, objNormals;
	std::vector<glm::vec2> objUvs;
	std::vector<unsigned int> objFaces;//face count;
	std::vector<std::string> objMtls;//use material

	std::vector<glm::vec3> kas, kds, kss;
	std::vector<std::string> Materials/*mtl-name*/, Textures/*texture file*/;

public:
	MovingObject();
	void Init(int type);
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);
	void End();

};