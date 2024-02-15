#include <fstream>
#include "gltfReader.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include "SingleTextureMesh.h"
#include "Model.h"

bool isBigEndian() {
    int test = 1;

    return *(char*)&test != 1;
}

gltfReader::gltfReader(std::string uri, Shader &shader) : shader(shader) {
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

        double max[16];
        double *maxAdr = accessor.contains("max") ? max : nullptr;
        for(int i = 0; i < accessor["max"].size(); i++) max[i] = accessor["max"][i];

        double min[16];
        double *minAdr = accessor.contains("min") ? min : nullptr;
        for(int i = 0; i < accessor["min"].size(); i++) min[i] = accessor["min"][i];

        if(accessor.contains("sparse")) {
            int sparseCount = accessor["sparse"]["count"];

            int indicesBufferView = accessor["sparse"]["indices"]["bufferView"];
            int indicesComponentType = accessor["sparse"]["indices"]["componentType"];
            int indicesOffset = 0;
            if(accessor["sparse"]["indices"].contains("byteOffset")) indicesOffset = accessor["sparse"]["indices"]["byteOffset"];
            Accessor::Sparse::Indices indices(indicesBufferView, indicesOffset, indicesComponentType);

            int valuesBufferView = accessor["sparse"]["values"]["bufferView"];
            int valuesOffset = 0;
            if(accessor["sparse"]["values"].contains("byteOffset")) valuesOffset = accessor["sparse"]["values"]["byteOffset"];
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
        std::string imageUri = image.value("uri", "");
        if(!imageUri.empty()) imageUri = uri + '/' + imageUri;
        std::string mimeType = image.value("mimeType", "");
        int bufferView = image.value("bufferView", -1);
        std::string name = image.value("name", "");
        images.emplace_back(imageUri, mimeType, bufferView, name);
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
                baseTextureIndex = pbrMR["baseColorTexture"]["index"];
                baseTexCoord = pbrMR["baseColorTexture"].value("texCoord", 0);
            }

            metallic = pbrMR.value("metallicFactor", 1.0f);
            roughness = pbrMR.value("roughnessFactor", 1.0f);

            if(pbrMR.contains("metallicRoughnessTexture")) {
                mrTextureIndex = pbrMR["metallicRoughnessTexture"]["index"];
                mrTexCoord = pbrMR["metallicRoughnessTexture"].value("texCoord", 0);
            }
        }

        int normalTextureIndex = -1, normalTexCoord = 0;
        float normalScale = 1;
        if(material.contains("normalTexture")) {
            normalTextureIndex = material["normalTexture"]["index"];
            normalTexCoord = material["normalTexture"].value("texCoord", 0);
            normalScale = material["normalTexture"].value("scale", 1.0f);
        }

        int occlusionTextureIndex = -1, occlusionTexCoord = 0;
        float occlusionStrength = 1;
        if(material.contains("occlusionTexture")) {
            occlusionTextureIndex = material["occlusionTexture"]["index"];
            occlusionTexCoord = material["occlusionTexture"].value("texCoord", 0);
            occlusionStrength = material["occlusionTexture"].value("strength", 1.0f);
        }

        int emissiveTextureIndex = -1, emissiveTexCoord = 0;
        if(material.contains("emissiveTexture")) {
            emissiveTextureIndex = material["emissiveTexture"]["index"];
            emissiveTexCoord = material["emissiveTexture"].value("texCoord", 0);
        }
        glm::vec3 emissiveFactor(0);
        int i = 0;
        for(auto &coef : material["emissiveFactor"]) emissiveFactor[i++] = coef;

        std::string alphaMode = material.value("alphaMode", "OPAQUE");
        float alphaCutoff = material.value("alphaCutoff", 0.5f);
        bool doubleSided = material.value("doubleSided", false);

        materials.emplace_back(name, baseColor, baseTextureIndex, baseTexCoord, metallic, roughness, mrTextureIndex, mrTexCoord,
                               normalTextureIndex, normalTexCoord, normalScale, occlusionTextureIndex, occlusionTexCoord,
                               occlusionStrength, emissiveTextureIndex, emissiveTexCoord, emissiveFactor, alphaMode, alphaCutoff, doubleSided);
    }

    for(auto &skin : data["skins"]) {
        int inverseBindMatrices = skin.value("inverseBindMatrices", -1);
        int skeleton = skin.value("skeleton", -1);
        std::string name = skin.value("name", "");

        std::vector<int> joints;
        for(auto &joint : skin["joints"]) joints.push_back(joint);
        skins.emplace_back(inverseBindMatrices, skeleton, joints, name);
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

    for(auto &camera : data["cameras"]) {
        std::string type = camera["type"];
        std::string name = camera.value("name", "");

        float arg1, arg2, zfar, znear;
        if(type == "perspective") {
            arg1 = camera["perspective"]["aspectRatio"];
            arg2 = camera["perspective"]["yfov"];
            zfar = camera["perspective"]["zfar"];
            znear = camera["perspective"]["znear"];
        }
        else {
            arg1 = camera["orthographic"]["xmag"];
            arg2 = camera["orthographic"]["ymag"];
            zfar = camera["orthographic"]["zfar"];
            znear = camera["orthographic"]["znear"];
        }

        cameras.emplace_back(name, type, arg1, arg2, zfar, znear);
    }

    for(auto &node : data["nodes"]) {
        int camera = node.value("camera", -1);
        int skin = node.value("skin", -1);
        int mesh = node.value("mesh", -1);
        std::string name = node.value("name", "");

        std::vector<int> children;
        for(auto &child : node["children"]) children.push_back(child);

        glm::mat4 matrix(1);
        if(node.contains("matrix"))
            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 4; j++) matrix[i][j] = node["matrix"][j * 4 + i];

        glm::quat rotation = {1, 0, 0, 0};
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
        std::string name = scene.value("name", "");

        std::vector<int> nodes;
        for(auto &node : scene["nodes"]) nodes.push_back(node);

        scenes.emplace_back(nodes, name);
    }
}

