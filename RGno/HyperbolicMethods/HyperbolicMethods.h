//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//      Garrett Vance 20191102 0921 
//
//      file: HyperbolicMethods.h 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

#pragma once 


namespace HvyDXBase
{
    struct HvyGeodesic
    {
    public:
        HvyGeodesic(); 

        HvyGeodesic(bool p_infinite_radius, double p_radius, HvyDXBase::HvyPlex p_center, D2D1_SWEEP_DIRECTION p_sweep); 

        HvyGeodesic(HvyDXBase::HvyGeodesic const& g1); 

        ~HvyGeodesic();

    public:
        bool                            e_infinite_radius;
        double                          e_radius;
        std::complex<double>            e_center;
        D2D1_SWEEP_DIRECTION            e_sweepDirection;
    };

    double HC_CircumradiusFromSchlafli_D(int schlafP, int schlafQ);

    double HC_ApothemFromSchlafli_D(int schlafP, int schlafQ);

    HvyGeodesic HC_GeodesicFromSchlafli_D(unsigned int schlafliP, unsigned int schlafliQ);

    HvyDXBase::HvyPlex HC_Polar_D(double phaseAngle, double modulus = 1.00); 

    HvyDXBase::HvyPlex HC_Negate_D(HvyDXBase::HvyPlex z);

    HvyDXBase::HvyPlex HC_HyperbolicTranslation_D(HvyDXBase::HvyPlex p_amount, HvyDXBase::HvyPlex p_z);

    HvyDXBase::HvyPlex HC_TranslationSpeed_D(HvyDXBase::HvyPlex p_amount, HvyDXBase::HvyPlex p_z);

    HvyDXBase::HvyPlex HC_ComplexSign_D(HvyDXBase::HvyPlex p_z);
}
//  Closes namespace HvyDXBase;




