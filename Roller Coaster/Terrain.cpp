#include "Terrain.h"
bool tess = 0;

Terrain::Terrain()
{
}

void Terrain::Init()
{
	if(tess)
		InitShader("./Shader/Terrain.vs", "./Shader/Terrain2.fs", "./Shader/Terrain.tesc", "./Shader/Terrain.tese");
	else
		InitShader("./Shader/Terrain.vs", "./Shader/Terrain.fs", "./Shader/Terrain.tesc", "./Shader/Terrain.tese");
	ObjLoader objLoader;
	if (this->objVertices.size() == 0) {
		objLoader.LoadObjFromFile("./Objs/terrain.obj", this->objVertices, this->objUvs, this->objNormals, this->objFaces, this->objMtls);
		objLoader.loadMTL("./Objs/terrain.mtl", this->kds, this->kas, this->kss, this->Materials, this->Textures);
		objLoader.LoadObjFromFile("./Objs/CobbleStones.obj", this->objVertices1, this->objUvs1, this->objNormals1, this->objFaces1, this->objMtls1);
		objLoader.loadMTL("./Objs/CobbleStones.mtl", this->kds1, this->kas1, this->kss1, this->Materials1, this->Textures1);
		objLoader.LoadObjFromFile("./Objs/Stump.obj", this->objVertices2, this->objUvs2, this->objNormals2, this->objFaces2, this->objMtls2);
		objLoader.loadMTL("./Objs/Stump.mtl", this->kds2, this->kas2, this->kss2, this->Materials2, this->Textures2);
	}
	InitVAO();
	InitVBO();
}

void Terrain::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}

void Terrain::InitVBO()
{
	//Set each vertex's position
	float size = 15;
	for (int i = 0; i < objVertices.size(); i++) {
		vertices << QVector3D(objVertices[i].x * size * 0.6, objVertices[i].y * size - 0, objVertices[i].z * size * 0.6);
	}
	for (int i = 0; i < objVertices1.size(); i++) {
		size = 30;
		vertices1 << QVector3D(objVertices1[i].x * size, objVertices1[i].y * size - 70, objVertices1[i].z * size);
	}
	for (int i = 0; i < objVertices2.size(); i++) {
		size = 30;
		vertices2 << QVector3D(objVertices2[i].x * size, objVertices2[i].y * size, objVertices2[i].z * size);
	}

	//Beitun
	float gap = 0.01;
	//Set each vertex's position & uv
	for (float x = -1; x <= 0.99; x += gap) {
		for (float z = -1; z <= 0.99; z += gap) {
			float scale = 400;
			vertices3 << QVector3D(x * scale, 0, z * scale)
				<< QVector3D(x * scale, 0, (z + gap) * scale)
				<< QVector3D((x + gap) * scale, 0, z * scale)
				<< QVector3D((x + gap) * scale, 0, z * scale)
				<< QVector3D(x * scale, 0, (z + gap) * scale)
				<< QVector3D((x + gap) * scale, 0, (z + gap) * scale);

			uvs3 << QVector2D(x / 2 + 0.5, z / 2 + 0.5)
				<< QVector2D(x / 2 + 0.5, (z + gap) / 2 + 0.5)
				<< QVector2D((x + gap) / 2 + 0.5, z / 2 + 0.5)
				<< QVector2D((x + gap) / 2 + 0.5, z / 2 + 0.5)
				<< QVector2D(x / 2 + 0.5, (z + gap) / 2 + 0.5)
				<< QVector2D((x + gap) / 2 + 0.5, (z + gap) / 2 + 0.5);
		}
	}

	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	//Set each vertex's uv
	for (int i = 0; i < objUvs.size(); i++) {
		uvs << QVector2D(objUvs[i].x, objUvs[i].y);
	}
	for (int i = 0; i < objUvs1.size(); i++) {
		uvs1 << QVector2D(objUvs1[i].x, objUvs1[i].y);
	}
	for (int i = 0; i < objUvs2.size(); i++) {
		uvs2 << QVector2D(objUvs2[i].x, objUvs2[i].y);
	}
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));

	//Set each vertex's position
	for (int i = 0; i < objNormals.size(); i++) {
		normals << QVector3D(objNormals[i].x, objNormals[i].y, objNormals[i].z);
	}
	for (int i = 0; i < objNormals1.size(); i++) {
		normals1 << QVector3D(objNormals1[i].x, objNormals1[i].y, objNormals1[i].z);
	}
	for (int i = 0; i < objNormals2.size(); i++) {
		normals2 << QVector3D(objNormals2[i].x, objNormals2[i].y, objNormals2[i].z);
	}
	// Create Buffer for position
	nvbo.create();
	// Bind the buffer so that it is the current active buffer
	nvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	nvbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	// Allocate and initialize the information
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
}

