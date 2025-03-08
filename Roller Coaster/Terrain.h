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

class Terrain {
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QOpenGLShader* controlShader;
	QOpenGLShader* evaluationShader;
	QVector<QVector3D> vertices;
	QVector<QVector2D> uvs;
	QVector<QVector3D> normals;
	QVector<QVector3D> vertices1;
	QVector<QVector2D> uvs1;
	QVector<QVector3D> normals1;
	QVector<QVector3D> vertices2;
	QVector<QVector2D> uvs2;
	QVector<QVector3D> normals2;
	QVector<QVector3D> vertices3;
	QVector<QVector2D> uvs3;
	QVector<QVector3D> normals3;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	QOpenGLBuffer nvbo;
	int terrainType = 0;

	std::vector<glm::vec3> objVertices, objNormals;
	std::vector<glm::vec2> objUvs;
	std::vector<unsigned int> objFaces;//face count;
	std::vector<std::string> objMtls;//use material

	std::vector<glm::vec3> kas, kds, kss;
	std::vector<std::string> Materials/*mtl-name*/, Textures/*texture file*/;

	std::vector<glm::vec3> objVertices1, objNormals1;
	std::vector<glm::vec2> objUvs1;
	std::vector<unsigned int> objFaces1;//face count;
	std::vector<std::string> objMtls1;//use material

	std::vector<glm::vec3> kas1, kds1, kss1;
	std::vector<std::string> Materials1/*mtl-name*/, Textures1/*texture file*/;

	std::vector<glm::vec3> objVertices2, objNormals2;
	std::vector<glm::vec2> objUvs2;
	std::vector<unsigned int> objFaces2;//face count;
	std::vector<std::string> objMtls2;//use material

	std::vector<glm::vec3> kas2, kds2, kss2;
	std::vector<std::string> Materials2/*mtl-name*/, Textures2/*texture file*/;

	std::vector<glm::vec3> objVertices3, objNormals3;
	std::vector<glm::vec2> objUvs3;
	std::vector<unsigned int> objFaces3;//face count;
	std::vector<std::string> objMtls3;//use material

	std::vector<glm::vec3> kas3, kds3, kss3;
	std::vector<std::string> Materials3/*mtl-name*/, Textures3/*texture file*/;

public:
	Terrain();
	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath, QString tcsPath, QString tesPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);
	void End();

};