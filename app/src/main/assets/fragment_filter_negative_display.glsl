#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, v_texCoord);
    vec4 final_color = vec4(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);
    outColor = final_color;
}
