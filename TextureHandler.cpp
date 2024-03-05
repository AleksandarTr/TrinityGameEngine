#include <iostream>
#include "TextureHandler.h"

TextureHandler::Node* TextureHandler::loadedTextures = nullptr;
std::priority_queue<TextureHandler::Job, std::vector<TextureHandler::Job>, TextureHandler::higherPriority> TextureHandler::jobs;
std::mutex TextureHandler::jobMutex;
std::queue<TextureHandler::Job*> TextureHandler::awaitingLoading;
std::mutex TextureHandler::loadingMutex;
int TextureHandler::activeTextures[TE_TextureTypeCount] = {0};
volatile bool TextureHandler::close = false;
std::thread TextureHandler::textureThread(&TextureHandler::loadInMemory);

void TextureHandler::loadTexture(TextureInfo info, Texture *destination) {
    Job job = {.info = std::move(info), .destination = destination};
    {
        std::scoped_lock<std::mutex> lock(jobMutex);
        jobs.push(job);
    }
}

void TextureHandler::loadInMemory() {
    while(!close) {
        if(jobs.empty()) continue;
        Job job;
        {
            std::scoped_lock<std::mutex> lock(jobMutex);
            job = jobs.top();
            jobs.pop();
        }

        bool found = false;
        Node *itr = loadedTextures;
        while(itr) {
            if (job.info.location == itr->job.info.location) {
                while (itr->job.data);
                itr->job.data = reinterpret_cast<unsigned char *>(1);
                itr->job.destination = job.destination;
                {
                    std::scoped_lock<std::mutex> lock(loadingMutex);
                    awaitingLoading.push(&itr->job);
                }
                found = true;
                break;
            }
            itr = itr->next;
        }
        if(found) continue;

        int imgW, imgH, colChNum;
        stbi_set_flip_vertically_on_load(true);
        int desiredChannels;
        if(job.info.format == "image/jpeg") desiredChannels = STBI_rgb;
        else if(job.info.format == "image/png") desiredChannels = STBI_rgb_alpha;
        else desiredChannels = 0;

        unsigned  char* bytes = stbi_load(job.info.location.c_str(), &imgW, &imgH, &colChNum, desiredChannels);

        job.data = bytes;
        job.info.width = imgW;
        job.info.height = imgH;

        Node *texture = new Node(job);
        texture->next = loadedTextures;
        loadedTextures = texture;

        std::scoped_lock<std::mutex> lock(loadingMutex);
        awaitingLoading.push(&loadedTextures->job);
    }
}

void TextureHandler::assignTexture() {
    while(!awaitingLoading.empty()) {
        Job *job;
        {
            std::scoped_lock<std::mutex> lock(loadingMutex);
            job = awaitingLoading.front();
            awaitingLoading.pop();
        }

        if(!job->result) {
            job->result = job->destination;
            GLuint textureId;
            glGenTextures(1, &textureId);
            job->destination->setId(textureId);
            glBindTexture(GL_TEXTURE_2D, job->result->getId());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, job->info.minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, job->info.magFilter);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, job->info.wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, job->info.wrapT);

            int format;
            if (job->info.format == "image/png") format = GL_RGBA;
            else if (job->info.format == "image/jpeg") format = GL_RGB;
            else format = GL_RED;

            glTexImage2D(GL_TEXTURE_2D, 0, format, job->info.width, job->info.height, 0, format, GL_UNSIGNED_BYTE, job->data);
            glGenerateMipmap(GL_TEXTURE_2D);
            job->result->unbind();
            job->result->getInfo() = job->info;

            stbi_image_free(job->data);
        }

        job->destination->setId(job->result->getId());
        job->info.type = std::max(job->info.type, static_cast<TextureType>(0));
        job->info.type = std::min(job->info.type, static_cast<TextureType>(TE_TextureTypeCount - 1));
        job->destination->getInfo() = job->result->getInfo();
        job->data = nullptr;
    }
}

void TextureHandler::bindTexture(Texture &texture, GLenum target) {
    if(activeTextures[static_cast<int>(texture.getInfo().type)] != texture.getId()) {
        activeTextures[static_cast<int>(texture.getInfo().type)] = texture.getId();
        glActiveTexture(GL_TEXTURE0 + static_cast<int>(texture.getInfo().type));
        texture.bind(target);
    }
}

void TextureHandler::killTextureHandler() {
    close = true;
    textureThread.join();
}

void TextureHandler::resetActiveTextures() {
    for(int i = 0; i < TE_TextureTypeCount; i++) activeTextures[i] = 0;
}

bool TextureHandler::higherPriority::operator()(Job &job1, Job &job2) {
    return job1.info.type > job2.info.type || (job1.info.type == job2.info.type && !job2.result);
}
