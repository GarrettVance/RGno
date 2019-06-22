




#pragma once

#include <string>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"



namespace HvyDXBase
{


    class Hvy2DHUD
    {
    public:

        Hvy2DHUD(const std::shared_ptr<DX::DeviceResources>& deviceResources);



        Hvy2DHUD::Hvy2DHUD(
            const std::shared_ptr<DX::DeviceResources>& p_deviceResources,
            const uint32_t&                             p_HUD_id,
            bool                                        p_is_a_button,
            const float&                                p_horizontal_percent,
            const float&                                p_vertical_percent,
            D2D1_COLOR_F                                p_my_color
        );


        // TODO: add copy ctor and dtor;



        void CreateDeviceDependentResources();

        void ReleaseDeviceDependentResources();




        void Hvy2DHUD::Update(
            DX::StepTimer       const&  p_timer,
            std::wstring        const&  p_wchar_message
        );


        void Render();


    public:

        uint32_t                                            m_HUD_id;

    private:

        std::shared_ptr<DX::DeviceResources>                m_deviceResources;
        std::wstring                                        m_text;

        bool                                                m_is_a_button; 

        float                                               m_screen_pos_vert;
        float                                               m_screen_pos_horz;
        D2D1_COLOR_F                                        m_special_color;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_text_SolidColorBrush;




        DWRITE_TEXT_METRICS                                 m_textMetrics;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1>     m_stateBlock;
        Microsoft::WRL::ComPtr<IDWriteTextLayout3>          m_textLayout;
        Microsoft::WRL::ComPtr<IDWriteTextFormat2>          m_textFormat;
    };






    class HvyHUDButton : public Hvy2DHUD
    {
        float                                               m_corner_radius;

        float                                               m_outline_thickness;

    };





}
//  Closes namespace HvyDXBase; 