int gltfReader::getSceneCount() {
    return scenes.size();
}

Model &gltfReader::getScene(int index) {
    std::vector<::Mesh*> dummy;
    auto &scene = *new Model(dummy);
    for(int node : scenes[index].nodes) {
        std::vector<::Mesh*> *meshes = nullptr;
        if(nodes[node].mesh != -1) meshes = &getMeshes(nodes[node].mesh);

        auto &model = *new Model(*meshes);
        insertChildNodes(model, node);
        scene += model;
    }

    delete [] meshBuffer;
    return scene;
}

void gltfReader::insertChildNodes(Model &model, int index) {
    for(int node : nodes[index].children) {
        std::vector<::Mesh*> *meshes = nullptr;
        if(nodes[node].mesh != -1) {
            meshes = &getMeshes(nodes[node].mesh);
        }

        auto &newModel = *new Model(*meshes);
        model += newModel;
        insertChildNodes(newModel, node);
    }

    model.transform(nodes[index].matrix);
    model.setLocalPosition();
}

std::vector<::Mesh *> &gltfReader::copyMeshes(int index) {
    auto &result = *new std::vector<::Mesh*>();

    for(int i = 0; i < meshBuffer[index]->size(); i++) {
        auto mesh = new SingleTextureMesh(dynamic_cast<SingleTextureMesh&>(*meshBuffer[index]->at(i)));
        mesh->bind();
        result.push_back(mesh);
    }

    return result;
}

