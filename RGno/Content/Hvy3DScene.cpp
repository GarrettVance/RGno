



#include "pch.h"


#include "Hvy3DScene.h"
#include "..\Common\DirectXHelper.h"


using namespace HvyDXBase; 
using namespace DirectX;
using namespace Windows::Foundation;









Hvy3DScene::Hvy3DScene(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_loadingComplete(false),
    m_indexCount(0),
    g0_debouncer_processed_mouse(false),
    g0_debouncer_processed_keyboard(false),

    e_invertColor(false), 


    e_Perambulation(false), 

    z_mouse(0.00, 0.00), 

    uniform_x0(0.50),  
    uniform_z0(0.00, 0.00),  
    uniform_dir(1.00, 0.00),  

    //          
    //    Validated Schlafli Symbols: 
    //       (p - 2) * (q - 2) > 4   
    //=============================
    //          
    //  p = 7, q = 3 Order-3 Heptagonal Tiling; 
    //  
    //  p = 7, q = 4 Order-4 Heptagonal Tiling; 
    //  
    //  p = 8, q = 3 okay;
    //  
    //  p = 5, q = 6 very nice;
    //  
    //  p = 4, q = 5 okay;
    //          

   

    //  schlafli_p(5),  schlafli_q(5), 


    // perfect: 
    schlafli_p(3),  schlafli_q(14), 


    //  optimized:  schlafli_p(5),  schlafli_q(10), 

    // schlafli_p(7),  schlafli_q(3),  // standard; 


    animate(false),

    sd_VariableRadiusFactor(1.f),  //  Neutral = 1.0f;

    m_deviceResources(deviceResources)
{

    e_keyboard = std::make_unique<DirectX::Keyboard>();
    e_keyboard->SetWindow(Windows::UI::Core::CoreWindow::GetForCurrentThread());

    e_mouse = std::make_unique<DirectX::Mouse>();
    e_mouse->SetWindow(Windows::UI::Core::CoreWindow::GetForCurrentThread());


    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();


    CalculateCircumradius();
}











        
void Hvy3DScene::GetSchlafliSymbolPQ(int& s_p, int& s_q)
{
    s_p = this->schlafli_p; 

    s_q = this->schlafli_q; 
}













void Hvy3DScene::CreateWindowSizeDependentResources()
{
    // Initializes view parameters when the window size changes.

    Size outputSize = m_deviceResources->GetOutputSize();
    float aspectRatio = outputSize.Width / outputSize.Height;
    float fovAngleY = 70.0f * XM_PI / 180.0f;
    if (aspectRatio < 1.0f) { fovAngleY *= 2.0f; }

    //  Use a right-handed coordinate system and row-major matrices: 

    XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
        fovAngleY, aspectRatio, 0.01f, 100.0f );

    XMFLOAT4X4 orientationF4X4 = m_deviceResources->GetOrientationTransform3D();
    XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientationF4X4);

    this->e_projection_matrix = perspectiveMatrix * orientationMatrix;

    XMStoreFloat4x4(
        &m_conbuf_MVP_Data.projection,
        XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
    );


    this->e_view_matrix = XMMatrixIdentity();

    XMStoreFloat4x4(&m_conbuf_MVP_Data.view, XMMatrixIdentity());
}















void Hvy3DScene::HandleMouseWheel(DirectX::Mouse::State const&  maus_state)
{
    static int prior_scrollWheelValue = 0;


    int scrollWheelDelta = 0;



    this->e_mouseWheel = maus_state.scrollWheelValue;



    if (maus_state.scrollWheelValue != prior_scrollWheelValue)
    {
        scrollWheelDelta = maus_state.scrollWheelValue - prior_scrollWheelValue;

        prior_scrollWheelValue = maus_state.scrollWheelValue;

        float const scaling_transform_min_scale = 1.00f;
        float const scaling_transform_max_scale = 5.00f;
        float const scaling_transform_step_size = 0.10f;

        int const mousewheel_chunk_size = 120;     //  too coarse: 230;

        float maybe = sd_VariableRadiusFactor + scrollWheelDelta * scaling_transform_step_size / (float)mousewheel_chunk_size;

        float lower_bound = (std::max)(scaling_transform_min_scale, maybe);
        float upper_bound = (std::min)(lower_bound, scaling_transform_max_scale);

        sd_VariableRadiusFactor = upper_bound;
    }
}










