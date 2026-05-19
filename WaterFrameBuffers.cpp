#include "WaterFrameBuffers.h"

WaterFrameBuffers::WaterFrameBuffers(int width, int height)
{
    initializeOpenGLFunctions();
    initialiseReflectionFrameBuffer();
    initialiseRefractionFrameBuffer();
}

WaterFrameBuffers::~WaterFrameBuffers()
{
    cleanUp();
}

void WaterFrameBuffers::cleanUp()
{
    glDeleteFramebuffers(1, &reflectionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);
    
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteTextures(1, &refractionDepthTexture);
}

void WaterFrameBuffers::bindReflectionFrameBuffer()
{
    bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterFrameBuffers::bindRefractionFrameBuffer()
{
    bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFrameBuffers::unbindCurrentFrameBuffer(int currentWidth, int currentHeight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, currentWidth, currentHeight);
}

void WaterFrameBuffers::initialiseReflectionFrameBuffer()
{
    reflectionFrameBuffer = createFrameBuffer();
    reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    
    // Route shader location 2 (gAlbedoSpec) to GL_COLOR_ATTACHMENT0 (reflectionTexture)
    GLenum drawBuffers[] = { GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(3, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug("Reflection Framebuffer not complete!");
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFrameBuffers::initialiseRefractionFrameBuffer()
{
    refractionFrameBuffer = createFrameBuffer();
    refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    
    // Route shader location 2 (gAlbedoSpec) to GL_COLOR_ATTACHMENT0 (refractionTexture)
    GLenum drawBuffers[] = { GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(3, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug("Refraction Framebuffer not complete!");
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFrameBuffers::bindFrameBuffer(GLuint frameBuffer, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, 0); // Para asegurarse de que no está binded como textura
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

GLuint WaterFrameBuffers::createFrameBuffer()
{
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return frameBuffer;
}

GLuint WaterFrameBuffers::createTextureAttachment(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    return texture;
}

GLuint WaterFrameBuffers::createDepthTextureAttachment(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    return texture;
}

GLuint WaterFrameBuffers::createDepthBufferAttachment(int width, int height)
{
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}
