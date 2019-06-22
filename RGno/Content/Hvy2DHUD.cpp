

#include "pch.h"

#include "Hvy2DHUD.h"
#include "Common/DirectXHelper.h"

using namespace HvyDXBase;
using namespace Microsoft::WRL;






float       g0_fontSize = 28.0f;   //  GOLD:  32.0f;





Hvy2DHUD::Hvy2DHUD(const std::shared_ptr<DX::DeviceResources>& deviceResources) : 
    m_text(L""),
    m_deviceResources(deviceResources)
{
    ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

    // Create device independent resources
    ComPtr<IDWriteTextFormat> textFormat;
    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            g0_fontSize, 
            L"en-US",
            &textFormat
            )
        );

    DX::ThrowIfFailed(
        textFormat.As(&m_textFormat)
        );

    DX::ThrowIfFailed(
        m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
        );

    DX::ThrowIfFailed(
        m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
        );

    CreateDeviceDependentResources();
}









Hvy2DHUD::Hvy2DHUD(
    const std::shared_ptr<DX::DeviceResources>& p_deviceResources,
    const uint32_t&                             p_HUD_id,
    bool                                        p_is_a_button,
    const float&                                p_horizontal_percent,
    const float&                                p_vertical_percent,
    D2D1_COLOR_F                                p_my_color
)
    : m_deviceResources(p_deviceResources),
    m_HUD_id(p_HUD_id),
    m_is_a_button(p_is_a_button),
    m_screen_pos_vert(p_vertical_percent),
    m_screen_pos_horz(p_horizontal_percent),
    m_special_color(p_my_color),
    m_text(L"")
{

    //      
    //      Class ctor for the FpsTextRenderer class 
    //  
    //    Initialize the D2D resources needed to render text 
    //  

    ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));



    // 
    //   First part:  Create device independent resources
    // 

    ComPtr<IDWriteTextFormat> textFormat;


    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextFormat(
            // L"Mametosca 026",   //   L"Segoe UI",

            L"Constantia",   //   L"Segoe UI",

            nullptr,
            DWRITE_FONT_WEIGHT_BOLD,   //   DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            g0_fontSize,
            L"en-US", &textFormat));



    DX::ThrowIfFailed(textFormat.As(&m_textFormat));

    DX::ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

    DX::ThrowIfFailed(m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock));



    //   
    //      Next,  create the device DEPENDENT resources, 
    //  
    //      for example, must create a colored brush.... 
    //  

    CreateDeviceDependentResources();

}  //  Closes class ctor VHG_HUD1::VHG_HUD1(); 























void Hvy2DHUD::Update(
    DX::StepTimer       const&  p_timer,
    std::wstring        const&  p_wchar_message
)
{
    m_text = p_wchar_message;


    //  orig : uint32 fps = p_timer.GetFramesPerSecond();
    //  orig : m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";


    ComPtr<IDWriteTextLayout> textLayout;

    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextLayout(
            m_text.c_str(),
            (uint32) m_text.length(),
            m_textFormat.Get(),
            340.0f, // Max width of the input text.
            50.0f, // Max height of the input text.
            &textLayout
            )
        );

    DX::ThrowIfFailed(
        textLayout.As(&m_textLayout)
        );

    DX::ThrowIfFailed(
        m_textLayout->GetMetrics(&m_textMetrics)
        );
}












void Hvy2DHUD::Render()
{
    ID2D1DeviceContext* d2dContext2D = m_deviceResources->GetD2DDeviceContext();
    Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

    d2dContext2D->SaveDrawingState(m_stateBlock.Get());
    d2dContext2D->BeginDraw();

    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //          
    //  ghv: experimental: 
    //          
    //  Procedure CaptureD2D1Drawing_CreateHitTextureResourceView(), 
    //  as one of its last actions, invokes ID2D1DeviceContext::SetTarget(nullptr). 
    //          
    //  So at this point of execution, the D2D1 device context HAS NO VALID TARGET.
    //  Need to set a valid target for the D2D1 context before trying to draw!!!
    //          

	d2dContext2D->SetTarget(
        m_deviceResources->GetD2DTargetBitmap()  //  Undo the effect of SetTarget(nullptr)...
    );

    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



    //  
    //      m_screen_pos_horz == 0.000 implies "position at left edge of screen";       
    //      m_screen_pos_horz == 0.999 implies "position at right edge of screen";       
    // 
    //      m_screen_pos_vert == 0.000 implies "position along the top edge of screen";       
    //      m_screen_pos_vert == 0.999 implies "position along the bottom edge of screen";    
    // 

    float location_x = (m_screen_pos_horz < 0.5f) ?
        30.f : (logicalSize.Width - m_textMetrics.layoutWidth - 30);

    float location_y = (m_screen_pos_vert < 0.5f) ?
        30.f : (logicalSize.Height - m_textMetrics.height - 30);


    D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(location_x, location_y); 

    d2dContext2D->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());


    DWRITE_TEXT_ALIGNMENT   v_lateral = (m_screen_pos_horz < 0.5f) ?
        DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_TRAILING;

    DX::ThrowIfFailed(m_textFormat->SetTextAlignment(v_lateral));

    //   ???   i wonder if this call to SetTextAlignment() invalidates some of the data held in m_textMetrics ??? 
    //   maybe should re-calculate text metrics now ??? 

    d2dContext2D->DrawTextLayout(D2D1::Point2F(0.f, 0.f), m_textLayout.Get(), m_text_SolidColorBrush.Get());



#ifdef GHV_OPTION_ENABLE_HUD_IS_A_BUTTON
    if (m_is_a_button)
    {
        //   The values of "Left" and "Top" in the following D2D1_RECT_F struct 
        //   must be set to zero because the prior call to d2dContext2D->SetTransform() 
        //   is still exerting its influence over all D2D1 drawing: 

        D2D1_RECT_F buttonRect = D2D1::RectF(0.f, 0.f, m_textMetrics.layoutWidth, m_textMetrics.height);
        D2D1_ROUNDED_RECT const buttonRoundedRect{ buttonRect, 17.f,  17.f };
        d2dContext2D->DrawRoundedRectangle(&buttonRoundedRect, m_text_SolidColorBrush.Get(), 3.0f, NULL);
    }
#endif



    // Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.   

    HRESULT hr = d2dContext2D->EndDraw();    //   <-----   D2D1 EndDraw;

    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    d2dContext2D->RestoreDrawingState(m_stateBlock.Get());
}




















void Hvy2DHUD::CreateDeviceDependentResources()
{
    DX::ThrowIfFailed(
        m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_text_SolidColorBrush)
        );
}





void Hvy2DHUD::ReleaseDeviceDependentResources()
{
    m_text_SolidColorBrush.Reset();
}