void Hvy3DScene::DynamicSchlafli(int p, int q)
{
    schlafli_p = p;

    schlafli_q = q; 

    CalculateCircumradius();

}

















void Hvy3DScene::Update(DX::StepTimer const& timer)
{
    static bool okayToRun = false;

    DirectX::Mouse::State    maus_state = e_mouse->GetState();

    e_mouseTracker.Update(maus_state);


    Coords_SetMouseHover(std::complex<double>(maus_state.x, maus_state.y));


    HandleMouseWheel(maus_state);



    if (maus_state.leftButton == true)
    {

        std::complex<double> mouse_click_physical = std::complex<double>(1.000 * maus_state.x, 1.000 * maus_state.y);


        if (g0_debouncer_processed_mouse == false)
        {
            g0_debouncer_processed_mouse = true; 

            this->z_mouse = Coords_LogicalCoordsFromPhysicalCoords(mouse_click_physical);
                
            // thus z_mouse stores position as Logical coords in [-1, +1];

            if (e_Perambulation == false)
            {
                okayToRun = true; //  Allow single click invocations of anim_callback;
            }
        }
    }


    //========================================================================================

    DirectX::Keyboard::State           kb = e_keyboard->GetState();





    if (kb.F5)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;

            int curr_p = 0; 
            int curr_q = 0; 
            GetSchlafliSymbolPQ(curr_p, curr_q); 
            if (!((curr_p == 5) && (curr_q == 10)))
            {
                DynamicSchlafli(5, 10);
            }
        }
    }


    if (kb.F4)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;

            int curr_p = 0; 
            int curr_q = 0; 
            GetSchlafliSymbolPQ(curr_p, curr_q); 
            if (!((curr_p == 5) && (curr_q == 6)))
            {
                DynamicSchlafli(5, 6);
            }
        }
    }


    if (kb.F3)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;

            int curr_p = 0; 
            int curr_q = 0; 
            GetSchlafliSymbolPQ(curr_p, curr_q); 
            if (!((curr_p == 3) && (curr_q == 14)))
            {
                DynamicSchlafli(3, 14);
            }
        }
    }


    if (kb.F7)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;

            int curr_p = 0; 
            int curr_q = 0; 
            GetSchlafliSymbolPQ(curr_p, curr_q); 
            if (!((curr_p == 7) && (curr_q == 3)))
            {
                DynamicSchlafli(7, 3);
            }
        }
    }




    if (kb.I)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;
            if (e_invertColor == false)
            {
                e_invertColor = true;
            }
            else if (e_invertColor == true)
            {
                e_invertColor = false;
            }
        }
    }



    if (kb.U)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;
            if (e_Perambulation == false)
            {
                e_Perambulation = true;
            }
            else if (e_Perambulation == true)
            {
                e_Perambulation = false;
            }
        }
    }




    if (kb.O)
    {
        if (!g0_debouncer_processed_keyboard)
        {
            g0_debouncer_processed_keyboard = true;

            // e_auto_roving = false;

            // eInvalidated = false; // halt CS Compute Shader invocation;


            GoFileOpenPicker();
        }
    }





















    //===================================================================
    //===================================================================


    if (e_Perambulation == true)
    {
        this->anim_callback_version_RGno(); 
    }
    else
    {
        //  Free-wheel animation has been halted; 
        //  Only single-click invocation is allowed.

        if (okayToRun == true)
        {
            this->anim_callback_version_RGno(); 
            okayToRun = false;
        }
    }



    //===================================================================
    //===================================================================
    //          
    //                      The Model Transform 
    //      
    // (keywords: modeltransform worldtransform world transform)
    //          
    //          

    float uniFactor = 0.7f * sd_VariableRadiusFactor;  

    this->e_model_matrix = XMMatrixScaling(uniFactor, uniFactor, 1.0f);

    DirectX::XMStoreFloat4x4(
        &m_conbuf_MVP_Data.model,
        XMMatrixTranspose(
            e_model_matrix
        )
    );

    //          
    //===================================================================
}

















