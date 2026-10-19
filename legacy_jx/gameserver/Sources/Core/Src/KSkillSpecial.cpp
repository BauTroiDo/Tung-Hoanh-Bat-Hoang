/*******************************************************************************
// FileName			:	KSkillSpecial.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-9-23 21:12:12
// FileDescription	:	
// Revision Count	:	
*******************************************************************************/
#include "KCore.h"
#include "KEngine.h"
#include "KNpc.h"
#include "KSkillSpecial.h"
#include "KSubWorld.h"
#ifndef _SERVER
#include "ImgRef.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "scene/KScenePlaceC.h"

KList SpecialMovieList;

KSkillSpecial::KSkillSpecial()
{
	m_nPX = m_nPY = m_nPZ = 0;
	m_nNpcIndex = m_dwMatchID = 0;
	m_pSndNode = NULL;
	m_RUImage.szImage[0] = 0;
	m_RUImage.uImage = 0;
	m_RUImage.nType = ISI_T_SPR;
	m_RUImage.Color.Color_b.a = 255;
	m_RUImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_RUImage.nISPosition = IMAGE_IS_POSITION_INIT;
	m_RUImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	//m_eStatus = 0;
	
#ifdef TOOLVERSION
	m_pSprNode = NULL;
#endif
};

BOOL KSkillSpecial::Init()
{
	//m_RUImage.szImage[0] = 0;
	ZeroMemory(m_RUImage.szImage,sizeof(m_RUImage.szImage));
	m_RUImage.uImage = 0;

	return TRUE;//Question Will Del
}

void KSkillSpecial::DrawC(DWORD nCurLifeFrame)
{
	
	if (nCurLifeFrame > m_nEndTime) return;
	
	//if (!m_RUImage.szImage[0])
	{
//		char SprFileName[260];
//		g_GetFullPath(SprFileName, m_pMissleRes->AnimFileName);
		//strcpy(m_RUImage.szImage, m_pMissleRes->AnimFileName);
		snprintf(m_RUImage.szImage, sizeof(m_RUImage.szImage), "%s", m_pMissleRes->AnimFileName);
	}
	
	if (m_nNpcIndex > 0 && Npc[m_nNpcIndex].IsMatch(m_dwMatchID))
	{
		int nSrcX = 0;
		int nSrcY = 0;
		SubWorld[0].NewMap2Mps(Npc[m_nNpcIndex].m_RegionIndex, Npc[m_nNpcIndex].m_MapX,  Npc[m_nNpcIndex].m_MapY,Npc[m_nNpcIndex].m_OffX, Npc[m_nNpcIndex].m_OffY, &nSrcX, &nSrcY);
		m_RUImage.oPosition.nX = m_nPX = nSrcX;
		m_RUImage.oPosition.nY = m_nPY = nSrcY + 1;
		m_RUImage.oPosition.nZ = Npc[m_nNpcIndex].m_MapZ;
	}
	else
	{
		m_RUImage.oPosition.nX =	m_nPX;
		m_RUImage.oPosition.nY =	m_nPY;
		m_RUImage.oPosition.nZ =	m_nPZ;
	}
	
	if (m_pMissleRes->nInterval > 0 && m_pMissleRes->nDir!=0)
	{
		int	nImageDir = (m_nCurDir / (64 / m_pMissleRes->nDir));
		int nImageDir1 = (m_nCurDir % (64 / m_pMissleRes->nDir));
		if (nImageDir1 >= 32 / m_pMissleRes->nDir) 	nImageDir ++;
		if (nImageDir >= m_pMissleRes->nDir)		nImageDir = 0;

		m_RUImage.nFrame = (nCurLifeFrame - m_nBeginTime ) / m_pMissleRes->nInterval + nImageDir * m_pMissleRes->nTotalFrame / m_pMissleRes->nDir;

#ifdef TOOLVERSION
		KSprite * pSprite = NULL;
		(KCacheNode *) 
			m_pSprNode = (KCacheNode *)g_SpriteCache.GetNode(m_RUImage.szImage ,  (KCacheNode *)m_pSprNode);
		pSprite = (KSprite*)m_pSprNode->m_lpData;
		if (pSprite)
		{
			pSprite->DrawAlpha(m_nPX, m_nPY, m_RUImage.nFrame);
		}
#else
		g_pRepresent->DrawPrimitives(1, &m_RUImage, RU_T_IMAGE, 0);

		//if (Player[CLIENT_PLAYER_INDEX].m_DebugMode)
		/*{//µ÷ÊÔÄ£Ê½
			KRULine		Line;
			Line.oPosition.nX = m_RUImage.oPosition.nX;
			Line.oPosition.nY = m_RUImage.oPosition.nY;
			Line.oPosition.nZ = m_RUImage.oPosition.nZ;
			Line.oEndPos.nX = Line.oPosition.nX +32;
			Line.oEndPos.nY = m_RUImage.oPosition.nX;
			Line.oEndPos.nZ = m_RUImage.oPosition.nZ;
			
			Line.Color.Color_dw = 0xffffffff;
			g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 0);
			char MissleId[128];
			//itoa(m_nMissleId, MissleId, 10);
			sprintf(MissleId, "×´Ì¬:%s", m_RUImage.szImage);
			g_pRepresent->OutputText(14, MissleId, KRF_ZERO_END, m_RUImage.oPosition.nX,m_RUImage.oPosition.nY, 0xffffffff, 3, 5);
		} */
#endif
	}
	return;
	
}

void KSkillSpecial::SetAnimation(char * szFile, unsigned int uImage)
{
	if (szFile)	strcpy(m_RUImage.szImage, szFile);
	m_RUImage.uImage = uImage;
}

void KSkillSpecial::Remove()
{
/*	if (!g_pGraphics)
return;
if (m_pObj)
{
//	::RemoveMissle(g_pGraphics, m_pObj);
RELEASE(m_pObj);
}
	*/	
}
#endif
