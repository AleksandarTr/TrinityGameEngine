#include <fstream>
#include "gltfReader.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

gltfReader::gltfReader(std::string uri) {
    std::ifstream reader(uri);
    nlohmann::json data = nlohmann::json::parse(reader);
    reader.close();

    int lastChar = uri.length();
    for(; lastChar > 0; lastChar--) if(uri[lastChar] == '/') break;
    uri = uri.substr(0, lastChar);

    for(auto &buffer : data["buffers"]) {
        std::string buffUri = buffer["uri"];
        std::size_t size = buffer["byteLength"];
        std::string name = buffer.value("name", "");
        buffers.emplace_back(buffUri, size, name, uri);
    }

    for(auto &bufferView : data["bufferViews"]) {
        int buffer = bufferView["buffer"];
        int offset = bufferView.value("byteOffset", 0);
        int size = bufferView["byteLength"];
        int stride = bufferView.value("byteStride", 0);
        int target = bufferView.value("target", 0);
        std::string name = bufferView.value("name", "");
        bufferViews.emplace_back(buffer, size, offset, stride, target, name);
    }

    for(auto &accessor : data["accessors"]) {
        int bufferView = accessor.value("bufferView", -1);
        int offset = accessor.value("byteOffset", 0);
        int componentType = accessor["componentType"];
        bool normalized = accessor.value("normalized", false);
        int count = accessor["count"];
        std::string type = accessor["type"];
        std::string name = accessor.value("name", "");

        double max[16], min[16];

        for(int i = 0; i < accessor["max"].size(); i++) max[i] = accessor["max"][i];
        double *maxAdr = accessor["max"].size() == 0 ? nullptr : max;

        for(int i = 0; i < accessor["min"].size(); i++) min[i] = accessor["min"][i];
        double *minAdr = accessor["min"].size() == 0 ? nullptr : min;

        if(accessor.contains("sparse")) {
            int sparseCount = accessor["sparse"]["count"];

            int indicesBufferView = accessor["sparse"]["indices"]["bufferView"];
            int indicesComponentType = accessor["sparse"]["componentType"];
            int indicesOffset = accessor["sparse"]["byteOffset"];
            Accessor::Sparse::Indices indices(indicesBufferView, indicesOffset, indicesComponentType);

            int valuesBufferView = accessor["sparse"]["values"]["bufferView"];
            int valuesOffset = accessor["sparse"]["values"]["byteOffset"];
            Accessor::Sparse::Values values(valuesBufferView, valuesOffset);

            Accessor::Sparse sparse(sparseCount, indices, values);
            accessors.emplace_back(componentType, count, type, bufferView, offset, normalized, maxAdr, minAdr, name, sparse);
            continue;
        }

        Accessor::Sparse::Indices indices(0, 0, 0);
        Accessor::Sparse::Values values(0, 0);
        Accessor::Sparse sparse(0, indices, values);
        accessors.emplace_back(componentType, count, type, bufferView, offset, normalized, maxAdr, minAdr, name, sparse);
    }

    for(auto &mesh : data["meshes"]) {
        std::vector<Mesh::Primitive> primitives;
        for(auto &primitive : mesh["primitives"]) {
            std::vector<std::string> attributes;
            std::vector<int> attributeValues;
            for(auto itr = primitive["attributes"].begin(); itr != primitive["attributes"].end(); ++itr) {
                attributes.push_back(itr.key());
                attributeValues.push_back(itr.value());
            }

            int indices = primitive.value("indices", -1);
            int material = primitive.value("material", -1);
            int mode = primitive.value("mode", GL_TRIANGLES);

            std::vector<std::string> targets;
            std::vector<int> targetValues;
            for(auto itr = primitive["targets"].begin(); itr != primitive["targets"].end(); ++itr) {
                targets.push_back(itr.key());
                targetValues.push_back(itr.value());
            }

            primitives.emplace_back(indices, material, mode, attributes, attributeValues, targets, targetValues);
        }

        std::vector<float> weights;
        for(auto &weight : mesh["weights"]) weights.push_back(weight);

        std::string name = mesh["name"];
        meshes.emplace_back(primitives, weights, name);
    }

    for(auto &sampler : data["samplers"]) {
        int magFilter = sampler.value("magFilter", 0);
        int minFilter = sampler.value("minFilter", 0);
        int wrapS = sampler.value("wrapS", GL_REPEAT);
        int wrapT = sampler.value("wrapT", GL_REPEAT);
        std::string name = sampler.value("name", "");
        samplers.emplace_back(magFilter, minFilter, wrapS, wrapT, name);
    }

    for(auto &texture : data["textures"]) {
        int sampler = texture.value("sampler", -1);
        int source = texture.value("source", -1);
        std::string name = texture.value("name", "");
        textures.emplace_back(sampler, source, name);
    }

    for(auto &image : data["images"]) {
        std::string uri = image.value("uri", "");
        std::string mimeType = image.value("mimeType", "image/jpeg");
        int bufferView = image.value("bufferView", -1);
        std::string name = image.value("name", "");
        images.emplace_back(uri, mimeType, bufferView, name);
    }

    for(auto &material : data["materials"]) {
        std::string name = material.value("name", "");

        glm::vec4 baseColor(1);
        int baseTextureIndex = -1, baseTexCoord = 0, mrTextureIndex = -1, mrTexCoord = 0;
        float metallic = 1, roughness = 1;
        if(material.contains("pbrMetallicRoughness")) {
            auto pbrMR = material["pbrMetallicRoughness"];
            int i = 0;
            for (auto &color: pbrMR["baseColorFactor"]) baseColor[i++] = color;

            if(pbrMR.contains("baseColorTexture")) {
                baseTextureIndex = pbrMR["baseColorTexture"].value("index", -1);
                baseTexCoord = pbrMR["baseColorTexture"].value("texCoord", 0);
            }

            metallic = pbrMR.value("metallicFactor", 1);
            roughness = pbrMR.value("roughnessFactor", 1);

            if(pbrMR.contains("metallicRoughnessTexture")) {
                mrTextureIndex = pbrMR["metallicRoughnessTexture"].value("index", -1);
                mrTexCoord = pbrMR["metallicRoughnessTexture"].value("texCoord", 0);
            }
        }

        int normalTextureIndex = -1, normalTexCoord = 0;
        float normalScale = 1;
        if(material.contains("normalTexture")) {
            normalTextureIndex = material["normalTexture"].value("index", -1);
            normalTexCoord = material["normalTexture"].value("texCoord", 0);
            normalScale = material["normalTexture"].value("scale", 1);
        }

        int occlusionTextureIndex = -1, occlusionTexCoord = 0;
        float occlusionStrength = 1;
        if(material.contains("occlusionTexture")) {
            int occlusionTextureIndex = material["occlusionTexture"].value("index", -1);
            int occlusionTexCoord = material["occlusionTexture"].value("texCoord", 0);
            float occlusionStrength = material["occlusionTexture"].value("strength", 1);
        }

        int emissiveTextureIndex = -1, emissiveTexCoord = 0;
        if(material.contains("emissiveTexture")) {
            int emissiveTextureIndex = material["emissiveTexture"].value("index", -1);
            int emissiveTexCoord = material["emissiveTexture"].value("texCoord", 0);
        }
        glm::vec3 emissiveFactor(0);
        int i = 0;
        for(auto coef : material["emissiveFactor"]) emissiveFactor[i++] = coef;

        std::string alphaMode = material.value("alphaMode", "OPAQUE");
        float alphaCutoff = material.value("alphaCutoff", 0.5f);
        bool doubleSided = material.value("doubleSided", false);

        materials.emplace_back(name, baseColor, baseTextureIndex, baseTexCoord, metallic, roughness, mrTextureIndex, mrTexCoord,
                               normalTextureIndex, normalTexCoord, normalScale, occlusionTextureIndex, occlusionTexCoord,
                               occlusionStrength, emissiveTextureIndex, emissiveTexCoord, emissiveFactor, alphaMode, alphaCutoff, doubleSided);
    }

    for(auto &animation : data["animations"]) {
        std::string name = animation.value("name", "");

        std::vector<Animation::Channel> channels;
        for(auto &channel : animation["channels"]) {
            int sampler = channel["sampler"];
            int node = channel["target"].value("node", -1);
            std::string path = channel["target"]["path"];
            channels.emplace_back(sampler, node, path);
        }

        std::vector<Animation::Sampler> samplers;
        for(auto &sampler : animation["samplers"]) {
            int input = sampler["input"];
            int output = sampler["output"];
            std::string interpolation = sampler.value("interpolation", "LINEAR");
            samplers.emplace_back(input, interpolation, output);
        }

        animations.emplace_back(channels, samplers, name);
    }

    for(auto &node : data["nodes"]) {
        int camera = node.value("camera", -1);
        int skin = node.value("skin", -1);
        int mesh = node.value("mesh", -1);
        std::string name = node.value("name", "");

        std::vector<int> children;
        for(auto child : node["children"]) children.push_back(child);

        glm::mat4 matrix(1);
        if(node.contains("matrix"))
            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 4; j++) matrix[i][j] = node["matrix"][j * 4 + i];

        glm::quat rotation = {0, 0, 0, 1};
        if(node.contains("rotation")) for(int i = 0; i < 4; i++) rotation[i] = node["rotation"][i];

        glm::vec3 scale = {1, 1, 1};
        if(node.contains("scale")) for(int i = 0; i < 3; i++) scale[i] = node["scale"][i];

        glm::vec3 translation = {0, 0, 0};
        if(node.contains("translation")) for(int i = 0; i < 3; i++) translation[i] = node["translation"][i];

        std::vector<float> weights;
        for(auto& weight : node["weights"]) weights.push_back(weight);

        nodes.emplace_back(camera, children, skin, matrix, mesh, rotation, scale, translation, weights, name);
    }

    for(auto &scene : data["scenes"]) {
        std::string name = scene["name"];

        std::vector<int> nodes;
        for(auto node : scene["nodes"]) nodes.push_back(node);

        scenes.emplace_back(nodes, name);
    }

    return;
}

