#version 430 core

in vec3 vertex;
in vec3 matamb;
in vec3 matdif;
in vec3 matspec;
in float matshin;
in vec3 normal;

uniform mat4 proj;
uniform mat4 view;

out vec3 vertexFS;
out vec3 matambFS;
out vec3 matdifFS;
out vec3 matspecFS;
out float matshinFS;

out vec3 normalSCO;
out vec4 vertexSCO;

uniform mat4 TG;

void main()
{
    matambFS = matamb;
    matdifFS = matdif;
    matspecFS = matspec;
    matshinFS = matshin;
    mat3 normalMatrix = inverse(transpose(mat3 (view * TG)));
    normalSCO = vec3(normalMatrix * normal);
    vertexSCO = view * TG * vec4(vertex, 1.0);
    gl_Position = proj * vertexSCO;
}
