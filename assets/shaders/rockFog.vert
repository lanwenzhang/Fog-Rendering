#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec3 cameraSpaceRockPos;
out vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform sampler2D rockSampler;	
uniform sampler2D parallaxMapSampler;	


void main(void)
{	
	
	vec4 transformPosition = vec4(aPos, 1.0f) + vec4((aNormal * ((texture2D(parallaxMapSampler, aUV).r)/2.5f)), 1.0f);

	uv = aUV;
	

	cameraSpaceRockPos = (viewMatrix * modelMatrix * transformPosition).xyz;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;

}
