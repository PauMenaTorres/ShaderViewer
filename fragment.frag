#version 430 core
out vec4 FragColor; // el primer out siempre especifica el color del fragment
in vec3 fcolor;
uniform vec2 screenSize;
uniform int idShader;


void Paint2Colors(void)
{
    if(gl_FragCoord.x < screenSize[0] / 2)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
}

void Paint4Colors(void)
{
    if(gl_FragCoord.x < screenSize[0] / 2 && gl_FragCoord.y > screenSize[1] / 2)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if(gl_FragCoord.x > screenSize[0] / 2 && gl_FragCoord.y > screenSize[1] / 2)
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else if(gl_FragCoord.x < screenSize[0] / 2 && gl_FragCoord.y < screenSize[1] / 2)
    {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
    else if(gl_FragCoord.x > screenSize[0] / 2 && gl_FragCoord.y < screenSize[1] / 2)
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
}

void PaintCircle(void)
{
    vec2 center = (screenSize /2);

    if(distance(center, gl_FragCoord.xy) < 75)
    {
        discard;
    }
    FragColor = vec4(fcolor, 1.0);
}

void Paint4ColorsSliced(void)
{
    if(int(gl_FragCoord.y) % 20 <= 10)
    {
        discard;
    }

    Paint4Colors();
}

void main(void)
{
    switch(idShader)
    {
        case 1:
            Paint2Colors();
            break;
        case 2:
            Paint4Colors();
            break;
        case 3:
            PaintCircle();
            break;
        case 4:
            Paint4ColorsSliced();
            break;
    }

}
