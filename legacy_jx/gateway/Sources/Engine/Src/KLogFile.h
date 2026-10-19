// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////
#ifndef KCLogFile_H
#define KCLogFile_H

//#include <winbase.h>
#include <string>

class ENGINE_API KCLogFile  
{
public:
	KCLogFile();
	virtual ~KCLogFile();

public:
	BOOL Initialize(const std::string& root, size_t threshold);
	BOOL Uninitialize();

	ULONG BeginBatch();
	ULONG EndBatch();
	BOOL IsBatched();
	void sLogProgram(BOOL bEnter,char *nAppName);
	void gTraceLogFile(LPCTSTR pBuffer, long nLen,char *strDir=NULL);

private:
	enum {
		CH_SEPARATOR	= '\\',
		CH_EXTSPLIT		= '~',
	};

	std::string m_strRoot;
	size_t m_threshold;

	HANDLE m_hFile;
	ULONG m_lBatch;

private:
	BOOL PrepareFile();
	static BOOL OpenUniDirectory(const std::string& dirname, std::string* pNewdir);
	static HANDLE CreateUniFile(const std::string& filename, std::string* pNewfile);
	int KCLogFile::gTimeStamp(char* sDest, char* pTail);
	size_t TraceLog(const void* pData, size_t size);
};
extern ENGINE_API KCLogFile* g_pCLogFile;
#endif 