void Hvy3DScene::Render()
{
    if (!m_loadingComplete)  // Loading is asynchronous. Only draw geometry after it's loaded.
    {
        return;
    }


    this->debouncer_for_mouse();

    this->debouncer_for_keyboard();



    conbufSetDataHC();


    auto d3d_context_3d = m_deviceResources->GetD3DDeviceContext();

    d3d_context_3d->UpdateSubresource1( m_conbuf_MVP_Buffer.Get(), 0, NULL, &m_conbuf_MVP_Data, 0, 0, 0 );



    UINT stride = sizeof(HvyDXBase::Vertex_Pos_Tex_Normal_t);
    UINT offset = 0;
    d3d_context_3d->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);


    //  Index Buffer: Each index is one 16-bit unsigned integer (short): 
    d3d_context_3d->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );


    d3d_context_3d->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    d3d_context_3d->IASetInputLayout(m_inputLayout.Get());


    d3d_context_3d->VSSetShader( m_vertexShader2D.Get(), nullptr, 0 );
    d3d_context_3d->VSSetConstantBuffers1( 0, 1, m_conbuf_MVP_Buffer.GetAddressOf(), nullptr, nullptr );

    d3d_context_3d->RSSetState(e_rasterizer_state.Get());



    d3d_context_3d->PSSetShader( m_pixelShader_01_FunDom.Get(), nullptr, 0 );
    d3d_context_3d->PSSetConstantBuffers1( 1, 1, m_conbuf_HC_Buffer.GetAddressOf(), nullptr, nullptr ); // Slot 1;
    d3d_context_3d->PSSetShaderResources(0, 1, e_srv_FunDomain.GetAddressOf());
    d3d_context_3d->PSSetSamplers(0, 1, e_SamplerState_for_FunDomain.GetAddressOf());


    d3d_context_3d->DrawIndexed( m_indexCount, 0, 0 );
}











void Hvy3DScene::debouncer_for_mouse(void)
{
    static uint32_t idx_waiting = 0;
    if (g0_debouncer_processed_mouse)
    {
        idx_waiting++;
        if (idx_waiting > 0)  //  typical wait = 30 counts;
        {
            idx_waiting = 0; 
            g0_debouncer_processed_mouse = false;
        }
    }
}





void Hvy3DScene::debouncer_for_keyboard(void)
{
    static uint32_t idx_waiting = 0;
    if (g0_debouncer_processed_keyboard)
    {
        idx_waiting++;
        if (idx_waiting > 30)
        {
            idx_waiting = 0;
            g0_debouncer_processed_keyboard = false;
        }
    }
}

















void Hvy3DScene::conbufSetDataHC()
{

    //  The only function of uniform_z0 is to provide mouse-controlled "scrolling" 
    //  of the finished tessellation. 
    //  Remove uniform_z0, or set it fixed at (0.0, 0.0), 
    //  and the tessellation will still be drawn, only the mouse won't have 
    //  any effect (won't give scrolling). But the tessellation looks fine 
    //  when uniform_z0 is fixed at zero!!!!
    //                      



    XMFLOAT2 z0XF2 = XMFLOAT2((float)uniform_z0.real(), (float)uniform_z0.imag());

    XMFLOAT2 dirXF2 = XMFLOAT2((float)uniform_dir.real(), (float)uniform_dir.imag());


    float   renderTargetHalfHeight = (m_deviceResources->GetOutputSize().Height) / 2.f;
    uniform_oneOverScale = 1.f / renderTargetHalfHeight;


    // XMFLOAT4 uniform_color0 = XMFLOAT4(1.f, 0.f, 0.f, 1.f ); // red; Color color0 used for interstitial zones;

    XMFLOAT4 uniform_color0 = XMFLOAT4(1.f, 1.f, 1.f, 1.f ); // white; Color color0 used for interstitial zones;


    XMFLOAT4 uniform_color1 = XMFLOAT4(1.f, 1.f, 1.f, 1.f ); // green; Color color1 for "Negative" Fundamental Domain;

    XMFLOAT4 uniform_color2 = XMFLOAT4(0.f, 0.f, 0.f, 1.f ); // blue; Color color2 is for Fundamental Domain;

    XMUINT4 isInverted = XMUINT4(
        (e_invertColor == true) ? 1 : 0, 
        0, 
        0, 
        0
    );



    conbuf7Struct tmp7Struct = { 
        z0XF2, 
        dirXF2, 
        schlafli_p, 
        schlafli_q, 
        (float)uniform_x0, 
        uniform_oneOverScale, 
        uniform_color0, 
        uniform_color1, 
        uniform_color2, 
        isInverted
    }; 


    D3D11_MAPPED_SUBRESOURCE mapped_subresource_cb7;
    ZeroMemory(&mapped_subresource_cb7, sizeof(D3D11_MAPPED_SUBRESOURCE));

    auto d3d_context_3d = m_deviceResources->GetD3DDeviceContext();


    d3d_context_3d->Map(m_conbuf_HC_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource_cb7);

    memcpy(
        mapped_subresource_cb7.pData,
        &tmp7Struct, 
        sizeof(HvyDXBase::conbuf7Struct)
    );

    d3d_context_3d->Unmap(m_conbuf_HC_Buffer.Get(), 0);
}














