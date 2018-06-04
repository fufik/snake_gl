#version 410 core
attribute vec4 coord;
varying vec2 texcoord;
void main()
{
    gl_Position = vec4(coord.xy, 0, 1.0);
    texcoord = coord.zw;
}

