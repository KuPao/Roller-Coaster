#include "MovingObject.h"

MovingObject::MovingObject()
{
}

void MovingObject::Init(int type)
{
	ObjLoader objLoader;
	switch (type) {
	case 0:
		InitShader("./Shader/eagle.vs", "./Shader/eagle.fs");
		objLoader.LoadObjFromFile("./Objs/Eagle02_sale.obj", this->objVertices, this->objUvs, this->objNormals, this->objFaces, this->objMtls);
		objLoader.loadMTL("./Objs/Eagle02_sale.mtl", this->kds, this->kas, this->kss, this->Materials, this->Textures);
		scale = 15;
		break;
	case 1:
		InitShader("./Shader/car.vs", "./Shader/car.fs");
		objLoader.LoadObjFromFile("./Objs/car.obj", this->objVertices, this->objUvs, this->objNormals, this->objFaces, this->objMtls);
		objLoader.loadMTL("./Objs/car.mtl", this->kds, this->kas, this->kss, this->Materials, this->Textures);
		scale = 3;
		break;
	case 2:
		InitShader("./Shader/ball.vs", "./Shader/ball.fs");
		objLoader.LoadObjFromFile("./Objs/ball.obj", this->objVertices, this->objUvs, this->objNormals, this->objFaces, this->objMtls);
		objLoader.loadMTL("./Objs/ball.mtl", this->kds, this->kas, this->kss, this->Materials, this->Textures);
		scale = 10;
		break;
	default:
		InitShader("./Shader/eagle.vs", "./Shader/eagle.fs");
		objLoader.LoadObjFromFile("Eagle02_sale.obj", this->objVertices, this->objUvs, this->objNormals, this->objFaces, this->objMtls);
		objLoader.loadMTL("Eagle02_sale.mtl", this->kds, this->kas, this->kss, this->Materials, this->Textures);
		break;
	}
	/*InitShader("./Shader/MovingObject.vs", "./Shader/MovingObject.fs");
	
	if (this->objVertices.size() == 0) {
		objLoader.LoadObjFromFile("MovingObject.obj", this->objVertices, this->objUvs, this->objNormals, this->objFaces, this->objMtls);
		objLoader.loadMTL("MovingObject.mtl", this->kds, this->kas, this->kss, this->Materials, this->Textures);
	}*/
	InitVAO();
	InitVBO();
}

void MovingObject::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}

void MovingObject::InitVBO()
{
	//Set each vertex's position
	for (int i = 0; i < objVertices.size(); i++) {
		vertices << QVector3D(objVertices[i].x * scale, objVertices[i].y * scale, objVertices[i].z * scale);
	}
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	//Set each vertex's uv
	for (int i = 0; i < objUvs.size(); i++) {
		uvs << QVector2D(objUvs[i].x, objUvs[i].y);
	}
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));

	//Set each vertex's position
	for (int i = 0; i < objNormals.size(); i++) {
		normals << QVector3D(objNormals[i].x, objNormals[i].y, objNormals[i].z);
	}
	// Create Buffer for position
	nvbo.create();
	// Bind the buffer so that it is the current active buffer
	nvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
}

void MovingObject::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void MovingObject::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void MovingObject::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}

void MovingObject::Paint(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("time", time);
	time += 0.005;
	if (time > 2 * 3.14159) {
		time = 0;
	}

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

	// Bind the buffer so that it is the current active buffer
	nvbo.bind();
	// Enable Attribute 2
	shaderProgram->enableAttributeArray(2);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	nvbo.release();

	shaderProgram->setUniformValue("Ka", kas[0].x, kas[0].y, kas[0].z);
	shaderProgram->setUniformValue("Kd", kds[0].x, kds[0].y, kds[0].z);
	shaderProgram->setUniformValue("Ks", kss[0].x, kss[0].y, kss[0].z);

	//Draw triangles with 4 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void MovingObject::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);

	vao.release();
	shaderProgram->release();
}