int gltfReader::getSceneCount() {
    return scenes.size();
}

Scene &gltfReader::getScene(int index) {
    auto &scene = *new ::Scene();
    for(int node : scenes[index].nodes) {
        auto &model = *new Model();
        insertChildNodes(model, node);
        scene += model;
    }

    return scene;
}

void gltfReader::insertChildNodes(Model &model, int index) {
    model.applyTransformation(nodes[index].translation, nodes[index].rotation, nodes[index].scale);

    for(int node : nodes[index].children) {
        auto &newModel = *new Model();
        model += newModel;

        insertChildNodes(newModel, node);
    }
}

gltfReader::Buffer::Buffer(std::string &uri, std::size_t size, std::string &name, std::string location) :
size(size), name(std::move(name)){
    if(uri.substr(0, 5) == "data:") {
        char *data = new char[size];
        int start = 5;
        while(uri[start] != ',') start++;
        for(int i = ++start; i < start + size; i++) data[i - start] = uri.at(i);
        return;
    }

    this->uri = location + '/' + uri;
}

gltfReader::Buffer::~Buffer() {
    delete [] data;
}

gltfReader::BufferView::BufferView(int buffer, int size, int offset, int stride, int target, std::string &name)
: buffer(buffer), size(size), offset(offset), stride(stride), name(std::move(name)), target(target) {}

