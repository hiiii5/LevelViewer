// TODO: Part 2b
#pragma pack_matrix(row_major)

[[vk::push_constant]] cbuffer MESH_INDEX { uint mesh_ID; };

struct OBJ_ATTRIBUTES {
  float3 Kd;       // diffuse reflectivity
  float d;         // dissolve (transparency)
  float3 Ks;       // specular reflectivity
  float Ns;        // specular exponent
  float3 Ka;       // ambient reflectivity
  float sharpness; // local reflection map sharpness
  float3 Tf;       // transmission filter
  float Ni;        // optical density (index of refraction)
  float3 Ke;       // emissive reflectivity
  uint illum;      // illumination model
};

struct SHADER_MODEL_DATA {
  float4 sunDirection;
  float4 sunColor;
  matrix viewMatrix;
  matrix projectionMatrix;
  matrix matricies[1024];
  OBJ_ATTRIBUTES attributes[1024];
};

StructuredBuffer<SHADER_MODEL_DATA> shaderData;

float4 lambert(float4 lightColor, float4 ambientColor, float4 lightDir, float4 nrm) {
    float4 lightNormal = normalize(lightDir);
    float4 normalizedNormal = normalize(nrm);
    float lightDiffuseNormal = dot(lightNormal, normalizedNormal);
    lightDiffuseNormal = max(lightDiffuseNormal, 0);
    return lightDiffuseNormal * lightColor + ambientColor;
}

// TODO: Part 4g
// TODO: Part 2i
// TODO: Part 3e
// an ultra simple hlsl pixel shader
// TODO: Part 4b
float4 main(float4 posH : SV_POSITION, float4 nrmW : NORMAL, float4 posW : WORLD) : SV_TARGET {
	float4 surfaceColor = float4(shaderData[0].attributes[mesh_ID].Kd, 0); // TODO: Part 1a
    float4 ambientColor = float4(0.1f, 0.1f, 0.1f, 0);

    float4 lambertRet = lambert(shaderData[0].sunColor, ambientColor, -shaderData[0].sunDirection, nrmW);
    float4 directionalResult = lambertRet * surfaceColor;
	
    return directionalResult;
	// TODO: Part 3a
	// TODO: Part 4c
	// TODO: Part 4g (half-vector or reflect method your choice)
}