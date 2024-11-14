#include "scene.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace jtxs = jtx::scene;

template <typename Buffer>
jtxs::Scene<Buffer> loadOBJ(const std::string &filepath) {
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

    jtxs::Scene<Buffer> scene;
    scene.meshes.reserve(ai_scene->mNumMeshes);

    auto processMesh = [&](aiMesh *ai_mesh) -> jtxs::Mesh<Buffer> {
        jtxs::Mesh<Buffer> mesh;

        for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
            const aiVector3D &position = ai_mesh->mVertices[i];
            const aiVector3D &normal = ai_mesh->HasNormals() ? ai_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f);
            const aiVector3D &texCoord = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);

            if constexpr (std::is_same_v<Buffer, jtxs::BufferAOS>) {
                jtxs::Vertex vertex;
                vertex.x = position.x;
                vertex.y = position.y;
                vertex.z = position.z;

                vertex.nx = normal.x;
                vertex.ny = normal.y;
                vertex.nz = normal.z;

                vertex.u = texCoord.x;
                vertex.v = texCoord.y;

                mesh.buffer.vertices.push_back(vertex);
            } else if constexpr (std::is_same_v<Buffer, jtxs::BufferSOA>) {
                mesh.buffer.x.push_back(position.x);
                mesh.buffer.y.push_back(position.y);
                mesh.buffer.z.push_back(position.z);

                mesh.buffer.nx.push_back(normal.x);
                mesh.buffer.ny.push_back(normal.y);
                mesh.buffer.nz.push_back(normal.z);

                mesh.buffer.u.push_back(texCoord.x);
                mesh.buffer.v.push_back(texCoord.y);
            } else {
                static_assert(std::is_same_v<Buffer, jtxs::BufferAOS> || std::is_same_v<Buffer, jtxs::BufferSOA>, "Unsupported buffer type");
            }
        }

        // Faces (indices)
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
            aiFace &ai_face = ai_mesh->mFaces[i];
            if (ai_face.mNumIndices != 3) {
                throw std::runtime_error("Non-triangular face detected in .obj file.");
            }
            jtxs::Face face;
            face.v1 = ai_face.mIndices[0];
            face.v2 = ai_face.mIndices[1];
            face.v3 = ai_face.mIndices[2];
            mesh.indices.push_back(face);
        }

        return mesh;
    };

    // Process each mesh in the scene
    for (unsigned int i = 0; i < ai_scene->mNumMeshes; ++i) {
        jtxs::Mesh<Buffer> mesh = processMesh(ai_scene->mMeshes[i]);
        scene.meshes.push_back(mesh);
    }

    // Recursive function to process nodes
    std::function<jtxs::Node(aiNode *)> processNode = [&](const aiNode *ai_node) -> jtxs::Node {
        jtxs::Node node;

        // Add meshes to the node
        for (unsigned int i = 0; i < ai_node->mNumMeshes; ++i) {
            node.meshes.push_back(ai_node->mMeshes[i]);
        }

        // Recursively process child nodes
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
