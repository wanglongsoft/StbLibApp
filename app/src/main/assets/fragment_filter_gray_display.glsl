#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

void main() {
    vec4 color = texture(uTexture, v_texCoord);
    float luminance = dot(color.rgb, W);
    outColor = vec4(luminance, luminance, luminance, color.a);
}
