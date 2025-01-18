#version 450 core

in vec3 FragPos;
in vec3 FragNormal;

out vec4 FragColor;

// Uniforms
uniform vec4 color;      // Base color of the quad
uniform vec3 viewPos;

const vec3 lightPos = vec3(0, 10, 0);     // Position of the light source
const vec3 lightColor = vec3(1, 1, 1);   // Color of the light

// Phong lighting model parameters
const float shininess = 32;   // Material shininess
const float diffuseStrength = 0.8;
const float ambientStrength = 0.3;
const float specularStrength = 0.5;

void main()
{
    //FragColor = color;return;
    //FragColor = vec4(1,1,1,1);return;

    // Ambient lighting (constant base light)
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting (Lambert's cosine law)
    vec3 lightDir = normalize(vec3(0,1,0));
    float diff = max(dot(FragNormal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular lighting (Blinn-Phong reflection)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, FragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);    
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all three components
    FragColor = vec4(ambient + diffuse + specular, 1.0) * color;
}
