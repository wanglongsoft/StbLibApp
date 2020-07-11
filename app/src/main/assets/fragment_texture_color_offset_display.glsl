#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

void main() {
    float offset_x = v_texCoord.x > 0.02 ? v_texCoord.x - 0.02 : 0.0;
    float offset_y = v_texCoord.y > 0.03 ? v_texCoord.y - 0.03 : 0.0;

    vec2 shiftCoord = vec2(offset_x, offset_y);
    vec4 base_color = texture(uTexture, v_texCoord);
    vec4 shift_color = texture(uTexture, shiftCoord);
//    outColor = mix(base_color, shift_color, 0.4);
    outColor = vec4(base_color.r, shift_color.g, shift_color.b, base_color.a);
}
