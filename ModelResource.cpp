#include "ModelResource.h"
#include <QFile>
#include <QDebug>
#include <QImage>

ModelResource::ModelResource() : program(nullptr)
{

}

ModelResource::~ModelResource()
{
    if (program)
    {
        delete program;
    }
    if (textureID) glDeleteTextures(1, &textureID);
    if (textureBumpID) glDeleteTextures(1, &textureBumpID);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    
    GLuint vbos[] = {VBO_vert, VBO_normals, VBO_matamb, VBO_matdiff, VBO_matspec, VBO_matshin};
    glDeleteBuffers(6, vbos);
}

void ModelResource::init(const QString& modelName, const QString& vertexShader, const QString& fragmentShader)
{
    initializeOpenGLFunctions();

    m.load(modelName.toStdString());
    initTexture();

    computeAABB();

    loadShaders(vertexShader, fragmentShader);
    createBuffers();
}

void ModelResource::initTexture()
{
    QString s(m.textureName.c_str());

    // 1. Comprovació: el path no és buit
    if (s.isEmpty())
    {
        qCritical() << "ERROR: textureName buit";
        textureID = 0;
        return;
    }

    // 2. Comprovació: el fitxer existeix
    if (!QFile::exists(s))
    {
        qCritical() << "ERROR: la textura no existeix:" << s;
        textureID = 0;
        return;
    }

    // 3. Carrega de la imatge
    QImage img(s);
    // 4. Comprovació: la imatge s'ha carregat correctament
    if (img.isNull())
    {
        qCritical() << "ERROR: no s'ha pogut carregar la imatge:" << s;
        textureID = 0;
        return;
    }
    // 5. Conversió a format OpenGL
    QImage imGL = img.convertToFormat(QImage::Format_RGBA8888).mirrored();
    if (imGL.isNull())
    {
        qCritical() << "ERROR: conversió de la imatge fallida:" << s;
        textureID = 0;
        return;
    }

    // 6. Creació de la textura OpenGL
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 imGL.width(), imGL.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imGL.bits());
    // 7. Comprovació OpenGL
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        qCritical() << "ERROR OpenGL carregant textura:" << err;
        textureID = 0;
        return;
    }
    qDebug() << "Textura carregada correctament:" << s
             << " mida:" << imGL.width() << "x" << imGL.height();

    // --- Bump Texture ---
    QString bumpPath(m.bumpName.c_str());
    if (!bumpPath.isEmpty() && QFile::exists(bumpPath))
    {
        QImage bumpImg(bumpPath);
        if (!bumpImg.isNull())
        {
            QImage bumpGL = bumpImg.convertToFormat(QImage::Format_RGBA8888).mirrored();
            if (!bumpGL.isNull())
            {
                glGenTextures(1, &textureBumpID);
                glBindTexture(GL_TEXTURE_2D, textureBumpID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                             bumpGL.width(), bumpGL.height(),
                             0, GL_RGBA, GL_UNSIGNED_BYTE, bumpGL.bits());
                GLenum bumpErr = glGetError();
                if (bumpErr != GL_NO_ERROR)
                {
                    qCritical() << "ERROR OpenGL carregant bump texture:" << bumpErr;
                    textureBumpID = 0;
                }
                else
                {
                    qDebug() << "Bump texture carregada correctament:" << bumpPath
                             << " mida:" << bumpGL.width() << "x" << bumpGL.height();
                }
            }
        }
    }

}

void ModelResource::loadShaders(const QString& vShader, const QString& fShader)
{
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, vShader);
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, fShader);
    program->link();

    program->bind();

    //Atrib Location
    vertexLoc = glGetAttribLocation(program->programId(), "vertex");
    normalLoc = glGetAttribLocation(program->programId(), "normal");
    matambLoc = glGetAttribLocation(program->programId(), "matamb");
    matdiffLoc = glGetAttribLocation(program->programId(), "matdif");
    matspecLoc = glGetAttribLocation(program->programId(), "matspec");
    matshinLoc = glGetAttribLocation(program->programId(), "matshin");
    texCoordLoc = glGetAttribLocation(program->programId(), "texCoord");
    tangentLoc = glGetAttribLocation(program->programId(), "tangent");
    bitangentLoc = glGetAttribLocation(program->programId(), "bitangent");

    //Uniform Location
    TGLoc = glGetUniformLocation(program->programId(), "TG");
    hasTextureLoc = glGetUniformLocation(program->programId(), "hasTexture");
    hasBumpLoc = glGetUniformLocation(program->programId(), "hasBumpTexture");
    difuseTexLoc = glGetUniformLocation(program->programId(), "diffuseTex");
    bumpTextureLoc = glGetUniformLocation(program->programId(), "bumpTex");
    attLoc = glGetUniformLocation(program->programId(), "att");

    program->release();
}

