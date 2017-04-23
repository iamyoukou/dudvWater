#version 330

layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection_skybox;
uniform mat4 view_skybox;
uniform mat4 model_skybox;

void main()
{
    gl_Position = projection_skybox * view_skybox * model_skybox * vec4(position, 1.0);
    TexCoords = -position;
}
