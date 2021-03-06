#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__
#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace eirikr {
    class Texture {
    public:
        unsigned int ID;
        std::string type;
        std::string path;
        int texWidth;
        int texHeight;
        int texComponents;
        
    public:
        Texture() {}
        unsigned int loadTextureFromPath(const char * path);
    };
    
    unsigned int Texture::loadTextureFromPath(const char * path) {
//        stbi_set_flip_vertically_on_load(true);
        unsigned int textureID;
        glGenTextures(1, &textureID);
        auto data = stbi_load(path, &texWidth, &texHeight, &texComponents, 0);
        if (data) {
            GLenum format;
            if (texComponents == 1) format = GL_RED;
            else if (texComponents == 3) format = GL_RGB;
            else if (texComponents == 4) format = GL_RGBA;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            stbi_image_free(data);
        }
        else {
            throw std::logic_error(std::string("Error: texture failed to load at path ") + std::string(path) + "\n");
            stbi_image_free(data);
        }
        return textureID;
    }
    
}

#endif __TEXTURE_HPP__
