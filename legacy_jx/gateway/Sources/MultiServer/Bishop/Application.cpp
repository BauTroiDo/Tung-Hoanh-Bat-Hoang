#include "stdafx.h"
#include "application.h"
#include "resource.h"

#include <process.h>
#include <time.h>

#include "GameServer.h"
#include "GamePlayer.h"

#include "KProtocolDef.h"
#include "AccountLoginDef.h"
#include "Buffer.h"

#include "msg_define.h"
#include "Macro.h"
#include "KWin32.h"
#include "KSG_MD5_String.h"
#include "inoutmac.h"

#include "SmartClient.h"

const char log_directory[] = "bishop_log";
const size_t log_threshold = 512 * 1024;

extern CLogFile  bs_LogFile;
extern HWND hbishopOutput;
extern HWND hMianHwd;
extern HWND hAccCheckList;
extern HWND hIpCheckList;
extern HWND hYuJianCheckList;
extern HWND hClientIpList;
extern HWND hForBitOpenIpList;
extern HWND hHaveConnetIpList;
extern HWND hDugList;


HWND hbishopOutput = NULL;
HWND hMianHwd = NULL;
HWND hAccCheckList=NULL;
HWND hIpCheckList=NULL;
HWND hClientIpList=NULL;
HWND hForBitOpenIpList=NULL;
HWND hHaveConnetIpList=NULL;
HWND hDugList=NULL;
HWND hYuJianCheckList=NULL;

HWND hAccCheckList_VIP=NULL;
//HWND hOutputList = NULL;	 IDC_LIST_CLIENTIPINFO	 IDC_LIST_FORBITROLEINFO

//using OnlineGameLib::Win32::CPackager;

static const char szBishopClassName[] = "BishopClass";
//static const DWORD g_dwServerIdentify = 0xAEFC07B5;

HINSTANCE		CBishopApp::m_hInst = NULL;
CNetwork		CBishopApp::m_theNetwork;
CGamePlayer		CBishopApp::m_theGameplayer;
CGameServer		CBishopApp::m_theGameSer;

CIntercessor	*CBishopApp::m_pIntercessor = NULL;

///////////////////////////////////////////////////////////////////////////////////////////
/*
#include <nb30.h>

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

// 输入参数：lana_num为网卡编号，一般地，从0开始，但在Windows 2000中并不一定是连续分配的 
void getmac_one(int lana_num, ASTAT& Adapter)
{
    NCB ncb;
    UCHAR uRetCode;

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = lana_num;   
    // 指定网卡号

	// 首先对选定的网卡发送一个NCBRESET命令，以便进行初始化 
    uRetCode = Netbios( &ncb );

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_num;     // 指定网卡号

    strcpy( (char *)ncb.ncb_callname, "*               ");
    ncb.ncb_buffer = (unsigned char *) &Adapter;

	// 指定返回的信息存放的变量 
    ncb.ncb_length = sizeof(Adapter);

	// 接着，可以发送NCBASTAT命令以获取网卡的信息 
    uRetCode = Netbios( &ncb );

    if ( uRetCode == 0 )
    {
		// 把网卡MAC地址格式化成常用的16进制形式，如0010-A4E4-5802 
//		lana_num
//		  Adapter.adapt.adapter_address[0],
//		  Adapter.adapt.adapter_address[1],
//		  Adapter.adapt.adapter_address[2],
//		  Adapter.adapt.adapter_address[3],
//		  Adapter.adapt.adapter_address[4],
//		  Adapter.adapt.adapter_address[5];
	}
}

int getmac_all(ASTAT** ppBuffer)
{
	NCB ncb;
    UCHAR uRetCode;
    LANA_ENUM lana_enum;

	lana_enum.length = 0;

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBENUM;

    ncb.ncb_buffer = (unsigned char *) &lana_enum;
    ncb.ncb_length = sizeof(lana_enum);

	// 向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等 
	uRetCode = Netbios( &ncb );
	if ( uRetCode == 0 && lana_enum.length > 0)
	{
		*ppBuffer = new ASTAT[lana_enum.length];
		// 对每一张网卡，以其网卡编号为输入编号，获取其MAC地址 
		for ( int i = 0; i < lana_enum.length; ++i)
		{
			getmac_one(lana_enum.lana[i], (*ppBuffer)[i]);
		}
	}

	return lana_enum.length;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////

void charToDouble(const char* ch, double& res)
{
    sscanf(ch,"%lf",&res);
}

CBishopApp::CBishopApp()
		: m_theOnlyOneInstance( NULL, FALSE, TRUE, szBishopClassName )
{
}

CBishopApp::CBishopApp( HINSTANCE hInstance )
		: m_theOnlyOneInstance( NULL, FALSE, TRUE, szBishopClassName )
{
	m_hInst = hInstance;
}

CBishopApp::~CBishopApp()
{

}
//入口函数
int CBishopApp::Run()
{
	/* Seed the random-number generator with current time so that
	 * the numbers will be different every time we run.
	 */
	TCHAR szExePath[MAX_PATH + 1]={0};
	KPIGetExePath( szExePath, MAX_PATH );
	
	TCHAR szLogDir[MAX_PATH + 1]={0};
	_tcscpy(szLogDir, szExePath);
	_tcscat(szLogDir, log_directory);
	::CreateDirectory(szLogDir, NULL);
	
	if (!bs_LogFile.Initialize(szLogDir, log_threshold))
	{
		::MessageBox(NULL, "Initializing LogFile fail", "error", MB_OK|MB_ICONERROR);
		return -1;
	}

	 //bs_LogFile.sLogProgram(TRUE,"Bishop");

	srand((unsigned)time(NULL));

	m_theNetwork.Create();   //创建线程 设置基本数据
   
	/*
	 * To affirm account that it is used to check the application
	 */
	if (LoginSystem())  //登陆成功 输入密码账号 界面
	{
		m_pIntercessor = new CIntercessor(m_theNetwork.GetMaxPlayerCount(),m_theNetwork);  //200人

		//ASSERT(m_pIntercessor);
		if (!m_pIntercessor)
		{
			::MessageBox(NULL, "CIntercessor Is NULL", "error", MB_OK|MB_ICONERROR);
		}

		MainDialog();  //初始化主窗口
	}

	m_theNetwork.Destroy();  //删除链接

	return 0;
}

/*
 * Log on to system
 */
bool CBishopApp::LoginSystem()
{
	if (WM_SERVER_LOGIN_SUCCESSFUL == ::DialogBox(m_hInst,MAKEINTRESOURCE(IDD_DLG_LOGIN),NULL,(DLGPROC)LoginDlgProc))
	{
		return true;
	}
    
	return false;
}

