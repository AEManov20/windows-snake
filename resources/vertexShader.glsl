#version 300 es

layout(location = 1) in vec3 position;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;

out vec3 attrPosition;
out vec2 attrUV;
out vec3 attrColor;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    attrPosition = position;
    attrUV = uv;
    attrColor = color;
}
