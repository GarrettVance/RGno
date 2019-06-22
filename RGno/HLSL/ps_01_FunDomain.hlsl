//      
//      


Texture2D           ColorTexture        :       register(t0);
SamplerState        LinearSampler       :       register(s0);



cbuffer conbuf7 : register(b1)
{
    float2      u_z;
    float2      u_dir;
    int         schlafli_q;
    int         schlafli_p; // swapped; TODO: 
    float       x0;
    float       pixWidth;
    float4      col0;

    float4      col1;  // Color for the "negative" Fundamental Domain;

    float4      col2;
}






struct PixelShaderInput
{
	float4      s_pos       : SV_POSITION;
	float3      s_color     : COLOR0;
    float2      s_texco     : TEXCOORD0;
    float2      s_model     : TEXCOORD1;
};



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



static const int loop_limitA = 100;  // TODO:  original loop_limit is 100;





#define tau 6.2831853071795864769252867665590   //  a lot more digits of 2pi;




float4 invertColorRGBA(float4 c0)
{
    return c0; // undo;

    float4 antiColor = float4(
        1.0 - c0.x,
        1.0 - c0.y,
        1.0 - c0.z,
        c0.w  //  Leave c0.w  alpha channel unchanged;
        );

    return antiColor;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float gv_squared_norm(float2 a)
{ 
    //  the "squared" norm: 
    //  ===================
    //  or could use MSFT intrinsic vector dot product... TODO: 

    return a.x * a.x + a.y * a.y;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float gv_arg_complex(float2 a)
{
    //   ghv : better to use atan2() rather than plain old atan()

    return atan2(a.y, a.x); 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float2 gv_mul_complex(float2 a, float2 b) 
{
    return float2(
        a.x * b.x - a.y * b.y, 
        a.x * b.y + a.y * b.x
    );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float2 gv_mulbis_complex(float2 a, float2 b) 
{ 
    //  return the product  a times (b bar)  

    return float2(
         a.x * b.x + a.y * b.y, 
        -a.x * b.y + a.y * b.x
    );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float2 gv_div_complex(float2 a, float2 b) 
{ 
    //   return the quotient  a / b

    float d = gv_squared_norm(b);

    return float2(
        (a.x * b.x + a.y * b.y) / d, 
        (-a.x * b.y + a.y * b.x) / d
    );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float2 gv_phi_complex(float2 z, float2 p_reference) 
{ 
    //  return   phi_reference(z) = (z + p_reference) / (1 + z * conj(p_reference))

    return gv_div_complex(
        z + p_reference, 
        float2(1.0, 0.0) + gv_mulbis_complex(z, p_reference) // actual math;
    );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


float2 dphi(float2 z, float2 a) 
{
  float2 b = float2(1.0, 0.0) + gv_mulbis_complex(z, a);

  return gv_div_complex(float2(1.0 - gv_squared_norm(a), 0.0), gv_mul_complex(b, b));
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float2 gv_ooo_complex(float a)
{
    return float2(
        cos(a), 
        sin(a)
    );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool gv_even_test(int n)
{
    float x = (float)n + 0.5;

    x = x - 2.0 * floor(x * 0.5);

    return (x < 1.0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





float4 ps_main(PixelShaderInput input) : SV_TARGET
{
    float4 col;

    if(gv_squared_norm(input.s_texco) < 0.98) 
    {
        bool inside = false; 

        float2 cxCircumradius = float2(x0, 0.0); 
        float2 z = input.s_texco; 



        // undo float2 dz = float2(pixWidth, 0.0);

        float2 dz = float2(1.5 * pixWidth, 0.0); // mult by 3 is too much...



        z = gv_mul_complex(z, u_dir);

        dz = gv_mul_complex(dz, dphi(z, u_z)); // z is pixel shader tex coord, u_z is from mouse click;

        z = gv_phi_complex(z, u_z);


        float2 w, dw; 
        float s, t;
        int c_the_negative = 0;

        int countA = 0;

        for(countA = 0; countA < loop_limitA; countA++) 
        {
            t = gv_arg_complex(z);

            s = t / tau * (float)schlafli_p;

            if(abs(s) > 0.5)
            {
                int n = (int)floor(0.5 + s);  // n = nearest integer to s
            
                z = gv_mul_complex(z, gv_ooo_complex(-tau / (float)schlafli_p * float(n))); 
            }


            if(z.y < 0.0) 
            { 
                c_the_negative = 1 - c_the_negative; 
                z.y = -z.y; 
            }

            dw = gv_mul_complex(dz, dphi(z, -cxCircumradius));

            w = -gv_phi_complex(z, -cxCircumradius);

            t = gv_arg_complex(w);

            s = t / tau * (float)schlafli_q;

            if(abs(s) < 0.5) 
            {
                if(w.y < 0.0) 
                { 
                    c_the_negative = 1 - c_the_negative; 
                    w.y = -w.y; 
                }

                dz = gv_mul_complex(dw, dphi(-w, cxCircumradius));

                z = gv_phi_complex(-w, cxCircumradius);

                break;
            }
            else 
            {
                int n = (int)floor(s + 0.5);

                w = gv_mul_complex(w, gv_ooo_complex(-tau / (float)schlafli_q * (float)n));  // w is the source of gawgai;

                dz = gv_mul_complex(dw, dphi(-w, cxCircumradius));

                z = gv_phi_complex(-w, cxCircumradius);
            }

        }
        //  Closes the "for" loop with countA < loop_limitA;



        float2 gawgai = float2(0.5 + w.x, 0.5 - w.y); 

        if (w.y < 0.00) { gawgai = float2(0.5 + w.x, 0.5 + w.y); }









        z = gv_mul_complex(z, gv_ooo_complex(tau / (float)(2 * schlafli_p))); //  create the red outline;














        if( z.y * z.y < gv_squared_norm(dz))
        {
            //  The point is interstitial, between two polygons: 

            col = col0; // col0 is the color for interstitial outlining; 
            
            // col = float4(1.f, 1.f, 1.f, 1.f); // white;

        }
        else if (c_the_negative == 0)
        {
            //  The point is in the "negative" of the Fundamental Domain:

            col = col1; // col1 is the color for "negative" Fundamental Domain;
        
            col = ColorTexture.Sample(LinearSampler, gawgai); 
        }
        else
        {
            //  The point is in the FD Fundamental Domain: 

            col = col2;  // col2 is color for the FD Fundamental Domain;

            col = invertColorRGBA(ColorTexture.Sample(LinearSampler, gawgai)); 
        }

    }
    //  Closes "if gv_squared_norm..."; 

    else if(gv_squared_norm(input.s_texco) < 1.00)
    {
        col = float4(0.0, 0.0, 0.0, 1.0);  //  paint it black;
    }
    else
    {
        col = float4(0.0, 0.0, 0.0, 0.0);  // paint is clear black;?;
    }

    return col;
}









