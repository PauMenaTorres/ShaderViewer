#ifndef WATERFRAMEBUFFERS_H
#define WATERFRAMEBUFFERS_H

#include <QOpenGLFunctions_4_3_Core>

class WaterFrameBuffers : public QOpenGLFunctions_4_3_Core
{
public:
    WaterFrameBuffers(int width, int height);
    ~WaterFrameBuffers();

    void cleanUp();
    void bindReflectionFrameBuffer();
    void bindRefractionFrameBuffer();
    void unbindCurrentFrameBuffer(int currentWidth, int currentHeight);

    GLuint getReflectionTexture() const { return reflectionTexture; }
    GLuint getRefractionTexture() const { return refractionTexture; }
    GLuint getRefractionDepthTexture() const { return refractionDepthTexture; }

private:
    void initialiseReflectionFrameBuffer();
    void initialiseRefractionFrameBuffer();
    
    void bindFrameBuffer(GLuint frameBuffer, int width, int height);
    GLuint createFrameBuffer();
    GLuint createTextureAttachment(int width, int height);
    GLuint createDepthTextureAttachment(int width, int height);
    GLuint createDepthBufferAttachment(int width, int height);

    static const int REFLECTION_WIDTH = 320;
    static const int REFLECTION_HEIGHT = 180;
    
    static const int REFRACTION_WIDTH = 1280;
    static const int REFRACTION_HEIGHT = 720;

    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;

    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;
};

#endif // WATERFRAMEBUFFERS_H
