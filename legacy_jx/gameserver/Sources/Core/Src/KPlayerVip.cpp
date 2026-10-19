//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerVip.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRoleVip Class 转生
//---------------------------------------------------------------------------


#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerVip.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif

#ifdef _SERVER

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
KPlayerVip::KPlayerVip()
{
	m_nPlayerIndex = 0;
	m_nRoleVipValue = 0;
	m_nRoleVipLevel=0;		     //当前转生的等级
	m_nRoleVipKeepQpiont=0;       //保留的潜能点
	m_nRoleVipKeepJpiont=0;       //保留的技能
	m_nRoleVipSetSkillMaxLevel=0; //技能等级上限增加
	m_nRoleVipSetFanYuMaxVal=0;   //防御上限增加
	m_nCurPlyaerLvel=0;	     //记录玩家转生时的等级	
	m_nCurRoleVipNum=0;
	m_nRoleVipLifeMaxVal =0;
	m_nRoleVipDamageMaxVal = 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerVip::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;

	m_nRoleVipValue				= 0;
	m_nRoleVipLevel=0;		      //当前转生的等级
	m_nRoleVipKeepQpiont=0;       //保留的潜能点
	m_nRoleVipKeepJpiont=0;       //保留的技能
	m_nRoleVipSetSkillMaxLevel=0; //技能等级上限增加
	m_nRoleVipSetFanYuMaxVal=0;   //防御上限增加
	m_nCurPlyaerLvel=0;	          //记录玩家转生时的等级
	m_nCurRoleVipNum=0;
	m_nRoleVipLifeMaxVal=0;
	m_nRoleVipDamageMaxVal=0;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void	KPlayerVip::SetRoleVipValue(int nValue)
{
	this->m_nRoleVipValue = (nValue < 0 ? 0 : nValue);
	/*
   
	if (m_nRoleVipValue > MAX_RoleVip_VALUE)
		m_nRoleVipValue = MAX_RoleVip_VALUE;

	  BYTE    m_nRoleVipLevel;		       //当前转生的等级
	  BYTE    m_nRoleVipKeepQpiont;       //保留的潜能点
	  BYTE	m_nRoleVipKeepJpiont;       //保留的技能
	  BYTE    m_nRoleVipSetSkillMaxLevel; //技能等级上限增加															  
	  BYTE    m_nRoleVipSetFanYuMaxVal;   //防御上限增加

	  */
       if (this->m_nRoleVipValue>0)
	   {  	 
		 this->m_nCurRoleVipNum  = EGetByte(this->m_nRoleVipValue,1); //第一个 偏移记录 行号 次数	   
		 this->m_nCurPlyaerLvel  = EGetByte(this->m_nRoleVipValue,2); //第二个偏移记录NPC转生时的等级
		 
		 int nCurRow=GetRoleVipLevelRowByCount(this->m_nCurRoleVipNum);
		 
		 if (nCurRow>0)
		 {
			 g_PlayerVipSetting.GetInteger(nCurRow,2,0,&m_nRoleVipLevel);
			 g_PlayerVipSetting.GetInteger(nCurRow,3,0,&m_nRoleVipKeepQpiont);
			 g_PlayerVipSetting.GetInteger(nCurRow,4,0,&m_nRoleVipKeepJpiont);
			 g_PlayerVipSetting.GetInteger(nCurRow,5,0,&m_nRoleVipSetSkillMaxLevel);
			 g_PlayerVipSetting.GetInteger(nCurRow,6,0,&m_nRoleVipSetFanYuMaxVal);
			 g_PlayerVipSetting.GetInteger(nCurRow,7,0,&m_nRoleVipLifeMaxVal);
			 g_PlayerVipSetting.GetInteger(nCurRow,8,0,&m_nRoleVipDamageMaxVal);
			 
		 }
		 else
		 {
			 this->m_nRoleVipLevel =	0;
			 this->m_nRoleVipKeepQpiont =	0;
			 this->m_nRoleVipKeepJpiont =	0;
			 this->m_nRoleVipSetSkillMaxLevel = 0;
			 this->m_nRoleVipSetFanYuMaxVal =	0;
			 this->m_nRoleVipLifeMaxVal = 0;
			 this->m_nRoleVipDamageMaxVal = 0;
		 }
	   }
		
	   if (this->m_nRoleVipValue<=0)
	   {
		   this->m_nCurRoleVipNum=0;
		    this->m_nCurPlyaerLvel =0;
		    this->m_nRoleVipLevel =	0;
			this->m_nRoleVipKeepQpiont =	0;
			this->m_nRoleVipKeepJpiont =	0;
			this->m_nRoleVipSetSkillMaxLevel = 0;
			this->m_nRoleVipSetFanYuMaxVal =	0;
			this->m_nRoleVipLifeMaxVal = 0;
			this->m_nRoleVipDamageMaxVal = 0;
	   }

	ROLEVIP_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_rolesyncvipvalue;
	sValue.m_nRoleVipValue            = m_nRoleVipValue;
	sValue.m_nCurPlyaerLvel          = m_nCurPlyaerLvel;
	sValue.m_nRoleVipLevel            = m_nRoleVipLevel;		      //当前转生的等级
	sValue.m_nRoleVipKeepQpiont       = m_nRoleVipKeepQpiont;       //保留的潜能点
    sValue.m_nRoleVipKeepJpiont       = m_nRoleVipKeepJpiont;       //保留的技能
    sValue.m_nRoleVipSetSkillMaxLevel = m_nRoleVipSetSkillMaxLevel; //技能等级上限增加
    sValue.m_nRoleVipSetFanYuMaxVal   = m_nRoleVipSetFanYuMaxVal;   //防御上限增加
	sValue.m_nRoleVipLifeMaxVal       = m_nRoleVipLifeMaxVal;
	sValue.m_nCurRoleVipNum           = m_nCurRoleVipNum;
	sValue.m_nRoleVipDamageMaxVal     = m_nRoleVipDamageMaxVal;

	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(ROLEVIP_VALUE_SYNC));
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
int		KPlayerVip::GetRoleVipValue()
{
	return m_nRoleVipValue;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerVip::AddRoleVipValue(int nAdd,int nLevel)
{
	//m_nRoleVipValue += nAdd;

	 m_nRoleVipValue=ESetByte(m_nRoleVipValue,1,m_nCurRoleVipNum+nAdd);
	 m_nRoleVipValue=ESetByte(m_nRoleVipValue,2,nLevel);

	if (m_nRoleVipValue < 0)
		m_nRoleVipValue = 0;
	
	/*if (m_nRoleVipValue > MAX_RoleVip_VALUE)
		m_nRoleVipValue = MAX_RoleVip_VALUE; */

	if (this->m_nRoleVipValue>0)
	{
		this->m_nCurRoleVipNum = EGetByte(this->m_nRoleVipValue,1); //第一个 偏移记录 行号  次数
		this->m_nCurPlyaerLvel = EGetByte(this->m_nRoleVipValue,2); //第二个偏移记录NPC转生时的等级

		int nCurRow=GetRoleVipLevelRowByCount(this->m_nCurRoleVipNum);
		
		if (nCurRow>0)
		{
			g_PlayerVipSetting.GetInteger(nCurRow,2,0,&m_nRoleVipLevel);
			g_PlayerVipSetting.GetInteger(nCurRow,3,0,&m_nRoleVipKeepQpiont);
			g_PlayerVipSetting.GetInteger(nCurRow,4,0,&m_nRoleVipKeepJpiont);
			g_PlayerVipSetting.GetInteger(nCurRow,5,0,&m_nRoleVipSetSkillMaxLevel);
			g_PlayerVipSetting.GetInteger(nCurRow,6,0,&m_nRoleVipSetFanYuMaxVal);
			g_PlayerVipSetting.GetInteger(nCurRow,7,0,&m_nRoleVipLifeMaxVal);
			g_PlayerVipSetting.GetInteger(nCurRow,8,0,&m_nRoleVipDamageMaxVal);
			
		}
		else
		{
			this->m_nRoleVipLevel =	0;
			this->m_nRoleVipKeepQpiont =	0;
			this->m_nRoleVipKeepJpiont =	0;
			this->m_nRoleVipSetSkillMaxLevel = 0;
			this->m_nRoleVipSetFanYuMaxVal =	0;
			this->m_nRoleVipLifeMaxVal  = 0;
			this->m_nRoleVipDamageMaxVal = 0;
		} 
	}

	if (this->m_nRoleVipValue<=0)
	{
		this->m_nCurRoleVipNum=0;
		this->m_nCurPlyaerLvel =0;
		this->m_nRoleVipLevel =	0;
		this->m_nRoleVipKeepQpiont =	0;
		this->m_nRoleVipKeepJpiont =	0;
		this->m_nRoleVipSetSkillMaxLevel = 0;
		this->m_nRoleVipSetFanYuMaxVal =	0;
		this->m_nRoleVipLifeMaxVal  = 0;
		this->m_nRoleVipDamageMaxVal = 0;
	}

	ROLEVIP_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_rolesyncvipvalue;
	sValue.m_nRoleVipValue            = m_nRoleVipValue;
	sValue.m_nCurRoleVipNum           = m_nCurRoleVipNum;
	sValue.m_nCurPlyaerLvel           = m_nCurPlyaerLvel;
	sValue.m_nRoleVipLevel            = m_nRoleVipLevel;		       //当前转生的等级
	sValue.m_nRoleVipKeepQpiont       = m_nRoleVipKeepQpiont;       //保留的潜能点
    sValue.m_nRoleVipKeepJpiont       = m_nRoleVipKeepJpiont;       //保留的技能
    sValue.m_nRoleVipSetSkillMaxLevel = m_nRoleVipSetSkillMaxLevel; //技能等级上限增加
    sValue.m_nRoleVipSetFanYuMaxVal   = m_nRoleVipSetFanYuMaxVal;   //防御上限增加
	sValue.m_nRoleVipLifeMaxVal       = m_nRoleVipLifeMaxVal;
	sValue.m_nRoleVipDamageMaxVal     = m_nRoleVipDamageMaxVal;

	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(ROLEVIP_VALUE_SYNC));
}

int  KPlayerVip::GetRoleVipLevelRowByCount(int nInLevel)
{
	int nRows=g_PlayerVipSetting.GetHeight()+1;
	int nReg=0;

	for (int i=2;i<nRows;i++)
	{
		int nCurLve=0;

		g_PlayerVipSetting.GetInteger(i,1,0,&nCurLve);

		if  (nCurLve==nInLevel)
		{
			nReg=i;
			break;
		}
	}

	return	 nReg;
}

int  KPlayerVip::GetRoleVipLevelRow(int nInLevel)
{
	 int nRows=g_PlayerVipSetting.GetHeight()+1;
	 int nReg=0;

	 for (int i=2;i<nRows;i++)
	 {
		 int nCurLve=0;
			 
			 g_PlayerVipSetting.GetInteger(i,2,0,&nCurLve);
			 
		 if  (nCurLve==nInLevel)
		 {
			 nReg=i;
		     break;
		 }
	 }

	 return	 nReg;
}

/*
int  KPlayerVip::SetRoleVipByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;
	
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayerVip::GetRoleVipByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;
	
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
 */

#endif

#ifndef _SERVER

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------




void	KPlayerVip::SetRoleVipValue(BYTE* pMsg)
{

	ROLEVIP_VALUE_SYNC	*pValue = (ROLEVIP_VALUE_SYNC*)pMsg;

	if (m_nRoleVipValue == pValue->m_nRoleVipValue)
	{
		return;
	}

	this->m_nRoleVipValue = (pValue->m_nRoleVipValue < 0 ? 0 : pValue->m_nRoleVipValue);
  /*
	if (m_nRoleVipValue > MAX_RoleVip_VALUE)
		m_nRoleVipValue = MAX_RoleVip_VALUE;
	*/
	this->m_nRoleVipLevel =	pValue->m_nRoleVipLevel;
	this->m_nRoleVipKeepQpiont =	pValue->m_nRoleVipKeepQpiont;
	this->m_nRoleVipKeepJpiont =	pValue->m_nRoleVipKeepJpiont;
	this->m_nRoleVipSetSkillMaxLevel = pValue->m_nRoleVipSetSkillMaxLevel;
	this->m_nRoleVipSetFanYuMaxVal =	pValue->m_nRoleVipSetFanYuMaxVal;
	this->m_nCurRoleVipNum=pValue->m_nCurRoleVipNum;
	this->m_nCurPlyaerLvel=pValue->m_nCurPlyaerLvel;
	this->m_nRoleVipLifeMaxVal  = pValue->m_nRoleVipLifeMaxVal;
	if (this->m_nRoleVipValue<=0)
	{
		this->m_nCurPlyaerLvel=0;
		this->m_nCurRoleVipNum=0;
		this->m_nRoleVipLevel =	0;
		this->m_nRoleVipKeepQpiont =	0;
		this->m_nRoleVipKeepJpiont =	0;
		this->m_nRoleVipSetSkillMaxLevel = 0;
	    this->m_nRoleVipSetFanYuMaxVal =	0;
		this->m_nRoleVipLifeMaxVal  = 0;
	}

	 /*
	  BYTE    m_nRoleVipLevel;		       //当前转生的等级
	  BYTE    m_nRoleVipKeepQpiont;       //保留的潜能点
	  BYTE	m_nRoleVipKeepJpiont;       //保留的技能
	  BYTE    m_nRoleVipSetSkillMaxLevel; //技能等级上限增加
	  BYTE    m_nRoleVipSetFanYuMaxVal;   //防御上限增加

	  */
}

/*
int  KPlayerVip::SetRoleVipByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;
	
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayerVip::GetRoleVipByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;
	
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
*/

void	KPlayerVip::AddRoleVipValue(int nAdd)
{
	m_nRoleVipValue += nAdd;
	if (m_nRoleVipValue < 0)
		m_nRoleVipValue = 0;
	/*
	if (m_nRoleVipValue > MAX_RoleVip_VALUE)
		m_nRoleVipValue = MAX_RoleVip_VALUE; */
}

#endif
