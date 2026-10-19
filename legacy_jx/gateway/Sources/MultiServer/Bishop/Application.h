/********************************************************************
	created:	2003/05/30
	file base:	Application
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_APPLICATION_H__
#define __INCLUDE_APPLICATION_H__

#include "mutex.h"

#include "Network.h"
#include "Intercessor.h"

class CBishopApp
{
public:

	CBishopApp();
	CBishopApp( HINSTANCE hInstance );

	virtual ~CBishopApp();
	
	int Run();

	//CLogFile  bs_LogFile;

	//HWND GetMianH() {return hwndMain;}

protected:
	
	/*
	 * Login dialog
	 */
	bool LoginSystem();

	static BOOL CALLBACK LoginDlgProc( HWND hwndDlg,
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam );

	static void EnterToAffirm( HWND hwndDlg );
	static void LeaveToAffirm( HWND hwndDlg );
	static bool CheckUserInfo( HWND hwndDlg );

	static DWORD WINAPI ServerLoginRoutine( HWND hwndDlg );
	
	static void SendAnnounceText( HWND hwndDlg, const char *pText, UINT uLength, UINT uOption, BOOL bAllGS,int nSelServer,BOOL nIsSavePai );
	static void EnableGameSvrCtrl( HWND hwndDlg, BOOL nEnable );
	static void EnableCtrl( HWND hwndDlg, UINT nCtrlID, BOOL nEnable );
	
	/*
	 * Main dialog
	 */
	BOOL RegisterMainWndClass();
	UINT MainDialog();

	static BOOL CALLBACK MainWndProc( HWND hwnd, 
                           UINT msg,
                           WPARAM wParam,
                           LPARAM lParam );

	static void InitMainDlg( HWND hDlg );
	static void CloseMainDlg( HWND hDlg );
	static void UpdateVariable( HWND hDlg ,BOOL nIsColse=FALSE);

private:
	
	static void AddGameServerInfo( HWND hDlg, UINT nID, const char *pInfo );
	static void DelGameServerInfo( HWND hDlg, UINT nID );
	
	enum enumServerLoginErrorCode
	{
		enumConnectFailed	= 0xA1,
		enumUsrNamePswdErr,
		enumIPPortErr,
		enumException
	};

	CMutex m_theOnlyOneInstance;
	static HINSTANCE			 m_hInst;

	static CNetwork				 m_theNetwork;

	static CGameServer           m_theGameSer;

    static CGamePlayer            m_theGameplayer;

	static CIntercessor			 *m_pIntercessor;

	

	//HWND hwndMain;               //主窗口句柄；

};
void AddOutputStringA(HWND hListCtrl, char* aStr);//添加操作输出文字
void KPIGetExePath( LPSTR lpExePath, DWORD dwSize );
#endif // __INCLUDE_APPLICATION_H__