#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include "Camera.h"
#include "Datatypes.h"
#include "WICTextureLoader.h"
#include "Lights\LightGrid.h"
#include "Utility\DepthStencil.h"
#include "Utility\ConstantBuffer.h"
#include "Utility\StructuredBuffer.h"
#include "Utility\ShaderResource.h"
#include "Lights\Sun.h"
#include "HybrisLoader\HybrisLoader.h"
#include "Fog.h"
#include "Utility\PingPongBuffer.h"

#include <SpriteBatch.h>

#include "RenderPass\GUIRenderPass.h"


namespace Graphics
{
	class Renderer
    {
    public:

        Renderer(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * backBuffer, Camera *camera);
        virtual ~Renderer();

        void render() const;
        void update(float deltaTime);
    private:
        ID3DUserDefinedAnnotation *DebugAnnotation;

        #pragma region Shared Shader Resources
        std::vector<RenderPass*> renderPasses;
        ShaderResource colorMap;
        ShaderResource glowMap;
        ShaderResource normalMap;
        DepthStencil shadowMap;
        DepthStencil depthStencil;

        ID3D11ShaderResourceView  * lightOpaqueGridSRV;
        ID3D11UnorderedAccessView * lightOpaqueGridUAV;
        StructuredBuffer<uint32_t> lightOpaqueIndexList;
        StructuredBuffer<Light>    lightsNew;

        ID3D11RenderTargetView * backBuffer;

        PingPongBuffer fakeBuffers;
        #pragma endregion

        #pragma region Instance Buffers
        void writeInstanceBuffers();

        struct StaticInstance
        {
            DirectX::SimpleMath::Matrix world;
            DirectX::SimpleMath::Matrix worldInvT;
            DirectX::SimpleMath::Vector3 color;
            UINT useGridTexture;
        };
        struct AnimatedInstance : StaticInstance
        {
            DirectX::SimpleMath::Matrix jointTransforms[20];
        };

        StructuredBuffer<StaticInstance> staticInstanceBuffer;
        StructuredBuffer<AnimatedInstance> animatedInstanceBuffer;
        #pragma endregion

        Sun sun;

        D3D11_VIEWPORT viewPort;

		void clear() const;
		

		ConstantBuffer <UINT> timeBuffer;
		UINT grassTime = 0;
		Shader foliageShader;
		Fog fog;
    };
};