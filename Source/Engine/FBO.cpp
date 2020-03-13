#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>

#include "FBO.h"
namespace PE
{
    void FBO::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, buffers);
    }

    void FBO::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PrerenderFBO::Create(int _width, int _height)
    {
        if (_width == 0 || _height == 0)
            return;

        width = _width;
        height = _height;

        // Create FBO
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        // Add depth buffer
        glGenRenderbuffers(1, &gDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);

        // position buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // normal buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        // diffuse buffer
        glGenTextures(1, &gDiffuse);
        glBindTexture(GL_TEXTURE_2D, gDiffuse);
        glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffuse, 0);

        // specular buffer
        glGenTextures(1, &gSpecular);
        glBindTexture(GL_TEXTURE_2D, gSpecular);
        glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);

        // Check for completeness/correctness
        int status = (int)glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != int(GL_FRAMEBUFFER_COMPLETE))
            std::cout << "FBO Error: " << status << std::endl;

        // Unbind the fbo until it's ready to be used
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowFBO::Create(int _width, int _height)
    {
        width = _width;
        height = _height;

        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        // depth buffer used as shadow map.
        glGenRenderbuffers(1, &gDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
            width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER, gDepth);

        // position buffer
        glGenTextures(1, &gShadow);
        glBindTexture(GL_TEXTURE_2D, gShadow);
        glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gShadow, 0);

        // Check for completeness/correctness
        int status = (int)glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != int(GL_FRAMEBUFFER_COMPLETE))
            std::cout << "FBO Error: " << status << std::endl;

        // Unbind the fbo until it's ready to be used
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