void ModelResource::createBuffers()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO[6];
    glGenBuffers(6, VBO);
    VBO_vert = VBO[0];
    VBO_normals = VBO[1];
    VBO_matamb = VBO[2];
    VBO_matdiff = VBO[3];
    VBO_matspec = VBO[4];
    VBO_matshin = VBO[5];

    // Vertexs
    glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_vertices(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_normals(), GL_STATIC_DRAW);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalLoc);

    //Ambient
    glBindBuffer(GL_ARRAY_BUFFER, VBO_matamb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_matamb(), GL_STATIC_DRAW);
    glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matambLoc);

    //Diffuse
    glBindBuffer(GL_ARRAY_BUFFER, VBO_matdiff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_matdiff(), GL_STATIC_DRAW);
    glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matdiffLoc);

    //Specular
    glBindBuffer(GL_ARRAY_BUFFER, VBO_matspec);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_matspec(), GL_STATIC_DRAW);
    glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matspecLoc);

    //Shininess
    glBindBuffer(GL_ARRAY_BUFFER, VBO_matshin);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3, m.VBO_matshin(), GL_STATIC_DRAW);
    glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matshinLoc);

    //Textures
    if (m.VBO_texCoords()!=NULL)
    {
       GLuint vbotex;
       glGenBuffers(1, &vbotex);
       glBindBuffer(GL_ARRAY_BUFFER, vbotex);
       glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 3 * m.faces().size(),
                    m.VBO_texCoords(), GL_STATIC_DRAW);
       glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0,0);
       glEnableVertexAttribArray(texCoordLoc);
    }

    //Tangent
    if (m.VBO_tangents()!=NULL)
    {
       GLuint vboTangent;
       glGenBuffers(1, &vboTangent);
       glBindBuffer(GL_ARRAY_BUFFER, vboTangent);
       glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 3 * 3 * m.faces().size(),
                    m.VBO_tangents(), GL_STATIC_DRAW);
       glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, 0,0);
       glEnableVertexAttribArray(tangentLoc);
    }

    //Bitangent
    if (m.VBO_bitangents()!=NULL)
    {
       GLuint vboBitangent;
       glGenBuffers(1, &vboBitangent);
       glBindBuffer(GL_ARRAY_BUFFER, vboBitangent);
       glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 3 * 3 * m.faces().size(),
                    m.VBO_bitangents(), GL_STATIC_DRAW);
       glVertexAttribPointer(bitangentLoc, 3, GL_FLOAT, GL_FALSE, 0,0);
       glEnableVertexAttribArray(bitangentLoc);
    }

    glBindVertexArray(0);
}

void ModelResource::render(const glm::mat4& TG, const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, bool textureActive, bool bumpTextureActive, float attValue, const glm::vec4& clipPlane, bool isReflectionOrRefraction)
{
    program->bind();
    program->setUniformValue("isReflectionOrRefraction", isReflectionOrRefraction);

    glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);

    GLuint viewLoc = program->uniformLocation("view");
    GLuint projLoc = program->uniformLocation("proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat[0][0]);

    // Attenuation
    glUniform1f(attLoc, attValue);

    // Pass clipPlane uniform
    GLuint clipPlaneLoc = program->uniformLocation("clipPlane");
    if (clipPlaneLoc != -1)
    {
        glUniform4fv(clipPlaneLoc, 1, &clipPlane[0]);
    }

    if (textureID != 0 && textureActive)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(difuseTexLoc, 0);
        glUniform1i(hasTextureLoc, 1);
    }
    else
    {
        glUniform1i(hasTextureLoc, 0);
    }

    if (textureBumpID != 0 && bumpTextureActive)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureBumpID);
        glUniform1i(bumpTextureLoc, 1);
        glUniform1i(hasBumpLoc, 1);
    }
    else
    {
        glUniform1i(hasBumpLoc, 0);
    }

    glm::vec3 lightPosSCO = glm::vec3(viewMat * glm::vec4(lightPos, 1.0f));
    GLuint lightPosLoc = program->uniformLocation("lightPos");
    glUniform3fv(lightPosLoc, 1, &lightPosSCO[0]);

    GLuint lightColorLoc = program->uniformLocation("lightColor");
    glUniform3fv(lightColorLoc, 1, &lightColor[0]);


    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, m.faces().size() * 3);
    glBindVertexArray(0);

    program->release();
}

