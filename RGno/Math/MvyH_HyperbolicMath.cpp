


#include "pch.h"

#include "MvyH_HyperbolicMath.h"



using namespace HvyDXBase;
using namespace DirectX;
using namespace Windows::Foundation;




double const HvyDXBase::konst_pi = 3.1415926535897932384626433;







std::complex<double> HvyDXBase::hyper_sign(std::complex<double> p_z)
{
    //          
    //      ghv : ...I wish I had found this documentation about 5 hours ago...
    //          
    //      https://docs.sympy.org/0.6.7/modules/mpmath/functions/numberparts.html?highlight=sign#mpmath.sign
    //          

    return p_z / std::abs(p_z);  //  ghv : projection onto the unit circle; 

}










std::complex<double> HvyDXBase::hyper_translate(std::complex<double> p_reference, std::complex<double> p_z)
{
    std::complex<double> konst_one = std::complex<double>(1.000, 0.000); 

    return (p_z + p_reference) / (p_z * std::conj(p_reference) + konst_one);
}










HvyDXBase::HvyPlex HvyDXBase::derivativePhi(HvyDXBase::HvyPlex z, HvyDXBase::HvyPlex reference_a)
{
    HvyPlex cxOne{ 1.00, 0.00 };  

    HvyPlex b = cxOne + z * std::conj(reference_a); 

    HvyPlex diff{ 1.00 - std::norm(reference_a), 0.00 };

    HvyPlex quot = diff / (b * b); 

    return quot;
}







HvyDXBase::HvyPlex HvyDXBase::complex_ooo(double p_real)
{
    return std::complex<double>(cos(p_real), sin(p_real));
}





    


HvyDXBase::HvyPlex HvyDXBase::complex_neg(HvyDXBase::HvyPlex p_z)
{
    return -p_z;  // Unary minus (negation);  
}









HvyDXBase::HvyPlex HvyDXBase::complex_normalize(HvyDXBase::HvyPlex z)
{

    double dbl = std::abs(z); 

    if (dbl == 0.00)
    {
        //  TODO: fix this comparison of float types...

        return std::complex<double>(0.00, 0.00); 
    }

    return std::complex<double>(z.real() / dbl, z.imag() / dbl); 
}






