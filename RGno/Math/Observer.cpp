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
#include "MvyH_HyperbolicMath.h"

using namespace HvyDXBase; 
using namespace DirectX;
using namespace Windows::Foundation;




void HvyDXBase::Hvy3DScene::CalculateCircumradius()
{
    //  This method translates a Schlafli Symbol {p, q} 
    //  into the circumradius of the tiling's central polygon.
    //  Example: Schlafli {7, 3} is a heptagonal tiling, 
    //  and the central heptagon has circumradius ~ 0.300 742 618 746...
    //      

    double circumradius = std::tanh(
        0.50 * std::acosh(1.00 / (tan(konst_pi / (double)(this->schlafli_p)) * std::tan(konst_pi / (double)(this->schlafli_q))))
    );

    uniform_x0 = circumradius; 

    uniform_z0 = std::complex<double>(circumradius, 0.00); 
}



HvyDXBase::HvyPlex complexScale(double aScalar, HvyDXBase::HvyPlex z)
{
    std::complex<double> retval = std::complex<double>(
        aScalar * z.real(),
        aScalar * z.imag()
        );

    return retval;
}


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

            HvyPlex ro = HvyDXBase::complex_ooo(-tau / schlafli_q * n); // swapped p and q; 

            uniform_z0 = uniform_z0 * ro;

            uniform_dir = uniform_dir * ro;
        }

        HvyPlex w = complex_neg(
            HvyDXBase::hyper_translate(
                complex_neg(cxCircumradius),
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
            uniform_dir = complex_neg(
                uniform_dir * derivativePhi(uniform_z0, complex_neg(cxCircumradius))
            );

            double n = std::floor(s + 0.50);

            HvyPlex ro = HvyDXBase::complex_ooo(-tau / schlafli_p * n);  // swapped p and q;

            w = w * ro;

            uniform_dir = uniform_dir * ro;


            uniform_z0 = HvyDXBase::hyper_translate(
                cxCircumradius, 
                complex_neg(w)
            );

            uniform_dir = complex_neg(uniform_dir) * derivativePhi(complex_neg(w), cxCircumradius);
        }

    } //  Closes "for" loop; 

    uniform_dir = HvyDXBase::complex_normalize(uniform_dir); // bug fixed: Must capture the return value of complex_normalize!!!

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

    uniform_dir = uniform_dir * HvyDXBase::derivativePhi(z, uniform_z0);

    HvyPlex z1 = HvyDXBase::hyper_translate(
        uniform_z0, 
        z
    );

    uniform_dir = uniform_dir * std::complex<double>(1.00 / std::abs(uniform_dir), 0.00);

    uniform_z0 = z1;

    correct_version_RGno();
}




