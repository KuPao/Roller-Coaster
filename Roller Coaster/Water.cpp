#include "Water.h"

Water::Water()
{
}

void Water::Init()
{
	InitShader("./Shader/Water.vs", "./Shader/Water.fs");
	LoadHeightmaps();
	InitVAO();
	InitVBO();
	SetWaterParameter();
}

void Water::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}

void Water::InitVBO()
{
	float gap;
	if (waveType == 1)
		gap = 0.1;
	else
		gap = 0.01;
	//Set each vertex's position & uv
	for (float x = -1; x <= 0.99; x += gap) {
		for (float z = -1; z <= 0.99; z += gap) {
			if (waveType == 0) {
				vertices << QVector3D(x * scale, 0, z * scale)
					     << QVector3D(x * scale, 0, (z + gap) * scale)
						 << QVector3D((x + gap) * scale, 0, z * scale)
						 << QVector3D((x + gap) * scale, 0, z * scale)
						 << QVector3D(x * scale, 0, (z + gap) * scale)
						 << QVector3D((x + gap) * scale, 0, (z + gap) * scale);
			}
			else if (waveType == 1) {
				vertices << QVector3D(x * scale, CalHeightByMap(x / 2 + 0.5, z / 2 + 0.5, time), z * scale)
						 << QVector3D(x * scale, CalHeightByMap(x / 2 + 0.5, (z + gap) / 2 + 0.5, time), (z + gap) * scale)
						 << QVector3D((x + gap) * scale, CalHeightByMap((x + gap) / 2 + 0.5, z / 2 + 0.5, time), z * scale)
						 << QVector3D((x + gap) * scale, CalHeightByMap((x + gap) / 2 + 0.5, z / 2 + 0.5, time), z * scale)
						 << QVector3D(x * scale, CalHeightByMap(x / 2 + 0.5, (z + gap) / 2 + 0.5, time), (z + gap) * scale)
						 << QVector3D((x + gap) * scale, CalHeightByMap((x + gap) / 2 + 0.5, (z + gap) / 2 + 0.5, time), (z + gap) * scale);
			}
			else {
				vertices << QVector3D(x * scale, 0, z * scale)
						 << QVector3D(x * scale, 0, (z + gap) * scale)
						 << QVector3D((x + gap) * scale, 0, z * scale)
						 << QVector3D((x + gap) * scale, 0, z * scale)
						 << QVector3D(x * scale, 0, (z + gap) * scale)
						 << QVector3D((x + gap) * scale, 0, (z + gap) * scale);
			}
			
			uvs << QVector2D(x / 2 + 0.5, z / 2 + 0.5)
				<< QVector2D(x / 2 + 0.5, (z + gap) / 2 + 0.5)
				<< QVector2D((x + gap) / 2 + 0.5, z / 2 + 0.5)
				<< QVector2D((x + gap) / 2 + 0.5, z / 2 + 0.5)
				<< QVector2D(x / 2 + 0.5, (z + gap) / 2 + 0.5)
				<< QVector2D((x + gap) / 2 + 0.5, (z + gap) / 2 + 0.5);
		}
	}
	for (int i = 0; i < vertices.size(); i += 3) {
		QVector3D normal = CalNormalByThreePts(vertices[i], vertices[i + 1], vertices[i + 2]);
		normals << normal << normal << normal;
	}
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));

	// Create Buffer for position
	nvbo.create();
	// Bind the buffer so that it is the current active buffer
	nvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
}

void Water::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void Water::SetWaterParameter()
{
	numWaves = 8;
	time = 0;

	amplitude[0] = 1;
	amplitude[1] = 1;
	amplitude[2] = 1;
	amplitude[3] = 0.75;
	amplitude[4] = 2;
	amplitude[5] = 0.5;
	amplitude[6] = 1;
	amplitude[7] = 1;

	wavelength[0] = 121;
	wavelength[1] = 50;
	wavelength[2] = 90;
	wavelength[3] = 28;
	wavelength[4] = 50;
	wavelength[5] = 77;
	wavelength[6] = 211;
	wavelength[7] = 34;

	speed[0] = 1;
	speed[1] = 1;
	speed[2] = 2;
	speed[3] = 3;
	speed[4] = 1;
	speed[5] = 3;
	speed[6] = 2;
	speed[7] = 1;

	direction[0] = QVector2D(1, 0).normalized();
	direction[1] = QVector2D(5, 1).normalized();
	direction[2] = QVector2D(-1, 0).normalized();
	direction[3] = QVector2D(0, -1).normalized();
	direction[4] = QVector2D(1.2, 1).normalized();
	direction[5] = QVector2D(1, -1.7).normalized();
	direction[6] = QVector2D(-1.9, 1).normalized();
	direction[7] = QVector2D(-12, -1).normalized();
}

