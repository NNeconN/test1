//--------------------------------------------------------------------------------------
// whiteps.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"

//--------------------------------------------------------------------------------------
//  ピクセルシェーダー
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
    float4 result;
    
    float4 col1, col2, col3;
    col1 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    col2 = float4(0.5f, 0.5f, 0.5f, 1.0f);
    col3 = float4(0.1f, 0.1f, 0.1f, 1.0f);
    
    float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);
    
    result = texcol.g * col1 + texcol.r * col2 + texcol.b * col3;
    result.a = 1.0f;
    return result;
}