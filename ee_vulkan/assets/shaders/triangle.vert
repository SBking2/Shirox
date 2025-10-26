#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 bones[100];
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in vec4 inWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCrood;

out gl_PerVertex{
	vec4 gl_Position;
};

void main(){
	mat4 boneTransform = ubo.bones[inBoneIDs[0]] * inWeights[0];
    boneTransform += ubo.bones[inBoneIDs[1]] * inWeights[1];
    boneTransform += ubo.bones[inBoneIDs[2]] * inWeights[2];
    boneTransform += ubo.bones[inBoneIDs[3]] * inWeights[3];

	gl_Position = ubo.projection * ubo.view * ubo.model * boneTransform * vec4(inPos.xyz, 1.0);
	fragColor = inColor;
	fragTexCrood = inTexCoord;
}