#ifndef TRINITYENGINE_GLTFREADER_H
#define TRINITYENGINE_GLTFREADER_H

#include <cstddef>
#include <string>
#include <nlohmann/json.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class gltfReader {
private:
    struct Buffer {
        char *data;
        std::string uri = "";
        std::size_t size;
        std::string name;

        Buffer(std::string uri, std::size_t size, std::string name = "", std::string location = "");

        ~Buffer();
    };

    struct BufferView {
        int buffer;
        int offset;
        int size;
        int stride;
        int target;
        std::string name;

        BufferView(int buffer, int size, int offset = 0, int stride = 0, int target = 0, std::string name = "");
    };

    struct Accessor {
        struct Sparse {
            struct Indices {
                int bufferView;
                int offset;
                int componentType;

                Indices(int bufferView, int offset, int componentType);
            };

            struct Values {
                int bufferView;
                int offset;

                Values(int bufferView, int offset);
            };

            int count;
            Indices indices;
            Values values;

            Sparse(int count, Indices indices, Values values);
        };

        int bufferView;
        int offset;
        int componentType;
        bool normalized;
        int count;
        std::string type;
        double max[16];
        double min[16];
        std::string name;
        Sparse sparse;

        Accessor(int componentType, int count, std::string type, int bufferView = -1, int offset = 0, bool normalized = false,
                 double *max = nullptr, double *min = nullptr, std::string name = "", Sparse sparse = Sparse(0, {0, 0, 0}, {0, 0}));
    };

    struct Node {
        int camera;
        std::vector<int> children;
        int skin;
        glm::mat4 matrix;
        int mesh;
        glm::quat rotation;
        glm::vec3 scale;
        glm::vec3 translation;
        std::vector<float> weights;
        std::string name;

        Node(int camera, std::vector<int> children, int skin, glm::mat4 matrix, int mesh, glm::quat rotation, glm::vec3 scale, glm::vec3 translation, std::vector<float> weights, std::string name);
    };

    struct Scene {
        std::vector<int> nodes;
        std::string name;

        Scene(std::vector<int> nodes, std::string name);
    };

    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferViews;
    std::vector<Accessor> accessors;

    std::vector<Node> nodes;
    std::vector<Scene> scenes;

public:
    gltfReader(std::string uri);
};

#endif
