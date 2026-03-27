#version 430 core
out vec4 FragColor; // el primer out siempre especifica el color del fragment
in vec3 fcolor;

void main(void)
{
    FragColor = vec4(fcolor, 1.0f);
}
