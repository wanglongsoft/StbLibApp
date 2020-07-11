#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, v_texCoord);
    outColor = color;
}
