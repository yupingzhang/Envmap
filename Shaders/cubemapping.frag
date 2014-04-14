//look up in the cubemap texture
//find the reflect color

#version 400 core

in vec3 position, fNomarl, light;
in mat4 inverseViewMatrix;
uniform samplerCube cubeTexture;
out vec4 fragColor;

void main () {

   // light vectors
    vec3 L = normalize(light - position); // light direction
    vec3 V = normalize(/*eye at 0,0,0*/ - position);
    vec3 H = normalize(V + L);
 
 	float ambient = 0.2;
    float diffuse = max(0., dot(fNomarl,L));  // diffuse lighting
    vec4 diffuse_color = (ambient + diffuse/3) * vec4(0.2, 0.3, 0.1, 1.0); 

    // reflect ray around normal from eye to surface 
    vec3 reflected = reflect(normalize(position), normalize(fNomarl));
    reflected = vec3 (inverseViewMatrix * vec4 (reflected, 0.0));

    //fragColor = texture(cubeTexture, reflected);
	fragColor = diffuse_color + texture(cubeTexture, reflected);
}

