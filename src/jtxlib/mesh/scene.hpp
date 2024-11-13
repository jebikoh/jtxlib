#pragma once
#include <vector>
// Roughly follow Assimp hierarchy
namespace jtx::scene {
    struct Face {
        size_t v1, v2, v3;
    };

    struct Mesh {
        struct BufferSOA {
            std::vector<float> positions_x;
            std::vector<float> positions_y;
            std::vector<float> positions_z;

            std::vector<float> normals_x;
            std::vector<float> normals_y;
            std::vector<float> normals_z;

            std::vector<float> texCoords_u;
            std::vector<float> texCoords_v;
        };

        BufferSOA buffer;
        std::vector<Face> indices;
    };

    struct Node {
        using mesh_index = size_t;

        std::vector<Node> children;
        std::vector<mesh_index> meshes;
    };

    struct Scene {
        Node rootNode;
        std::vector<Mesh> meshes;
    };

    Scene loadOBJ(const std::string &filepath);
}