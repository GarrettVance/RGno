




#pragma once


#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"


#include "Content\Hvy3DScene.h"
#include "Content\Hvy2DHUD.h"




namespace RGno
{

    class RGnoMain : public DX::IDeviceNotify
    {
    public:
        RGnoMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        ~RGnoMain();



        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();


        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:

        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        std::unique_ptr<HvyDXBase::Hvy3DScene>          m_sceneRenderer;

        std::unique_ptr<HvyDXBase::Hvy2DHUD>            m_HUDUpperLeft;
        std::unique_ptr<HvyDXBase::Hvy2DHUD>            m_HUDLowerLeft;
        std::unique_ptr<HvyDXBase::Hvy2DHUD>            m_HUDUpperRight;
        std::unique_ptr<HvyDXBase::Hvy2DHUD>            m_HUDLowerRight;


        DX::StepTimer                                   m_timer;
    };
}