void Hvy3DScene::CreateDeviceDependentResources()
{
 
    D3D11_RASTERIZER_DESC   rasterizer_description;
    ZeroMemory(&rasterizer_description, sizeof(rasterizer_description));
    rasterizer_description.MultisampleEnable = FALSE;
    rasterizer_description.FillMode = D3D11_FILL_SOLID;
    rasterizer_description.FrontCounterClockwise = false;
    rasterizer_description.CullMode = D3D11_CULL_BACK; 

    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRasterizerState(
        &rasterizer_description,
        e_rasterizer_state.ReleaseAndGetAddressOf()
    ));

    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    //  Sampler State for the Pixel Shader to render the Fundamental Domain Triangle and its reflections: 

    {
        D3D11_SAMPLER_DESC sampDescFunDomain;
        ZeroMemory(&sampDescFunDomain, sizeof(sampDescFunDomain));
        sampDescFunDomain.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;


        //  To render the bitmap via pixel shader use D3D11_TEXTURE_ADDRESS_CLAMP:

        // sampDescFunDomain.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        // sampDescFunDomain.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        // sampDescFunDomain.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

        sampDescFunDomain.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDescFunDomain.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDescFunDomain.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        sampDescFunDomain.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDescFunDomain.MinLOD = 0;
        sampDescFunDomain.MaxLOD = D3D11_FLOAT32_MAX;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateSamplerState(
                &sampDescFunDomain,
                e_SamplerState_for_FunDomain.ReleaseAndGetAddressOf()
            )
        );
    }


    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    Microsoft::WRL::ComPtr<ID3D11Resource>  temp_resource;

    wchar_t file_path_to_image[] = L"Assets\\a_test.png";

    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            m_deviceResources->GetD3DDevice(), 
            file_path_to_image,
            temp_resource.ReleaseAndGetAddressOf(),
            e_srv_FunDomain.ReleaseAndGetAddressOf(),
            0)
    );




    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    D3D11_RENDER_TARGET_BLEND_DESC  rt_blend_descr = { 0 };
    rt_blend_descr.BlendEnable = TRUE;


    rt_blend_descr.SrcBlend = D3D11_BLEND_SRC_ALPHA;        // SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rt_blend_descr.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;   // DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rt_blend_descr.BlendOp = D3D11_BLEND_OP_ADD;            // BlendOp = D3D11_BLEND_OP_ADD;

    rt_blend_descr.BlendOp = D3D11_BLEND_OP_SUBTRACT; // undo

    rt_blend_descr.SrcBlendAlpha = D3D11_BLEND_ONE;



    rt_blend_descr.DestBlendAlpha = D3D11_BLEND_ZERO; 
    // rt_blend_descr.DestBlendAlpha = D3D11_BLEND_ONE; //undo



    rt_blend_descr.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rt_blend_descr.BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT; // undo


#undef GHV_OPTION_ENABLE_D3D11_BLEND_WEIRD

#ifdef GHV_OPTION_ENABLE_D3D11_BLEND_WEIRD

    //   Weird effect: the d2d1 lines become glassy transparent: 

    rt_blend_descr.SrcBlend = D3D11_BLEND_SRC_COLOR;
    rt_blend_descr.DestBlend = D3D11_BLEND_INV_SRC_COLOR;
    rt_blend_descr.BlendOp = D3D11_BLEND_OP_ADD;

    rt_blend_descr.SrcBlendAlpha = D3D11_BLEND_ONE;
    rt_blend_descr.DestBlendAlpha = D3D11_BLEND_ZERO;
    rt_blend_descr.BlendOpAlpha = D3D11_BLEND_OP_ADD;
