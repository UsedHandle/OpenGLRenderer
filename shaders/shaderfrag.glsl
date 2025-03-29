#version 330 core
out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};


struct PointLight {
  vec3 lightPos;

  vec3 ambience;
  vec3 diffuse;
  vec3 specular;
};

in vec3 FragPos;
in vec3 surfacenorm;


uniform vec3 cameraPos;
uniform Material material;
uniform PointLight light;


in vec2 TexCoords;

void main(){
  vec3 objspec = texture(material.specular, TexCoords).rgb;
  vec3 objdiff = texture(material.diffuse,  TexCoords).rgb;

  float alpha = texture(material.diffuse, TexCoords).a;

  if(alpha == 0.0f)
    discard;

  vec3 lightDir = normalize(light.lightPos - FragPos);
  vec3 cameraDir = normalize(cameraPos - FragPos);

  vec3 ambience = light.ambience * objdiff;

  float diffFactor = max( dot(surfacenorm, lightDir), 0.0f );
  vec3 diffuse = light.diffuse * diffFactor * objdiff;

  vec3 reflectedLight = reflect(-lightDir, surfacenorm);
  vec3 midvec = normalize(lightDir + cameraDir);

  float specFactor = 0.0f;

  if(diffFactor > 0.0f)
    specFactor = pow( max(dot( surfacenorm, midvec ), 0.0), material.shininess );

  vec3 specular = light.specular * specFactor * objspec;

  FragColor = vec4(ambience + diffuse + specular, 1.0f);
  //FragColor = vec4(1.0f);
}
