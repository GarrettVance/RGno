//      
//      
//      Constant buffer stores three column-major matrices: 
//      
//      


cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};



struct VertexShaderInput
{
    float3      pos     :       POSITION;
    float2      tex     :       TEXCOORD;
    float3      normal  :       NORMAL;
};






struct PixelShaderInput
{
	float4      s_pos       : SV_POSITION;
	float3      s_color     : COLOR0;
    float2      s_texco     : TEXCOORD0;
    float2      s_model     : TEXCOORD1;
};





PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos4 = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	float4 worldpos = mul(pos4, model);

	float4 viewpos = mul(worldpos, view);

	float4 projpos = mul(viewpos, projection);

	output.s_pos = projpos;

    output.s_color = float3(0.f, 0.f, 1.f);


#if 5 == 5
    //  Specialty texture coords (i forgot what for):

    output.s_model = float2(
        (1.0 + input.pos.x) / 2.0,
        (1.0 - input.pos.y) / 2.0
    );
#else
    output.s_model = float2( input.pos.x, input.pos.y );
#endif


    //   
    //  Complex Plane aka Poincare Disk texture coordinates: 
    //   

#if 7 == 7
    //      
    //  This mode is used for project RNaud: 
    //          
    output.s_texco = float2(input.pos.x, input.pos.y);  //  bypasses the Model Transform;
#else
    //  
    //  This mode is used for project PixelShady: 
    //  
    output.s_texco = float2(input.tex.x, input.tex.y);
#endif


	return output;
}
