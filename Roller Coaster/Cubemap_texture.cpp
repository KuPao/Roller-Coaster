#include "Cubemap_texture.h"

CubemapTexture::CubemapTexture(const std::string & Directory, const std::string & PosXFilename, const std::string & NegXFilename, const std::string & PosYFilename, const std::string & NegYFilename, const std::string & PosZFilename, const std::string & NegZFilename)
{
	m_fileNames[0] = Directory + "/" + PosXFilename;
	m_fileNames[1] = Directory + "/" + NegXFilename;
	m_fileNames[2] = Directory + "/" + PosYFilename;
	m_fileNames[3] = Directory + "/" + NegYFilename;
	m_fileNames[4] = Directory + "/" + PosZFilename;
	m_fileNames[5] = Directory + "/" + NegZFilename;
}

void CubemapTexture::Init()
{
	InitShader("./Shader/Skybox.vs", "./Shader/Skybox.fs");
	Load();
	InitVAO();
	InitVBO();
}

void CubemapTexture::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}

void CubemapTexture::InitVBO()
{
	float size = 400;
	//Set each vertex's position
	vertices << QVector3D(-1.0f,  1.0f, -1.0f) * size
			 << QVector3D(-1.0f, -1.0f, -1.0f) * size
			 << QVector3D( 1.0f, -1.0f, -1.0f) * size
			 << QVector3D( 1.0f, -1.0f, -1.0f) * size
			 << QVector3D( 1.0f,  1.0f, -1.0f) * size
			 << QVector3D(-1.0f,  1.0f, -1.0f) * size

			 << QVector3D(-1.0f, -1.0f,  1.0f) * size
			 << QVector3D(-1.0f, -1.0f, -1.0f) * size
			 << QVector3D(-1.0f,  1.0f, -1.0f) * size
			 << QVector3D(-1.0f,  1.0f, -1.0f) * size
			 << QVector3D(-1.0f,  1.0f,  1.0f) * size
		 	 << QVector3D(-1.0f, -1.0f,  1.0f) * size
		
		   	 << QVector3D( 1.0f, -1.0f, -1.0f) * size
			 << QVector3D( 1.0f, -1.0f,  1.0f) * size
			 << QVector3D( 1.0f,  1.0f,  1.0f) * size
			 << QVector3D( 1.0f,  1.0f,  1.0f) * size
			 << QVector3D( 1.0f,  1.0f, -1.0f) * size
			 << QVector3D( 1.0f, -1.0f, -1.0f) * size
		
			 << QVector3D(-1.0f, -1.0f,  1.0f) * size
			 << QVector3D(-1.0f,  1.0f,  1.0f) * size
			 << QVector3D( 1.0f,  1.0f,  1.0f) * size
			 << QVector3D( 1.0f,  1.0f,  1.0f) * size
			 << QVector3D( 1.0f, -1.0f,  1.0f) * size
			 << QVector3D(-1.0f, -1.0f,  1.0f) * size

			 << QVector3D(-1.0f,  1.0f, -1.0f) * size
			 << QVector3D( 1.0f,  1.0f, -1.0f) * size
			 << QVector3D( 1.0f,  1.0f,  1.0f) * size
			 << QVector3D( 1.0f,  1.0f,  1.0f) * size
			 << QVector3D(-1.0f,  1.0f,  1.0f) * size
			 << QVector3D(-1.0f,  1.0f, -1.0f) * size
		
			 << QVector3D(-1.0f, -1.0f, -1.0f) * size
			 << QVector3D(-1.0f, -1.0f,  1.0f) * size
			 << QVector3D( 1.0f, -1.0f, -1.0f) * size
			 << QVector3D( 1.0f, -1.0f, -1.0f) * size
			 << QVector3D(-1.0f, -1.0f,  1.0f) * size
			 << QVector3D( 1.0f, -1.0f,  1.0f) * size;
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	//Set each vertex's uv
	uvs << QVector3D(-1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f, -1.0f)

		/*<< QVector3D(1.0f, -1.0f,  -1.0f)
		<< QVector3D(1.0f, -1.0f, 1.0f)
		<< QVector3D(1.0f,  1.0f, 1.0f)
		<< QVector3D(1.0f,  1.0f, 1.0f)
		<< QVector3D(1.0f,  1.0f,  -1.0f)
		<< QVector3D(1.0f, -1.0f,  -1.0f)*/

		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D(-1.0f, -1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f,  1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)

		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f,  1.0f)
		<< QVector3D( 1.0f,  1.0f,  1.0f)
		<< QVector3D( 1.0f,  1.0f,  1.0f)
		<< QVector3D( 1.0f,  1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		//
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D(-1.0f,  1.0f,  1.0f)
		<< QVector3D( 1.0f,  1.0f,  1.0f)
		<< QVector3D( 1.0f,  1.0f,  1.0f)
		<< QVector3D( 1.0f, -1.0f,  1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		//
		<< QVector3D(1.0f,  1.0f, 1.0f)
		<< QVector3D( -1.0f,  1.0f, 1.0f)
		<< QVector3D( -1.0f,  1.0f,  -1.0f)
		<< QVector3D( -1.0f,  1.0f,  -1.0f)
		<< QVector3D(1.0f,  1.0f,  -1.0f)
		<< QVector3D(1.0f,  1.0f, 1.0f)

		<< QVector3D(-1.0f, -1.0f, -1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D( 1.0f, -1.0f,  1.0f);
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector3D));
}

void CubemapTexture::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void CubemapTexture::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void CubemapTexture::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}

void CubemapTexture::Paint(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix)
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

	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	uvbo.release();

	//Draw triangles with 4 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void CubemapTexture::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

bool CubemapTexture::Load()
{
	glGenTextures(1, &m_textureObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	QImage* pImage;

	for (unsigned int i = 0; i < 6; i++) {
		pImage = new QImage(QString(m_fileNames[i].c_str()), "RGBA");
		QImage t = pImage->mirrored();

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, pImage->width(), pImage->height(), 0, GL_RGBA, 
			GL_UNSIGNED_BYTE, pImage->bits());
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		delete pImage;
	}
	return true;
}

void CubemapTexture::Bind(GLenum TextureUnit)
{
	//glActiveTexture(TextureUnit);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}