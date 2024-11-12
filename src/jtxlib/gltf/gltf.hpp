#pragma once
#include <jtxlib/math/vector.hpp>
#include <jtxlib/math/mat4.hpp>

namespace jtx::gltf {
    struct Node {
        enum class Type {
            MESH,
            CAMERA
        };

        Node::Type type;
        // Mesh or camera index
        union {
            size_t meshIndex;
            size_t cameraIndex;
        };
        std::vector<size_t> children;

        Mat4 matrix;
        Vec3f translation;
        Vec4f rotation;
        Vec3f scale;
    };

    struct Attributes {
        size_t position;
        size_t normal;
        size_t texCoord;
    };

    // Only support triangles
    // No support for morphs
    struct Primitive {
        size_t indices;
        Attributes attributes;
    };

    struct Camera {
        enum class Type {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        Camera::Type type;
        union {
            struct {
                float aspectRatio;
                float yfov;
            };
            struct {
                float xmag;
                float ymag;
            };
        };
        float znear;
        float zfar;
    };

    struct Scene {
        // Array of indices to the scene's children nodes
        std::vector<size_t> nodes;
    };

    struct GLTF {
        size_t defaultScene;
        std::vector<Scene> scenes;
        std::vector<Node> nodes;

        // Controls the layout of position data
        // Normals and UVs are always AoS
        // Not functional yet
        enum class Layout {
            SOA,
            AOS
        };
        Layout layout;


        struct Buffer {
            std::vector<Vec3f> positions;
            std::vector<Vec3f> normals;
            std::vector<Vec2f> UVs;
        };
        Buffer buffer;
        std::vector<Camera> cameras;
    };
}