gltfReader::Accessor::Accessor(int componentType, int count, std::string &type, int bufferView, int offset, bool normalized, double *max, double *min, std::string &name, Sparse &sparse) :
componentType(componentType), count(count), bufferView(bufferView), type(type), offset(offset), normalized(normalized), name(name), sparse(sparse) {
    int len = 0;
    if(type == "SCALAR") len = 1;
    else if(type == "VEC2") len = 2;
    else if(type == "VEC3") len = 3;
    else if(type == "VEC4") len = 4;
    else if(type == "MAT2") len = 4;
    else if(type == "MAT3") len = 9;
    else if(type == "MAT4") len = 16;

    if(max) for(int i = 0; i < len; i++) this->max[i] = max[i];
    if(min) for(int i = 0; i < len; i++) this->min[i] = min[i];
}

gltfReader::Accessor::Sparse::Sparse(int count, gltfReader::Accessor::Sparse::Indices &indices, gltfReader::Accessor::Sparse::Values &values) :
count(count), indices(indices), values(values) {}

gltfReader::Accessor::Sparse::Indices::Indices(int bufferView, int offset, int componentType) :
bufferView(bufferView), offset(offset), componentType(componentType) {}

gltfReader::Accessor::Sparse::Values::Values(int bufferView, int offset) : bufferView(bufferView), offset(offset) {}

