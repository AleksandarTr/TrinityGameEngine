#include "TextureHandler.h"
#include <iostream>

TextureHandler &TextureHandler::getTextureHandler() {
    static TextureHandler textureHandler;
    return textureHandler;
}

void TextureHandler::loadTexture(TextureInfo info, Texture **destination) {
    Job job = {.info = std::move(info), .destination = destination};
    jobs.push(job);
    if(!textureThread) textureThread = new std::thread(&TextureHandler::loadInMemory);
}

void TextureHandler::loadInMemory() {
    while(true) {
        if(getTextureHandler().jobs.empty()) continue;
        Job job = getTextureHandler().jobs.front();
        getTextureHandler().jobs.pop();
        std::cout << "Loaded: " << job.info.location << std::endl;

        bool found = false;
        Node *itr = getTextureHandler().loadedTextures;
        while(itr) {
            if (job.info.location == itr->job.info.location) {
                while (itr->job.data);
                itr->job.data = reinterpret_cast<unsigned char *>(1);
                itr->job.destination = job.destination;
                getTextureHandler().awaitingLoading.push(&itr->job);
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
        if(getTextureHandler().loadedTextures) getTextureHandler().lastTexture = texture;
        else getTextureHandler().loadedTextures = texture;
        getTextureHandler().lastTexture = texture;

        getTextureHandler().awaitingLoading.push(&getTextureHandler().lastTexture->job);
    }
}

void TextureHandler::assignTexture() {
    while(!awaitingLoading.empty()) {
        Job *job = awaitingLoading.front();
        awaitingLoading.pop();

        if(!job->result) {
            job->result = new Texture();
            glGenTextures(1, &job->result->textureId);
            glActiveTexture(GL_TEXTURE0 + static_cast<int>(job->info.type));
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

            stbi_image_free(job->data);
        }

        *job->destination = job->result;
        job->data = nullptr;
    }
}