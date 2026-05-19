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
out vec4 clipSpaceCoords;
out vec2 texCoordsFS;
out mat3 TBN;

void main()
{
    matdifFS = matdif;

    mat4 modelView = view * TG;
    vertexSCO = modelView * vec4(vertex, 1.0);
    clipSpaceCoords = proj * vertexSCO;
    gl_Position = clipSpaceCoords;

    mat3 normalMatrix = inverse(transpose(mat3(modelView)));
    normalSCO = normalize(normalMatrix * normal);

    // Compute SCO tangent-space basis (TBN) for the flat water quad (horizontal on XZ plane)
    vec3 T = normalize(normalMatrix * vec3(1.0, 0.0, 0.0));
    vec3 B = normalize(normalMatrix * vec3(0.0, 0.0, 1.0));
    vec3 N = normalize(normalMatrix * vec3(0.0, 1.0, 0.0));
    TBN = mat3(T, B, N);

    // Tile the texture coordinates to make wave details more fine-grained
    texCoordsFS = texCoord * 8.0;

    // Vector from vertex to camera (in SCO, camera is at origin)
    toCameraVector = -vertexSCO.xyz; 
}
