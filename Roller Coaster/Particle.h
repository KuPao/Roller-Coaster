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
#include <time.h>
#include <math.h>


class Particle
{
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector2D> uvs;
	QVector<QVector3D> velocitys;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	

	//particle attribute
	const float gravity = -50;
	QVector3D position;
	//QVector3D velocity;
	float gravityEffect;
	float lifeLength;
	float rotation;
	float scale;
	clock_t start, lastFrame;
	
	float elaspedTime = 0;
	bool alive = true;
	
	//particle system
	int nParticles;
	QOpenGLBuffer velocityBuffer;
public:
	Particle();
	Particle(QVector3D position, QVector3D velocity, float gravityEffect, float lifeLength, float rotation, float scale);
	void Init();
	void InitVAO();
	void InitVBO();
	void InitVelocity();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);
	void End();
	
	float randFloat(float min, float max);
};

class ParticleSystem
{
public:
	QVector<Particle> particles;

	int nParticles;

public:
	void Init();
	void Begin();
	void Paint();
	void End();
};