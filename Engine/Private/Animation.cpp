#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, vector<_uint>& KeyFrameIndices, const class CModel* pModel)
{
	m_Duration = pAIAnimation->mDuration;
	m_SpeedPerSec = pAIAnimation->mTicksPerSecond;

	/* �� �ִϸ��̼��� ����ϴ� ���� ���������Ѵ�. */
	m_iNumChannels = pAIAnimation->mNumChannels;

	KeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _float fTimeDelta)
{
	/* ���� �����ġ�� ����ϳ�. */
	*pCurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

	if (*pCurrentTrackPosition >= m_Duration)
	{
		if (false == isLoop)
			return true;

		*pCurrentTrackPosition = 0.f;
	}

	/* ���� �����ġ�� �°� ���� �ִϸ��̼��� ��Ʈ���ؾ� �� ���� ���µ��� �������ش�. */
	_uint		iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex++], *pCurrentTrackPosition);
	}

	return false;
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, vector<_uint>& KeyFrameIndices, const class CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, KeyFrameIndices, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);	
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
