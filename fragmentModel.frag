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


vec4 texColor;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform int hasTexture;
uniform int hasBumpTexture;

vec3 colFocus = vec3(1, 1, 1);

vec3 Lambert (vec3 NormSCO, vec3 L, vec3 diffColor)
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matambFS;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * diffColor * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 diffColor)
{
    // Els vectors estan normalitzats

    // Inicialitzem color a Lambert
    vec3 colRes = Lambert (NormSCO, L, diffColor);

    // Calculem R i V
    if (dot(NormSCO,L) < 0)
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshinFS == 0))
      return colRes;  // no hi ha component especular

    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshinFS);
    return (colRes + matspecFS * colFocus * shine);
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

    vec3 L = normalize(lightPos - vertexSCO.xyz);

    if (hasTexture == 1)
    {
        texColor = texture(diffuseTex, texCoordFS);
        lightRes = Phong(N, L, vertexSCO, texColor.rgb);
    }
    else
    {
        lightRes = Phong(N, L, vertexSCO, matdifFS);
    }

    FragColor = vec4(lightRes, 1.0);
}
