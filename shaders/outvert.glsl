#version 330

layout ( location = 0 ) in vec2 pos;
layout ( location = 1 ) in vec2 coord;

out vec2 TexCoord;

void main(){
  gl_Position = vec4(pos, 0.0f, 1.0f);
  TexCoord = coord;
}
