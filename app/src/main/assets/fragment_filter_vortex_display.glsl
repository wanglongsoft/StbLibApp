#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

const float uD = 90.0;//顺时针旋转弧度
const float uR = 0.2;

const vec2 center = vec2(1.0/2.0, 1.0/2.0);

void main() {
    vec2 st = v_texCoord;
    vec2 xy = st;
    vec2 scale_xy = st;
    vec2 dxy = scale_xy - center;
    float r = length(dxy);
    bool is_change = false;
    if(r < uR) {
        float beta = atan(dxy.y, dxy.x) - radians(uD);
//        float beta = atan(dxy.y, dxy.x) - radians(uD) * (1.0 - (r / uR) * ( r / uR));
        xy = center + r * vec2(cos(beta), sin(beta));
    }

    vec4 color = texture(uTexture, xy);
    outColor = color;
}
