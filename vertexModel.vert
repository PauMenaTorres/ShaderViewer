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
uniform mat4 TG;
uniform vec4 clipPlane = vec4(0.0, 1.0, 0.0, 100000.0); // default neutral plane

out vec3 matambFS;
out vec3 matdifFS;
out vec3 matspecFS;
out float matshinFS;
out vec2 texCoordFS;

out vec3 normalSCO;
out vec4 vertexSCO;

out mat3 TBN;

void main()
{
    // Clip distance calculation
    vec4 worldPosition = TG * vec4(vertex, 1.0);
    gl_ClipDistance[0] = dot(worldPosition, clipPlane);

    // Material
    matambFS = matamb;
    matdifFS = matdif;
    matspecFS = matspec;
    matshinFS = matshin;

    // Transformaciones
    mat4 modelView = view * TG;

    vertexSCO = modelView * vec4(vertex, 1.0);
    gl_Position = proj * vertexSCO;

    // Normales
    mat3 normalMatrix = inverse(transpose(mat3(modelView)));
    normalSCO = normalize(normalMatrix * normal);

    // Tangent space (para bump)
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);
    TBN = mat3(T, B, N);

    texCoordFS = texCoord;
}