std::vector<::Mesh *> &gltfReader::getMeshes(int index) {
    if(meshBuffer == nullptr) {
        meshBuffer = new std::vector< ::Mesh *>*[meshes.size()];
        for(int i = 0; i < meshes.size(); i++) meshBuffer[i] = nullptr;
    }
    if(meshBuffer[index] != nullptr) return copyMeshes(index);

    bool padding = isBigEndian();
    auto &result = *new std::vector<::Mesh*>();
    for(Mesh::Primitive primitive : meshes[index].primitives) {
        glm::vec3 color = materials[primitive.material].baseColor;

        GLenum drawMode = primitive.mode;
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        int ind = 0;
        while(ind < primitive.attributes.size() && primitive.attributes[ind] != "POSITION") ind++;
        if(ind == primitive.attributes.size()) throw std::invalid_argument("Cannot make a mesh without position of vertices of a primitive mesh.");

        int positionAccessor = primitive.attributeValues[ind];
        if(accessors[positionAccessor].type != "VEC3") throw std::invalid_argument("Vertex must be given as a 3d vector");
        if(accessors[positionAccessor].componentType != GL_FLOAT) throw std::invalid_argument("Vertex coordinates must be given as floats");
        int indexWidth, indexSize;
        char *positionData = readAccessor(positionAccessor, indexWidth, indexSize);

        ind = 0;
        char *normalData = nullptr;
        int normalAccessor = -1;
        int normalWidth, normalSize;
        while(ind < primitive.attributes.size() && primitive.attributes[ind] != "NORMAL") ind++;
        if(ind != primitive.attributes.size()) {
            normalAccessor = primitive.attributeValues[ind];
            normalData = readAccessor(normalAccessor, normalWidth, normalSize);
        }

        ind = 0;
        char *texCoordData = nullptr;
        int texCoordAccessor = -1;
        int texCoordWidth, texCoordSize;
        while(ind < primitive.attributes.size() && primitive.attributes[ind] != "TEXCOORD_0") ind++;
        if(ind != primitive.attributes.size()) {
            texCoordAccessor = primitive.attributeValues[ind];
            texCoordData = readAccessor(texCoordAccessor, texCoordWidth, texCoordSize);
        }

        char *indexData = nullptr;
        if(primitive.indices != -1) indexData = readAccessor(primitive.indices, indexWidth, indexSize);

        glm::vec3 position(0);
        glm::vec3 normal(0);
        glm::vec3 texCoord(0);

        for(int i = 0; i < accessors[positionAccessor].count; i++) {
            char *positionAdr = (char*) &position;
            memcpy(positionAdr, positionData + i * 12, 12);

            if(normalData) {
                char *normalAdr = (char*) &normal;
                memcpy(normalAdr, normalData + i * normalSize, normalWidth);
                if (normalSize / normalWidth >= 2)
                    memcpy(normalAdr + 4, normalData + i * normalSize + normalWidth,normalWidth);
                if (normalSize / normalWidth >= 3)
                    memcpy(normalAdr + 8, normalData + i * normalSize + 2 * normalWidth,normalWidth);
            }

            if(texCoordData) {
                char *texCoordAdr = (char*) &texCoord;
                memcpy(texCoordAdr, texCoordData + i * texCoordSize, texCoordWidth);
                if (texCoordSize / texCoordWidth >= 2)
                    memcpy(texCoordAdr + 4, texCoordData + i * texCoordSize + texCoordWidth,texCoordWidth);
                if (texCoordSize / texCoordWidth >= 3)
                    memcpy(texCoordAdr + 8,texCoordData + i * texCoordSize + 2 * texCoordWidth, texCoordWidth);
            }

            texCoord.y = -texCoord.y;

            vertices.push_back({position, color, texCoord, normal});
            position = glm::vec3(0);
            normal = glm::vec3(0);
            texCoord = glm::vec3(0);
        }

        int indexPoint = 0;
        char* indexAdr = (char*) &indexPoint;
        int pad = padding ? sizeof(int) - indexWidth : 0;
        if(indexData != nullptr) for(int i = 0; i < accessors[primitive.indices].count * indexWidth; i += indexWidth) {
            memcpy(indexAdr + pad, indexData + i, indexWidth);
            indices.push_back(indexPoint);
        }

        if(indices.size() % 3 == 0) for(int i = 0; i < indices.size(); i += 3) {
                Vertex &v0 = vertices[indices[i]];
                Vertex &v1 = vertices[indices[i+1]];
                Vertex &v2 = vertices[indices[i+2]];

                glm::vec3 e0 = v1.position - v0.position;
                glm::vec3 e1 = v2.position - v0.position;
                glm::vec2 deltaTC0 = v1.texPosition - v0.texPosition;
                glm::vec2 deltaTC1 = v2.texPosition - v0.texPosition;

                glm::vec3 tan = deltaTC1.y * e0 - deltaTC0.y * e1;
                v0.tangent = tan;
                v1.tangent = tan;
                v2.tangent = tan;

                v0.bitangent = glm::cross(v0.lightNormal, tan);
                v1.bitangent = glm::cross(v1.lightNormal, tan);
                v2.bitangent = glm::cross(v2.lightNormal, tan);
            }

        delete [] positionData;
        delete [] normalData;
        delete [] texCoordData;
        delete [] indexData;

        TextureInfo diffuseTexture;
        TextureInfo specularTexture;
        TextureInfo normalTexture;

        if(primitive.material != -1) {
            int diffuseTextureIndex = materials[primitive.material].baseTextureIndex;
            if (diffuseTextureIndex != -1) diffuseTexture = loadTexture(diffuseTextureIndex);

            int specularTextureIndex = materials[primitive.material].mrTextureIndex;
            if (specularTextureIndex != -1) specularTexture = loadTexture(specularTextureIndex);

            int normalTextureIndex = materials[primitive.material].normalTextureIndex;
            if (normalTextureIndex != -1) normalTexture = loadTexture(normalTextureIndex);
        }


        ::SingleTextureMesh& mesh = *new SingleTextureMesh(vertices, indices, shader, drawMode, diffuseTexture, specularTexture, normalTexture);
        mesh.bind();
        if(primitive.material != -1) {
            mesh.setMetallic(materials[primitive.material].metallic);
            mesh.setRoughness(materials[primitive.material].roughness);
        }
        result.push_back(&mesh);
    }

    meshBuffer[index] = &result;
    return result;
}

