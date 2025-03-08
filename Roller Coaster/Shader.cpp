#include "Shader.h"

Shader::Shader(const char *vertex_path, const char *geometry_path, const char *fragment_path)
{
	this->program = loadShader(vertex_path, geometry_path, fragment_path);
}

std::string Shader::readFile(const char *filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open())
	{
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Shader::loadShader(const char *vertex_path, const char *geometry_path, const char *fragment_path)
{
	// �ǳƳ��I�ۦ⾹  // �гy���I�ۦ⾹
	GLuint vertShader = createBasicShader(0, vertex_path);
	// �ǳƹ����ۦ⾹  // �гy�����ۦ⾹
	GLuint fragShader = createBasicShader(1, fragment_path);
	// �ǳƴX��ۦ⾹  // �гy�X��ۦ⾹
	GLuint geomShader = createBasicShader(2, fragment_path);

	/*********************�إߵ{���j�w�ۦ⾹**********************/
	std::cout << "Linking program" << std::endl;
	// �إߵۦ⾹�{�� // �гy�ۦ⾹�{��
	GLuint program = glCreateProgram();
	// �N�ݭn���ۦ⾹���J��ۦ⾹�{����
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glAttachShader(program, geomShader);
	// ���J����s����ۦ⾹
	glLinkProgram(program);

	// �T�{�ۦ⾹�{���O�_���\�s��
	debugShader(program);

	// �R�����Ϊ��ۦ⾹
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	glDeleteShader(geomShader);

	return program;
}

// 0 vertex 1 fragment 2 geometry
GLuint Shader::createBasicShader(int shaderCmd, const char *path)
{
	// �ǳƵۦ⾹  // �гy�ۦ⾹
	GLuint shader;
	if (shaderCmd == 0)
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (shaderCmd == 1)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else if (shaderCmd == 2)
		shader = glCreateShader(GL_GEOMETRY_SHADER);
	// Read shaders
	std::string shaderStr = readFile(path);
	const char *shaderSrc = shaderStr.c_str();
	return compileShader(shaderCmd, shader, shaderSrc);
}

GLuint Shader::compileShader(int shaderCmd, GLuint shader, const char *shaderSrc)
{
	if (shaderCmd == 0)
		std::cout << "Compiling vertex shader." << std::endl;
	else if (shaderCmd == 1)
		std::cout << "Compiling fragment shader." << std::endl;
	else if (shaderCmd == 2)
		std::cout << "Compiling geometry shader." << std::endl;
	// �N�ۦ⾹����l�X(��)�j�w��ۦ⾹��
	glShaderSource(shader, 1, &shaderSrc, NULL);
	// �sĶ�ۦ⾹
	glCompileShader(shader);

	// Check vertex shader
	//�T�{�ۦ⾹�O�_�j�w���\
	debugShader(shader);
	return shader;
}

void Shader::debugShader(GLuint shader)
{
	GLint result = GL_FALSE;
	int logLength;
	// Check vertex shader
	//�T�{�ۦ⾹�O�_�j�w���\
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> shaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &shaderError[0]);
	std::cout << &shaderError[0] << std::endl;
}