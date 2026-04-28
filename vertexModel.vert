#version 430 core

in vec3 vertex;
in vec3 matamb;
in vec3 matdif;
in vec3 matspec;
in float matshin;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 proj;
uniform mat4 view;

out vec3 vertexFS;
out vec3 matambFS;
out vec3 matdifFS;
out vec3 matspecFS;
out float matshinFS;
out vec2 texCoordFS;
out mat3 TBN;

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
    texCoordFS = texCoord;

    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);
    TBN = mat3(T,B,N);

}
