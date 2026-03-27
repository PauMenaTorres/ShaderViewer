#version 430 core

in vec3 vertex;
in vec3 color;  // variable que prové d'un VBO
out vec3 fcolor;
uniform mat4 TG;

void main(void)
{
    fcolor = color;
    gl_Position = TG * vec4(vertex, 1.0);
}
