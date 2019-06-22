


#include "pch.h"

#include "RGnoMain.h"
#include "Common\DirectXHelper.h"




using namespace RGno;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;






RGnoMain::RGnoMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{

	// Register to be notified if the Device is lost or recreated

	m_deviceResources->RegisterDeviceNotify(this);




	m_sceneRenderer = std::unique_ptr<HvyDXBase::Hvy3DScene>(new HvyDXBase::Hvy3DScene(m_deviceResources));



	m_HUDUpperLeft = std::unique_ptr<HvyDXBase::Hvy2DHUD>(new HvyDXBase::Hvy2DHUD(m_deviceResources, 1, false, 0.01f, 0.01f, D2D1::ColorF(D2D1::ColorF::Red)));
	m_HUDLowerLeft = std::unique_ptr<HvyDXBase::Hvy2DHUD>(new HvyDXBase::Hvy2DHUD(m_deviceResources, 2, true,  0.01f, 0.99f, D2D1::ColorF(D2D1::ColorF::Green)));



	m_HUDUpperRight = std::unique_ptr<HvyDXBase::Hvy2DHUD>(new HvyDXBase::Hvy2DHUD(m_deviceResources, 3, false, 0.99f, 0.01f, D2D1::ColorF(D2D1::ColorF::Blue)));
	m_HUDLowerRight = std::unique_ptr<HvyDXBase::Hvy2DHUD>(new HvyDXBase::Hvy2DHUD(m_deviceResources, 4, false, 0.99f, 0.99f, D2D1::ColorF(D2D1::ColorF::White)));



	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:

	// m_timer.SetFixedTimeStep(true); m_timer.SetTargetElapsedSeconds(1.0 / 15);

}






RGnoMain::~RGnoMain()
{
	// Deregister device notification

	m_deviceResources->RegisterDeviceNotify(nullptr);
}











void RGnoMain::CreateWindowSizeDependentResources() 
{
    // Updates application state when the window size changes (e.g. device orientation change)

	m_sceneRenderer->CreateWindowSizeDependentResources();
}










#include <iomanip>   //  ghv : 20181222 : for "setprecision" used in stringstream call; 


void RGnoMain::Update()
{

	m_timer.Tick([&]()
	{
		m_sceneRenderer->Update(m_timer);

        //**********************************************************************************
        //**********************************************************************************
        
        {
            HvyDXBase::HvyPlex mouseHoverPhys = m_sceneRenderer->GetMouseHoverPhysical();
            std::wstringstream  wStringStreem;
            std::wstring mousePhysLocation{ L"Physical: " };

            mousePhysLocation += L"\n";   //  std::endl;
            mousePhysLocation += L"x: ";
            wStringStreem << std::fixed << std::setprecision(2) << mouseHoverPhys.real();
            mousePhysLocation += wStringStreem.str();

            wStringStreem.str(std::wstring());  //  Zero-out the wstringstream;

            mousePhysLocation += L",";
            mousePhysLocation += L"\n";   //  std::endl;
            mousePhysLocation += L"y: ";
            wStringStreem << std::fixed << std::setprecision(2) << mouseHoverPhys.imag();
            mousePhysLocation += wStringStreem.str();

            m_HUDUpperLeft->Update(m_timer, mousePhysLocation.c_str());
        }

        //**********************************************************************************
        //**********************************************************************************

        {
            HvyDXBase::HvyPlex mouseHoverLogical = m_sceneRenderer->GetMouseHoverLogical();
            std::wstringstream  wStringStreem;
            std::wstring mouseLogicalLocation{ L"Logical: " };

            mouseLogicalLocation += L"\n";   //  std::endl;
            mouseLogicalLocation += L"x: ";
            wStringStreem << std::fixed << std::setprecision(4) << mouseHoverLogical.real();
            mouseLogicalLocation += wStringStreem.str();

            wStringStreem.str(std::wstring());  //  Zero-out the wstringstream;

            mouseLogicalLocation += L",";
            mouseLogicalLocation += L"\n";   //  std::endl;
            mouseLogicalLocation += L"y: ";
            wStringStreem << std::fixed << std::setprecision(4) << mouseHoverLogical.imag();
            mouseLogicalLocation += wStringStreem.str();

            m_HUDLowerLeft->Update(m_timer, mouseLogicalLocation.c_str());  //  Lower left;
        }

        //**********************************************************************************
        //**********************************************************************************


        std::wstring  HUD3wstr = L"Schlafli "; 
        int schl_p = 0; 
        int schl_q = 0; 
        m_sceneRenderer->GetSchlafliSymbolPQ(schl_p, schl_q); 

        HUD3wstr += std::to_wstring(schl_p); 

        HUD3wstr += L", "; 

        HUD3wstr += std::to_wstring(schl_q); 

        m_HUDUpperRight->Update(m_timer, HUD3wstr.c_str());


        //**********************************************************************************
        //**********************************************************************************

        {
            float hVRF = m_sceneRenderer->GetVariableRadiusFactor();

            std::wstringstream  wStringStreem;
            std::wstring theVRF{ L"VRF: " };

            theVRF += L"\n";   //  std::endl;
            theVRF += L"z = ";
            wStringStreem << std::fixed << std::setprecision(6) << hVRF;
            theVRF += wStringStreem.str();

            wStringStreem.str(std::wstring());  //  Zero-out the wstringstream;


            m_HUDLowerRight->Update(m_timer, theVRF.c_str());  //  Lower right HUD;
        }

        //**********************************************************************************

	});
}















bool RGnoMain::Render() 
{
    // Returns true if the frame was rendered and is ready to be displayed.


	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}




	auto context = m_deviceResources->GetD3DDeviceContext();



	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);



	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());




	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);




    //  TODO:  undo this fiasco: swap order of rendering
    //  from Classic d2d1 HUD rendered then d3d11 pipeline rendered
    //  to novel experiment: First render the d3d11 pipeline, 
    //  then render d2d1 HUDs on top of the d3d11 graphics: 


	m_sceneRenderer->Render(); // undo 




#ifdef GHV_OPTION_ENABLE_HUD

	m_HUDUpperLeft->Render();
	m_HUDLowerLeft->Render();
	m_HUDUpperRight->Render();
	m_HUDLowerRight->Render();
#endif 


	//  undo:  m_sceneRenderer->Render(); // GOLD;


	return true;
}













void RGnoMain::OnDeviceLost()
{
    // Notifies renderers that device resources need to be released.

	m_sceneRenderer->ReleaseDeviceDependentResources();


	m_HUDUpperLeft->ReleaseDeviceDependentResources();
	m_HUDLowerLeft->ReleaseDeviceDependentResources();

	m_HUDUpperRight->ReleaseDeviceDependentResources();
	m_HUDLowerRight->ReleaseDeviceDependentResources();
}











void RGnoMain::OnDeviceRestored()
{
    // Notifies renderers that device resources may now be recreated.

	m_sceneRenderer->CreateDeviceDependentResources();


	m_HUDUpperLeft->CreateDeviceDependentResources();
	m_HUDLowerLeft->CreateDeviceDependentResources();


	m_HUDUpperRight->CreateDeviceDependentResources();
	m_HUDLowerRight->CreateDeviceDependentResources();


	CreateWindowSizeDependentResources();
}