BOOL CALLBACK CBishopApp::LoginDlgProc(HWND hwndDlg,\
		UINT message,\
		WPARAM wParam,\
		LPARAM lParam)
{
	char szBuffer[MAX_PATH];

    switch (message)
    {
	case WM_SERVER_LOGIN_SUCCESSFUL:

		::EndDialog( hwndDlg, WM_SERVER_LOGIN_SUCCESSFUL );
		
		break;

	case WM_SERVER_LOGIN_FAILED://失败

		switch( wParam )
		{
		case enumUsrNamePswdErr:
//姓名密码错误
			::LoadString( m_hInst, IDS_NAMEPSWD_ERROR, szBuffer, MAX_PATH );

			::MessageBox( hwndDlg, szBuffer, NULL, MB_ICONEXCLAMATION );

			::PostMessage( hwndDlg, WM_LEAVE_AFFIRM, 0L, 0L );

			break;

		case enumConnectFailed:
		case enumIPPortErr:
//网卡不可用
			::LoadString( m_hInst, IDS_NETWORK_ERROR, szBuffer, MAX_PATH );

			::MessageBox( hwndDlg, szBuffer, NULL, MB_ICONEXCLAMATION );

			::PostMessage( hwndDlg, WM_LEAVE_AFFIRM, 0L, 0L );

			break;

		case enumException:
		default:
			::MessageBox( hwndDlg, "Exception!", NULL, MB_ICONEXCLAMATION );
			::EndDialog( hwndDlg, WM_SERVER_LOGIN_FAILED );
			break;
		}
		
		break;

	case WM_ENTER_AFFIRM:

		EnterToAffirm(hwndDlg);
		
		break;

	case WM_LEAVE_AFFIRM:

		LeaveToAffirm(hwndDlg);

		break;

	case WM_INITDIALOG:

		::SetFocus(::GetDlgItem(hwndDlg,IDC_EDIT_USERNAME));	
		::SetDlgItemText( hwndDlg, IDC_EDIT_LOGONTO_IP, m_theNetwork.GetAccSvrIP() );
		::SetDlgItemInt( hwndDlg, IDC_EDIT_LOGONTO_PORT, m_theNetwork.GetAccSvrPort(), FALSE );
		::SetDlgItemInt( hwndDlg, IDC_EDIT_TWO_REVIVE, m_theNetwork.GetTwoRevivrPos(), FALSE );
		//::SetDlgItemInt( hwndDlg, IDC_EDIT_TWO_REVIVE, m_theNetwork.GetBishopVer(), FALSE );
		::SetDlgItemInt( hwndDlg, IDC_EDIT_FU,0,FALSE);
		::SetDlgItemInt( hwndDlg, IDC_EDIT_QUYU,0,FALSE);
		char sBuffer[16];
		sprintf(sBuffer,"%.2lf",m_theNetwork.GetBishopVer());
		::SetDlgItemText(hwndDlg,IDC_EDIT_BISHOPVER,sBuffer);

		::SetDlgItemInt( hwndDlg, IDC_EDIT_FORBITOPENUM, m_theNetwork.GetOpenCount(), FALSE );
		::SetDlgItemInt( hwndDlg, IDC_EDIT_SERVER,0,FALSE);
		//char szBuffer[8];
		//::GetDlgItemText( hWndPlayr,IDC_EDIT_BISHOPVER,szBuffer,8);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{//点击 OK 
		case IDOK:

			if (CheckUserInfo(hwndDlg))
			{
				::PostMessage(hwndDlg,WM_ENTER_AFFIRM,0L,0L);
			}
			break;
		case IDCANCEL:
			
			::EndDialog( hwndDlg, WM_SERVER_LOGIN_FAILED );	  //登录失败

			return TRUE; 
		} 
    }
    //::MessageBox( hwndDlg, "启动失败", NULL, MB_ICONEXCLAMATION );
    return FALSE;
}

void CBishopApp::EnterToAffirm( HWND hwndDlg )
{
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_PASSWORD ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_IP ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_PORT ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, ID_START ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDCANCEL ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_QUYU ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_FU ), TRUE );
	//::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_TWO_REVIVE ), FALSE );

	/*
	 * Start up a listening thread to wait a required confirm from the account server
	 */
	//DWORD dwThreadID = 0;

	//HANDLE hThread = ::CreateThread( NULL, 0, ServerLoginRoutine, ( void * )hwndDlg, 0, &dwThreadID );
	
	//SAFE_CLOSEHANDLE( hThread );
	//::MessageBox( hwndDlg, "开始向peysystem发包", "Warning", MB_ICONASTERISK );
	ServerLoginRoutine(hwndDlg);


}

void CBishopApp::LeaveToAffirm( HWND hwndDlg )
{
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_PASSWORD ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_IP ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_PORT ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, ID_START ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDCANCEL_OUT ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_TWO_REVIVE ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_FORBITOPENUM ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_BISHOPVER ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_QUYU ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_FU ), TRUE );
}

bool CBishopApp::CheckUserInfo( HWND hwndDlg )
{
	UINT	nLen = 0;
	char	szBuffer[MAX_PATH];
	
	if ( 0 == ( nLen = ::GetDlgItemText( hwndDlg, IDC_EDIT_USERNAME, szBuffer, MAX_PATH ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ) );

		::MessageBox( hwndDlg, "Please input a valid username!", "Warning", MB_ICONASTERISK );

		return false;
	}

	if ( 0 == ( nLen = ::GetDlgItemText( hwndDlg, IDC_EDIT_PASSWORD, szBuffer, MAX_PATH ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_PASSWORD ) );
		
		::MessageBox( hwndDlg, "Please input a valid password!", "Warning", MB_ICONASTERISK );
		
		return false;
	}

	if ( 0 == ( nLen = ::GetDlgItemText( hwndDlg, IDC_EDIT_LOGONTO_IP, szBuffer, MAX_PATH ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_IP ) );

		::MessageBox( hwndDlg, "Please input a valid IP!", "Warning", MB_ICONASTERISK );

		return false;
	}
	
	BOOL bTranslated = FALSE;

	if ( 0 == ( nLen = ::GetDlgItemInt( hwndDlg, IDC_EDIT_LOGONTO_PORT, &bTranslated, TRUE ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_PORT ) );

		::MessageBox( hwndDlg, "Please input a valid port!", "Warning", MB_ICONASTERISK );

		return false;
	}

	return true;
}

