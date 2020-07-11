#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D uTexture;

const vec2 mosaicSize = vec2(30.0, 30.0);

const float screen_width = 1080.0;
const float screen_height = 1920.0;

void main() {
    vec2 scale_xy = vec2(v_texCoord.x * screen_width, v_texCoord.y * screen_height);
    vec2 mosaic = vec2(floor(scale_xy.x / mosaicSize.x) * mosaicSize.x, floor(scale_xy.y / mosaicSize.y) * mosaicSize.y);
    vec2 final = vec2(mosaic.x / screen_width, mosaic.y / screen_height);
    vec4 color = texture(uTexture, final);
    outColor = color;
}
