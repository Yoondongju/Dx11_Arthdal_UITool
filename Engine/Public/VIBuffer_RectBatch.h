#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectBatch final : public CVIBuffer
{
public:
	typedef struct
	{
		_uint				iUIIndex = 0;
		class CUI*			pUI = { nullptr };
		
	}UIBATCH_INFO;

private:
	CVIBuffer_RectBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_RectBatch(const CVIBuffer_RectBatch& Prototype);
	virtual ~CVIBuffer_RectBatch() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	HRESULT Initialize_Batch(void* pArg);
	virtual HRESULT Render() override;

public:
	void AddUIQuad(_float4x4* pWorldMatrix , _float fX, _float fY, _float fSizeX, _float fSizeY, const _float2& uvStart, const _float2& uvEnd);

public:
	void Update_UIVertex(const _float4x4* pWorldMatrix, _uint iUIIndex,  const _float2& newUVStart, const _float2& newUVEnd);	 // 어떤 UI를 변경할지 키값을준다.


private:
	
	vector<VTXPOSTEX>						m_Vertices;
	vector<_ushort>							m_Indices;


public:
	static CVIBuffer_RectBatch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END