#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform mat4 transform;
uniform mat4 perspective;
uniform mat4 view;
void main()
{
    gl_Position = perspective * view * transform * vec4(aPos, 1.0f);
    ourColor = aColor;
}