gltfReader::Node::Node(int camera, std::vector<int> &children, int skin, glm::mat4 &matrix, int mesh, glm::quat &rotation, glm::vec3 &scale, glm::vec3 &translation, std::vector<float> &weights, std::string &name) :
camera(camera), children(std::move(children)), skin(skin), matrix(std::move(matrix)), mesh(mesh), weights(std::move(weights)), name(std::move(name)), translation(std::move(translation)), rotation(std::move(rotation)), scale(std::move(scale)) {
    if(matrix == glm::mat4(1)) {
        glm::mat4 translationMatrix(1);
        translationMatrix = glm::translate(translationMatrix, translation);

        glm::quat rot = rotation;
        glm::mat4 rotationMatrix = glm::mat4_cast(rot);

        glm::mat4 scalingMatrix(1);
        scalingMatrix = glm::scale(scalingMatrix, scale);

        this->matrix = translationMatrix * rotationMatrix * scalingMatrix;
    }
    else {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(this->matrix, this->scale, this->rotation, this->translation, skew, perspective);
        this->rotation = glm::conjugate(this->rotation);
    }
}

gltfReader::Scene::Scene(std::vector<int> &nodes, std::string &name) : nodes(std::move(nodes)), name(name) {}

gltfReader::Animation::Channel::Channel(int sampler, int node, std::string &path) : sampler(sampler), node(node), path(std::move(path)) {}

gltfReader::Animation::Sampler::Sampler(int input, std::string &interpolation, int output) : input(input), interpolation(std::move(interpolation)), output(output) {}

gltfReader::Animation::Animation(std::vector<Channel> &channels, std::vector<Sampler> &samplers, std::string &name) :
channels(std::move(channels)), samplers(std::move(samplers)), name(std::move(name)) {}

gltfReader::Mesh::Primitive::Primitive(int indices, int material, int mode, std::vector<std::string> &attributes, std::vector<int> &attributeValues, std::vector<std::string> &targets, std::vector<int> &targetValues) :
indices(indices), material(material), mode(mode), attributes(std::move(attributes)), attributeValues(std::move(attributeValues)), targets(std::move(targets)), targetValues(std::move(targetValues)) {}

gltfReader::Mesh::Mesh(std::vector<Primitive> &primitives, std::vector<float> &weights, std::string &name) :
primitives(std::move(primitives)), weights(std::move(weights)), name(std::move(name)) {}

gltfReader::Material::Material(std::string &name, glm::vec4 &baseColor, int baseTextureIndex, int baseTexCoord,
float metallic, float roughness, int mrTextureIndex, int mrTexCoord, int normalTextureIndex, int normalTexCoord,
float normalScale, int occlusionTextureIndex, int occlusionTexCoord, float occlusionStrength, int emissiveTextureIndex,
int emissiveTexCoord, glm::vec3 &emissiveFactor, std::string &alphaMode, float alphaCutoff, bool doubleSided) :
name(std::move(name)), baseColor(std::move(baseColor)), baseTextureIndex(baseTextureIndex), baseTexCoord(baseTexCoord),
metallic(metallic), roughness(roughness), mrTextureIndex(mrTextureIndex), mrTexCoord(mrTexCoord),
normalTextureIndex(normalTextureIndex), normalTexCoord(normalTexCoord), normalScale(normalScale),
occlusionTextureIndex(occlusionTextureIndex), occlusionTexCoord(occlusionTexCoord), occlusionStrength(occlusionStrength),
emissiveTextureIndex(emissiveTextureIndex), emissiveTexCoord(emissiveTexCoord), emissiveFactor(emissiveFactor),
alphaMode(std::move(alphaMode)), alphaCutoff(alphaCutoff), doubleSided(doubleSided) {}

gltfReader::Texture::Texture(int sampler, int source, std::string &name) : sampler(sampler), source(source), name(std::move(name)) {}

gltfReader::Image::Image(std::string &uri, std::string &mimeType, int bufferView, std::string &name) :
uri(std::move(uri)), mimeType(std::move(mimeType)), bufferView(bufferView), name(std::move(name)) {}

gltfReader::Sampler::Sampler(int magFilter, int minFilter, int wrapS, int wrapT, std::string &name) :
magFilter(magFilter), minFilter(minFilter), wrapT(wrapT), wrapS(wrapS), name(std::move(name)) {}

gltfReader::Skin::Skin(int inverseBindMatrices, int skeleton, std::vector<int> &joints, std::string &name) :
inverseBindMatrices(inverseBindMatrices), skeleton(skeleton), joints(std::move(joints)), name(std::move(name)) {}
