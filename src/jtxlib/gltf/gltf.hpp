#pragma once
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vec4.hpp>
#include <jtxlib/math/mat4.hpp>

namespace jtx::gltf {
    struct Scene {
        // Array of indices to the scene's children nodes
        std::vector<size_t> nodes;
    };

    struct Node {
        enum Type {
            MESH,
            CAMERA
        };

        // Specifies what the index is for
        Node::Type type;
        size_t index;
        std::vector<size_t> children;

        Mat4 matrix;
        Vec3f translation;
        Vec4f rotation;
        Vec3f scale;
    };

    struct GLTF {
        size_t defaultScene;
        std::vector<Scene> scenes;
        std::vector<Node> nodes;
    };

    struct Attributes {
        size_t position;
        size_t normal;
        size_t texCoord;
    };

    struct Primitive {
        enum Mode {
            POINTS,
            LINES,
            LINE_LOOP,
            LINE_STRIP,
            TRIANGLES,
            TRIANGLE_STRIP,
            TRIANGLE_FAN
        };

        Mode mode;
        size_t indices;

        // No support for morphs
        std::vector<Attributes> attributes;
    };

    struct Camera {
        enum Type {
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
}


