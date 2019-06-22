

#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"




namespace HvyDXBase
{

    struct ModelViewProjectionConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
    };






    struct conbuf7Struct
    {
        DirectX::XMFLOAT2       uniform_z0;

        DirectX::XMFLOAT2       uniform_dir;

        int                     schlafli_p;
        int                     schlafli_q;

        float                   uniform_x0;
        float                   uniform_oneOverScale;

        DirectX::XMFLOAT4       uniform_color0;
        DirectX::XMFLOAT4       uniform_color1;
        DirectX::XMFLOAT4       uniform_color2;

        DirectX::XMUINT4        color_inverted;
    };





    struct Vertex_Pos_Tex_Normal_t
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tex;
        DirectX::XMFLOAT3 normal;
    };






    class Hvy3DScene
    {
    public:

        Hvy3DScene(const std::shared_ptr<DX::DeviceResources>& deviceResources);


        void                        CreateDeviceDependentResources();

        void                        CreateWindowSizeDependentResources();

        void                        ReleaseDeviceDependentResources();


        void                        Update(DX::StepTimer const& timer);


        void                        Render();


        HvyDXBase::HvyPlex          GetMouseHoverPhysical() { return m_mouse_hover_phys; }

        HvyDXBase::HvyPlex          GetMouseHoverLogical() { return m_mouse_hover_logical; }


        void                        GetSchlafliSymbolPQ(int& s_p, int& s_q); 



        int                         GetMouseWheel()     const { return e_mouseWheel; }

        float                       GetVariableRadiusFactor() const  { return sd_VariableRadiusFactor; }



        void                        DynamicSchlafli(int p, int q);





    private:

        float                       g_cdisk_phys_optimal_radius();

        D2D1_POINT_2F               g_getCentroidPhys();

        std::complex<double>        Coords_LogicalCoordsFromPhysicalCoords(std::complex<double> p_physical);

        void                        Coords_SetMouseHover(std::complex<double> physMouse);




        void                        CalculateCircumradius();  //  intrinsic geometry per Schlafli Symbol; 



        void                        anim_callback_version_RGno();


        void                        correct_version_RGno();



        void                        conbufSetDataHC();

        void                        MeshMonoQuad();

        void                        debouncer_for_mouse(void);

        void                        debouncer_for_keyboard(void);

        void                        HandleMouseWheel(DirectX::Mouse::State const&  maus_state);




    private:
        std::shared_ptr<DX::DeviceResources>                    m_deviceResources;




        bool                                                    m_loadingComplete;
        bool                                                    g0_debouncer_processed_mouse;
        bool                                                    g0_debouncer_processed_keyboard;









        std::unique_ptr<DirectX::Keyboard>                      e_keyboard;
        std::unique_ptr<DirectX::Mouse>                         e_mouse;
        DirectX::Mouse::ButtonStateTracker                      e_mouseTracker;
        int                                                     e_mouseWheel;

        DirectX::SimpleMath::Matrix                             e_model_matrix;
        DirectX::SimpleMath::Matrix                             e_view_matrix;
        DirectX::SimpleMath::Matrix                             e_projection_matrix;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                    m_conbuf_MVP_Buffer;
        ModelViewProjectionConstantBuffer                       m_conbuf_MVP_Data;

        Microsoft::WRL::ComPtr<ID3D11InputLayout>               m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                    m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                    m_indexBuffer;
        uint32                                                  m_indexCount;

        Microsoft::WRL::ComPtr<ID3D11VertexShader>              m_vertexShader2D;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>               m_pixelShader_01_FunDom;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>              e_SamplerState_for_FunDomain;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_FunDomain;
        Microsoft::WRL::ComPtr<ID3D11BlendState>                s3d_blend_state;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>           e_rasterizer_state;







        Microsoft::WRL::ComPtr<ID3D11Buffer>                    m_conbuf_HC_Buffer;




        float                                                   m_percentOfTrick;

        bool                                                    e_invertColor;

        bool                                                    e_Perambulation;



        HvyDXBase::HvyPlex              z_mouse; // also used for type 7; 



        double                          uniform_x0;  //  TODO: x0 will be redundant with circumradius;

        HvyDXBase::HvyPlex              uniform_z0;  
        HvyDXBase::HvyPlex              uniform_dir; 

        int                             schlafli_p;   //  Vertex Count;
        int                             schlafli_q;   //  Vertex Valence;

        float                           uniform_oneOverScale;  // reciprocal of render target half-height; 
        bool                            animate; 


        float                           sd_VariableRadiusFactor;
        std::complex<double>            m_mouse_hover_phys;
        std::complex<double>            m_mouse_hover_logical;



    };

}
//  Closes namespace HvyDXBase; 




