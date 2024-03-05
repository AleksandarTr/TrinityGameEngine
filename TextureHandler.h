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

    static Node* loadedTextures;

    static std::priority_queue<Job, std::vector<Job>, higherPriority> jobs;
    static std::mutex jobMutex;
    static std::queue<Job*> awaitingLoading;
    static std::mutex loadingMutex;
    static int activeTextures[TE_TextureTypeCount];
    static volatile bool close;
    static std::thread textureThread;

public:
    static void loadTexture(TextureInfo info, Texture *destination);

    static void assignTexture();

    static void bindTexture(Texture &texture, GLenum target = GL_TEXTURE_2D);

    static void resetActiveTextures();

    static void loadInMemory();

    static void killTextureHandler();
};

#endif