DWORD WINAPI CBishopApp::ServerLoginRoutine( HWND hwndDlg )
{
	ASSERT( hwndDlg );

	/*
	 * Ask for log on to the account server
	 */

	UINT nPort = 0;

	char szBuffer[MAX_PATH]={0};
	char szUsername[NAME_PWD_EX_LEN]={0};
	char szPassword[NAME_PWD_EX_LEN]={0};

	::GetDlgItemText( hwndDlg, IDC_EDIT_LOGONTO_IP, szBuffer, MAX_PATH );

	BOOL bTranslated = TRUE;

	nPort = ::GetDlgItemInt( hwndDlg, IDC_EDIT_LOGONTO_PORT,&bTranslated,TRUE);

	::GetDlgItemText( hwndDlg, IDC_EDIT_USERNAME, szUsername, NAME_PWD_EX_LEN );

	::GetDlgItemText( hwndDlg, IDC_EDIT_PASSWORD, szPassword, NAME_PWD_EX_LEN );

	g_theSmartClient.Open(szBuffer,nPort,hwndDlg,szUsername,szPassword,true,m_theNetwork.GetMaxPlayerCount());	 //m_theNetwork.GetMaxPlayerCount()

/*	IClient *pAccSvrClient = m_theNetwork.CreateAccSvrClient( szBuffer, nPort );

	if ( !pAccSvrClient )
	{
		::PostMessage( hwndDlg, WM_SERVER_LOGIN_FAILED, enumConnectFailed, 0L );

		SAFE_RELEASE( pAccSvrClient );
		return 1L;
	}
*/
	if (!g_theSmartClient.Valid())
	{//网络无效的 
		::PostMessage(hwndDlg, WM_SERVER_LOGIN_FAILED, enumConnectFailed, 0L );
		return 1L;
	}

//	SAFE_RELEASE( pAccSvrClient );

	return 0L;
}

BOOL CBishopApp::RegisterMainWndClass()
{
    WNDCLASSEX wcx; 
 
    /*
	 * Fill in the window class structure with parameters 
     * that describe the main window. 
	 */
    wcx.cbSize = sizeof( wcx );					// size of structure 
    wcx.style = CS_HREDRAW | CS_VREDRAW;		// redraw if size changes 
    wcx.lpfnWndProc = ( WNDPROC )MainWndProc;	// points to window procedure 
    wcx.cbClsExtra = 0;							// no extra class memory 
    wcx.cbWndExtra = 0;							// no extra window memory 
    wcx.hInstance = m_hInst;					// handle to instance 
    wcx.hIcon = LoadIcon(NULL, 
        IDI_APPLICATION);						// predefined app. icon 
    wcx.hCursor = LoadCursor(NULL, 
        IDC_ARROW);								// predefined arrow 
    wcx.hbrBackground = ( HBRUSH )GetStockObject( 
        WHITE_BRUSH);							// white background brush 
    wcx.lpszMenuName =  NULL;					// name of menu resource 
    wcx.lpszClassName = _T( "IntercessorClass" );	// name of window class 
    wcx.hIconSm = ( HICON )LoadImage(m_hInst,		// small class icon 
        MAKEINTRESOURCE(IDI_LOCK),
        IMAGE_ICON, 
        GetSystemMetrics(SM_CXSMICON), 
        GetSystemMetrics(SM_CYSMICON), 
        LR_DEFAULTCOLOR); 
 
    /*
	 * Register the window class. 
	 */ 
    return ::RegisterClassEx( &wcx );
}

