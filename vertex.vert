#version 430 core

in vec3 vertex;
in vec3 color;  // variable que prové d'un VBO

void main(void)
{
    gl_Position = vec4(vertex, 1.0);
}
