/********************************************************************
	created:	2003/05/11
	file base:	IGServer
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_IGSERVER_H__
#define __INCLUDE_IGSERVER_H__

//增加检测 防止不同gs之间重复登录
struct RoleLoginInfo
{
	int     m_RegServerIndex;//大区的索引
	int     m_GsNumBer;      //Gs的编号
	size_t 	m_idx;           //连接索引号 
	bool    m_IsOffLine;     //是否离线
};


class IGServer
{
public:
	/*
	 * Interface
	 */
	virtual bool Create() = 0;
	virtual bool Destroy() = 0;
	
	virtual bool Attach( const char *pAccountName ) = 0;
	
	virtual void SendText( const char *pText, int nLength, UINT uOption,BOOL nIsSavePai=FALSE) = 0;
	virtual void SendData( const unsigned long ulnClientID, const void * const pData, const size_t datalength ) = 0;
	
	virtual size_t GetID() = 0;
	virtual size_t GetContent() = 0;
	virtual size_t GetCapability() = 0;
	virtual size_t GetSerNoIndex() = 0;
	virtual size_t GetSerGsNumber() = 0;
	virtual size_t GetIP( BYTE cType ) = 0;
	virtual size_t GetIndentify() = 0;

	virtual int  getRoleLock(char* szRole,RoleLoginInfo *resInfo)=0;
	
	virtual bool AnalyzeRequire( const void *pData, size_t datalength ) = 0;

	virtual bool DispatchTask( UINT nTask, const void *pData, size_t datalength, DWORD nData,DWORD nLeftTime,int m_nVipType=-1,DWORD nExtGameLeftTime=0,unsigned int nTempEnTerMapIndx=0,unsigned int nTempKeyId=0) = 0;

	virtual ~IGServer(){};
};

#endif // __INCLUDE_IGSERVER_H__