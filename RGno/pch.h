
#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>

#include <agile.h>  // ghv : if want to #include <agile.h> then must compile with /ZW.
//  #include <ppltasks.h>       //  ghv : for asynchronous file reader tasks;
#include <concrt.h>



#include <complex>          //  ghv;
#include <vector>           //  ghv;
#include <map>              //  ghv : 2018_07_29;
#include <random>           //  ghv : for Mersenne twister;



#include <d2d1helper.h>     //  ghv : 2018_08_03 : for ctor D2D1::Point2F(); 
#include <d2d1_1.h>         //  ghv : 2018_08_09 : for D2D1_BITMAP_PROPERTIES1 structure;

#include "SimpleMath.h"     //  ghv : for DirectX::SimpleMath::Matrix class;


#include "Keyboard.h"
#include "Mouse.h"
#include "WICTextureLoader.h" 



#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "dxguid")   //  ghv : 2018_07_30 for unresolved symbol CLSID_D2D1Opacity for D2D1Effect;




#undef  GHV_OPTION_ENABLE_DOUBLE_PRECISION_MATH


#define GHV_OPTION_ENABLE_HUD


#undef GHV_OPTION_ENABLE_HUD_IS_A_BUTTON






namespace HvyDXBase
{
    extern double const konst_pi;


    using HvyPlex = std::complex<double>;
}







template <class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != nullptr)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = nullptr;
    }
}










template <class DestInterface, class SourceInterace>
inline void
SafeReplace(
    DestInterface **ppDestInterface,
    SourceInterace *pSourceInterface
)
{
    if (*ppDestInterface != nullptr)
    {
        (*ppDestInterface)->Release();
    }

    *ppDestInterface = pSourceInterface;

    if (pSourceInterface)
    {
        (*ppDestInterface)->AddRef();
    }
}







