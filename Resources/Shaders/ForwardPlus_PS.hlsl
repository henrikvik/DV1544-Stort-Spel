#include "include/Camera.hlsli"
#include "include/Fragment.hlsli"
#include "include/LightCalc.hlsli"
#include "ShaderConstants.hlsli"


cbuffer cb0 : register(b0) { Camera camera; };
cbuffer cb1 : register(b1) { DirectionalLight globalLight; };

SamplerState           linearClamp     : register(s0);
SamplerState           linearWrap      : register(s1);
SamplerComparisonState comparison      : register(s2);

Texture2D              shadowMap       : register(t3);
Texture2DArray         AO              : register(t4);

Texture2D              diffuseTexture  : register(t12);
Texture2D              normalTexture   : register(t13);
Texture2D              specularTexture : register(t14);
Texture2D              glowTexture     : register(t15);

struct Targets
{
    float4 color      : SV_Target0;
    float4 glow       : SV_Target1;
    float4 viewNormal : SV_Target2;
};

Targets PS(Fragment fragment)
{
    Targets targets;

    float3 normal = calcNormal(normalTexture.Sample(linearClamp, fragment.uv).xyz, fragment.normal, fragment.binormal, fragment.tangent);
    float specularExponent = specularTexture.Sample(linearClamp, fragment.uv).r;

    float aoTL = AO.SampleLevel(linearClamp, float3(fragment.ndcPosition.xy / SCREEN_SIZE, 0), 0.0).x;
    float aoBR = AO.SampleLevel(linearClamp, float3(fragment.ndcPosition.xy / SCREEN_SIZE, 1), 0.0).x;
    float ao = (aoTL + aoBR) * 0.5;

    float shadowFactor = calcShadowFactor(comparison, shadowMap, fragment.lightPos);
    float3 viewDir = normalize(camera.position.xyz - fragment.position.xyz);
    float3 lightSum = ao * globalLight.ambient;
    lightSum += shadowFactor * calcLight(globalLight, fragment.position.xyz, normal, viewDir, specularExponent);
    lightSum += calcAllLights(fragment.ndcPosition, fragment.position.xyz, normal, viewDir, specularExponent);
    //float3 fog = float3(0.8, 0.1, 0.1) * calcFog(camera.position.xyz, fragment.position.xyz);

    float3 color = diffuseTexture.Sample(linearClamp, fragment.uv).xyz;
    color = lightSum * color * fragment.color;
    color = lerp(color, FOG_COLOR, saturate(distance(camera.position.xyz, fragment.position.xyz) / 130.f));

    targets.color = float4(color, 1); //500~


    float3 glow = 2.3 * glowTexture.Sample(linearClamp, fragment.uv);
    float luminance = dot(float3(0.2126, 0.7152, 0.0722), glow);
    if (luminance > 1.0) {
        targets.glow = float4(glow, 0);
    }
    else {
        targets.glow = float4(0, 0, 0, 0);
    }

    targets.viewNormal = fragment.viewNormal;

    return targets;
}