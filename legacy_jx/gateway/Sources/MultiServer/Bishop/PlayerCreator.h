#ifndef CPlayerCreatorH
#define	CPlayerCreatorH

#include "S3DBInterface.h"

#include <map>
#include <vector>
#include <string>
#include <list>
using namespace std;

class CPlayerCreator
{
public:
	
#define	MAX_PLAYERTYPE_VALUE	10
#define	MAX_NEWPLAYER_BUFFER	1024  //新手的存档包初始化大小
#define	PLAYERCREATOR_FILE		"settings\\player\\newplayerini%02d.ini"
#define REVIVALID_FILENAME		"settings\\player\\revivalid.ini"   //出生点
#define	PLAYERNAME_FILE			"settings\\player\\UTF8.ini"
#define	PLAYERINVALIDNAME_FILE			"settings\\player\\InvalidName.ini"

	typedef struct tagRoleGeneratorParam
	{
		char	szAccName[ACC_LENGTH];
		char	szName[NAME_LENGHT];
		int		nSeries;
		int		nSex;
		int		nMapID;
//		int     nRevID;
	} ROLEPARAM, NEAR *PROLEPARAM, FAR *LPROLEPARAM;

private:
	std::list<std::string> m_InvalidName;
	std::list<std::string> m_UTF8Code;
	BYTE*		m_pRoleData[MAX_PLAYERTYPE_VALUE];
	
	typedef std::vector< unsigned >					stdRevivalID;
	typedef std::map< size_t, stdRevivalID >		stdMapID2RID;

	stdMapID2RID			m_theMapID2RID;

public:

	CPlayerCreator();
	~CPlayerCreator();

	enum enumSearchType
	{
		enumRandom //随机？
	};
	
	const TRoleData*	GetRoleData( unsigned int &uDataLength, LPROLEPARAM lpParam ,unsigned int nIsSerVer=1);
	unsigned			GetRevivalID(UINT nMapID, UINT nType = enumRandom );
	int					CountUTF8Character(unsigned char* bytes);
	std::string			GetUTF8CharacterHex(unsigned char* bytes, int idx);
	bool				CheckNameValid(unsigned char* bytes);

	int					CountTCVN3Characters(const char* str);
	bool				IsTCVN3NameValid(const char* str);

private:

	bool				Init();

	bool				GetRoleDataFromIni( BYTE* pData, const char* szFileName );

};
#endif