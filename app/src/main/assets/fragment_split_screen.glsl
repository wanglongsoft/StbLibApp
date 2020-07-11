#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

const float horizontal = 3.0;
const float vertical= 3.0;

void main() {
    vec2 n_texCoord = mod(v_texCoord, vec2(1.0 / vertical, 1.0 / horizontal));
    n_texCoord.x = n_texCoord.x * vertical;
    n_texCoord.y = n_texCoord.y * horizontal;
    vec4 color = texture(uTexture, n_texCoord);
    outColor = color;
}
