#version 460 core

in vec3 cameraSpaceRockPos;
in vec2 uv;
out vec4 fragColor;

uniform vec3 cameraPosition;

uniform sampler2D rockSampler;	
uniform sampler2D displacementMapSampler;	

void main(){

	vec4  fogColor = vec4(0.7, 0.8, 0.9, 1.0);
	float fogStart = 0.8;
	float fogEnd = 8;


	float dist = length(cameraSpaceRockPos.xyz - cameraPosition);
	float fogFactor = clamp(((fogEnd-dist)/(fogEnd-fogStart)), 0.0, 1.0);


	vec4 rockColor = texture(rockSampler,uv);

	fragColor = mix(fogColor, rockColor, fogFactor);

}
