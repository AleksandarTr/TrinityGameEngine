#ifndef TRINITYENGINE_TEXTUREHANDLER_H
#define TRINITYENGINE_TEXTUREHANDLER_H

#include <queue>
#include <utility>
#include <thread>
#include <mutex>
#include "Texture.h"
#include "Mesh.h"

class TextureHandler {
private:
    TextureHandler() = default;

    struct Job {
        TextureInfo info;
        unsigned char* data = nullptr;
        Texture* destination = nullptr;
        Texture* result = nullptr;
    };

    struct higherPriority {
        bool operator()(Job &, Job &);
    };

    struct Node {
        Job job;
        Node* next = nullptr;

        Node(Job job) : job(std::move(job)) {}
    };

    Node* loadedTextures = nullptr;
    Node* lastTexture = nullptr;

    std::priority_queue<Job, std::vector<Job>, higherPriority> jobs;
    std::mutex jobMutex;
    std::queue<Job*> awaitingLoading;
    std::mutex loadingMutex;
    static int activeTextures[static_cast<unsigned long long>(TextureType::Count)];
    static volatile bool close;
    static std::thread textureThread;

public:
    static TextureHandler& getTextureHandler();

    void loadTexture(TextureInfo info, Texture *destination);

    void assignTexture();

    static void bindTexture(Texture &texture);

    static void loadInMemory();

    static void killTextureHandler();
};

#endif
