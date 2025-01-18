module;
#include <glad/glad.h>
#include <cstdlib>

export module shader;

import std;
import glm;


std::string getShaderCode(const std::string& filePath)
{
    std::string shaderCode;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        shaderFile.open(filePath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        shaderCode = vShaderStream.str();
    }
    catch (std::ifstream::failure const&)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ, path: " << filePath << std::endl;
        exit(1);
    }
    return shaderCode;
}

GLuint compileShader(GLuint type, const char* shaderCode)
{
    int success;
    char infoLog[512];

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);
    // print compile errors if any
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    };
    return shader;
}

/// <summary>
/// A wrapper class for GLSL shaders. heavily inspired by https://learnopengl.com/Getting-started/Shaders
/// </summary>
export class Shader
{
public:
    // the program ID
    unsigned int id = 0;
    // use/activate the shader
    virtual ~Shader() = default;

    inline void use() const
    {
        glUseProgram(id);
    }

    // utility uniform functions
    void setFloat(const char* name, float value) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform1f(location, value);
    }

    void setInt(const char* name, int value) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform1i(location, value);
    }

    void setVector(const char* name, const glm::vec3& vector) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform3f(location, vector.x, vector.y, vector.z);
    }

    void setVector(const char* name, const glm::vec4& vector) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
    }

    void setMatrix(const char* name, const glm::mat4& matrix) const
    {
        int location = glGetUniformLocation(id, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

protected:
    void checkShaderErrors()
    {
        // print linking errors if any
        int success;
        char infoLog[512];

        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
};

export class VertFragShader : public Shader
{
public:
    VertFragShader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode = getShaderCode(vertexPath);
        std::string fragmentCode = getShaderCode(fragmentPath);

        // 2. compile shaders
        GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
        GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

        // 3. shader Program
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);

        checkShaderErrors();

        // 4. delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
};

export class VertFragTessShader : public Shader
{
public:
    VertFragTessShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& tescPath, const std::string& tesePath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode = getShaderCode(vertexPath);
        std::string fragmentCode = getShaderCode(fragmentPath);
        std::string tescCode = getShaderCode(tescPath);
        std::string teseCode = getShaderCode(tesePath);

        // 2. compile shaders
        GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
        GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());
        GLuint tesc = compileShader(GL_TESS_CONTROL_SHADER, tescCode.c_str());
        GLuint tese = compileShader(GL_TESS_EVALUATION_SHADER, teseCode.c_str());

        // 3. shader Program
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glAttachShader(id, tesc);
        glAttachShader(id, tese);
        glLinkProgram(id);

        checkShaderErrors();

        // 4. delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(tesc);
        glDeleteShader(tese);
    }
};


export class UniformColorShader : public VertFragShader
{
public:
    UniformColorShader() : VertFragShader("shaders/uniformcolor.vert", "shaders/uniformcolor.frag") {}
};

export class InfiniteGridShader : public VertFragShader
{
public:
    InfiniteGridShader() : VertFragShader("shaders/infinitegrid.vert", "shaders/infinitegrid.frag") {}
};

export class PhongShader : public VertFragShader
{
public:
    PhongShader() : VertFragShader("shaders/phong.vert", "shaders/phong.frag") {}
};

export class MultiColorShader : public VertFragShader
{
public:
    MultiColorShader() : VertFragShader("shaders/multicolor.vert", "shaders/multicolor.frag") {}
};