#ifndef KMagicDescH
#define	KMagicDescH

#include "KIniFile.h"

class KMagicDesc
{
private:
	KIniFile	m_MIniFile;
	char		m_szDesc[512];
public:
	KMagicDesc();
	~KMagicDesc();
	BOOL		Init();
	const char*	GetDesc(void* pData,int nType=0); ;//0为物品 1 为技能
	const char* BaoShiGetDesc(void *pData);
};

extern KMagicDesc	g_MagicDesc;
#endif
