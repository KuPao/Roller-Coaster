#include "Loader.h"

Loader::Loader()
{
}

void Loader::loadTexture(std::string fileName)
{
	QOpenGLTexture* texture = new QOpenGLTexture(QImage(fileName.c_str()));
	textures.push_back(texture);
	textureIDs.push_back(texture->textureId());
}

void Loader::cleanUp()
{
	/*for (int i = 0; i < vaos.size(); i++)
		vaos[i].release();
	for (int i = 0; i < vbos.size(); i++)
		vbos[i].release();*/
	for (int i = 0; i < textureIDs.size(); i++)
		glDeleteTextures(1, &textureIDs[i]);
}

GLuint Loader::loadCubeMap(std::vector<std::string> textureFiles)
{
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	for (int i = 1; i < textureFiles.size(); i++) {
		QImage texture = QImage(textureFiles[i].c_str()).mirrored().convertToFormat(QImage::Format_RGBA8888);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	textureIDs.push_back(texID);
	return texID;
}

void Loader::Init()
{
	InitShader("./Shader/Square.vs", "./Shader/Square.fs");
	InitVAO();
	InitVBO();
	//InitTexture();
}

void Loader::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}

void Loader::InitVBO()
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

void Loader::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void Loader::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void Loader::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}

void Loader::Paint(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

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

void Loader::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

