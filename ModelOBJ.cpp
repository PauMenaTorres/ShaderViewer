#include "ModelOBJ.h"
#include <QFile>
#include <QDebug>

ModelOBJ::ModelOBJ() : program(nullptr), TG(1.0f)
{

}

ModelOBJ::~ModelOBJ()
{
    if (program)
    {
        delete program;
    }
}

void ModelOBJ::init(const QString& modelName, const QString& vertexShader, const QString& fragmentShader)
{
    initializeOpenGLFunctions();

    m.load(modelName.toStdString());

    computeAABB();

    loadShaders(vertexShader, fragmentShader);
    createBuffers();
}

void ModelOBJ::loadShaders(const QString& vShader, const QString& fShader)
{
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, vShader);
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, fShader);
    program->link();

    program->bind();
    vertexLoc = glGetAttribLocation(program->programId(), "vertex");
    normalLoc = glGetAttribLocation(program->programId(), "normal");
    matambLoc = glGetAttribLocation(program->programId(), "matamb");
    matdiffLoc = glGetAttribLocation(program->programId(), "matdif");
    matspecLoc = glGetAttribLocation(program->programId(), "matspec");
    matshinLoc = glGetAttribLocation(program->programId(), "matshin");

    TGLoc = glGetUniformLocation(program->programId(), "TG");
    program->release();
}

void ModelOBJ::createBuffers()
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

    glBindVertexArray(0);
}

void ModelOBJ::modelTransform(const glm::mat4& transform)
{
    TG = transform;
}

void ModelOBJ::render(const glm::mat4& viewMat, const glm::mat4& projMat)
{
    program->bind();

    glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);

    GLuint viewLoc = program->uniformLocation("view");
    GLuint projLoc = program->uniformLocation("proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, m.faces().size() * 3);
    glBindVertexArray(0);

    GLuint lightPosLoc = program->uniformLocation("lightPos");
    glm::vec3 lightPos(2.0f);
    glUniform3fv(lightPosLoc, 1, &lightPos[0]);

    program->release();
}

void ModelOBJ::computeAABB()
{
    float xmin, xmax, ymin, ymax, zmin, zmax;
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

glm::vec3 ModelOBJ::getCenter() const
{
    return glm::vec3(
        (aabb[0] + aabb[1]) * 0.5f,
        (aabb[2] + aabb[3]) * 0.5f,
        (aabb[4] + aabb[5]) * 0.5f
    );
}
