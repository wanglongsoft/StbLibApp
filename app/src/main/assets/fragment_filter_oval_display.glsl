#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

const float ratio = 1440.0 / 2560.0;//由外部传值，显示图像的宽高比或者窗口的宽高比，且小于 1
const float focus = 0.15;//椭圆焦点距离

void main() {
    vec2 left_focus = vec2(0.5 - focus, 0.5);
    vec2 right_focus = vec2(0.5 + focus, 0.5);
    vec2 d_left = v_texCoord - left_focus;
    vec2 d_right = v_texCoord - right_focus;

    // |PF1| + |PF2| = 2a（2a>|F1F2|）
    //  c*c = a*a - b * b, 长半轴：a, 短半轴：b, 两焦点半距离：c
    if((length(d_left) + length(d_right)) <= (2.0 * sqrt(focus * focus / (1.0 - ratio * ratio)))) {
        outColor = vec4(0.33, 0.44, 0.55, 1.0);
    } else {
        outColor = texture(uTexture, v_texCoord);
    }
}
