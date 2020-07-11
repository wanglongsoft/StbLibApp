#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, v_texCoord);
    vec4 final_color = vec4(
    0.393 * color.r + 0.769 * color.g + 0.189 * color.b,
    0.349 * color.r + 0.686 * color.g + 0.168 * color.b,
    0.272 * color.r + 0.534 * color.g + 0.131 * color.b,
    color.a);
    outColor = final_color;
}
