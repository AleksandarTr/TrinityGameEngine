#ifndef TRINITYENGINE_TEXTUREHANDLER_H
#define TRINITYENGINE_TEXTUREHANDLER_H

#include <queue>
#include <thread>
#include <utility>
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
    std::queue<Job*> awaitingLoading;
    std::thread *textureThread;
    static int activeTextures[static_cast<unsigned long long>(TextureType::Count)];
public:
    static TextureHandler& getTextureHandler();

    void loadTexture(TextureInfo info, Texture *destination);

    void assignTexture();

    static void bindTexture(Texture &texture);

    [[noreturn]] static void loadInMemory();
};

#endif
