#include <fstream>
#include "gltfReader.h"
#include <glm/gtx/transform.hpp>

gltfReader::gltfReader(std::string uri) {
    std::ifstream reader(uri);
    nlohmann::json data = nlohmann::json::parse(reader);
    reader.close();

    int lastChar = uri.length();
    for(; lastChar > 0; lastChar--) if(uri[lastChar] == '/') break;
    uri = uri.substr(0, lastChar);

    for(auto buffer : data["buffers"]) {
        std::string buffUri = buffer["uri"];
        std::size_t size = buffer["byteLength"];
        std::string name = buffer.value("name", "");
        buffers.emplace_back(buffUri, size, name, uri);
    }

    for(auto bufferView : data["bufferViews"]) {
        int buffer = bufferView["buffer"];
        int offset = bufferView.value("byteOffset", 0);
        int size = bufferView["byteLength"];
        int stride = bufferView.value("byteStride", 0);
        int target = bufferView.value("target", 0);
        std::string name = bufferView.value("name", "");
        bufferViews.emplace_back(buffer, size, offset, stride, target, name);
    }

    for(auto accessor : data["accessors"]) {
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

        accessors.emplace_back(componentType, count, type, bufferView, offset, normalized, maxAdr, minAdr, name);
    }

    for(auto node : data["nodes"]) {
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
        for(auto weight : node["weights"]) weights.push_back(weight);

        nodes.emplace_back(camera, children, skin, matrix, mesh, rotation, scale, translation, weights, name);
    }

    for(auto scene : data["scenes"]) {
        std::string name = scene["name"];

        std::vector<int> nodes;
        for(auto node : scene["nodes"]) nodes.push_back(node);

        scenes.emplace_back(nodes, name);
    }

    return;
}

gltfReader::Buffer::Buffer(std::string uri, std::size_t size, std::string name, std::string location) :
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

gltfReader::BufferView::BufferView(int buffer, int size, int offset, int stride, int target, std::string name)
: buffer(buffer), size(size), offset(offset), stride(stride), name(std::move(name)), target(target) {}

gltfReader::Accessor::Accessor(int componentType, int count, std::string type, int bufferView, int offset, bool normalized, double *max, double *min, std::string name, Sparse sparse) :
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

gltfReader::Accessor::Sparse::Sparse(int count, gltfReader::Accessor::Sparse::Indices indices, gltfReader::Accessor::Sparse::Values values) :
count(count), indices(indices), values(values) {}

gltfReader::Accessor::Sparse::Indices::Indices(int bufferView, int offset, int componentType) :
bufferView(bufferView), offset(offset), componentType(componentType) {}

gltfReader::Accessor::Sparse::Values::Values(int bufferView, int offset) : bufferView(bufferView), offset(offset) {}

gltfReader::Node::Node(int camera, std::vector<int> children, int skin, glm::mat4 matrix, int mesh, glm::quat rotation, glm::vec3 scale, glm::vec3 translation, std::vector<float> weights, std::string name) :
camera(camera), children(std::move(children)), skin(skin), matrix(matrix), mesh(mesh), weights(std::move(weights)), name(std::move(name)), translation(translation), rotation(rotation), scale(scale) {
    if(matrix == glm::mat4(1)) {
        glm::mat4 translationMatrix(1);
        translationMatrix = glm::translate(translationMatrix, translation);

        glm::quat rot = rotation;
        glm::mat4 rotationMatrix = glm::mat4_cast(rot);

        glm::mat4 scalingMatrix(1);
        scalingMatrix = glm::scale(scalingMatrix, scale);

        this->matrix = translationMatrix * rotationMatrix * scalingMatrix;
    }
}

gltfReader::Scene::Scene(std::vector<int> nodes, std::string name) : nodes(std::move(nodes)), name(name) {}
