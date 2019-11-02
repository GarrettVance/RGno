//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//      Garrett Vance 20191102 0921 
//
//      file: HyperbolicMethods.cpp 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

#include "pch.h"
#include <complex>
#include "HyperbolicMethods.h"

using namespace HvyDXBase; 
using namespace Windows::Foundation;


double const HvyDXBase::konst_pi = 3.1415926535897932384626433;



double HvyDXBase::HC_CircumradiusFromSchlafli_D(int schlafP, int schlafQ)
{
    //  Obtain circumradius from a Schlafli Symbol {p, q}. 
    //       
    //  Example: Schlafli {7, 3} has circumradius 0.300 742 618 746...
    //      
    //  The problem at hand is to solve a hyperbolic right triangle 
    //  in the Poincare disk model given only the three angle measures. 
    //  Solution is possible because similarity (AAA) implies congruence; 
    //          
    //  Start from the Hyperbolic Law of Cosines: 
    //  
    //  cos C = -cosA cosB + sinA sinB cosh(cHyp). 
    //          
    //  The triangle is fully determined by its Schlafli symbol, 
    //  and its three angles are pi/p,  pi/q  and pi/2. 
    //      
    //  In the formula above, "cHyp" is the hypotenuse (and also
    //  the circumradius of the tessellation's polygon) 
    //  and angle "C" is pi/2, for which the cosine is zero. 
    //      
    //      
    //              cos pi/2 + cos pi/p * cos pi/q 
    //  cosh cHyp = --------------------------------
    //                  sin pi/p * sin pi/q
    //          
    //      
    //      
    //                zero + cos pi/p * cos pi/q 
    //  cosh cHyp = --------------------------------
    //                    sin pi/p * sin pi/q
    //          
    //      
    //      
    //                           1 * 1
    //  cosh cHyp = --------------------------------
    //                    tan pi/p * tan pi/q
    //          
    //      
    //      
    //                           1
    //  cosh cHyp = --------------------------------
    //                       tanProduct
    //          
    //  where
    //      

    double tanProduct =
        std::tan(konst_pi / (double)schlafP) *
        std::tan(konst_pi / (double)schlafQ);

    double cHyp = std::acosh(1.00 / tanProduct);

    //  
    //  cHyp is the circumradius in hyperbolic measure. 
    //          
    //  Convert from hyperbolic distance to Euclidean distance: 
    //  ======================================================= 
    //  Use Hvidsten Corollary 17.15 to convert
    //  from hyperbolic distance to Euclidean: 
    //      
    //  cEuc aka circumradius in Euclidean measure 
    //      = (exp(cHyp) - 1) / (exp(cHyp) + 1);
    //      
    //  Finally recognize that this expression for cEuc 
    //  looks a lot like the definition of tanh(). 
    //      

    double cEuc = std::tanh( cHyp / 2.000 );
    return cEuc;
}


    
double HvyDXBase::HC_ApothemFromSchlafli_D(int schlafliP, int schlafliQ)
{
    //  This is based on the following identity 
    //  for hyperbolic right triangles: 
    //  
    //  cosh(adjacent) = cos(pi/q) / sin(pi/p)
    //      

    double cosPiQ = cos(konst_pi / (double)schlafliQ);
    double sinPiP = sin(konst_pi / (double)schlafliP);

    double apothemHyp = std::acosh(cosPiQ / sinPiP); 

    //  Convert from hyperbolic measure to Euclidean measure: 

    double apothem_Euc = std::tanh(apothemHyp / 2.000); 
    return apothem_Euc;
}