void Terrain::InitShader(QString vertexShaderPath, QString fragmentShaderPath, QString tcsPath, QString tesPath)
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

	if (tess) {
		QFileInfo  tcsFile(tcsPath);
		if (tcsFile.exists())
		{
			controlShader = new QOpenGLShader(QOpenGLShader::TessellationControl);
			if (controlShader->compileSourceFile(tcsPath))
				shaderProgram->addShader(controlShader);
			else
				qWarning() << "control Shader Error " << controlShader->log();
		}
		else
			qDebug() << tcsFile.filePath() << " can't be found";

		QFileInfo  tesFile(tesPath);
		if (tesFile.exists())
		{
			evaluationShader = new QOpenGLShader(QOpenGLShader::TessellationEvaluation);
			if (evaluationShader->compileSourceFile(tesPath))
				shaderProgram->addShader(evaluationShader);
			else
				qWarning() << "evaluation Shader Error " << evaluationShader->log();
		}
		else
			qDebug() << tesFile.filePath() << " can't be found";
	}
	
	shaderProgram->link();
}

void Terrain::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void Terrain::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}

void Terrain::Paint(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("heightType", terrainType == 3 ? 1 : 0);//if type == 3, use heightmap

	switch (terrainType) {
	case 0:
		vvbo.bind();
		vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
		uvbo.bind();
		uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
		nvbo.bind();
		nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
		break;
	case 1:
		vvbo.bind();
		vvbo.allocate(vertices1.constData(), vertices1.size() * sizeof(QVector3D));
		uvbo.bind();
		uvbo.allocate(uvs1.constData(), uvs1.size() * sizeof(QVector2D));
		nvbo.bind();
		nvbo.allocate(normals1.constData(), normals1.size() * sizeof(QVector3D));
		break;
	case 2:
		vvbo.bind();
		vvbo.allocate(vertices2.constData(), vertices2.size() * sizeof(QVector3D));
		uvbo.bind();
		uvbo.allocate(uvs2.constData(), uvs2.size() * sizeof(QVector2D));
		nvbo.bind();
		nvbo.allocate(normals2.constData(), normals2.size() * sizeof(QVector3D));
		break;
	case 3:
		vvbo.bind();
		vvbo.allocate(vertices3.constData(), vertices3.size() * sizeof(QVector3D));
		uvbo.bind();
		uvbo.allocate(uvs3.constData(), uvs3.size() * sizeof(QVector2D));
		break;
	default:
		vvbo.bind();
		vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
		uvbo.bind();
		uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
		nvbo.bind();
		nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
		break;
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
	if (tess) {
		shaderProgram->setPatchVertexCount(3);
		//int c = shaderProgram->patchVertexCount();
		
		switch (terrainType) {
		case 0:
			glDrawArrays(GL_PATCHES, 0, vertices.size());
			break;
		case 1:
			glDrawArrays(GL_PATCHES, 0, vertices1.size());
			break;
		case 2:
			glDrawArrays(GL_PATCHES, 0, vertices2.size());
			break;
		case 3:
			glDrawArrays(GL_PATCHES, 0, vertices3.size());
			break;
		default:
			glDrawArrays(GL_PATCHES, 0, vertices.size());
			break;
		}
	}
	else {
		switch (terrainType) {
		case 0:
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			break;
		case 1:
			glDrawArrays(GL_TRIANGLES, 0, vertices1.size());
			break;
		case 2:
			glDrawArrays(GL_TRIANGLES, 0, vertices2.size());
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 0, vertices3.size());
			break;
		default:
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			break;
		}
	}
	
}

void Terrain::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);

	vao.release();
	shaderProgram->release();
}
