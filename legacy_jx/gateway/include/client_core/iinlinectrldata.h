/*----------------------------------------------------------------------------------------
*	Copyright (c) 2005 珠海.金山.西山居工作室
*	All right reserved.
*
*	Filename	:   IInlineCtrlType.h
*	Description :	嵌入式窗口控件所带的数据接口
*	Current Version:	1.0
*	Author	:	ChenWeiMing(chenweiming@kingsoft.net)
*	CreateTime:	2005-9-15
------------------------------------------------------------------------------------------*/
#ifndef _iinlinectrldata_h_
#define _iinlinectrldata_h_

// 补全指针类型定义
//typedef const BYTE*		LPCBYTE;
typedef CHAR*			LPCHAR;
typedef CONST CHAR*		LPCCHAR;
typedef UCHAR*			LPUCHAR;
typedef CONST UCHAR*	LPCUCHAR;
typedef SHORT*			LPSHORT;
typedef CONST SHORT*	LPCSHORT;
typedef USHORT*			LPUSHORT;
typedef CONST USHORT*	LPCUSHORT;
typedef CONST WORD*		LPCWORD;
typedef CONST INT*		LPCINT;
typedef UINT*			LPUINT;
typedef CONST UINT*		LPCUINT;
typedef CONST DWORD*	LPCDWORD;
typedef INT64*			LPINT64;
typedef CONST INT64*	LPCINT64;
typedef CONST BOOL*		LPCBOOL;
typedef FLOAT*			LPFLOAT;
typedef CONST FLOAT*	LPCFLOAT;
typedef DOUBLE*			LPDOUBLE;
typedef CONST DOUBLE*	LPCDOUBLE;

namespace KITEMLINK
{
	enum KE_ITEMLINK_GENPARAM
	{
		emKTYPE,
		emKGENRE,
		emKDETAIL,
		emKPARTICULAR,
		emKLEVEL,
		emKSERIES,
		emKENHANCE,
		emKLUCKY,
		emKVERSION,
		emKRANDSEED,
		emKCOUNT,
		emKCURDUR,
		emKMAXDUR,
		emKBIND,
		emKPARAM0,
		emKPARAM1,
		emKPARAM2,
		emKPARAM3,
		emKPARAM4,
		emKPARAM5,
		emKPARAM6,
		emKPARAM7,
		emKPARAM8,
		emKPARAM9,
		emKPARAM10,
		emKPARAM11,
		emKPARAM12,
		emKPARAM13,
		emKPARAM14,
		emKPARAM15,
		emKPARAM16,
		emKPARAM17,
		emKVERIFY,
		emKNUM,
	};

	enum KE_ITEMLINK_AUCTION
	{
		emKSALEKEY = emKNUM,
		emKSELLERNAME,
		emKSALEPRICE,
		emKSALENUM,
	};
}



enum CONCTROL_DATA_TYPE
{
	enum_item = 0,
	enum_itemex = 1,
	conctrol_data_type_num,
};

struct IInlineCtrlData
{
   virtual  INT		TextToData(LPWSTR pStr, INT nStrLen) = 0; 
   virtual 	INT		DataToText(LPWSTR pStr, INT nStrLen) = 0;
	
   virtual 	INT		GetData(SHORT wDataPos) = 0;
   virtual 	BOOL	SetData(SHORT wDataPos, INT nData) = 0;
   virtual  INT		CalcVerifyCode() = 0;
   virtual	INT		GetDataCount() = 0;
};

#endif