#endif


    rt_blend_descr.RenderTargetWriteMask = 0x0F;


    D3D11_BLEND_DESC  d3d11_blend_descr = { 0 };





    d3d11_blend_descr.AlphaToCoverageEnable = FALSE;  //  <---------------- this is boring. 

    d3d11_blend_descr.AlphaToCoverageEnable = TRUE; // undo






    // d3d11_blend_descr.IndependentBlendEnable = FALSE;
    d3d11_blend_descr.IndependentBlendEnable = TRUE;


    d3d11_blend_descr.RenderTarget[0] = { rt_blend_descr };


    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBlendState(
            &d3d11_blend_descr, 
            s3d_blend_state.GetAddressOf()
        )
    );


    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    {
        D3D11_BUFFER_DESC conbufDescHCBuffer;
        ZeroMemory(&conbufDescHCBuffer, sizeof(D3D11_BUFFER_DESC));


        conbufDescHCBuffer.Usage = D3D11_USAGE_DYNAMIC;
        conbufDescHCBuffer.ByteWidth = sizeof(HvyDXBase::conbuf7Struct);

        conbufDescHCBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        conbufDescHCBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        conbufDescHCBuffer.MiscFlags = 0;

        static_assert((sizeof(conbuf7Struct) % 16) == 0, "Constant buffer struct must be 16-byte aligned");

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &conbufDescHCBuffer,
                nullptr,
                &m_conbuf_HC_Buffer
            )
        );
    }



    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    // Load shaders asynchronously.


    auto loadVS_2D_Task = DX::ReadDataAsync(L"vs_2D_vertex_shader.cso");

    auto loadPS_01_Task = DX::ReadDataAsync(L"ps_01_FunDomain.cso");










    auto createVS_2D_Task = loadVS_2D_Task.then([this](const std::vector<byte>& fileData) 
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateVertexShader( 
                &fileData[0], 
                fileData.size(), 
                nullptr, 
                &m_vertexShader2D 
            ) 
        );


        static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
        {
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0,                             D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0,  D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },

        };


        DX::ThrowIfFailed( 
            m_deviceResources->GetD3DDevice()->CreateInputLayout( 
                vertexDesc, 
                ARRAYSIZE(vertexDesc), 
                &fileData[0], 
                fileData.size(), 
                &m_inputLayout 
            ) 
        );
    });











    auto createPS_01_Task = loadPS_01_Task.then([this](const std::vector<byte>& fileData) {

        DX::ThrowIfFailed( m_deviceResources->GetD3DDevice()->CreatePixelShader(
                &fileData[0], fileData.size(), nullptr, &m_pixelShader_01_FunDom)
        );

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);

        static_assert((sizeof(ModelViewProjectionConstantBuffer) % 16) == 0, "Constant buffer struct must be 16-byte aligned");

        DX::ThrowIfFailed( m_deviceResources->GetD3DDevice()->CreateBuffer( &constantBufferDesc, nullptr, &m_conbuf_MVP_Buffer ) );
    });








    auto createCubeTask = (createPS_01_Task && createVS_2D_Task).then([this]()
    {
        this->MeshMonoQuad();  // ghv : one quad composed of two triangles;
    });







    createCubeTask.then([this] () {
        m_loadingComplete = true;
    });
}















void Hvy3DScene::ReleaseDeviceDependentResources()
{
    m_loadingComplete = false;
     

    m_vertexShader2D.Reset();


    m_inputLayout.Reset();


    m_pixelShader_01_FunDom.Reset();



    m_conbuf_MVP_Buffer.Reset();


    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
}














