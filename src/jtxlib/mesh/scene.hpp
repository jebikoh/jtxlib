#pragma once
#ifdef USE_ASSIMP
#include <vector>
#include <type_traits>

// Roughly follow Assimp hierarchy
// Pass BufferAOS to store mesh in AoS layout
// Pass BufferSOA to store mesh in SoA layout
namespace jtx::scene {
    struct Face {
        uint32_t v1, v2, v3;
    };

    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };

    struct BufferAOS {
        std::vector<Vertex> vertices;
    };

    struct BufferSOA {
        std::vector<float> x;
        std::vector<float> y;
        std::vector<float> z;

        std::vector<float> nx;
        std::vector<float> ny;
        std::vector<float> nz;

        std::vector<float> u;
        std::vector<float> v;
    };

    template <typename T>
    struct is_valid_buffer : std::false_type {};

    template <>
    struct is_valid_buffer<BufferAOS> : std::true_type {};

    template <>
    struct is_valid_buffer<BufferSOA> : std::true_type {};

    template <typename T>
    constexpr bool is_valid_buffer_v = is_valid_buffer<T>::value;

    template <typename Buffer>
    struct Mesh {
        static_assert(is_valid_buffer_v<Buffer>, "Mesh can only be instantiated with BufferAOS or BufferSOA");
        Buffer buffer;
        std::vector<Face> indices;
    };

    struct Node {
        using mesh_index = size_t;

        std::vector<Node> children;
        std::vector<mesh_index> meshes;
    };

    template <typename Buffer>
    struct Scene {
        static_assert(is_valid_buffer_v<Buffer>, "Mesh can only be instantiated with BufferAOS or BufferSOA");

        Node rootNode;
        std::vector<Mesh<Buffer>> meshes;
    };

    template <typename Buffer>
    Scene<Buffer> loadOBJ(const std::string &filepath);
}
#endif