void Water::LoadHeightmaps()
{
	for (int i = 0; i < 200; i++) {
		char str[26];
		snprintf(str, 26, "./Textures/waves5/%03d.png", i);
		heightmaps[i] = QImage(str, "Format_Indexed8");
	}
}

float Water::CalHeightByMap(float x, float z, float time)
{
	float height = 0;
	float maxH = 30;
	int xValue = (int)(x * heightmaps[(int)time].width());
	int zValue = (int)(z * heightmaps[(int)time].width());
	QColor color = QColor(heightmaps[(int)time].pixel(xValue, zValue));
	float average = (color.red() + color.green() + color.blue()) / 3;
	height = average / 255 * maxH - maxH / 2;
	return height;
}

void Water::GenTexture(int width, int height)
{
	// The texture we're going to render to 
	glGenTextures(1, &reflectTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture 
	glBindTexture(GL_TEXTURE_2D, reflectTexture);
	// Give an empty image to OpenGL ( the last "0" ) 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering. Needed ! 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// The texture we're going to render to 
	glGenTextures(1, &refractTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture 
	glBindTexture(GL_TEXTURE_2D, refractTexture);
	// Give an empty image to OpenGL ( the last "0" ) 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering. Needed ! 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

QVector3D Water::CalNormalByThreePts(QVector3D a, QVector3D b, QVector3D c)
{
	QVector3D n = QVector3D::normal(a - c, b - a);
	n.setY(1.0);
	return n.normalized();
}

void Water::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void Water::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}

void Water::Paint(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("waveType", waveType);
	shaderProgram->setUniformValue("numWaves", numWaves);
	shaderProgram->setUniformValue("time", time);
	shaderProgram->setUniformValueArray("amplitude", amplitude, 8, 1);
	shaderProgram->setUniformValueArray("wavelength", wavelength, 8, 1);
	shaderProgram->setUniformValueArray("speed", speed, 8, 1);
	shaderProgram->setUniformValueArray("direction", direction, 8);

	if (waveType == 0) {
		if (!reverse)
			time += 0.01;
		else
			time -= 0.01;
		if (time > 50)
			reverse = true;
		if (time < 0)
			reverse = false;
	}
	
	else if (waveType == 1) {
		/*time = (int)time + 3;
		if (time > 199)
			time = (int)time % 200;
		for (int i = 0; i < vertices.size(); i += 3) {
			float height = CalHeightByMap(vertices[i].x() / scale / 2 + 0.5, vertices[i].z() / scale / 2 + 0.5, time);
			vertices[i].setY(height);
			height = CalHeightByMap(vertices[i+1].x() / scale / 2 + 0.5, vertices[i+1].z() / scale / 2 + 0.5, time);
			vertices[i+1].setY(height);
			height = CalHeightByMap(vertices[i+2].x() / scale / 2 + 0.5, vertices[i+2].z() / scale / 2 + 0.5, time);
			vertices[i+2].setY(height);

			QVector3D normal = CalNormalByThreePts(vertices[i], vertices[i + 1], vertices[i + 2]);
			normals[i] = normals[i+1] = normals[i+2] = normal;
		}*/
		time += 0.0001;
		if (time > 1)
			time -= 1;
		vvbo.bind();
		// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
		vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
		// Allocate and initialize the information
		vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

		nvbo.bind();
		// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
		nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
		// Allocate and initialize the information
		nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
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
	// Set Attribute 1 to be uv
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 2, NULL);
	//unbind buffer
	uvbo.release();

	if (waveType == 1) {
		// Bind the buffer so that it is the current active buffer
		nvbo.bind();
		// Enable Attribute 0
		shaderProgram->enableAttributeArray(2);
		// Set Attribute 0 to be position
		shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 3, NULL);
		//unbind buffer
		nvbo.release();
	}

	//Draw triangles with 4 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Water::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

void Water::CalProjectorMatrix(float aspect)
{
	glm::vec3 projPos = glm::vec3(camPos.x(), camPos.y(), camPos.z());
	glm::vec3 projAt = glm::vec3(0, 0, 0);
	glm::vec3 projUp = glm::vec3(0, 1, 0);

	glm::mat4 projView = glm::lookAt(projPos, projAt, projUp);
	glm::mat4 projProj = glm::perspective(40.0f, aspect, 0.1f, 1000.0f);

	glm::mat4 m = projView * projProj;
	projMat[0][0] = m[0][0]; projMat[0][1] = m[0][1]; projMat[0][2] = m[0][2]; projMat[0][3] = m[0][3];
	projMat[1][0] = m[1][0]; projMat[1][1] = m[1][1]; projMat[1][2] = m[1][2]; projMat[1][3] = m[1][3];
	projMat[2][0] = m[2][0]; projMat[2][1] = m[2][1]; projMat[2][2] = m[2][2]; projMat[2][3] = m[2][3];
	projMat[3][0] = m[3][0]; projMat[3][1] = m[3][1]; projMat[3][2] = m[3][2]; projMat[3][3] = m[3][3];
	return;
}
