//
//  model.hpp
//  Mesh
//
//  Created by Scott Sanett on 26/07/2017.
//  Copyright © 2017 Scott Sanett. All rights reserved.
//

#ifndef model_h
#define model_h
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.hpp"
#include "texture.hpp"

namespace eirikr {
    class Model {
    public:
        Model(char const * path) { loadModel(path); }
        void draw(Shader & shader);
        
    private:
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded;
        std::string directory;
        
    private:
        void loadModel(std::string const & path);
        void processNode(aiNode * node, const aiScene * scene);
        Mesh processMesh(aiMesh * mesh, const aiScene * scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
    };
    
    void Model::draw(Shader & shader) {
        for (unsigned int i = 0; i < meshes.size(); ++i) {
            meshes[i].draw(shader);
        }
    }
    
    void Model::loadModel(std::string const & path) {
        Assimp::Importer importer;
        // aiProcess_Triangulate: triangulate the model if it's not all triangle
        // aiProcess_FlipUVs: get UV coordinates with the upper-left corner as origin
        // aiProcess_GenNormals: generate normal for every vertex
        // aiProcess_SplitLargeMeshes: split larger marshes to smaller ones, could be useful when the number of vertices is limited
        // aiProcess_OptimizeMeshes: opposite of the one above
        auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Error::Assimp: " << importer.GetErrorString() << std::endl;
            return;
        }
        
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }
    
    void Model::processNode(aiNode * node, const aiScene * scene) {
        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene);
        }
    }
    
    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;
            /* processes vertices, normal, and texcoords */
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
            
            // a vertex may have 8 texture coordinates
            // but we only care about the first
            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }
            
            // tangent
            if (mesh->mTangents) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;
            }
            // bitangent
            if (mesh->mBitangents) {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }
            
            vertices.push_back(vertex);
        }
        
        /* process indices */
        // Assimp defines a `face` array for each mesh
        for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            for (unsigned j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
        
        // deals with materials
        // very much like a vertex node, a vertex only contains an index to the real material object
        // to retrieve the actual material data, we need to index `mMaterials`
        // the material index is stored in its `mMaterialIndex` attribute
        
        if (mesh->mMaterialIndex >= 0) { // if it has a material attribute
            auto material = scene->mMaterials[mesh->mMaterialIndex];
            auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            auto normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            auto heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }
        return Mesh(vertices, indices, textures);
    }
    
    std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<Texture> textures;
        for (unsigned i = 0; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            
            bool skip = false;
            for (auto j = 0; j < textures_loaded.size(); ++j) {
                if (textures_loaded[j].path == std::string(str.C_Str())) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            
            if (!skip) {
                Texture texture;
                auto fullpath = directory + "/" + str.C_Str();
                texture.ID = texture.loadTextureFromPath(fullpath.c_str());
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
}

#endif /* model_h */
