//              
//              
//      Garrett Vance 20191022 0621 
//              
//      file: Observer.cpp 
//      
//      Attribution: Much of the mathematical content of 
//      the RGno project is derived from published work 
//      by Arnaud Cheritat. 
//      
//      Please see  https://www.math.univ-toulouse.fr/~cheritat/AppletsDivers/Klein/
//      for Arnaud Cheritat's excellent javascript Klein Quartic. 
//              

#include "pch.h"
#include "..\Content\Hvy3DScene.h"
#include "..\Common\DirectXHelper.h"
#include "..\HyperbolicMethods\HyperbolicMethods.h"

using namespace HvyDXBase; 
using namespace DirectX;
using namespace Windows::Foundation;





void HvyDXBase::Hvy3DScene::correct_version_RGno()
{
    HvyPlex cxCircumradius = std::complex<double>(uniform_x0, 0.00); // complex circumradius of central polygon;

    double tau = 2.00 * konst_pi; 

    for (uint32_t count = 0; count < 20; count++)
    {
        double t = std::arg(uniform_z0);

        double s = t / tau * schlafli_q; // swapped p and q;

        if (std::abs(s) > 0.50)
        {
            double n = std::floor(s + 0.50);

            HvyPlex ro = HvyDXBase::HC_Polar_D(-tau / schlafli_q * n); // swapped p and q; 

            uniform_z0 = uniform_z0 * ro;

            uniform_dir = uniform_dir * ro;
        }

        HvyPlex w = HC_Negate_D(
            HvyDXBase::HC_HyperbolicTranslation_D(
                HC_Negate_D(cxCircumradius),
                uniform_z0
            )
        );

        t = std::arg(w);

        s = t / tau * schlafli_p; // swapped p and q;

        if (std::abs(s) < 0.50)
        {
            break;
        }
        else
        {
            uniform_dir = HC_Negate_D(
                // uniform_dir * derivativePhi(uniform_z0, HC_Negate_D(cxCircumradius))
                uniform_dir * HC_TranslationSpeed_D(HC_Negate_D(cxCircumradius), uniform_z0)
            );

            double n = std::floor(s + 0.50);

            HvyPlex ro = HvyDXBase::HC_Polar_D(-tau / schlafli_p * n);  // swapped p and q;

            w = w * ro;

            uniform_dir = uniform_dir * ro;


            uniform_z0 = HvyDXBase::HC_HyperbolicTranslation_D(
                cxCircumradius, 
                HC_Negate_D(w)
            );

            //  uniform_dir = HC_Negate_D(uniform_dir) * derivativePhi(HC_Negate_D(w), cxCircumradius);
            uniform_dir = HC_Negate_D(uniform_dir) * HC_TranslationSpeed_D(cxCircumradius, HC_Negate_D(w));
        }

    } //  Closes "for" loop; 

    uniform_dir = HvyDXBase::HC_ComplexSign_D(uniform_dir); // bug fixed: Must capture the return value of complex_normalize!!!

    //   
    //  ghv: The following tests using "isnan()" are shown copied verbatim and commented out,
    //  and then as I believe they were intended to read:
    //      
    //  if ( isnan(uniform_z0.real()) || isnan(uniform_z0.real()) || isnan(uniform_dir.real()) || isnan(uniform_dir.real()) )
    //      

    if (isnan(uniform_z0.real()) || isnan(uniform_z0.imag()) || isnan(uniform_dir.real()) || isnan(uniform_dir.imag()))
    {
        uniform_z0 = std::complex<double>(0.00, 0.00); 
        uniform_dir = std::complex<double>(1.00, 0.00); 
    }
}







void HvyDXBase::Hvy3DScene::anim_callback_version_RGno()
{
    HvyPlex z = std::complex<double>(0.025, 0.00) * z_mouse;

    double d = std::norm(z);  // real * real + imag * imag;

    if (d > 0.25) 
    { 
        z = std::complex<double>(0.50 / sqrt(d), 0.00) * z; 
    }

    z = z * uniform_dir;

    // uniform_dir = uniform_dir * HvyDXBase::derivativePhi(z, uniform_z0);
    uniform_dir = uniform_dir * HvyDXBase::HC_TranslationSpeed_D(uniform_z0, z);

    HvyPlex z1 = HvyDXBase::HC_HyperbolicTranslation_D(
        uniform_z0, 
        z
    );

    uniform_dir = uniform_dir * std::complex<double>(1.00 / std::abs(uniform_dir), 0.00);

    uniform_z0 = z1;

    correct_version_RGno();
}