char* gltfReader::readAccessor(int accessor, int &width, int &size) {
    //TODO: Implement sparse accessors
    switch(accessors[accessor].componentType) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            size = 1;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            size = 2;
            break;
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
        default:
            size = 4;
            break;
    }
    width = size;

    if(accessors[accessor].type == "VEC2") size *= 2;
    else if(accessors[accessor].type == "VEC3") size *= 3;
    else if(accessors[accessor].type == "VEC4") size *= 4;
    else if(accessors[accessor].type == "MAT2") size *= 4;
    else if(accessors[accessor].type == "MAT3") size *= 9;
    else if(accessors[accessor].type == "MAT4") size *= 16;

    char *destination = new char[size * accessors[accessor].count];
    int bufferView = accessors[accessor].bufferView;
    if(bufferView == -1) {
        for(int i = 0; i < size * accessors[accessor].count; i++) destination[i] = 0;
        return destination;
    }

    int buffer = bufferViews[bufferView].buffer;
    bool readFile = buffers[buffer].data == nullptr;
    bool flip = isBigEndian();

    int stride = bufferViews[bufferView].stride - size;
    if(stride < 0) stride = 0;
    int pos = bufferViews[bufferView].offset + accessors[accessor].offset;

    int sectorSize = 0x4000;
    sectorSize -= sectorSize % size;
    char *sector = new char[sectorSize];
    char buf[width];
    if(readFile) {
        std::ifstream reader(buffers[buffer].uri, std::ios::binary);
        reader.seekg(pos);

        for(int i = 0; i < size * accessors[accessor].count; i += size) {
            if(i % sectorSize < size) {
                reader.read(sector, sectorSize);
                pos += sectorSize;
                reader.seekg(pos);
            }
            memcpy(destination + i, sector + i % sectorSize, size);

            if(flip) {
                for(int j = 0; j < size; j += width) {
                    for(int k = 0; k < width; k++) buf[k] = *(destination + i + j + k);
                    for(int k = 0; k < width; k++) *(destination + i + j + k) = buf[width - k - 1];
                }
            }
        }
    }
    else {
        for(int i = 0; i < size * accessors[accessor].count; i += size) {
            memcpy(destination + i, buffers[buffer].data + pos, size);
            pos += stride;

            for(int j = 0; j < size; j += width) {
                for(int k = 0; k < width; k++) buf[k] = *(destination + i + j + k);
                for(int k = 0; k < width; k++) *(destination + i + j + k) = buf[width - k - 1];
            }
        }
    }

    delete sector;
    return destination;
}

