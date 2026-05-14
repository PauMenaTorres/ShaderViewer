#version 430 core

in vec3 vertex;
in vec3 matamb;
in vec3 matdif;
in vec3 matspec;
in float matshin;
in vec3 normal;
in vec2 texCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

out vec3 matdifFS;
out vec3 normalSCO;
out vec4 vertexSCO;
out vec3 toCameraVector;

void main()
{
    matdifFS = matdif;

    mat4 modelView = view * TG;
    vertexSCO = modelView * vec4(vertex, 1.0);
    gl_Position = proj * vertexSCO;

    mat3 normalMatrix = inverse(transpose(mat3(modelView)));
    normalSCO = normalize(normalMatrix * normal);

    // Vector desde el vértice a la cámara. En SCO la cámara está en (0,0,0)
    toCameraVector = -vertexSCO.xyz; 
}
