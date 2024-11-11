#include "..\Public\VIBuffer_RectBatch.h"

CVIBuffer_RectBatch::CVIBuffer_RectBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_RectBatch::CVIBuffer_RectBatch(const CVIBuffer_RectBatch& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_RectBatch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_RectBatch::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_RectBatch::Initialize_Batch(void* pArg)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = (_uint)m_Vertices.size();
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = (_uint)m_Indices.size();
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_Vertices.data();
	
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_Indices.data();

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_RectBatch::Render()
{
	// pVertices를 돌면서 어떤 UI니? 찾지 근데 무지성으로 빨리찾는 자료ㅗ구조를 쓰는생각을해야대
	// 어떤 UI인지 키값으로체크하고 걔를 또 어캐 바꿀건데? 이걸 함수의 매개변수로 클라에서 던져줘 

	
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void CVIBuffer_RectBatch::AddUIQuad(_float4x4* pWorldMatrix , _float fX, _float fY, _float fSizeX, _float fSizeY, const _float2& uvStart, const _float2& uvEnd)
{
	_uint iBaseIndex = static_cast<_uint>(m_Vertices.size()); // 현재 인덱스 시작 위치

	_float3 LocalPositions[4] = {
	   { fX - (fSizeX * 0.5f), fY + (fSizeY * 0.5f), 0.f },  
	   { fX + (fSizeX * 0.5f), fY + (fSizeY * 0.5f), 0.f },  
	   { fX + (fSizeX * 0.5f), fY - (fSizeY * 0.5f), 0.f },  
	   { fX - (fSizeX * 0.5f), fY - (fSizeY * 0.5f), 0.f }   
	};

	for (_uint i = 0; i < 4; ++i)
	{
		_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&LocalPositions[i]), XMLoadFloat4x4(pWorldMatrix));
		_float3 FinalPosition;
		XMStoreFloat3(&FinalPosition, vWorldPos);

		
		if (i == 0)
			m_Vertices.push_back({ FinalPosition, uvStart  });				// 내가세팅하는 정점버퍼마다 키값, 이름 ,ID 뭐 이런걸 넣엇다쳐봐
		else if (i == 1)
			m_Vertices.push_back({ FinalPosition, _float2(uvEnd.x, uvStart.y)  });
		else if (i == 2)
			m_Vertices.push_back({ FinalPosition, uvEnd });
		else if (i == 3)
			m_Vertices.push_back({ FinalPosition, _float2(uvStart.x, uvEnd.y) });
	}

	


	// 두 개의 삼각형으로 인덱스 추가
	m_Indices.push_back(iBaseIndex);
	m_Indices.push_back(iBaseIndex + 1);
	m_Indices.push_back(iBaseIndex + 2);

	m_Indices.push_back(iBaseIndex);
	m_Indices.push_back(iBaseIndex + 2);
	m_Indices.push_back(iBaseIndex + 3);
}

void CVIBuffer_RectBatch::Update_UIVertex(const _float4x4* pWorldMatrix, _uint iUIIndex, const _float2& newUVStart, const _float2& newUVEnd)
{
	_float3 LocalPositions[4] = {
	  { -0.5f, 0.5f, 0.f },
	  { 0.5f, 0.5f, 0.f },
	  { 0.5f, -0.5f, 0.f },
	  { -0.5f, -0.5f, 0.f }
	};

	// 정점 위치 갱신
	for (_uint i = 0; i < 4; ++i) 
	{
		_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&LocalPositions[i]), XMLoadFloat4x4(pWorldMatrix));	// 각 UI들의 트랜스폼->월행을 넘겨주자
		_float3 FinalPosition;
		XMStoreFloat3(&FinalPosition, vWorldPos);
	
		m_Vertices[iUIIndex + i].vPosition = FinalPosition;
	}

	

	m_Vertices[iUIIndex + 0].vTexcoord = newUVStart;
	m_Vertices[iUIIndex + 1].vTexcoord = _float2(newUVEnd.x, newUVStart.y);
	m_Vertices[iUIIndex + 2].vTexcoord = newUVEnd;
	m_Vertices[iUIIndex + 3].vTexcoord = _float2(newUVStart.x, newUVEnd.y);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
		memcpy(mappedResource.pData, m_Vertices.data(), sizeof(VTXPOSTEX) * m_Vertices.size());
		m_pContext->Unmap(m_pVB, 0);
	}

}

CVIBuffer_RectBatch* CVIBuffer_RectBatch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_RectBatch* pInstance = new CVIBuffer_RectBatch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_RectBatch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_RectBatch::Clone(void* pArg)
{
	CVIBuffer_RectBatch* pInstance = new CVIBuffer_RectBatch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_RectBatch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectBatch::Free()
{
	__super::Free();

}
