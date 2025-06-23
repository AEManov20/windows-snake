#version 300 es

precision highp float;

in vec3 attrPosition;
in vec3 attrColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(attrColor, 1.);
}
