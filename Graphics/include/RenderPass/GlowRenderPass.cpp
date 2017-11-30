#include "GlowRenderPass.h"
#include "../Device.h"
#include "../CommonState.h"
#include <Engine\DebugWindow.h>

namespace Graphics
{
    GlowRenderPass::GlowRenderPass(
        PingPongBuffer * backBuffers,
        std::initializer_list<ID3D11RenderTargetView*> targets,
        std::initializer_list<ID3D11ShaderResourceView*> resources,
        std::initializer_list<ID3D11Buffer*> buffers,
        ID3D11DepthStencilView * depthStencil) :
        RenderPass(targets, resources, buffers, depthStencil),
        m_KawaseDualFilterDownsample(Resources::Shaders::KawaseDualFilterDownsample, ShaderType::PS),
        m_KawaseDualFilterUpsample(Resources::Shaders::KawaseDualFilterUpsample, ShaderType::PS),
        mipCombinder(Resources::Shaders::GlowMipCombinder),
        mipGenerator(Resources::Shaders::GlowDownSampler),
        merger(Resources::Shaders::GlowMerger),
        glow(Resources::Shaders::GlowBlurHorizontal),
        glow2(Resources::Shaders::GlowBlurVertical),
        glowPass0(WIN_WIDTH, WIN_HEIGHT),
        glowPass1(WIN_WIDTH, WIN_HEIGHT),
        backBuffers(backBuffers)
    {
        createMips();

        DebugWindow::getInstance()->registerCommand("GFX_SET_GLOW", [&](std::vector<std::string> &args)->std::string
        {
            std::string catcher = "";
            try
            {
                if (args.size() != 0)
                {
                    enabled = std::stoi(args[0]);

                    if (enabled)
                        catcher = "Glow enabled!";

                    else
                        catcher = "Glow disabled!";
                }
                else
                {
                    catcher = "missing argument 0 or 1.";
                }
            }
            catch (const std::exception&)
            {
                catcher = "Argument must be 0 or 1.";
            }

            return catcher;
        });
    }

    GlowRenderPass::~GlowRenderPass()
    {
        for (int i = 0; i < MIP_LEVELS; i++)
        {
            SAFE_RELEASE(srvs[i]);
            SAFE_RELEASE(rtvs[i]);
        }

        SAFE_RELEASE(srvAllMips);
    }

    void GlowRenderPass::update(float deltaTime)
    {
    }

    void GlowRenderPass::render() const
    {







        // TODOOOOOOOOOOOOOOOOO:
        //
        // �ndra offset i pixel shader f�r kawase dual filter









        if (!enabled)
            return;

        PROFILE_BEGIN("Glow");
        backBuffers->swap();
        
        static float color[4] = { 0, 0, 0, 0 };
        for (int i = 0; i < MIP_LEVELS; i++)
        {
            Global::context->ClearRenderTargetView(rtvs[i], color);
        }

        Global::context->RSSetState(Global::cStates->CullNone());
        Global::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        Global::context->IASetInputLayout(nullptr);
        Global::context->VSSetShader(mipGenerator, nullptr, 0);
        Global::context->PSSetShader(m_KawaseDualFilterDownsample, nullptr, 0);

        auto sampler = Global::cStates->LinearWrap();
        Global::context->PSSetSamplers(0, 1, &sampler);

        //stupid first case
        Global::context->PSSetShaderResources(0, 1, &resources[0]);
        Global::context->OMSetRenderTargets(1, &rtvs[0], nullptr);

        setMipViewPort(0);

        Global::context->Draw(3, 0);

        Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);

        for (int i = 0; i < MIP_LEVELS - 1; i++)
        {
            Global::context->PSSetShaderResources(0, 1, &srvs[i]);
            Global::context->OMSetRenderTargets(1, &rtvs[i + 1], nullptr);

            setMipViewPort(i + 1);

            Global::context->Draw(3, 0);

            Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
            Global::context->PSSetShaderResources(0, 1, Global::nulls);
        }

        Global::context->OMSetRenderTargets(0, nullptr, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);


        setMipViewPort(MIP_LEVELS - 1);

