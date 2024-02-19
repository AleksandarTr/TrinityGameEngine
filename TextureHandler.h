#ifndef TRINITYENGINE_TEXTUREHANDLER_H
#define TRINITYENGINE_TEXTUREHANDLER_H

#include <queue>
#include <thread>
#include <utility>
#include "Texture.h"

class TextureHandler {
private:
    TextureHandler() = default;

    struct Job {
        TextureInfo info;
        unsigned char* data = nullptr;
        Texture** destination = nullptr;
        Texture* result = nullptr;
    };

    struct Node {
        Job job;
        Node* next = nullptr;

        Node(Job job) : job(std::move(job)) {}
    };

    Node* loadedTextures = nullptr;
    Node* lastTexture = nullptr;

    std::queue<Job> jobs;
    std::queue<Job*> awaitingLoading;
    std::thread *textureThread;
public:
    static TextureHandler& getTextureHandler();

    void loadTexture(TextureInfo info, Texture **destination);

    void assignTexture();

    static void loadInMemory();
};

#endif
