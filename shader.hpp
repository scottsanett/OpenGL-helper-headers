//
//  Shader.h
//  Shader
//
//  Created by Scott Sanett on 16/07/2017.
//  Copyright © 2017 Scott Sanett. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace eirikr {
    
    class Shader {
    private:
        template<typename T> void printCompileError(T);
        template<typename T> void printLinkError(T);
        
    public:
        GLuint ID;
        Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
        void use();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string & name, int value) const;
        void setFloat(const std::string & name, float value) const;
        void setMat4(const std::string & name, glm::mat4 value) const;
        void setVec3(const std::string & name, float _0, float _1, float _2);
        
    };
    
    template<typename T>
    void Shader::printCompileError(T t) {
        int success;
        char infoLog[512];
        glGetShaderiv(t, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(t, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    
    template<typename T>
    void Shader::printLinkError(T t) {
        int success;
        char infoLog[512];
        glGetShaderiv(t, GL_LINK_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(t, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
    
    Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath) {
        std::string vertexCode, fragmentCode;
        std::ifstream vShaderFile, fShaderFile;
        // make sure ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
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
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FAIL_NOT_SUCCESSFULLY_READ" << std::endl;
        }
        
        const char * vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        
        auto vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        printCompileError(vertex);
        
        auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        printCompileError(fragment);
        
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        printLinkError(ID);
    }
    
    void Shader::use() {
        glUseProgram(ID);
    }
    
    void Shader::setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    
    void Shader::setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void Shader::setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void Shader::setMat4(const std::string &name, glm::mat4 value) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void Shader::setVec3(const std::string & name, float _0, float _1, float _2) {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), _0, _1, _2);
    }
    
}

#endif /* Shader_h */