TextureInfo gltfReader::loadTexture(int textureId) {
    TextureInfo texture;
    if(textureId != -1) {
        texture.location = images[textures[textureId].source].uri;

        texture.format = images[textures[textureId].source].mimeType;
        if(texture.format.empty()) {
            int extension = texture.location.length() - 1;
            for(; extension > 0; extension--)
                if(texture.location[extension] == '.') break;

            if(texture.location.substr(extension + 1) == "jpg") texture.format = "image/jpeg";
            else if(texture.location.substr(extension + 1) == "jpeg") texture.format = "image/jpeg";
            else if(texture.location.substr(extension + 1) == "jpe") texture.format = "image/jpeg";
            else if(texture.location.substr(extension + 1) == "jfif") texture.format = "image/jpeg";
            else if(texture.location.substr(extension + 1) == "png") texture.format = "image/png";
        }

        if(textures[textureId].sampler != -1) {
            texture.magFilter = samplers[textures[textureId].sampler].magFilter;
            texture.minFilter = samplers[textures[textureId].sampler].minFilter;
            texture.wrapT = samplers[textures[textureId].sampler].wrapT;
            texture.wrapS = samplers[textures[textureId].sampler].wrapS;
        }
    }

    return texture;
}

gltfReader::Buffer::Buffer(std::string &uri, std::size_t size, std::string &name, std::string &location) :
size(size), name(std::move(name)){
    if(uri.substr(0, 5) == "data:") {
        char *data = new char[size];
        int start = 5;
        while(uri[start] != ',') start++;
        for(int i = ++start; i < start + size; i++) data[i - start] = uri.at(i);
        return;
    }

    data = nullptr;
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
    else this->max[0] = DBL_MIN;
    if(min) for(int i = 0; i < len; i++) this->min[i] = min[i];
    else this->min[0] = DBL_MAX;
}

gltfReader::Accessor::Sparse::Sparse(int count, gltfReader::Accessor::Sparse::Indices &indices, gltfReader::Accessor::Sparse::Values &values) :
count(count), indices(indices), values(values) {}

gltfReader::Accessor::Sparse::Indices::Indices(int bufferView, int offset, int componentType) :
bufferView(bufferView), offset(offset), componentType(componentType) {}

gltfReader::Accessor::Sparse::Values::Values(int bufferView, int offset) : bufferView(bufferView), offset(offset) {}

gltfReader::Node::Node(int camera, std::vector<int> &children, int skin, glm::mat4 &matrix, int mesh, glm::quat &rotation, glm::vec3 &scale, glm::vec3 &translation, std::vector<float> &weights, std::string &name) :
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
name(std::move(name)), baseColor(baseColor), baseTextureIndex(baseTextureIndex), baseTexCoord(baseTexCoord),
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

gltfReader::Camera::Camera(std::string &name, std::string &type, float arg1, float arg2, float zfar, float znear) :
name(std::move(name)), type(std::move(type)){
    orthographic.xmag = arg1;
    orthographic.ymag = arg2;
    orthographic.zfar = zfar;
    orthographic.znear = znear;
}