UINT CBishopApp::MainDialog()
{
	/*
	 * Create the main window as dialog. 
	 */
/*	if ( !RegisterMainWndClass() )
	{
		return 0L;
	}
*/
	HWND hwndMain = ::CreateDialog( m_hInst, 
		MAKEINTRESOURCE( IDD_DLG_INTERCESSOR ),
		NULL, 
		(DLGPROC)MainWndProc);

	ASSERT(hwndMain);

	::ShowWindow( hwndMain, SW_SHOWNORMAL );
	::UpdateWindow( hwndMain );

	hMianHwd =	hwndMain;
	hbishopOutput = ::GetDlgItem( hwndMain, IDC_LIST_GAMESERVER2 );//服务器信息输出list
	hAccCheckList =	::GetDlgItem( hwndMain, IDC_LIST_FORBITACC);   //账号检测list
	hIpCheckList  =	::GetDlgItem( hwndMain, IDC_LIST_FORBITIP);	   //过滤IP列表
	hClientIpList = ::GetDlgItem( hwndMain, IDC_LIST_CLIENTIPINFO);	//客户端ip显示列表
	hForBitOpenIpList =	::GetDlgItem( hwndMain, IDC_LIST_FORBITOPEN_INFO);
	hHaveConnetIpList=	::GetDlgItem( hwndMain,IDC_LIST_HAVECONETIP);
	hDugList =	 ::GetDlgItem( hwndMain,IDC_LIST_FORBITROLEINFO);	 //IDC_LIST_FORBITROLEINFO
    hYuJianCheckList   =	::GetDlgItem( hwndMain, IDC_LIST_YUJIAN );
	hAccCheckList_VIP =	::GetDlgItem( hwndMain, IDC_LIST_VIP);   //账号检测list
	BOOL bRet;
	MSG msg;
	
	while ( ( bRet = ::GetMessage( &msg, NULL, 0, 0 ) ) != 0 )
	{
		if ( !IsWindow( hwndMain ) || !IsDialogMessage( hwndMain, &msg ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}

	return msg.wParam;
}

/*
 *
 * MainWndProc() - Main window callback procedure.
 *  
 */

static UINT g_nViewTimer = 0x100;
static UINT g_nTimer = 0;

BOOL CALLBACK CBishopApp::MainWndProc( HWND hwnd, 
                           UINT msg,
                           WPARAM wParam,
                           LPARAM lParam )
{
	static size_t nGameServerNumber = 0;
	static char szInfo[64];

	UINT uLength = 0;
	TCHAR szAnnounceText[256];

	switch ( msg )
	{
	case WM_INITDIALOG:
		
		/*
		 * Initialize
		 */
		InitMainDlg( hwnd );
		
		return TRUE;

		break;

	case WM_CLOSE:
		
		/*if (IDYES == ::MessageBox(hwnd,"Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION))
		{
			if (g_nTimer)
			{
				::KillTimer(hwnd,g_nViewTimer);
				g_nTimer = 0;
			}
			//::MessageBox(NULL, "退出成功A!", "警告", MB_OK );
			CloseMainDlg(hwnd);
			if (m_pIntercessor)
			{
				m_pIntercessor->Destroy();
			}
			//::MessageBox(NULL, "退出成功B!", "警告", MB_OK );
			::DestroyWindow(hwnd);
			::PostQuitMessage(0); 
			//PostMessage(hwnd,WM_QUIT,0,0); 
		}*/	

		return TRUE;

		break;

	case WM_TIMER:
		if (g_nViewTimer == wParam)
		{
			LONG nw = CGamePlayer::GetWorking();     //当前链接
			LONG nc = CGamePlayer::GetCapability();  //总人数 链接数
			sprintf(szInfo,"%d/%d",nw,nc);
			::SetDlgItemText(hwnd,IDC_STATIC_TASK,(LPCTSTR)szInfo);
		}
		break;
	
	case WM_COMMAND:

		switch ( wParam )
		{
		case ID_START://启动按钮  设置按钮 是否可操作		
			::EnableWindow( GetDlgItem( hwnd, ID_START ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_CLIENT_PORT ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_GAMESVR_PORT ), FALSE );

			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_ROLESVRIP ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_ROLESVRPORT ), FALSE );

			//::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_TWO_REVIVE ), FALSE );

			::CheckDlgButton( hwnd, IDC_CHECK_PLAYER, BST_CHECKED );  //
			::CheckDlgButton( hwnd, IDC_CHECK_GAMESVR, BST_CHECKED );

			::CheckDlgButton( hwnd, IDC_CHECK_ACCSVR, BST_CHECKED );
			::CheckDlgButton( hwnd, IDC_CHECK_REVIVE, BST_CHECKED );

			UpdateVariable(hwnd); //设置相关端口 和IP

			if ( !m_pIntercessor->Create(hwnd))
			{//如何 客户端服务  和 没有连接 GODDES 和 GS服务没有创建成功的话 就启动失败
				::EnableWindow(GetDlgItem(hwnd, ID_START),TRUE);
				m_pIntercessor->Destroy();
			}
			else
			{//否则 启动成功
				g_nTimer = ::SetTimer(hwnd,g_nViewTimer,5000,NULL);
				::SetWindowText( hwnd, "Bishop - [enable]" );
				bs_LogFile.sLogProgram(TRUE,"Bishop");
			}

			break;
		case IDOK: 
			break;
		case IDCANCEL_OUT: //发送关闭程序信息到 主窗口
			//::PostMessage(hwnd,WM_CLOSE,0L,0L);
			{
				if (IDYES == ::MessageBox(hwnd,"Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION))
				{
				  bs_LogFile.sLogProgram(FALSE,"Bishop");	
			      if (g_nTimer)
				  {  
				    ::KillTimer(hwnd,g_nViewTimer);
				    g_nTimer = 0;
				  }   
			      
			      if (m_pIntercessor)
				  {  
				    m_pIntercessor->Destroy();
				  }   
				   CloseMainDlg(hwnd);
			       ::DestroyWindow(hwnd);
			       ::PostQuitMessage(0); 
				} 
			}
			break;
		/*case IDC_BUTTON_REVIVEPOS:
			{//保存重生点
			 // m_theGameplayer.SetPlayerRevivePos();
                UINT nValue=0;
                BOOL bTranslated=0;
                nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
				m_theNetwork.SaveSet(nValue); 
			} 
			break;*/

		case IDC_BUTTON_PAY:
			{
				g_theSmartClient.reConnectPayServer(true);
				break;
			}
		case IDC_BUTTON_CHONG:
			{
				//m_theGameSer.GetContent();
				//::MessageBox(NULL, "暂无消息!", "警告", MB_OK );
				if (m_pIntercessor)
				{
				   if (m_pIntercessor->RetConnnetGoddess())
					   ::MessageBox(NULL, "数据库重连成功!", "提示", MB_OK );
				   else
				   {
					   ::MessageBox(NULL, "数据库重连失败或已经连接!", "提示", MB_OK );
				   }
				}
				else
					::MessageBox(NULL, "m_pIntercessor error!", "提示", MB_OK );
			}
			break;
		case IDC_BUTTON_SAVE:
		{//保存设置
			// m_theGameplayer.SetPlayerRevivePos();
			///nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
			char szBuffer[16];
			::GetDlgItemText(hwnd,IDC_EDIT_BISHOPVER,szBuffer,16);
			m_theNetwork.SaveVerSet(szBuffer);
			
			UINT nValue=0;
			BOOL bTranslated=0;
			nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
				m_theNetwork.SaveSet(nValue);

			nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_FORBITOPENUM, &bTranslated, FALSE );
				m_theNetwork.SaveOpenCount(nValue);

		} 
		break;	
		case IDC_ACC_ADD_BTN:
		{//增加账号
			// m_theGameplayer.SetPlayerRevivePos();
			///nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
			char szBuffer[ACC_LENGTH];
			::GetDlgItemText(hwnd,IDC_ACC_EDIT,szBuffer, ACC_LENGTH);
			AddOutputStringA(hAccCheckList,szBuffer);
			::SetDlgItemText(hwnd,IDC_ACC_EDIT,"");
		} 
		break;
		case IDC_CLEARACC_BTN:
		{//清空信息
			int i=0,nCount = ::SendMessage(hAccCheckList, LB_GETCOUNT, 0, 0 ); //获取信息的数量
			if (nCount==-1)
				break; 
			for (i=0;i<=nCount;++i)
			{
				::SendMessage(hAccCheckList,LB_DELETESTRING,i,0);  //删除信息
				Sleep(1);
			}
		}
		break;
		case IDC_IP_ADD_BTN:
			{//增加账号
				// m_theGameplayer.SetPlayerRevivePos();
				///nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
				char szBuffer[32];
				::GetDlgItemText(hwnd,IDC_IP_EDIT,szBuffer,32);
				AddOutputStringA(hIpCheckList,szBuffer);
				::SetDlgItemText(hwnd,IDC_IP_EDIT,"");
			} 
		break;
		case IDC_CLEARIP_BTN:
			{//清空IP信息
				int i=0,nCount = ::SendMessage(hIpCheckList, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break; 
				for (i=0;i<=nCount;++i)
				{
					::SendMessage(hIpCheckList,LB_DELETESTRING,i,0);  //删除信息
					Sleep(1);
				}
			}
		break;
		case IDC_DAORUIP_BTN:
		{
			int  i,mCount=m_theNetwork.GetIpListCount();
			for (i=0;i<=mCount;++i)
			{
				if (hIpCheckList)
					AddOutputStringA(hIpCheckList,m_theNetwork.GetIpString(i));
				else
					break;
			}
		}
		break;
		case IDC_SAVEIP_BTN:
			{//保存过滤列表
				int nCount = ::SendMessage(hIpCheckList, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break;
				m_theNetwork.ClearIpList("ip");
				
				for (int i=0;i<=nCount;++i)
				{
					char buf[NAME_LENGHT]; 
					::SendMessage(hIpCheckList, LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
					if (buf[0])
					{
						char nSecName[NAME_LENGHT];
						snprintf(nSecName, sizeof(nSecName),"%s_%d","ip",i);
						m_theNetwork.SaveIpList(nSecName,"ip",buf,i);
					}
					
				}	   
			}	
			break;
		case IDC_CLIENTIP_ADD_BTN:
		{
			int nSelIndex=-1;
			
			if (LB_ERR != ( nSelIndex = ::SendMessage(hClientIpList, LB_GETCURSEL,0,0)))
			{  
				//::SendMessage(hIpCheckList,LB_DELETESTRING,nSelIndex,0);  //删除信息
				char buf[32] ={0};
				::SendMessage(hClientIpList, LB_GETTEXT,nSelIndex,(LPARAM)buf);
				AddOutputStringA(hIpCheckList,buf);
			}
			else
			{
				::MessageBox(NULL, "请选择一条需要添加的IP!", "警告", MB_OK );
			}
		}
		break;
		case IDC_IP_FORBITOPEN_ADD_BTN:
		{
			int nSelIndex=-1;
			
			if (LB_ERR != ( nSelIndex = ::SendMessage(hClientIpList, LB_GETCURSEL,0,0)))
			{  
				//::SendMessage(hIpCheckList,LB_DELETESTRING,nSelIndex,0);  //删除信息
				char buf[32] ={0};
				::SendMessage(hClientIpList, LB_GETTEXT,nSelIndex,(LPARAM)buf);
				AddOutputStringA(hForBitOpenIpList,buf);
			}
			else
			{
				::MessageBox(NULL, "请在客户端IP列表中选择一条需要添加的IP!", "警告", MB_OK );
			}
		}
		break;

		case IDC_BUTTON_ADDHAVEIP:
			{//增加IP
				// m_theGameplayer.SetPlayerRevivePos();
				///nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
				char szBuffer[32];
				::GetDlgItemText(hwnd,IDC_IP_EDIT,szBuffer,32);
				if (szBuffer[0])
				   AddOutputStringA(hHaveConnetIpList,szBuffer);
				else
				   ::MessageBox(NULL, "请在过滤IP列表编辑框中填写IP!", "警告", MB_OK );

				::SetDlgItemText(hwnd,IDC_IP_EDIT,"");
			} 
		break;
		case IDC_BUTTON_DELHAVEIP:
			{
				int nSelIndex=-1;
				
				if ( LB_ERR != ( nSelIndex = ::SendMessage(hHaveConnetIpList, LB_GETCURSEL,0,0)))
				{  
					::SendMessage(hHaveConnetIpList,LB_DELETESTRING,nSelIndex,0);  //删除信息
				}
				else
				{
					::MessageBox(NULL, "请选择一条需要删除的IP!", "警告", MB_OK );
				}
			}
		break;
		//设置预建角色  IDC_ACC_EDIT_YUJIAN
		case IDC_ACC_ADD_BTN_YUJIAN:
			{   //是否选择开启了预建立角色
				char szBuffer[32];
				::GetDlgItemText(hwnd,IDC_ACC_EDIT_YUJIAN,szBuffer,32);
				AddOutputStringA(hYuJianCheckList,szBuffer);
				::SetDlgItemText(hwnd,IDC_ACC_EDIT_YUJIAN,"");
			}
			break;
		case IDC_ACC_DEL_BTN_YUJIAN:
			{
				int nSelIndex=-1;
				if ( LB_ERR != ( nSelIndex = ::SendMessage(hYuJianCheckList, LB_GETCURSEL,0,0)))
					::SendMessage(hYuJianCheckList,LB_DELETESTRING,nSelIndex,0);  //删除信息
				else
					::MessageBox(NULL, "请选择一条需要删除的账号!", "警告", MB_OK );
			}
			break;
		case IDC_BUTTON_CLEARHAVEIP:
			{//清空IP信息
				int i=0,nCount = ::SendMessage(hHaveConnetIpList, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break; 
				for (i=0;i<=nCount;++i)
				{
					::SendMessage(hHaveConnetIpList,LB_DELETESTRING,i,0);  //删除信息
					Sleep(1);
				}
			}
			break;
		case IDC_IP_FORBITOPEN_CLEAR_BTN:
		{
			int nSelIndex=-1;
			
			if ( LB_ERR != ( nSelIndex = ::SendMessage(hForBitOpenIpList, LB_GETCURSEL,0,0)))
			{  
				::SendMessage(hForBitOpenIpList,LB_DELETESTRING,nSelIndex,0);  //删除信息
			}
			else
			{
				::MessageBox(NULL, "请选择一条需要删除的IP!", "警告", MB_OK );
			}
		}
		break;
		case IDC_CLIENTIP_DEL_BTN:
		{
			int i=0,nCount = ::SendMessage(hClientIpList, LB_GETCOUNT,0,0); //获取信息的数量
			if (nCount==-1)
				break; 
			for (i=0;i<=nCount;++i)
			{
				::SendMessage(hClientIpList,LB_DELETESTRING,i,0);  //删除信息
				Sleep(1);
			}
		}
		break;
		case IDC_IP_DEL_BTN:
			{//减少账号
				// m_theGameplayer.SetPlayerRevivePos();
				///nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
				int nSelIndex=-1;
				
				if ( LB_ERR != ( nSelIndex = ::SendMessage(hIpCheckList, LB_GETCURSEL,0,0)))
				{  
					::SendMessage(hIpCheckList,LB_DELETESTRING,nSelIndex,0);  //删除信息
				}
				else
				{
					::MessageBox(NULL, "请选择一条需要删除的IP!", "警告", MB_OK );
				}
			} 
		break;
		case IDC_CLEARACC_VIP:
			{//清空信息
				int i=0,nCount = ::SendMessage(hAccCheckList_VIP, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break; 
				for (i=0;i<=nCount;++i)
				{
					::SendMessage(hAccCheckList_VIP,LB_DELETESTRING,i,0);  //删除信息
					Sleep(1);
				}
			}
			break;
		case IDC_ACC_ADD_BTN_VIP:
			{
				char szBuffer[32];
				::GetDlgItemText(hwnd,IDC_ACC_EDIT_VIP,szBuffer,32);
				AddOutputStringA(hAccCheckList_VIP,szBuffer);
				::SetDlgItemText(hwnd,IDC_ACC_EDIT_VIP,"");
			}
			break;
		case IDC_DAORUACC_VIP:
			{
				int i=0,nCount = ::SendMessage(hAccCheckList_VIP, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break; 
				for (i=0;i<=nCount;++i)
					::SendMessage(hAccCheckList_VIP,LB_DELETESTRING,i,0);  //删除信息

				int mCount=m_theNetwork.GetAccListCount("acclist_vip.cfg");
				for (i=0;i<=mCount;++i)
				{
					if (hAccCheckList_VIP)
						AddOutputStringA(hAccCheckList_VIP,m_theNetwork.GetAccString(i,"acclist_vip.cfg"));
					else
						break;
				}
			}
			break;
		case IDC_SAVEACC_VIP:
			{//保存过滤列表
				int nCount = ::SendMessage(hAccCheckList_VIP, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break;
				m_theNetwork.ClearAccList("accname","acclist_vip.cfg");

				for (int i=0;i<=nCount;++i)
				{
					char buf[NAME_LENGHT]={0};
					::SendMessage(hAccCheckList_VIP, LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
					if (buf[0])
					{
						char nSecName[NAME_LENGHT]={0};
						snprintf(nSecName, sizeof(nSecName),"%s_%d","accname",i);
						m_theNetwork.SaveAccList(nSecName,"name",buf,"acclist_vip.cfg");
					}

				}	   
			}	
			break;
		case IDC_ACC_DEL_BTN_VIP:
			{//减少账号
				int nSelIndex=-1;
				if ( LB_ERR != ( nSelIndex = ::SendMessage(hAccCheckList_VIP, LB_GETCURSEL,0,0)))
					::SendMessage(hAccCheckList_VIP,LB_DELETESTRING,nSelIndex,0);  //删除信息
				else
					::MessageBox(NULL, "请选择一条需要删除的账号!", "警告", MB_OK );
			} 
			break;
		case IDC_DAORUACC_BTN:
		{//导入过滤列表
			int i=0,nCount = ::SendMessage(hAccCheckList, LB_GETCOUNT, 0, 0 ); //获取信息的数量
			if (nCount==-1)
				break; 
			for (i=0;i<=nCount;++i)
			{
			    ::SendMessage(hAccCheckList,LB_DELETESTRING,i,0);  //删除信息
			}

			int mCount=m_theNetwork.GetAccListCount("accforbitlist.cfg");
			for (i=0;i<=mCount;++i)
			{
				if (hAccCheckList)
					AddOutputStringA(hAccCheckList,m_theNetwork.GetAccString(i,"accforbitlist.cfg"));
				else
					break;
			}

		}	
		break;
		case IDC_SAVEACC_BTN:
			{//保存过滤列表
			   	int nCount = ::SendMessage(hAccCheckList, LB_GETCOUNT, 0, 0 ); //获取信息的数量
				if (nCount==-1)
					break;
			    m_theNetwork.ClearAccList("accname","accforbitlist.cfg");

			   for (int i=0;i<=nCount;++i)
			   {
				   char buf[NAME_LENGHT]={0};
				   ::SendMessage(hAccCheckList, LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
				   if (buf[0])
				   {
					   char nSecName[NAME_LENGHT]={0};
					   snprintf(nSecName, sizeof(nSecName),"%s_%d","accname",i);
				       m_theNetwork.SaveAccList(nSecName,"name",buf,"accforbitlist.cfg");
				   }
				   
			   }	   
			}	
		break;
		case IDC_ACC_DEL_BTN:
			{//减少账号
				// m_theGameplayer.SetPlayerRevivePos();
				///nValue = ::GetDlgItemInt(hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
				int nSelIndex=-1;

				if ( LB_ERR != ( nSelIndex = ::SendMessage(hAccCheckList, LB_GETCURSEL,0,0)))
				{  
                   ::SendMessage(hAccCheckList,LB_DELETESTRING,nSelIndex,0);  //删除信息
				//	::SendMessage(hIpCheckList, LB_GETTEXT, i, (LPARAM)buf);
				}
				else
				{
					::MessageBox(NULL, "请选择一条需要删除的账号!", "警告", MB_OK );
				}
			} 
		break;


		case IDC_BTN_SEND_MSG:
			{//向服务器发送游戏操作信息
				BOOL bToAllGS = FALSE;
				BOOL nIsSavePai = FALSE;
				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_INDEX_SEND2GAMESERVER ) )
				{//全部服务器
					bToAllGS = TRUE;
				}
				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_CHECK_SAVEPAI ) )
				{//全部服务器
					nIsSavePai = TRUE;
				}
				
				UINT uOption = AP_NOTIFY_ALL_PLAYER; //默认是通知服务器发送公告！

				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_INDEX_GS2CLOSE ) )
				{//警告要安全关闭服务器
					uOption = AP_NOTIFY_GAMESERVER_SAFECLOSE;
				}

				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_INDEX_WANINGGS2CLOSE ) )
				{//警告玩家退出游戏
					uOption = AP_WARNING_ALL_PLAYER_QUIT;
				}
				int  nSelServer=1;
				BOOL bTranslated = TRUE;
				     nSelServer =::GetDlgItemInt(hwnd,IDC_EDIT_QUYU,&bTranslated,FALSE);
				     uLength = ::GetDlgItemText( hwnd, IDC_EDIT_ANNOUNCE_MSG, szAnnounceText, 256 );
				szAnnounceText[uLength] = '\0';
				//向这个客户端GS发送信息
				SendAnnounceText(hwnd, ( const char * )szAnnounceText, uLength, uOption, bToAllGS,nSelServer,nIsSavePai);
			}
			break;

		case IDC_BTN_GAMESVRINFO:
			{//显示有多少个连接到BISHOPDE 客户端 IP
				HWND hCtrl = ::GetDlgItem( hwnd, IDC_LIST_GAMESERVER );

				BOOL bRet = FALSE;

				if ( hCtrl && ::IsWindow( hCtrl ) )
				{
					int nSelIndex = LB_ERR;
					
					if ( LB_ERR != ( nSelIndex = ::SendMessage( hCtrl, LB_GETCURSEL, 0, 0 ) ) )
					{
						UINT nSearchID = 0;
				
						if ( LB_ERR != ( nSearchID = ::SendMessage( hCtrl, LB_GETITEMDATA, nSelIndex, 0 ) ) )
						{
							IGServer * pGServer = CGameServer::GetServer( nSearchID );
							
							if (pGServer)
							{
								size_t nNum = pGServer->GetContent();//获取GS连接数
								
								bRet = ::SetDlgItemInt( hwnd, IDC_STATIC_GAMESERVER_CONTEXT, nNum, TRUE );
							}
						}
					}
				}

				if ( !bRet )
				{//设置内容显示
					::SetDlgItemText( hwnd, IDC_STATIC_GAMESERVER_CONTEXT, "0" );
				}
			}
			break;
		}

		break;
		
	case WM_SERVER_STATUS:

		switch ( wParam )
		{
		case ACCOUNTSERVER_NOTIFY:

			if ( CONNECTED == lParam )
			{//连接成功
				::CheckDlgButton( hwnd, IDC_CHECK_ACCSVR, BST_CHECKED );
			}
			else // DICONNECTED
			{//连接失败
				::CheckDlgButton( hwnd, IDC_CHECK_ACCSVR, BST_UNCHECKED );
			}

			break;

		case DBROLESERVER_NOTIFY:

			if ( CONNECTED == lParam )
			{
				::CheckDlgButton( hwnd, IDC_CHECK_ROLESVR, BST_CHECKED ); //按下
			}
			else // DICONNECTED
			{
				::CheckDlgButton( hwnd, IDC_CHECK_ROLESVR, BST_UNCHECKED );//放开
			}
			
			break;
		}

		break;	

	case WM_GAMESERVER_EXCHANGE:
		{
			switch ( wParam )
			{
			case ADD_GAMESERVER_ACTION:
				{//连接bishop的IP显示数据信息
					if (m_pIntercessor)
					{
						//const char *pInfo = 
						char pInfo[128]	={0};
						m_pIntercessor->GetGameServerInfo( lParam ,pInfo,sizeof(pInfo));

						AddGameServerInfo(hwnd, lParam, pInfo);

						if ( 0 == nGameServerNumber++)
						{
							EnableGameSvrCtrl( hwnd, TRUE );

							HWND hCtrl = ::GetDlgItem( hwnd, IDC_LIST_GAMESERVER );
							
							if ( hCtrl && ::IsWindow( hCtrl ) )
							{
								::SendMessage( hCtrl, LB_SETCURSEL, 0, 0 );
							}
						}
					}
				}
				break;

			case DEL_GAMESERVER_ACTION:
				{
					if ( m_pIntercessor )
					{
						if ( -- nGameServerNumber <= 0 )
						{
							nGameServerNumber = 0;

							EnableGameSvrCtrl( hwnd, FALSE );

							::SetDlgItemText( hwnd, IDC_STATIC_GAMESERVER_CONTEXT, "0" );
						}

						DelGameServerInfo(hwnd,lParam);
					}				
				}
				break;
			}
		}

		break;

	default:

		break;
		
	}
	
	/*
	 * Clean up any unused messages by calling DefWindowProc
	 */
	return FALSE;
	//return ::DefWindowProc( hwnd, msg, wParam, lParam );
}

void CBishopApp::AddGameServerInfo( HWND hDlg, UINT nID, const char *pInfo )
{
	if ( pInfo && pInfo[0] )
	{
		HWND hCtrl = ::GetDlgItem(hDlg, IDC_LIST_GAMESERVER );
		
		if ( hCtrl && ::IsWindow( hCtrl ) )
		{
			int nIndex = ::SendMessage( hCtrl, LB_ADDSTRING, 0, ( LPARAM )pInfo );

			if ( LB_ERR != nIndex )
			{
				::SendMessage( hCtrl, LB_SETITEMDATA, nIndex, nID );
			}
		}
	}
}

void CBishopApp::DelGameServerInfo(HWND hDlg, UINT nID)
{
	HWND hCtrl = ::GetDlgItem(hDlg,IDC_LIST_GAMESERVER);
	
	if ( hCtrl && ::IsWindow(hCtrl))
	{
		int nCount = ::SendMessage(hCtrl,LB_GETCOUNT,0,0);  //获取列表信息数量

		if ( LB_ERR != nCount)
		{			
			for (int i=0; i<nCount; ++i)
			{
				UINT nSearchID = 0;
				
				if (nID == (nSearchID = ::SendMessage(hCtrl,LB_GETITEMDATA,i,0)))
				{
					::SendMessage(hCtrl,LB_DELETESTRING,i,0);
					return;
				}
			}
		}
	}
}

void CBishopApp::InitMainDlg( HWND hDlg )
{
	::SetDlgItemText( hDlg, IDC_EDIT_ACCSVRIP, m_theNetwork.GetAccSvrIP() );
	::SetDlgItemInt( hDlg, IDC_EDIT_ACCSVRPORT, m_theNetwork.GetAccSvrPort(), FALSE );

	::SetDlgItemText( hDlg, IDC_EDIT_ROLESVRIP, m_theNetwork.GetRoleSvrIP() );
	::SetDlgItemInt( hDlg, IDC_EDIT_ROLESVRPORT, m_theNetwork.GetRoleSvrPort(), FALSE );

	::SetDlgItemInt( hDlg, IDC_EDIT_CLIENT_PORT, m_theNetwork.GetClientOpenPort(), FALSE );

	::SetDlgItemInt( hDlg, IDC_EDIT_GAMESVR_PORT, m_theNetwork.GetGameSvrOpenPort(), FALSE );

	::SetDlgItemInt( hDlg, IDC_EDIT_TWO_REVIVE, m_theNetwork.GetTwoRevivrPos(), FALSE);
   
	char szBuffer[16]={0};
	sprintf(szBuffer,"%.2lf",m_theNetwork.GetBishopVer());
	::SetDlgItemText(hDlg, IDC_EDIT_BISHOPVER,szBuffer);
	::SetDlgItemInt( hDlg, IDC_EDIT_SERVER,0,FALSE);
	::SetDlgItemInt( hDlg, IDC_EDIT_FORBITOPENUM, m_theNetwork.GetOpenCount(), FALSE );
	::SetDlgItemInt( hDlg, IDC_EDIT_QUYU,0, FALSE );
	::SetDlgItemInt( hDlg, IDC_EDIT_FU,0, FALSE );
	::CheckDlgButton( hDlg, IDC_INDEX_SEND2GAMESERVER, BST_CHECKED );
	::CheckDlgButton( hDlg, IDC_RADIO_ANNOUNCE_OPTION, BST_CHECKED );

	g_theSmartClient.m_hwndContain = hDlg;
}

void CBishopApp::CloseMainDlg(HWND hDlg)
{
	UpdateVariable(hDlg,TRUE);
	g_theSmartClient.Close();
}

void CBishopApp::UpdateVariable( HWND hDlg ,BOOL nIsColse)
{

	if (nIsColse)
		return;

	char szBuffer[MAX_PATH];

	::GetDlgItemText( hDlg, IDC_EDIT_ROLESVRIP, szBuffer, MAX_PATH);
	m_theNetwork.SetRoleSvrIP(szBuffer);

	UINT nValue = 0;
	BOOL bTranslated = TRUE;

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_ROLESVRPORT, &bTranslated, FALSE);
	m_theNetwork.SetRoleSvrPort(nValue);

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_CLIENT_PORT, &bTranslated, FALSE);
	m_theNetwork.SetClientOpenPort(nValue);

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_GAMESVR_PORT, &bTranslated, FALSE);
	m_theNetwork.SetGameSvrOpenPort(nValue);

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE);
    m_theNetwork.SetTwoRevivrPos(nValue);

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_FORBITOPENUM, &bTranslated, FALSE);
    m_theNetwork.SetOpenCount(nValue);

	::GetDlgItemText(hDlg, IDC_EDIT_BISHOPVER, szBuffer, MAX_PATH);
	double xx=0.0;
	charToDouble(szBuffer,xx);
	m_theNetwork.SetBishopVer(xx);

