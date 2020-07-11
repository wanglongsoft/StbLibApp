#version 300 es
precision highp float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

const float uProgress = 1.7;

void main() {
    float t = 0.7; //周期
    float maxAlpha = 0.4;//第二图最大透明度
    float maxScale = 1.8;//第二图放大最大比率
    float progress = mod(uProgress, t) / t;
    //当前的透明度
    float alpha = maxAlpha * (1.0 - progress);
    //当前的放大比例
    float scale = 1.0 + (maxScale - 1.0) * progress;
    //根据放大比例获取新的图层纹理坐标
    vec2 weakPos = vec2(0.5 + (v_texCoord.x - 0.5) / scale, 0.5 + (v_texCoord.y - 0.5) / scale);

    //新图层纹理坐标对应的纹理像素值
    vec4 weakMask = texture(uTexture, weakPos);
    vec4 mask = texture(uTexture, v_texCoord);

    //纹理像素值的混合公式，获得混合后的实际颜色
    outColor = mask * (1.0 - alpha) + weakMask * alpha;
}
