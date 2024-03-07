#include "Shader.h"
#include <algorithm>
#include "Light.h"

GLuint Shader::activeShader = 0;

Shader::Shader(std::string fragmentShaderSource, std::string vertexShaderSource, std::string geometryShaderSource) {
    programId = glCreateProgram();

    std::string fragmentShaderStr = readFile(fragmentShaderSource);
    std::string vertexShaderStr = readFile(vertexShaderSource);
    std::string geometryShaderStr = geometryShaderSource.empty() ? "" : readFile(geometryShaderSource);
    const char* fragmentShader = fragmentShaderStr.c_str();
    const char* vertexShader = vertexShaderStr.c_str();
    const char* geometryShader = geometryShaderStr.c_str();

    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertexShader, 1, &vertexShader, nullptr);
    glCompileShader(this->vertexShader);

    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragmentShader, 1, &fragmentShader, nullptr);
    glCompileShader(this->fragmentShader);

    if(!geometryShaderSource.empty()) {
        this->geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(this->geometryShader, 1, &geometryShader, nullptr);
        glCompileShader(this->geometryShader);

        GLint isCompiled = 0;
        glGetShaderiv(this->geometryShader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(this->geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(this->geometryShader, maxLength, &maxLength, &errorLog[0]);

            for(char i : errorLog) printf("%c", i);
            glDeleteShader(this->geometryShader); // Don't leak the shader.
            exit(-1);
        }
    }

    //Create shader program
    glAttachShader(programId, this->vertexShader);
    glAttachShader(programId, this->fragmentShader);
    if(!geometryShaderSource.empty()) glAttachShader(programId, this->geometryShader);
    glLinkProgram(programId);
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

void Shader::activate() {
    if(activeShader != programId) {
        glUseProgram(programId);
        activeShader = programId;
    }
}

void Shader::unloadFiles() {
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    if(geometryShader) glDeleteShader(geometryShader);
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

GLuint Shader::getProgramID() {
    return programId;
}

GLuint Shader::getActiveShader() {
    return activeShader;
}
