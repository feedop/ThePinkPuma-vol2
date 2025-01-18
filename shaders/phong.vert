#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

out vec3 FragPos;                 // Fragment position for the fragment shader
out vec3 FragNormal;              // Normal vector for lighting

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
    mat4 transform = rotateZUp * model;
    vec4 frag = transform * vec4(aPos, 1.0);
    FragPos = vec3(frag);
    FragNormal = normalize(vec3(transform * vec4(aNorm, 0.0)));
    gl_Position = projection * view * frag;
}