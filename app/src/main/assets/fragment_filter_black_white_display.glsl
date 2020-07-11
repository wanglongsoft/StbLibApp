#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
const float threshold = 0.3;//阈值,可外部传值，利用滑动条选择最合适的值

void main() {
    vec4 color = texture(uTexture, v_texCoord);
    float luminance = dot(color.rgb, W);
    float final_color = luminance <= threshold ? 0.0 : 1.0;
    outColor = vec4(final_color, final_color, final_color, color.a);
}