void ModelResource::renderWater(const glm::mat4& TG, const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, GLuint reflectionTex, GLuint refractionTex, GLuint dudvTex, GLuint normalTex, float moveFactor, const glm::vec3& cameraPos, float waveStrength, float waterShininess)
{
    program->bind();

    glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);

    GLuint viewLoc = program->uniformLocation("view");
    GLuint projLoc = program->uniformLocation("proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat[0][0]);

    // Attenuation
    glUniform1f(attLoc, attValue);

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflectionTex);
    program->setUniformValue("reflectionTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refractionTex);
    program->setUniformValue("refractionTexture", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dudvTex);
    program->setUniformValue("dudvMap", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    program->setUniformValue("normalMap", 3);

    // Uniforms
    program->setUniformValue("moveFactor", moveFactor);
    program->setUniformValue("cameraPosition", QVector3D(cameraPos.x, cameraPos.y, cameraPos.z));

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    program->setUniformValue("viewportSize", QVector2D(viewport[2], viewport[3]));

    program->setUniformValue("waveStrength", waveStrength);
    program->setUniformValue("waterShininess", waterShininess);

    // Also clip plane for water itself (neutral, default to no-clipping for standard draw)
    GLuint clipPlaneLoc = program->uniformLocation("clipPlane");
    if (clipPlaneLoc != -1)
    {
        glm::vec4 neutralClipPlane(0.0f, 1.0f, 0.0f, 100000.0f);
        glUniform4fv(clipPlaneLoc, 1, &neutralClipPlane[0]);
    }

    glm::vec3 lightPosSCO = glm::vec3(viewMat * glm::vec4(lightPos, 1.0f));
    GLuint lightPosLoc = program->uniformLocation("lightPos");
    glUniform3fv(lightPosLoc, 1, &lightPosSCO[0]);

    GLuint lightColorLoc = program->uniformLocation("lightColor");
    glUniform3fv(lightColorLoc, 1, &lightColor[0]);

    // Shine / reflectivity
    program->setUniformValue("shineDamper", 20.0f);
    program->setUniformValue("reflectivity", 0.6f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, m.faces().size() * 3);
    glBindVertexArray(0);

    program->release();
}

glm::vec3 ModelResource::getMin()
{
    return glm::vec3(xmin, ymin, zmin);
}

glm::vec3 ModelResource::getMax()
{
    return glm::vec3(xmax, ymax, zmax);
}

void ModelResource::computeAABB()
{
    xmin = xmax = m.vertices()[0];
    ymin = ymax = m.vertices()[1];
    zmin = zmax = m.vertices()[2];

    for(size_t i = 0; i < m.vertices().size(); i += 3)
    {
        if(m.vertices()[i] < xmin) xmin = m.vertices()[i];
        if(m.vertices()[i] > xmax) xmax = m.vertices()[i];

        if(m.vertices()[i+1] < ymin) ymin = m.vertices()[i + 1];
        if(m.vertices()[i+1] > ymax) ymax = m.vertices()[i + 1];

        if(m.vertices()[i+2] < zmin) zmin = m.vertices()[i + 2];
        if(m.vertices()[i+2] > zmax) zmax = m.vertices()[i + 2];
    }

    aabb[0] = xmin;
    aabb[1] = xmax;
    aabb[2] = ymin;
    aabb[3] = ymax;
    aabb[4] = zmin;
    aabb[5] = zmax;

}

glm::vec3 ModelResource::getCenter() const
{
    return glm::vec3(
        (aabb[0] + aabb[1]) * 0.5f,
        (aabb[2] + aabb[3]) * 0.5f,
        (aabb[4] + aabb[5]) * 0.5f
    );
}
