#version 330 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

// in vec4 inPosition;
// in vec4 inColor;
out vec4 exColor;

uniform mat4 Matrix;

void main(void) {
    gl_Position = Matrix * inPosition;
    exColor = inColor;
}
