//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerRoleVip.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRepute Class
//---------------------------------------------------------------------------

#ifndef KPLAYERVIP_H
#define KPLAYERVIP_H


#ifdef _SERVER
//服务器端
class KPlayerVip
{
	friend class KPlayer;
private:
	int	   m_nPlayerIndex;					
	int	   m_nRoleVipValue;
	int    m_nRoleVipLevel;		              //当前转生的等级
	int    m_nRoleVipKeepQpiont;           //保留的潜能点
	int	   m_nRoleVipKeepJpiont;             //保留的技能
	int    m_nRoleVipSetSkillMaxLevel;   //技能等级上限增加
	int    m_nRoleVipSetFanYuMaxVal;   //防御上限增加
	int    m_nRoleVipLifeMaxVal;             //生命上限增加
	int    m_nRoleVipDamageMaxVal;
	int    m_nCurPlyaerLvel;
	int    m_nCurRoleVipNum;

	int    GetRoleVipLevelRow(int nInLevel);
	int    GetRoleVipLevelRowByCount(int nInLevel);
public:
	KPlayerVip();							
	void	Init(int nPlayerIdx);			
	void	SetRoleVipValue(int nValue);			
	int		GetRoleVipValue();					
	void	AddRoleVipValue(int nAdd,int nLevel);	
	int		GetRoleVipLevel()	{return m_nRoleVipLevel;};
	int		GetRoleVipKeepQpiont()	{return m_nRoleVipKeepQpiont;};
	int		GetRoleVipKeepJpiont()	{return m_nRoleVipKeepJpiont;};
	int		GetRoleVipSkillMaxLevel()	{return m_nRoleVipSetSkillMaxLevel;};
	int		GetRoleVipFanYuMaxVal()	{return m_nRoleVipSetFanYuMaxVal;};
	int		GetRoleVipLifeMaxVal()	{return m_nRoleVipLifeMaxVal;};
    int		GetRoleVipCurLevel()	{return m_nCurPlyaerLvel;};
	int		GetRoleVipNum()	{return m_nCurRoleVipNum;};
	int		GetRoleVipDamageMaxVal()	{return m_nRoleVipDamageMaxVal;};

    //int     SetRoleVipByte(int nIntValue,int nByteNumber,int nByteValue);
    //int     GetRoleVipByte(int nIntValue,int nByteNumber);

};

#endif
//客户端
#ifndef _SERVER
//客户端
class KPlayerVip
{
	friend class KPlayer;
private:
	int	   m_nRoleVipValue;						//转生值  （记录转生次数,以及转生的等级,保留的技能点,潜能点）
	int    m_nRoleVipLevel;		       //当前转生的等级
	int    m_nRoleVipKeepQpiont;       //保留的潜能点
	int	   m_nRoleVipKeepJpiont;       //保留的技能
	int    m_nRoleVipSetSkillMaxLevel; //技能等级上限增加
	int    m_nRoleVipSetFanYuMaxVal;   //防御上限增加
	int    m_nRoleVipLifeMaxVal;       //生命上限增加
	int    m_nRoleVipDamageMaxVal;     //伤害上限增加
	int    m_nCurPlyaerLvel;
	int    m_nCurRoleVipNum;



public:
	void	SetRoleVipValue(BYTE* pMsg);	  //int nValue
	void	AddRoleVipValue(int nAdd);	
	int		GetRoleVipValue()	{return m_nRoleVipValue;};
	int		GetRoleVipLevel()	{return m_nRoleVipLevel;};
	int		GetRoleVipKeepQpiont()	{return m_nRoleVipKeepQpiont;};
	int		GetRoleVipKeepJpiont()	{return m_nRoleVipKeepJpiont;};
	int		GetRoleVipSkillMaxLevel()	{return m_nRoleVipSetSkillMaxLevel;};
	int		GetRoleVipFanYuMaxVal()	{return m_nRoleVipSetFanYuMaxVal;};
	int		GetRoleVipLifeMaxVal()	{return m_nRoleVipLifeMaxVal;};
	int		GetRoleVipCurLevel()	{return m_nCurPlyaerLvel;};
	int		GetRoleVipNum()	{return m_nCurRoleVipNum;};
	int		GetRoleVipDamageMaxVal()	{return m_nRoleVipDamageMaxVal;};
	
	//int     SetRoleVipByte(int nIntValue,int nByteNumber,int nByteValue);
    //int     GetRoleVipByte(int nIntValue,int nByteNumber);

};
#endif
   


#endif