/*
   if (nIsColse==FALSE)
   { 
	//::GetDlgItemInt( hDlg, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
	//m_theGameSer.SetTwoRevivePos(nValue);
    //m_theGameplayer.SetTwoRevivePos(nValue);
   } 
*/
}

void CBishopApp::EnableGameSvrCtrl( HWND hwndDlg, BOOL nEnable )
{//设置是否可以操作
	EnableCtrl( hwndDlg, IDC_BTN_GAMESVRINFO, nEnable );
	EnableCtrl( hwndDlg, IDC_LIST_GAMESERVER, nEnable );

	EnableCtrl( hwndDlg, IDC_RADIO_ANNOUNCE_OPTION, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_GS2CLOSE, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_WANINGGS2CLOSE, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_SEND2GAMESERVER, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_SEND2GS_ISSEL, nEnable );
	EnableCtrl( hwndDlg, IDC_EDIT_ANNOUNCE_MSG, nEnable );
	EnableCtrl( hwndDlg, IDC_BTN_SEND_MSG, nEnable );
	EnableCtrl( hwndDlg, IDC_EDIT_QUYU, nEnable );
	EnableCtrl( hwndDlg, IDC_EDIT_FU, nEnable );
}

void CBishopApp::EnableCtrl( HWND hwndDlg, UINT nCtrlID, BOOL nEnable )
{
	HWND hCtrl = ::GetDlgItem( hwndDlg, nCtrlID );
	
	if ( hCtrl && ::IsWindow( hCtrl ) )
	{
		::EnableWindow( hCtrl, nEnable );
	}	
}