        Global::context->IASetInputLayout(nullptr);
        Global::context->VSSetShader(glow, nullptr, 0);
        Global::context->PSSetShader(glow, nullptr, 0);
        Global::context->OMSetRenderTargets(1, &glowtempRTV, nullptr);
        Global::context->PSSetShaderResources(0, 1, &srvs[MIP_LEVELS - 1]);
        Global::context->Draw(3, 0);

        Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);

        Global::context->OMSetRenderTargets(1, &rtvs[MIP_LEVELS - 1], nullptr);
        Global::context->PSSetShaderResources(0, 1, &glowtempSRV);

        Global::context->VSSetShader(glow2, nullptr, 0);
        Global::context->PSSetShader(glow2, nullptr, 0);
        Global::context->Draw(3, 0);

        Global::context->OMSetRenderTargets(0, nullptr, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);


        Global::context->PSSetShader(m_KawaseDualFilterUpsample, nullptr, 0);
        for (int i = MIP_LEVELS - 1; i > 1; i--)
        {
            Global::context->PSSetShaderResources(0, 1, &srvs[i]);
            Global::context->OMSetRenderTargets(1, &rtvs[i - 1], nullptr);

            setMipViewPort(i - 1);

            Global::context->Draw(3, 0);

            Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
            Global::context->PSSetShaderResources(0, 1, Global::nulls);
        }

       /* Global::context->PSSetShader(mipCombinder, nullptr, 0);
        Global::context->OMSetRenderTargets(1, glowPass1, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);
        setMipViewPort(0);

        Global::context->Draw(3, 0);

        Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);*/
        //First pass

        setMipViewPort(0);

        Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);



        Global::context->VSSetShader(merger, nullptr, 0);
        Global::context->PSSetShader(merger, nullptr, 0);


        Global::context->PSSetShaderResources(4, 1, *backBuffers);
        Global::context->PSSetShaderResources(5, 1, &srvs[1]);
        Global::context->OMSetRenderTargets(1, *backBuffers, nullptr);

        Global::context->PSSetSamplers(2, 1, &sampler);

        Global::context->Draw(3, 0);

        Global::context->OMSetRenderTargets(1, Global::nulls, nullptr);
        Global::context->PSSetShaderResources(4, 1, Global::nulls);
        Global::context->PSSetShaderResources(5, 1, Global::nulls);
        PROFILE_END();

    }

    //Very ugly
    void GlowRenderPass::createMips()
    {
        ID3D11Texture2D * texture;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Width = WIN_WIDTH;
        textureDesc.Height = WIN_HEIGHT;
        textureDesc.MipLevels = MIP_LEVELS;
        textureDesc.ArraySize = 1;
        textureDesc.SampleDesc.Count = 1;

        ThrowIfFailed(Global::device->CreateTexture2D(&textureDesc, 0, &texture));

        D3D11_RENDER_TARGET_VIEW_DESC renderDesc = {};
        renderDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = {};
        resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        resourceDesc.Texture2D.MipLevels = 1;

        for (int i = 0; i < MIP_LEVELS; i++)
        {
            resourceDesc.Texture2D.MostDetailedMip = i;
            renderDesc.Texture2D.MipSlice = i;

            ThrowIfFailed(Global::device->CreateRenderTargetView(texture, &renderDesc, &this->rtvs[i]));
            ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &this->srvs[i]));
        }

        resourceDesc.Texture2D.MipLevels = MIP_LEVELS;
        resourceDesc.Texture2D.MostDetailedMip = 0;

        ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &srvAllMips));

        SAFE_RELEASE(texture);

        auto p = (int)powf(2.f, (float)MIP_LEVELS - 1);
        textureDesc.Width = WIN_WIDTH / p;
        textureDesc.Height = WIN_HEIGHT / p;
        textureDesc.MipLevels = 1;
        ThrowIfFailed(Global::device->CreateTexture2D(&textureDesc, 0, &texture));

        auto glow_level = MIP_LEVELS - 1;
        resourceDesc.Texture2D.MipLevels = 1;
        resourceDesc.Texture2D.MostDetailedMip = 0;
        renderDesc.Texture2D.MipSlice = 0;

        ThrowIfFailed(Global::device->CreateRenderTargetView(texture, &renderDesc, &glowtempRTV));
        ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &glowtempSRV));
        SAFE_RELEASE(texture);

    }
    
    void GlowRenderPass::setMipViewPort(int level) const
    {
        level = (int)powf(2.f, (float)level);
        D3D11_VIEWPORT viewPort = {};
        viewPort.Height = float(WIN_HEIGHT / level);
        viewPort.Width = float(WIN_WIDTH / level);
        viewPort.MaxDepth = 1.f;

        Global::context->RSSetViewports(1, &viewPort);
    }
}