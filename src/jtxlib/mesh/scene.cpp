#include "scene.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdexcept>
#include <string>

namespace jtxs = jtx::scene;

jtxs::Scene loadOBJ(const std::string &filepath) {
    Assimp::Importer importer;

    const aiScene *ai_scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType
    );

    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
        throw std::runtime_error("Assimp failed to load .obj file: " + std::string(importer.GetErrorString()));
    }

    jtxs::Scene scene;
    scene.meshes.reserve(ai_scene->mNumMeshes);
    auto processMesh = [&](aiMesh *ai_mesh) -> jtxs::Mesh {
        jtxs::Mesh mesh;

        // Positions
        for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
            mesh.buffer.positions_x.push_back(ai_mesh->mVertices[i].x);
            mesh.buffer.positions_y.push_back(ai_mesh->mVertices[i].y);
            mesh.buffer.positions_z.push_back(ai_mesh->mVertices[i].z);
        }

        // Normals
        if (ai_mesh->HasNormals()) {
            for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
                mesh.buffer.normals_x.push_back(ai_mesh->mNormals[i].x);
                mesh.buffer.normals_y.push_back(ai_mesh->mNormals[i].y);
                mesh.buffer.normals_z.push_back(ai_mesh->mNormals[i].z);
            }
        } else {
            for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
                mesh.buffer.normals_x.push_back(0.0f);
                mesh.buffer.normals_y.push_back(0.0f);
                mesh.buffer.normals_z.push_back(0.0f);
            }
        }

        // UVs
        if (ai_mesh->mTextureCoords[0]) {
            for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
                mesh.buffer.texCoords_u.push_back(ai_mesh->mTextureCoords[0][i].x);
                mesh.buffer.texCoords_u.push_back(ai_mesh->mTextureCoords[0][i].y);
            }
        } else {
            for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
                mesh.buffer.texCoords_u.push_back(0.0f);
                mesh.buffer.texCoords_u.push_back(0.0f);
            }
        }

        for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
            aiFace ai_face = ai_mesh->mFaces[i];
            if (ai_face.mNumIndices != 3) {
                throw std::runtime_error("Non-triangular face detected in .obj file.");
            }
            jtxs::Face face; // NOLINT(*-pro-type-member-init)
            face.v1 = static_cast<size_t>(ai_face.mIndices[0]);
            face.v2 = static_cast<size_t>(ai_face.mIndices[1]);
            face.v3 = static_cast<size_t>(ai_face.mIndices[2]);
            mesh.indices.push_back(face);
        }

        return mesh;
    };

    // Meshes
    for (unsigned int i = 0; i < ai_scene->mNumMeshes; ++i) {
        jtxs::Mesh mesh = processMesh(ai_scene->mMeshes[i]);
        scene.meshes.push_back(mesh);
    }

    // Nodes
    std::function<jtxs::Node(aiNode *)> processNode = [&](aiNode *ai_node) -> jtxs::Node {
        jtxs::Node node;

        for (unsigned int i = 0; i < ai_node->mNumMeshes; ++i) {
            node.meshes.push_back(ai_node->mMeshes[i]);
        }

        for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
            aiNode *child_ai_node = ai_node->mChildren[i];
            jtxs::Node child_node = processNode(child_ai_node);
            node.children.push_back(child_node);
        }

        return node;
    };

    scene.rootNode = processNode(ai_scene->mRootNode);
    return scene;
}
