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
    static std::mutex jobMutex; //Mutex for assigning and taking jobs
    static std::queue<Job*> awaitingLoading;
    static std::mutex loadingMutex; //Mutex for getting and taking finished jobs
    static int activeTextures[TE_TextureTypeCount];
    static volatile bool close;
    static std::thread textureThread;

    static void loadInMemory(); //Load texture into memory and wait for it to be bound

public:
    static void loadTexture(TextureInfo info, Texture *destination); //Load given texture and put it in the given destination

    static void assignTexture(); //Load textures into openGL

    static void bindTexture(Texture &texture, GLenum target = GL_TEXTURE_2D); //Bind texture, which needs to be done each frame

    static void resetActiveTextures(); //Reset buffer which reduces unnecessary texture bindings, which needs to be done each frame

    static void killTextureHandler(); //End the thread on which the texture loader is (texture loader cannot be started again)
};

#endif
