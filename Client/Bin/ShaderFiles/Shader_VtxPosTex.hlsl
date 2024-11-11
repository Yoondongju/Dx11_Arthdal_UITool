
#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;
Texture2DArray  g_Textures;

texture2D		g_DepthTexture;

float2 g_StartUV = { 0.f,0.f};
float2 g_EndUV = { 1.f, 1.f };
float  g_fTime = { 0.f };


struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;		

};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;

};

struct VS_OUT_EFFECT
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT VS_MAIN_BATCH( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
    //vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vector vPosition = mul(vector(In.vPosition, 1.f), g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT_EFFECT		Out = (VS_OUT_EFFECT)0;

	/* 정점에 위치를 월드 뷰 투영변환한다.*/
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = vPosition;

	return Out;
}


/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
};

struct PS_IN_EFFECT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

vector Sample(float2 vTexcoord)
{
	return g_Texture.Sample(LinearSampler, vTexcoord);
}

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    float2 adjustedUV;
    adjustedUV.x = lerp(g_StartUV.x, g_EndUV.x, In.vTexcoord.x);
    adjustedUV.y = lerp(g_StartUV.y, g_EndUV.y, In.vTexcoord.y);
	
    Out.vColor = g_Texture.Sample(LinearSampler, adjustedUV); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

	//Out.vColor.gb = Out.vColor.r;
	//Out.vColor.a = Out.vColor.a * 0.5f;

	return Out;
}

PS_OUT PS_MAIN_TEST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/
    
	
	//Out.vColor.gb = Out.vColor.r;
	//Out.vColor.a = Out.vColor.a * 0.5f;

    return Out;
}

PS_OUT PS_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float speed = 7.f;					// 흐르는 속도 조절
    float t = frac(g_fTime * speed); // 0에서 1까지 반복되는 값을 생성 (triangle wave 같은 효과)

    float2 minUV = In.vTexcoord - float2(0.3f, 0.3f);
    float2 maxUV = In.vTexcoord;
	
    float2 animatedTexcoord = lerp(minUV, maxUV, t);
    Out.vColor = g_Texture.Sample(LinearSampler, animatedTexcoord);
    Out.vColor = lerp(Out.vColor, float4(0.7f, 0.2f, 0.1f, 1.f), 0.93f);

    return Out;
}

PS_OUT PS_MP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float speed = 7.f; // 흐르는 속도 조절
    float t = frac(g_fTime * speed); // 0에서 1까지 반복되는 값을 생성 (triangle wave 같은 효과)

    float2 minUV = In.vTexcoord - float2(0.3f, 0.3f);
    float2 maxUV = In.vTexcoord;
	
    float2 animatedTexcoord = lerp(minUV, maxUV, t);
    Out.vColor = g_Texture.Sample(LinearSampler, animatedTexcoord);
    Out.vColor = lerp(Out.vColor, float4(0.1f, 0.2f, 0.7f, 1.f), 0.93f);

    return Out;
}





PS_OUT PS_MAINMENU_PANEL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float speed = 0.08f; // 흐르는 속도 조절
    float t = g_fTime * speed;

    float scale = 0.5f + sin(g_fTime * 0.5f) * 0.5f;
    float2 vNewTexcoord = In.vTexcoord * scale;
	
    vNewTexcoord.x += t;
    vNewTexcoord.y += t * 5.f;
    
    
    float4 colorSample = g_Texture.Sample(LinearSampler, vNewTexcoord);
   
    float brightness = dot(colorSample.rgb, float3(0.3f, 0.59f, 0.11f)); // 밝기 계산 (0.0 - 1.0)
    float alpha = lerp(0.f, 1.0f, brightness); // 밝기에 따라 알파 값 선형 조정 (0.5 ~ 1.0 사이)

    Out.vColor = colorSample * 1.5f;
    Out.vColor.a *= alpha; // 알파 값을 조정

    return Out;
}



technique11	DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
	pass UI
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass UI_TEST
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BATCH();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TEST();
    }


    pass UI_HP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HP();
    }

    pass UI_MP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MP();
    }



	/* 디스토션 + 블렌딩 */
	pass MainMenuPanel
	{
		SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAINMENU_PANEL();
    }
}