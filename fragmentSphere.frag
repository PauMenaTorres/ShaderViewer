#version 430 core
out vec4 FragColor; // el primer out siempre especifica el color del fragment

uniform vec2 screenSize;
uniform int numSpheres;

struct Sphere
{
    vec4 position;
    vec4 color;
    float radius;
};

uniform Sphere spheres[50];


float CalculateIntersection(vec3 r0, vec3 rd, vec3 C, float r)
{
    vec3 oc = r0 - C;

    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = (dot(oc, oc) - pow(r, 2.0));

    float disc = pow(b, 2.0) - 4.0 * a * c;

    if(disc < 0.0)
    {
        return -1;
    }
    else
    {
        float s = sqrt(disc);
        float t0 = (-b - s) / (2.0 * a);
        float t1 = (-b + s) / (2.0 * a);
        float t = (t0 > 0.0) ? t0 : t1;

        if(t <= 0.0)
        {
            return -1;
        }
        else
        {
            return t;
        }
    }
}


vec3 Ambient(vec3 ambient, vec3 globalAmbient)
{
    return ambient * globalAmbient;
}

vec3 Diffuse(vec3 point, vec3 normal, vec3 difuse, vec3 lightPos, vec3 lightColor)
{
    return difuse * lightColor * max(0, dot(normal, normalize(lightPos - point)));
}

vec3 Specular(vec3 point, vec3 normal, vec3 specular, float shininess, vec3 obs, vec3 lightPos, vec3 lightColor)
{
    vec3 I = normalize(point - lightPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 V = normalize(obs - point);

    float specAngle = max(0.0, dot(R, V));

    return specular * lightColor * pow(specAngle, shininess);
}


void main(void)
{
    float tMin = 1e30;
    int iMin = 5;

    vec2 uv = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;
    uv.x *= screenSize.x / screenSize.y;

    vec3 r0 = vec3(0.0, 0.0, 1.0);
    vec3 rd = normalize(vec3(uv, -1.0));

    vec3 centerRed = vec3(0.0, 0.0, 0.0);
    float radiusRed = 0.5;

    vec3 centerGreen = vec3(0.0, -0.7, 0.0);
    float radiusGreen = 0.2;

    vec3 centerBlue = vec3(0.0, 0.7, 0.0);
    float radiusBlue = 0.2;

    vec3 centerPink = vec3(0.7, 0.0, 0.0);
    float radiusPink = 0.3;

    vec3 centerYellow = vec3(-0.7, 0.0, 0.0);
    float radiusYellow = 0.3;
    /*
    spheres[0].position.xyz = centerRed;
    spheres[0].color = vec4(1.0, 0.0, 0.0, 1.0);
    spheres[0].radius = radiusRed;

    spheres[1].position.xyz = centerGreen;
    spheres[1].color = vec4(0.0, 1.0, 0.0, 1.0);
    spheres[1].radius = radiusGreen;

    spheres[2].position.xyz = centerBlue;
    spheres[2].color = vec4(0.0, 0.0, 1.0, 1.0);
    spheres[2].radius = radiusBlue;

    spheres[3].position.xyz = centerPink;
    spheres[3].color = vec4(1.0, 0.0, 1.0, 1.0);
    spheres[3].radius = radiusPink;

    spheres[4].position.xyz = centerYellow;
    spheres[4].color = vec4(1.0, 1.0, 0.0, 1.0);
    spheres[4].radius = radiusYellow;
*/

    for(int i = 0; i < numSpheres; i++)
    {
        float t = CalculateIntersection(r0, rd, spheres[i].position.xyz, spheres[i].radius);

        if (t >= 0)
        {
            if (t < tMin)
            {
                tMin = t;
                iMin = i;
            }
        }
    }

    if (tMin != 1e30)
    {
        //Ambient
        vec3 ambient = vec3(0.5f, 0.5f, 0.5f);
        vec3 globalAmbient = vec3(0.5f, 0.5f, 0.5f);
        vec3 ambientCalc =  spheres[iMin].color.xyz * Ambient(ambient, globalAmbient);

        //Diffuse
        vec3 point = r0 + tMin*rd;
        vec3 normal = normalize(point - spheres[iMin].position.xyz);
        vec3 diffuse = spheres[iMin].color.xyz;
        vec3 lightPos = vec3(1.0, 1.0, 1.0);
        vec3 lightColor = vec3(1.0, 1.0, 1.0);

        vec3 diffuseCalc = Diffuse(point, normal, diffuse, lightPos, lightColor);
        //Specular
        vec3 specular = vec3(0.6f, 0.6f, 0.6f);
        float shininess = 128.0f;

        vec3 specularCalc = Specular(point, normal, specular, shininess, r0, lightPos, lightColor);

        FragColor = vec4(ambientCalc + diffuseCalc + specularCalc, 1.0f);
        //FragColor = spheres[iMin].color;
    }
    else
    {
        discard;
    }

    /*
    float tGreen = CalculateIntersection(r0, rd, centerGreen, radiusGreen);
    float tRed = CalculateIntersection(r0, rd, centerRed, radiusRed);

    if (tRed >= 0 && tGreen >= 0)
    {
        if(tRed < tGreen)
        {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else
        {
            FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    }
    else if (tRed >= 0)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if (tGreen >= 0)
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else
    {
        discard;
    }*/


}
