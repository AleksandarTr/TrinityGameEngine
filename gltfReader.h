#ifndef TRINITYENGINE_GLTFREADER_H
#define TRINITYENGINE_GLTFREADER_H

#include <cstddef>
#include <string>
#include <nlohmann/json.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Model;
class Shader;
class Mesh;

class gltfReader {
private:
    struct Buffer {
        char *data = nullptr;
        std::string uri;
        std::size_t size;
        std::string name;

        Buffer(std::string &uri, std::size_t size, std::string &name, std::string &location);

        ~Buffer();
    };

    struct BufferView {
        int buffer;
        int offset;
        int size;
        int stride;
        int target;
        std::string name;

        BufferView(int buffer, int size, int offset, int stride, int target, std::string &name);
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

            Sparse(int count, Indices &indices, Values &values);
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

        Accessor(int componentType, int count, std::string &type, int bufferView, int offset, bool normalized,
                 double *max, double *min, std::string &name, Sparse &sparse);
    };

    struct Mesh {
        struct Primitive {
            int indices;
            int material;
            int mode;
            std::vector<std::string> attributes;
            std::vector<int> attributeValues;
            std::vector<std::string> targets;
            std::vector<int> targetValues;

            Primitive(int indices, int material, int mode, std::vector<std::string> &attributes, std::vector<int> &attributeValues, std::vector<std::string> &targets, std::vector<int> &targetValues);
        };

        std::vector<Primitive> primitives;
        std::vector<float> weights;
        std::string name;

        Mesh(std::vector<Primitive> &primitives, std::vector<float> &weights, std::string &name);
    };

    struct Sampler {
        int magFilter;
        int minFilter;
        int wrapS;
        int wrapT;
        std::string name;

        Sampler(int magFilter, int minFilter, int wrapS, int wrapT, std::string &name);
    };

    struct Texture {
        int sampler;
        int source;
        std::string name;

        Texture(int sampler, int source, std::string &name);
    };

    struct Image {
        std::string uri;
        std::string mimeType;
        int bufferView;
        std::string name;

        Image(std::string &uri, std::string &mimeType, int bufferView, std::string &name);
    };

    struct Material {
        std::string name;
        glm::vec4 baseColor;
        int baseTextureIndex;
        int baseTexCoord;
        float metallic;
        float roughness;
        int mrTextureIndex;
        int mrTexCoord;
        int normalTextureIndex;
        int normalTexCoord;
        float normalScale;
        int occlusionTextureIndex;
        int occlusionTexCoord;
        float occlusionStrength;
        int emissiveTextureIndex;
        int emissiveTexCoord;
        glm::vec3 emissiveFactor;
        std::string alphaMode;
        float alphaCutoff;
        bool doubleSided;

        Material(std::string &name, glm::vec4 &baseColor, int baseTextureIndex, int baseTexCoord, float metallic, float roughness,
                 int mrTextureIndex, int mrTexCoord, int normalTextureIndex, int normalTexCoord, float normalScale,
                 int occlusionTextureIndex, int occlusionTexCoord, float occlusionStrength, int emissiveTextureIndex,
                 int emissiveTexCoord, glm::vec3 &emissiveFactor, std::string &alphaMode, float alphaCutoff, bool doubleSided);
    };

    struct Skin {
        int inverseBindMatrices;
        int skeleton;
        std::vector<int> joints;
        std::string name;

        Skin(int inverseBindMatrices, int skeleton, std::vector<int> &joints, std::string &name);
    };

    struct Animation {
        struct Channel {
            int sampler;
            int node;
            std::string path;

            Channel(int sampler, int node, std::string &path);
        };

        struct Sampler {
            int input;
            std::string interpolation;
            int output;

            Sampler(int input, std::string &interpolation, int output);
        };

        std::vector<Channel> channels;
        std::vector<Sampler> samplers;
        std::string name;

        Animation(std::vector<Channel> &channels, std::vector<Sampler> &samplers, std::string &name);
    };

    struct Camera {
        struct Orthographic {
            float xmag;
            float ymag;
            float zfar;
            float znear;
        };

        struct Perspective {
            float aspectRatio;
            float yfov;
            float zfar;
            float znear;
        };

        union {
            Orthographic orthographic;
            Perspective perspective;
        };
        std::string type;
        std::string name;

        Camera(std::string &name, std::string &type, float arg1, float arg2, float zfar, float znear);
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

        Node(int camera, std::vector<int> &children, int skin, glm::mat4 &matrix, int mesh, glm::quat &rotation, glm::vec3 &scale, glm::vec3 &translation, std::vector<float> &weights, std::string &name);
    };

    struct Scene {
        std::vector<int> nodes;
        std::string name;

        Scene(std::vector<int> &nodes, std::string &name);
    };

    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferViews;
    std::vector<Accessor> accessors;
    std::vector<Sampler> samplers;
    std::vector<Texture> textures;
    std::vector<Image> images;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Skin> skins;
    std::vector<Animation> animations;
    std::vector<Camera> cameras;
    std::vector<Node> nodes;
    std::vector<Scene> scenes;
    Shader &shader;

    std::vector<::Mesh*> **meshBuffer = nullptr;

    void insertChildNodes(Model& model, int index);

    std::vector<::Mesh*> &getMeshes(int index);

    std::vector<::Mesh *> &copyMeshes(int index);

    [[nodiscard]] char* readAccessor(int accessor, int &width, int &size);

public:
    gltfReader(std::string uri, Shader &shader);

    [[nodiscard]] ::Model& getScene(int index);

    int getSceneCount();
};

#endif
