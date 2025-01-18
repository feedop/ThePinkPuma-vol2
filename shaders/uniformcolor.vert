#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const mat4 rotateZUp = mat4(
        1.0,  0.0,  0.0,  0.0,
        0.0,  0.0, -1.0,  0.0,
        0.0,  1.0,  0.0,  0.0,
        0.0,  0.0,  0.0,  1.0
    );

void main()
{
    gl_Position = projection * view * rotateZUp * model * vec4(aPos, 1.0);
}