
#pragma once 

#include "SimpleMath.h"  // ghv : for Vector2; 




namespace HvyDXBase
{

    struct HvyGeodesic  //  Required by hyperbolicMath.cpp; 
    {
    public:
        bool                            e_infinite_radius;
        double                          e_radius;
        std::complex<double>            e_center;
    };




    std::complex<double> hyper_sign(std::complex<double> p_z);


    std::complex<double> hyper_translate(std::complex<double> p_reference, std::complex<double> p_z);


    HvyDXBase::HvyPlex derivativePhi(HvyDXBase::HvyPlex z, HvyDXBase::HvyPlex reference_a);


    HvyDXBase::HvyPlex complex_ooo(double p_real);


    HvyDXBase::HvyPlex complex_neg(HvyDXBase::HvyPlex p_z);


    HvyDXBase::HvyPlex complex_normalize(HvyDXBase::HvyPlex z);

}
//  Closes namespace HvyDXBase;




