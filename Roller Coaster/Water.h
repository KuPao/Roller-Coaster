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
#include <QPoint>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


class Water {
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector3D> vertices1;
	QVector<QVector2D> uvs;
	QVector<QVector3D> normals;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	QOpenGLBuffer nvbo;

	int waveType = 0;
	float scale = 400;

	int numWaves;
	float time = 0;
	float amplitude[8];
	float wavelength[8];
	float speed[8];
	QVector2D direction[8];

	bool reverse = false;
	QImage heightmaps[200];

	GLuint FramebufferName = 0;
	GLuint refractFBO = 1;
	QVector3D camPos;
	GLfloat projMat[4][4];
	GLuint reflectTexture;
	GLuint refractTexture;
	float refractAngle;
	QVector3D refractStart;
	QVector3D refractNow;
		 
public:
	Water();
	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void SetWaterParameter();
	void LoadHeightmaps();
	float CalHeightByMap(float x, float z, float time);
	void GenTexture(int width, int height);
	QVector3D CalNormalByThreePts(QVector3D a, QVector3D b, QVector3D c);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);
	void End();
	void CalProjectorMatrix(float aspect);
};