#version 430 core

in vec4 vertexSCO;
in vec3 normalSCO;
in vec3 matambFS;
in vec3 matdifFS;
in vec3 matspecFS;
in float matshinFS;
in vec2 texCoordFS;

in mat3 TBN;

out vec4 FragColor;

vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
uniform vec3 lightPos;
uniform vec3 lightColor;

vec4 texColor;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

float att = 0.9;
float kc = 1.0;
float kl = 0.5;
float kq = 0.2;

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform int hasTexture;
uniform int hasBumpTexture;

vec3 Lambert (vec3 NormSCO, vec3 L, vec3 diffColor, float at)
{
    vec3 colRes = llumAmbient * matambFS;

    if (dot (L, NormSCO) > 0)
      colRes = colRes + (lightColor * at) * diffColor * dot (L, NormSCO);

    return colRes;
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 diffColor, float at)
{
    vec3 colRes = Lambert (NormSCO, L, diffColor, at);

    if (dot(NormSCO, L) < 0)
      return colRes;

    vec3 R = reflect(-L, NormSCO);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshinFS == 0))
      return colRes;

    float shine = pow(max(0.0, dot(R, V)), matshinFS);
    return (colRes + matspecFS * (lightColor * at) * shine);
}

void main()
{
    vec3 N;
    vec3 lightRes;

    if (hasBumpTexture == 1)
    {
        vec3 tangentNormal = texture(bumpTex, texCoordFS).xyz * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }
    else
    {
        N = normalize(normalSCO);
    }

    // 1. Calculem la distància entre el vèrtex i la posició de la llum
    float dist = distance(lightPos, vertexSCO.xyz);

    // 2. Apliquem la fórmula d'atenuació
    float at = att / (kc + kl * dist + kq * (dist * dist));

    vec3 L = normalize(lightPos - vertexSCO.xyz);

    if (hasTexture == 1)
    {
        vec4 texColor = texture(diffuseTex, texCoordFS);
        lightRes = Phong(N, L, vertexSCO, texColor.rgb, at);
    }
    else
    {
        lightRes = Phong(N, L, vertexSCO, matdifFS, at);
    }

    FragColor = vec4(lightRes, 1.0);
}
