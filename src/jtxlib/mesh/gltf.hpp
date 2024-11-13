/**
 * A simplified representation of GLTF2 scene
 * No longer using this, there is no loader/parser
 */

#pragma once
#include <jtxlib/math/mat4.hpp>

namespace jtx::gltf {
    struct Node {
        enum class Type {
            MESH,
            CAMERA
        };

        Node::Type type;
        union {
            size_t meshIndex;
            size_t cameraIndex;
        };

        // Indices of children nodes
        std::vector<size_t> children;

        Mat4 matrix;
        Vec3f translation;
        Vec4f rotation;
        Vec3f scale;
    };

    // Only support triangles
    // No support for morphs
    struct Primitive {
        size_t v1, v2, v3;
    };

    struct Mesh {
        std::vector<Primitive> primitives;
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
        std::vector<Mesh> meshes;

        // Layout is stuck as SOA for now
        enum class Layout {
            SOA,
            AOS
        };
        Layout layout = Layout::SOA;

        struct BufferSOA {
            std::vector<float> positions_x;
            std::vector<float> positions_y;
            std::vector<float> positions_z;

            std::vector<float> normals_x;
            std::vector<float> normals_y;
            std::vector<float> normals_z;

            std::vector<float> uvs_u;
            std::vector<float> uvs_v;
        };

        BufferSOA buffer;
        std::vector<Camera> cameras;
    };
}


