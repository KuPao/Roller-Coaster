#include "Particle.h"
#include <iostream>

Particle::Particle()
{
}
Particle::Particle(QVector3D position, QVector3D velocity, float gravityEffect, float lifeLength, float rotation, float scale):
	position(position), /*velocity(velocity),*/ gravityEffect(gravityEffect), lifeLength(lifeLength), rotation(rotation), scale(scale)
{
}

void Particle::Init()
{
	InitShader("./Shader/Particle.vs", "./Shader/Particle.fs");
	InitVAO();
	InitVBO();
	//InitTexture();
}
void Particle::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Particle::InitVBO()
{
	//Set each vertex's position
	vertices << QVector3D(0.0f, 2.0f, -10.0f)
		<< QVector3D(0.0f, 2.0f, 10.0f)
		<< QVector3D(-20.0f, 2.0f, 10.0f)
		<< QVector3D(-20.0f, 2.0f, -10.0f);
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	//Set each vertex's uv
	uvs << QVector2D(0.0f, 0.0f)
		<< QVector2D(0.0f, 1.0f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(1.0f, .0f);
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));

}
void Particle::InitVelocity()
{
	QVector3D v;
	float velocity, theta, phi;
	for (int i = 0; i < nParticles; i++) {
		theta = randFloat(0.0f, (float)M_PI / 6.0f);
		phi = randFloat(0.0f, (float)M_PI * 2);

		v.setX(sinf(theta) * cosf(phi));
		v.setY(cosf(theta));
		v.setZ(sinf(theta) * sinf(phi));

		velocity = randFloat(1.25f, 1.5f);

		v = v * velocity;
		velocitys.push_back(v);
	}
	velocityBuffer.create();
	velocityBuffer.bind();
	velocityBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	velocityBuffer.allocate(velocitys.constData(), velocitys.size() * sizeof(QVector3D));
}
void Particle::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderProgram->link();
}

void Particle::DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j<4; j++)
		for (int k = 0; k<4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
}
void Particle::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();

	start = lastFrame = clock();
}
void Particle::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	/*double diff = ((double)(clock() - lastFrame)) / CLOCKS_PER_SEC;;
	velocity.y += gravity * gravityEffect * diff;
	QVector3D change = QVector3D(velocity);
	change *= diff;
	position = change + position;
	elaspedTime += diff;
	if (elaspedTime > lifeLength)
		alive = false;
	lastFrame = time(NULL);*/

	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Enable Attribute 1
	shaderProgram->enableAttributeArray(1);

	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 2, NULL);
	uvbo.release();

	//Draw triangles with 4 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
}
void Particle::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

float Particle::randFloat(float min, float max)
{
	srand(static_cast <unsigned> (time(0)));
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));;
}