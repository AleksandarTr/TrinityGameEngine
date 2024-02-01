#include "Shader.h"

Shader::Shader() {
    programId = glCreateProgram();
}

std::string Shader::readFile(std::string location) {
    std::ifstream reader(location);
    if(!reader.is_open()) throw std::invalid_argument("Could not open given file");

    std::string result = "";
    std::string line;
    while(std::getline(reader, line)) result += line + "\n";

    reader.close();
    return result;
}

void Shader::activate(std::string fragmentShaderSource, std::string vertexShaderSource) {
    std::string fragmentShaderStr = readFile(fragmentShaderSource);
    std::string vertexShaderStr = readFile(vertexShaderSource);
    const char* fragmentShader = fragmentShaderStr.c_str();
    const char* vertexShader = vertexShaderStr.c_str();

    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertexShader, 1, &vertexShader, NULL);
    glCompileShader(this->vertexShader);

    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragmentShader, 1, &fragmentShader, NULL);
    glCompileShader(this->fragmentShader);

    //Create shader program
    glAttachShader(programId, this->vertexShader);
    glAttachShader(programId, this->fragmentShader);
    glLinkProgram(programId);

    glUseProgram(programId);
}

void Shader::unloadFiles() {
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

GLuint Shader::getProgramID() {
    return programId;
}
