/*-----------------------------------------------------------------------------
*	Copyright (c) 2005 珠海.金山.西山居工作室
*	All right reserved.
*
*	Filename	:   ijxreplay.h
*	Description :	剑侠录像功能的对外接口
*	Current Version:	1.0
*	Author	:	ChenWeiMing
*	CreateTime:	2005-7-4
-----------------------------------------------------------------------------*/
#ifndef _IJXREPLAY_H_ 
#define _IJXREPLAY_H_

#ifdef _STANDALONE
	#define JXREPLAY_API
#else
	#ifdef JXREPLAY_EXPORTS
		#define JXREPLAY_API __declspec(dllexport)
	#else
		#define JXREPLAY_API __declspec(dllimport)
	#endif
#endif


enum ENUM_FUNC_TYPE
{
	//-------------------------------------图像的录制类别--------------------
	enum_RepreFun_Create = 0,
	enum_RepreFun_CreateImage,				//1
	enum_RepreFun_CreateAFont,				//2
	enum_RepreFun_SetAdjustColorList,		//3
	enum_RepreFun_ClearImageData,			//4
	enum_RepreFun_FreeAllImage,				//5
	enum_RepreFun_FreeImage,				//6
	enum_RepreFun_GetBitmapDataBuf,			//7
	enum_RepreFun_RelBitmapDataBuf,			//8
	enum_RepreFun_Reset,					//9
	enum_RepreFun_SetImageStoreBar,			//10

	enum_RepreFun_DrawPrimitives,			//11
	enum_RepreFun_DrawPrimitivesOnImg,		//12,
	enum_RepreFun_OutPutRichText,			//13
	enum_RepreFun_OutPutText,				//14
	enum_RepreFun_GetImgFrameParam,			//15
	enum_RepreFun_ViewPortCoordToSpaceCoord,//16
	enum_RepreFun_LookAt,					//17
	enum_RepreFun_GetImageParam,			//18
	enum_RepreFun_GetJpgImage,				//19
	enum_RepreFun_SetOption,				//Zoom out, Zoom in,20
	enum_RepreFun_SetLightInfo,				//21

	//---------------------------------------声音的录制类别----------------------
	enum_SoundFun_PlayASound,				//22
	enum_SoundFun_StopASound,				//23
	enum_SoundFun_PlayMusic,				//24
	enum_SoundFun_StopMusic,				//25

	enum_RepreFun_FuncNum,
};

enum ENUM_JXR_STATE
{
	enum_Replay_Init = 0,
	enum_Replay_Recing,
	enum_Replay_Saving,
	enum_Replay_Openning,
	enum_Replay_PreDraw,
	enum_Replay_Playing,
	enum_Replay_PauseRec,

	enum_Replay_StateNum,
};


struct IRepresentShell;
class  ISoundShell;
class IJXReplay
{
public:
	//有序的记录各绘制接口函数的参数。
	virtual VOID RECjxr(INT nFrame, ENUM_FUNC_TYPE eFuncType, LPSTR cFormat, ...) = 0;
	//开始录像录制
	virtual VOID StartREC(LPSTR szProducer, DWORD dwTime) = 0;
	//暂停录制
	virtual VOID PauseREC() = 0;
	//停止录像录制，录像结束
	virtual VOID EndREC(LPCSTR pFilePath) = 0;		
	//加载录像
	virtual bool Loadjxr(LPCSTR pFilePath) = 0;
	//播放录像
	virtual bool Playjxr() = 0;
	//文件大小
	virtual size_t RepTotalSize() = 0;
	//当前读取的大小
	virtual size_t CurReadSize() = 0;
	//停止播放
	virtual VOID Stop() = 0;
	//暂停
	virtual VOID Pause() = 0;
	//继续
	virtual VOID GoOn() = 0;
	//加速
	virtual VOID SpeedUp() = 0;
	//减速
	virtual VOID SlowDown() = 0;
	//返回当前状态
	virtual INT	 GetCurState() = 0;
	//设置记录初始化环境变量
	virtual VOID SetInitFlag(ENUM_FUNC_TYPE eFuncType, BOOL bFlag) = 0;
	//设置绘制接口
	virtual VOID SetRepresent(IRepresentShell* pRepresent) = 0;
	//设置声音接口
	virtual VOID SetSoundShell(ISoundShell* pSoundShell) = 0;

	//释放资源
	virtual VOID Release() = 0;
	//设置当前状态
	virtual VOID SetCurState(ENUM_JXR_STATE eState) = 0;
	//释放录像内容
	virtual VOID ReleaseRecData() = 0;
	//录像超过时间
	virtual BOOL IsRecOverTime() = 0;
	//是否重播时暂停
	virtual BOOL IsReplayPaused() = 0;
	//设置分辨率a/b/c
	virtual VOID SetDisplayMode(char chMode) = 0;
	// 设置加载显示字符串
	virtual VOID SetReplayLoadStr(LPCTSTR pszStr) = 0;
};

typedef IJXReplay*				(*fnCreateReplayer)();
extern "C" JXREPLAY_API IJXReplay* CreateJxReplayInterface();
#define	FN_CREATE_JXREPLAY		"CreateJxReplayInterface"

#endif






