HvyGeodesic HvyDXBase::HC_GeodesicFromSchlafli_D(unsigned int schlafliP, unsigned int schlafliQ)
{
    double tmpCosQ = cos(konst_pi / (double)schlafliQ);
    double tmpSinP = sin(konst_pi / (double)schlafliP);

    double apothemHyp = std::acosh(tmpCosQ / tmpSinP); 

    //  Convert from hyperbolic measure to Euclidean measure: 

    double apothemEuc = std::tanh(apothemHyp / 2.000);  // TODO: call ApothemFromSchlafli(); 

    //      
    //      ...continue the proof until in the form of...
    //      
    //   cCenter.x = sqrt(cosq2 / (cosq2 - sinp2));
    //   cCenter.y = 0.0;
    //   cRadius = sqrt(sinp2 / (cosq2 - sinp2));
    //      

    double tCenterX = (1.0 + apothemEuc * apothemEuc) / (2.0 * apothemEuc);
    double tCenterY = 0.000;
    double tRadius = (1.0 - apothemEuc * apothemEuc) / (2.0 * apothemEuc);

    //  
    //  TODO:    test:   want apothemEuc + cRadius = cCenter.x
    //  
    // double lhs = apothemEuc + cRadius;
    // double rhs = cCenter.x;
    //  
    //  throw if not precise...
    //  
    double lhs = apothemEuc + tRadius;
    double rhs = tCenterX;

    double relError = (lhs - rhs) / rhs;



    HvyDXBase::HvyGeodesic theGeodesic;  

    theGeodesic.e_infinite_radius = false;
    theGeodesic.e_radius = tRadius;
    theGeodesic.e_center = std::complex<double>(tCenterX, tCenterY);

    // TODO:  need to compute sweep direction  e_sweepDirection;

    return theGeodesic;
}


    
HvyDXBase::HvyPlex HvyDXBase::HC_Polar_D(double phaseAngle, double modulus)
{
    return std::complex<double>(
        modulus * cos(phaseAngle),
        modulus * sin(phaseAngle)
    );
}



HvyDXBase::HvyPlex HvyDXBase::HC_Negate_D(HvyDXBase::HvyPlex z)
{
    return -z;
}



HvyDXBase::HvyPlex HvyDXBase::HC_HyperbolicTranslation_D(HvyDXBase::HvyPlex p_amount, HvyDXBase::HvyPlex p_z)
{
    //  cf. hyperbolic translation, translation axis, translation length; 

    return (p_z + p_amount) / (std::complex<double>(1.00, 0.00) + p_z * std::conj(p_amount));
}



HvyDXBase::HvyPlex HvyDXBase::HC_TranslationSpeed_D(HvyDXBase::HvyPlex p_amount, HvyDXBase::HvyPlex p_z)
{
    //      
    //  This method computes the first derivative of the hyperbolic 
    //  translation. I first saw it in javascript published by Arnaud Cheritat. 
    //      
    //  Please see  https://www.math.univ-toulouse.fr/~cheritat/AppletsDivers/Klein/
    //  for Arnaud Cheritat's excellent javascript Klein Quartic. 
    //      
    HvyPlex cxOne{ 1.00, 0.00 };

    HvyPlex b = cxOne + p_z * std::conj(p_amount);

    HvyPlex diff{ 1.00 - std::norm(p_amount), 0.00 };

    return diff / (b * b);
}



HvyDXBase::HvyPlex HvyDXBase::HC_ComplexSign_D(HvyDXBase::HvyPlex p_z)
{
    //          
    //  This method is sometimes referred to as "Normalize". 
    //  
    //  ...I wish I had found this documentation about 5 hours ago...
    //  https://docs.sympy.org/0.6.7/modules/mpmath/functions/numberparts.html?highlight=sign#mpmath.sign
    //          

    //  TODO: test for divide-by-zero;

    return p_z / std::abs(p_z);  //  ghv : projection onto the unit circle; 
}



HvyDXBase::HvyGeodesic::HvyGeodesic()
    :
    e_infinite_radius(false),
    e_radius(1.0),
    e_center(HvyPlex(1.0, 0.0)),
    e_sweepDirection(D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE)
{}



HvyDXBase::HvyGeodesic::HvyGeodesic(
    bool p_infinite_radius,
    double p_radius,
    HvyDXBase::HvyPlex p_center,
    D2D1_SWEEP_DIRECTION p_sweep)
    :
    e_infinite_radius(p_infinite_radius),
    e_radius(p_radius),
    e_center(p_center),
    e_sweepDirection(p_sweep)
{}



HvyDXBase::HvyGeodesic::HvyGeodesic(HvyDXBase::HvyGeodesic const& g1)
{
    //  copy ctor

    this->e_infinite_radius = g1.e_infinite_radius;
    this->e_radius = g1.e_radius;
    this->e_center = g1.e_center;
    this->e_sweepDirection = g1.e_sweepDirection;
}



HvyDXBase::HvyGeodesic::~HvyGeodesic()
{
    ;
}