void CBishopApp::SendAnnounceText( HWND hwndDlg, const char *pText, UINT uLength, UINT uOption, BOOL bAllGS,int nSelServer,BOOL nIsSavePai )
{
	if ( !pText || 0 == uLength )
	{
		::MessageBox( hwndDlg, "Please input a valid message!", "Warning", MB_OK | MB_ICONINFORMATION );
		
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_ANNOUNCE_MSG ) );
		
		return;
	}

	BOOL bTranslated = TRUE;

	if (bAllGS)
	{//全部的GS服务器发送信息
		CGameServer::SendToAll(pText, uLength, uOption,nIsSavePai);
	}
	else
	{
		UINT _RegSerVerIndex = ::GetDlgItemInt(hwndDlg, IDC_EDIT_FU, &bTranslated, FALSE);
		UINT _GsNumBerIndex  = ::GetDlgItemInt(hwndDlg, IDC_EDIT_QUYU, &bTranslated, FALSE);
		IGServer * pGServer = CGameServer::GetSerVerIdByIndex(_RegSerVerIndex,_GsNumBerIndex);//GetServer(nSelServer);
		if  (pGServer)
		     pGServer->SendText( pText, uLength, uOption );

		/*HWND hCtrl = ::GetDlgItem( hwndDlg, IDC_LIST_GAMESERVER );

		if ( hCtrl && ::IsWindow( hCtrl ) )
		{
		int nSelIndex = LB_ERR;

		if ( LB_ERR != ( nSelIndex = ::SendMessage( hCtrl, LB_GETCURSEL, 0, 0 ) ) )
		{
		UINT nSearchID = 0;

		if ( LB_ERR != ( nSearchID = ::SendMessage( hCtrl, LB_GETITEMDATA, nSelIndex, 0 ) ) )
		{
		IGServer * pGServer = CGameServer::GetServer( nSearchID );

		if ( pGServer )
		{
		pGServer->SendText( pText, uLength, uOption );
		}
		}
		}
		}*/
		//if  (nSelServer<1)
		//	nSelServer =1;
		//IGServer * pGServer = CGameServer::GetSerVerIdByIndex(nSelServer);//GetServer(nSelServer);
		//if (pGServer)
		//{
		//	pGServer->SendText( pText, uLength, uOption,nIsSavePai);
		//}
	}
}

void AddOutputStringA(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if (hListCtrl && ::IsWindow( hListCtrl ))
	{
		int nCount = ::SendMessage( hListCtrl, LB_GETCOUNT, 0, 0 ); //获取信息的数量
		if(nCount >= 100)
		{
			::SendMessage( hListCtrl, LB_DELETESTRING, 100, 0 );  //删除信息
		}
		int nIndex = ::SendMessage( hListCtrl, LB_INSERTSTRING, 0, ( LPARAM )aStr);//把新的信息添到第一个
	}
}


void KPIGetExePath( LPSTR lpExePath, DWORD dwSize )
{
	if ( lpExePath )
	{
		memset( lpExePath, 0, dwSize );
		GetModuleFileName( NULL, lpExePath, dwSize );
		DWORD dwEndAt;
		dwEndAt = strlen( lpExePath ) - 1;
		while ( dwEndAt >= 1 )
		{
			if ( lpExePath[dwEndAt-1] == '\\' )
			{
				lpExePath[dwEndAt] = 0;
				break;
			}
			dwEndAt--;
		}
	}
}
