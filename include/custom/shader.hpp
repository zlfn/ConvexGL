#ifndef OPENGL_SHADER_HPP
#define OPENGL_SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }

        catch(std::ifstream::failure e)
        {
            std::cout << vertexPath << std::endl;
            std::cout << fragmentPath << std::endl;
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    void use()
    {
        glUseProgram(ID);
    }
    void setUniform1b(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setUniform1i(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setUniform1f(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setUniform4f(const std::string &name, float v1, float v2, float v3, float v4) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3, v4);
    }
    void setUniformVec3(const std::string &name, glm::vec3 &vec) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &vec[0]);
    }
    void setUniformDvec3(const std::string &name, glm::dvec3 &vec) const
    {
        glUniformMatrix4dv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &vec[0]);
    }
    void setUniformMat4(const std::string &name, glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

class ComputeShader
{
public:
    unsigned int ID;

    ComputeShader(const char* computePath)
    {
        std::string computeCode;
        std::ifstream cShaderFile;

        cShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;

            cShaderStream << cShaderFile.rdbuf();

            cShaderFile.close();

            computeCode = cShaderStream.str();
        }

        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        const char* cShaderCode = computeCode.c_str();
        unsigned int compute;
        int success;
        char infoLog[512];

        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(compute, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(compute);
    }

    void use()
    {
        glUseProgram(ID);
    }
};

extern std::string readFromFile(const char* pathToFile);

#endif //OPENGL_SHADER_HPP