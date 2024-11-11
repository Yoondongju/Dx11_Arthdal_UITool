
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
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;

};

struct VS_OUT_EFFECT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(/*����*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT VS_MAIN_BATCH( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
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

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = vPosition;

	return Out;
}


/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

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

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
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
	
    float speed = 7.f;					// �帣�� �ӵ� ����
    float t = frac(g_fTime * speed); // 0���� 1���� �ݺ��Ǵ� ���� ���� (triangle wave ���� ȿ��)

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
	
    float speed = 7.f; // �帣�� �ӵ� ����
    float t = frac(g_fTime * speed); // 0���� 1���� �ݺ��Ǵ� ���� ���� (triangle wave ���� ȿ��)

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
	
    float speed = 0.08f; // �帣�� �ӵ� ����
    float t = g_fTime * speed;

    float scale = 0.5f + sin(g_fTime * 0.5f) * 0.5f;
    float2 vNewTexcoord = In.vTexcoord * scale;
	
    vNewTexcoord.x += t;
    vNewTexcoord.y += t * 5.f;
    
    
    float4 colorSample = g_Texture.Sample(LinearSampler, vNewTexcoord);
   
    float brightness = dot(colorSample.rgb, float3(0.3f, 0.59f, 0.11f)); // ��� ��� (0.0 - 1.0)
    float alpha = lerp(0.f, 1.0f, brightness); // ��⿡ ���� ���� �� ���� ���� (0.5 ~ 1.0 ����)

    Out.vColor = colorSample * 1.5f;
    Out.vColor.a *= alpha; // ���� ���� ����

    return Out;
}



technique11	DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
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



	/* ����� + ���� */
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