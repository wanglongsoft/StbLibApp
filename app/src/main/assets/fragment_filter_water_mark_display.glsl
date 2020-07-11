#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;
uniform sampler2D waterMark;

const float x_scale = 2.5;
const float y_scale = 10.0;

void main() {
    if(v_texCoord.x >= 0.6
    && v_texCoord.y>= 0.70
    && v_texCoord.y<= 0.80) {
        vec2 water = vec2((v_texCoord.x - 0.6) * x_scale, (v_texCoord.y - 0.7) * y_scale);
        outColor = texture(waterMark, water);
    } else {
        outColor = texture(uTexture, v_texCoord);
    }
}
