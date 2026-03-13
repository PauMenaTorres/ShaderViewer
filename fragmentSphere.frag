#version 430 core
out vec4 FragColor; // el primer out siempre especifica el color del fragment

uniform vec2 screenSize;



bool CalculateIntersection(vec3 r0, vec3 rd, vec3 C, float r)
{
    vec3 oc = r0 - C;

    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = (dot(oc, oc) - pow(r, 2.0));

    float disc = pow(b, 2.0) - 4.0 * a * c;

    if(disc < 0.0)
    {
        return false;
    }
    else
    {
        float s = sqrt(disc);
        float t0 = (-b - s) / (2.0 * a);
        float t1 = (-b + s) / (2.0 * a);
        float t = (t0 > 0.0) ? t0 : t1;

        if(t <= 0.0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

void main(void)
{
    /*
    vec3 C = vec3(0.0, 0.0, -1.0);

    float r = 0.5f;


    vec2 uv = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;
    uv.x*= screenSize.x/screenSize.y;
    vec3 r0 = vec3(0.0, 0.0, 0.0);

    vec3 oc = r0 - C;

    vec3 rd = normalize(vec3(uv, -1));
    float a = dot(rd, rd);
    float b = 2*dot(oc, rd);
    float c = (dot(oc, oc) - pow(r,2));

    float disc = pow(b,2) - 4*a*c;

    if(disc < 0.0)
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        float s = sqrt(disc);
        float t0 = (-b - s) / (2.0*a);
        float t1 = (-b + s) / (2.0*a);
        float t = (t0 > 0.0) ? t0 : t1;

        if(t <= 0.0)
        {
            discard;
        }
        else
        {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    }*/
    vec2 uv = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;
    uv.x *= screenSize.x / screenSize.y;

    vec3 r0 = vec3(0.0, 0.0, 2.0);
    vec3 rd = normalize(vec3(uv, -1.0));

    vec3 centerRed = vec3(0.0, 0.0, 0.0);
    float radiusRed = 0.5;

    vec3 centerGreen = vec3(-0.2, 0.0, 0.0);
    float radiusGreen = 0.3;

    bool hitGreen = CalculateIntersection(r0, rd, centerGreen, radiusGreen);
    bool hitRed = CalculateIntersection(r0, rd, centerRed, radiusRed);

    if(hitGreen)
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if(hitRed)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }


}
