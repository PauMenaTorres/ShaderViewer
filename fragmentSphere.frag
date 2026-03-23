#version 430 core
out vec4 FragColor; // el primer out siempre especifica el color del fragment

uniform vec2 screenSize;
uniform int numSpheres;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 globalAmbient;

struct Sphere
{
    vec4 position;
    vec4 color;
    float radius;
    vec3 ka; // Constante ambiental
    vec3 kd; // Constante difusa
    vec3 ks; // Constante especular
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

float CalculatePlaneIntersection(vec3 ro, vec3 rd, vec3 p0, vec3 n)
{
    float denom = dot(rd, n);

    if (abs(denom) < 1e-6)
    {
        return -1;
    }
    else
    {
        float t = dot(p0 - ro, n) / denom;
        if (t >= 0.0)
            return t;
    }
    return -1;
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
    int iMin = -1;

    vec2 uv = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;
    uv.x *= screenSize.x / screenSize.y;

    vec3 r0 = vec3(0.0, 0.0, 1.0);
    vec3 rd = normalize(vec3(uv, -1.0));

    vec3 planePos = vec3(0.0, -1.0, 0.0);
    vec3 planeNormal = vec3(0.0, 1.0, 0.0);

    float tPlane = CalculatePlaneIntersection(r0, rd, planePos, planeNormal);

    if (tPlane > 0.0)
    {
        tMin = tPlane;
        iMin = -1;
    }

    for(int i = 0; i < numSpheres; i++)
    {
        float tSpheres = CalculateIntersection(r0, rd, spheres[i].position.xyz, spheres[i].radius);

        if (tSpheres >= 0)
        {
            if (tSpheres < tMin)
            {
                tMin = tSpheres;
                iMin = i;
            }
        }
    }

    if (tMin != 1e30)
    {
        vec3 point = r0 + tMin*rd;
        vec3 normal;
        vec3 objectColor;

        vec3 kaObj;
        vec3 kdObj;
        vec3 ksObj;

        if (iMin == -1)
        {
            normal = planeNormal;
            float size = 1.0;
            float checker = mod(floor(point.x / size) + floor(point.z / size), 2.0);

            if (checker < 1.0)
            {
                objectColor = vec3(0.2, 0.2, 0.2); // Gris oscuro
            }
            else
            {
                objectColor = vec3(0.4, 0.4, 0.4); // Gris claro
            }

            kaObj = vec3(1.0, 1.0, 1.0);
            kdObj = vec3(1.0, 1.0, 1.0);
            ksObj = vec3(1.0, 1.0, 1.0);
        }
        else
        {
            normal = normalize(point - spheres[iMin].position.xyz);
            objectColor = spheres[iMin].color.xyz;
            kaObj = vec3(spheres[iMin].ka);
            kdObj = vec3(spheres[iMin].kd);
            ksObj = vec3(spheres[iMin].ks);
        }

        //Ambient
        vec3 ambient = vec3(0.5f, 0.5f, 0.5f);
        vec3 globalAmbient = vec3(0.5f, 0.5f, 0.5f);

        vec3 ambientCalc = objectColor * Ambient(kaObj, globalAmbient);

        //Diffuse
        vec3 diffuse = objectColor;

        vec3 diffuseCalc = Diffuse(point, normal, diffuse * kdObj, lightPosition, lightColor);

        //Specular
        vec3 specular = vec3(0.6f, 0.6f, 0.6f);
        float shininess = 128.0f;

        vec3 specularCalc = Specular(point, normal, ksObj, shininess, r0, lightPosition, lightColor);

        FragColor = vec4(ambientCalc + diffuseCalc + specularCalc, 1.0f);
        //FragColor = spheres[iMin].color;
    }
    else
    {
        FragColor = vec4(0.76, 0.69, 0.52, 1.0);
    }

}
