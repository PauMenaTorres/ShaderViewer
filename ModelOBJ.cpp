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
    vertexLoc = program->attributeLocation("vertex");
    colorLoc = program->attributeLocation("color");
    TGLoc = program->uniformLocation("TG");
    program->release();
}

void ModelOBJ::createBuffers()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO[2];
    glGenBuffers(2, VBO);
    VBO_vert = VBO[0];
    VBO_mat = VBO[1];

    glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_vertices(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_mat);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size() * 3 * 3, m.VBO_matdiff(), GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

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
