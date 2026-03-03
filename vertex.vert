#version 430 core

in vec3 vertex;
in vec3 color;  // variable que prové d'un VBO
out vec3 fcolor;

void main(void)
{
    fcolor = color;
    gl_Position = vec4(vertex, 1.0);
}
