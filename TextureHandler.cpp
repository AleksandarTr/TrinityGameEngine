#include <iostream>
#include "TextureHandler.h"

std::thread TextureHandler::textureThread(&TextureHandler::loadInMemory);
volatile bool TextureHandler::close = false;

int TextureHandler::activeTextures[TE_TextureTypeCount] = {0};

TextureHandler &TextureHandler::getTextureHandler() {
    static TextureHandler textureHandler;
    return textureHandler;
}

void TextureHandler::loadTexture(TextureInfo info, Texture *destination) {
    Job job = {.info = std::move(info), .destination = destination};
    {
        std::scoped_lock<std::mutex> lock(jobMutex);
        jobs.push(job);
    }
}

void TextureHandler::loadInMemory() {
    while(!close) {
        if(getTextureHandler().jobs.empty()) continue;
        Job job;
        {
            std::scoped_lock<std::mutex> lock(getTextureHandler().jobMutex);
            job = getTextureHandler().jobs.top();
            getTextureHandler().jobs.pop();
        }

        bool found = false;
        Node *itr = getTextureHandler().loadedTextures;
        while(itr) {
            if (job.info.location == itr->job.info.location) {
                while (itr->job.data);
                itr->job.data = reinterpret_cast<unsigned char *>(1);
                itr->job.destination = job.destination;
                {
                    std::scoped_lock<std::mutex> lock(getTextureHandler().loadingMutex);
                    getTextureHandler().awaitingLoading.push(&itr->job);
                }
                found = true;
                break;
            }
            itr = itr->next;
        }
        if(found) continue;

        int imgW, imgH, colChNum;
        stbi_set_flip_vertically_on_load(true);
        unsigned  char* bytes = stbi_load(job.info.location.c_str(), &imgW, &imgH, &colChNum, 0);

        job.data = bytes;
        job.info.width = imgW;
        job.info.height = imgH;

        Node *texture = new Node(job);
        texture->next = getTextureHandler().loadedTextures;
        getTextureHandler().loadedTextures = texture;

        std::scoped_lock<std::mutex> lock(getTextureHandler().loadingMutex);
        getTextureHandler().awaitingLoading.push(&getTextureHandler().loadedTextures->job);
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
            glGenTextures(1, &job->result->textureId);
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
            job->result->info = job->info;

            stbi_image_free(job->data);
        }

        job->destination->textureId = job->result->textureId;
        job->info.type = std::max(job->info.type, static_cast<TextureType>(0));
        job->info.type = std::min(job->info.type, static_cast<TextureType>(TE_TextureTypeCount - 1));
        job->destination->info = job->result->info;
        job->data = nullptr;
    }
}

void TextureHandler::bindTexture(Texture &texture, GLenum target) {
    if(activeTextures[static_cast<int>(texture.info.type)] != texture.textureId) {
        activeTextures[static_cast<int>(texture.info.type)] = texture.textureId;
        glActiveTexture(GL_TEXTURE0 + static_cast<int>(texture.info.type));
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
