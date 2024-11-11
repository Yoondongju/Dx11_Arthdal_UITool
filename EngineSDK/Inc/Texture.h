#pragma once

#include "Component.h"

/* DirectXTex Lib */ /* .dds, .tga, wic(.jpg, .png, .bmp etc) */
/* DirectXTK Lib ToolKit */ /* .dds, wic(.jpg, .png, .bmp etc) */

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{	
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView*			GetTexture() { return m_SRVs.back(); }
	vector<ID3D11ShaderResourceView*>&  GetTextures() { return m_SRVs; }

	const _wstring&						Get_MyPath() { return m_LStrMyPath; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShadeResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Bind_ShadeResources(class CShader* pShader, const _char* pConstantName);


private:
	vector<ID3D11ShaderResourceView*>		m_SRVs;
	_uint									m_iNumTextures = { 0 };

private:
	_wstring								m_LStrMyPath = L"";

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures ,  CTexture** pOut = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END