void Hvy3DScene::MeshMonoQuad()
{
    // ghv : Values of texco texture coordinates will be 
    // set inside the VS vertex shader. 

    float a = 0.577f;  //  sqrt of one-third; 

    float u_min = 0.0f; float u_max = 1.f;
    float v_min = 0.0f; float v_max = 1.f;


    // u_min = 0.2f; u_max = 0.8f; v_min = 0.2f; v_max = 0.8f;


    static const HvyDXBase::Vertex_Pos_Tex_Normal_t   monoQuadVertices[] =
    {
        //   increasing negative values of z push the mesh deeper into the background: 

        {XMFLOAT3(-1.0f, -1.0f,  -1.f), XMFLOAT2(u_min, v_max), XMFLOAT3(0.0f, 0.0f, 0.0f)},
        {XMFLOAT3(+1.0f, -1.0f,  -1.f), XMFLOAT2(u_max, v_max), XMFLOAT3(0.0f, 0.0f, 0.0f)},
        {XMFLOAT3(-1.0f, +1.0f,  -1.f), XMFLOAT2(u_min, v_min), XMFLOAT3(0.0f, 0.0f, 0.0f)},

        {XMFLOAT3(-1.0f, +1.0f,  -1.f), XMFLOAT2(u_min, v_min), XMFLOAT3(1.0f, 1.0f, 1.0f)},
        {XMFLOAT3(+1.0f, -1.0f,  -1.f), XMFLOAT2(u_max, v_max), XMFLOAT3(1.0f, 1.0f, 1.0f)},
        {XMFLOAT3(+1.0f, +1.0f,  -1.f), XMFLOAT2(u_max, v_min), XMFLOAT3(1.0f, 1.0f, 1.0f)},
    };

    D3D11_SUBRESOURCE_DATA quad_vb_data = { 0 };
    quad_vb_data.pSysMem = monoQuadVertices;
    quad_vb_data.SysMemPitch = 0;
    quad_vb_data.SysMemSlicePitch = 0;

    CD3D11_BUFFER_DESC quad_vb_descr(sizeof(monoQuadVertices), D3D11_BIND_VERTEX_BUFFER);

    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(
        &quad_vb_descr, &quad_vb_data, &m_vertexBuffer
    )
    );



    //===============================================
    //  Each triangle below is FRONT_FACE_CLOCKWISE: 
    //          
    //  (cf D3D11_RASTERIZER_DESC.FrontCounterClockwise);
    //      
    //  Each trio of index entries represents one triangle. 
    //===============================================

    static const unsigned short quadIndices[] =
    {
        0,2,1,
        5,4,3,
    };

    m_indexCount = ARRAYSIZE(quadIndices);


    D3D11_SUBRESOURCE_DATA quad_ib_data = { 0 };
    quad_ib_data.pSysMem = quadIndices;
    quad_ib_data.SysMemPitch = 0;
    quad_ib_data.SysMemSlicePitch = 0;

    CD3D11_BUFFER_DESC quad_ib_descr(sizeof(quadIndices), D3D11_BIND_INDEX_BUFFER);

    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer( &quad_ib_descr, &quad_ib_data, &m_indexBuffer ) );
}

















//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                  
//                  Simplified Coordinates
//                  ======================
//          
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





float Hvy3DScene::g_cdisk_phys_optimal_radius()
{
    //  Half-height of the RenderTarget is the optimal 
    //  radius for the Poincare Disk (in Physical Coords aka pixels): 

    Windows::Foundation::Size   renderTargetPixels = m_deviceResources->GetOutputSize();

    return renderTargetPixels.Height / 2.0f;
}






D2D1_POINT_2F Hvy3DScene::g_getCentroidPhys()
{

    Windows::Foundation::Size   renderTargetPixels = m_deviceResources->GetOutputSize();

    float halfWidthRT = renderTargetPixels.Width / 2.f;
    float halfHeightRT = renderTargetPixels.Height / 2.f;

    D2D1_POINT_2F   centroid_2f = D2D1::Point2F(halfWidthRT, halfHeightRT);

    return centroid_2f;
}







std::complex<double> Hvy3DScene::Coords_LogicalCoordsFromPhysicalCoords(std::complex<double> p_physical)
{
    //  Note the signum of the Logical y-coordinate: 
    //================================================================


    double x_logical =  (p_physical.real() - g_getCentroidPhys().x) / g_cdisk_phys_optimal_radius() / sd_VariableRadiusFactor;

    double y_logical =  (g_getCentroidPhys().y - p_physical.imag()) / g_cdisk_phys_optimal_radius() / sd_VariableRadiusFactor; // signum!!!

    //  TODO:   remove sd_VariableRadiusFactor; it's just awful...

    return std::complex<double>(x_logical, y_logical);
}









void Hvy3DScene::Coords_SetMouseHover(std::complex<double> physMouse)
{
    this->m_mouse_hover_phys = physMouse;

    this->m_mouse_hover_logical = this->Coords_LogicalCoordsFromPhysicalCoords(this->m_mouse_hover_phys); 

}























