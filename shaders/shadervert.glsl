#version 330 core

layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec3 normal;
layout ( location = 2 ) in vec2 coords;

out vec3 surfacenorm;
out vec3 FragPos;

out vec2 TexCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 object;

uniform mat4 sceneTrans;

void main(){
  gl_Position = proj * view * sceneTrans * object * vec4(pos, 1.0f);

  surfacenorm = mat3(transpose(inverse(sceneTrans * object)))*normal;
  surfacenorm = normalize(surfacenorm);
  FragPos = vec3( sceneTrans * object * vec4(pos, 1.0f) );
  
  TexCoords = coords;
}
