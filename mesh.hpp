//
//  mesh.hpp
//  Mesh
//
//  Created by Scott Sanett on 26/07/2017.
//  Copyright © 2017 Scott Sanett. All rights reserved.
//

#ifndef mesh_h
#define mesh_h
#include <string>
#include <vector>
#include <headers.hpp>

namespace eirikr {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 texCoords;
    };
    
    /*
    struct Texture {
        unsigned int id;
        std::string type;
    };
     */
    
    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void draw(eirikr::Shader shader);
        
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        
    private:
        void setupMesh();
    };
    
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
    }

    void Mesh::setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
        
        // offsetof (s, m): s is short for struct, m is short for its name in the struct
        // returns the byte offset
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        glBindVertexArray(0);
    }
    
    /* naming convention for textures 
     * uniform sampler2D texture_diffuse1;
     * uniform sampler2D texture_diffuse2;
     * uniform sampler2D texture_specular1;
     * uniform sampler2D texture_specular2;
     */
     
    void Mesh::draw(Shader shader) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            auto name = textures[i].type;
            auto number = std::string();
            if (name == "texture_diffuse") { number = std::to_string(diffuseNr++); }
            else if (name == "texture_specular") { number = std::to_string(specularNr++); }
            else if (name == "texture_normal") { number = std::to_string(normalNr++); }
            else if (name == "texture_height") { number = std::to_string(heightNr++); }
            shader.setInt((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].ID);
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // unbind
        glActiveTexture(GL_TEXTURE0);
    }
    
} // end of namespace eirikr

#endif /* mesh_h */
