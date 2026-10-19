//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KFile.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Win32 File Operation Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KFilePath.h"
#include "KFile.h"
#ifndef WIN32
#include "zport.h"
#endif
//---------------------------------------------------------------------------
// 函数:	KFile
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KFile::KFile()
{
	m_hFile	= NULL;
	m_dwLen	= 0;
	m_dwPos	= 0;
}
//---------------------------------------------------------------------------
// 函数:	~KFile
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KFile::~KFile()
{
	Close();
}
//---------------------------------------------------------------------------
// 函数:	Open
// 功能:	打开一个文件，准备读取
// 参数:	FileName	文件名
// 返回:	成功返回TRUE，失败返回FALSE。
//---------------------------------------------------------------------------
#ifdef WIN32
static FILE* KFile_FopenWithFallback(const char* PathName, const char* mode, const wchar_t* wmode)
{
	FILE* fp = fopen(PathName, mode);
	if (!fp)
	{
		wchar_t wszPath[MAX_PATH * 2] = {0};
		if (MultiByteToWideChar(936, 0, PathName, -1, wszPath, MAX_PATH * 2) > 0)
		{
			fp = _wfopen(wszPath, wmode);
		}
		if (!fp && MultiByteToWideChar(CP_UTF8, 0, PathName, -1, wszPath, MAX_PATH * 2) > 0)
		{
			fp = _wfopen(wszPath, wmode);
		}
	}
	return fp;
}
#endif

BOOL KFile::Open(LPSTR FileName)
{
	char PathName[MAXPATH];
	
	// close prior file handle
	//if (m_hFile != INVALID_HANDLE_VALUE)
	//	Close();

	if (m_hFile != NULL)
		Close();

	// get full path name
	g_GetFullPath(PathName, FileName);
/*#ifndef WIN32
	char *name_ptr = PathName;
	while(*name_ptr) {
		if(*name_ptr == '\\') *name_ptr = '/';
		name_ptr++;
	}
#endif
	// Open the file for read
	m_hFile = CreateFile(
		PathName,		// pointer to name of the file with path
		GENERIC_READ,	// access (read-write) mode
		FILE_SHARE_READ,// share mode
		NULL,			// pointer to security attributes
		OPEN_EXISTING,	// how to create
		FILE_ATTRIBUTE_NORMAL,// file attributes
		NULL);			// template file*/
#ifndef WIN32
        char *ptr = PathName;
        while(*ptr) {
          if(*ptr == '\\') *ptr = '/';
          ptr++;
        }

		//open lower case file for linux
		char lcasePathName[MAXPATH];
		char szRootPath[MAXPATH];
		g_GetRootPath(szRootPath);
		strcpy(lcasePathName, PathName);
		if (memcmp(lcasePathName, szRootPath, strlen(szRootPath)) == 0)
			strlwr(lcasePathName + strlen(szRootPath));
		else
			strlwr(lcasePathName);
		if (NULL == (m_hFile = fopen(lcasePathName, "rb")))
#endif
#ifdef WIN32
	m_hFile = KFile_FopenWithFallback(PathName, "rb", L"rb");
#else
	m_hFile = fopen(PathName, "rb");
#endif

	// check file handle
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return FALSE;

	if (m_hFile == NULL)
	{
		return FALSE;
	}
	
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Create
// 功能:	创建一个文件，准备写入。
// 参数:	FileName	文件名
// 返回:	成功返回TRUE，失败返回FALSE。
//---------------------------------------------------------------------------
BOOL KFile::Create(LPSTR FileName)
{
	char PathName[MAXPATH];
	
	// close prior file handle
	//if (m_hFile != INVALID_HANDLE_VALUE)
	//	Close();

	if (m_hFile != NULL) //如果存在就关闭
		Close();
	
	// get full path name
	g_GetFullPath(PathName, FileName);

	// change file attribute for write
//	SetFileAttributes(PathName, FILE_ATTRIBUTE_NORMAL);

	// create file for write
/*	m_hFile = CreateFile(
		PathName,		// pointer to name of the file with path
		GENERIC_WRITE,	// access (read-write) mode
		FILE_SHARE_READ,// share mode
		NULL,			// pointer to security attributes
		CREATE_ALWAYS,	// create or over write
		FILE_ATTRIBUTE_NORMAL, // file attributes
		NULL);			// template file
*/	
#ifdef WIN32
	m_hFile = KFile_FopenWithFallback(PathName, "wb+", L"wb+");
#else
	m_hFile = fopen(PathName, "wb+");
#endif
	// check file handle
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return FALSE;

	if (m_hFile == NULL)
		return FALSE;
	return TRUE;
}

//---------------------------------------------------------------------------
// 函数:	Append
// 功能:	打开一个文件，准备在后部添加数据，如果此文件不存在，则产生这个文件。
// 参数:	FileName	文件名
// 返回:	成功返回TRUE，失败返回FALSE。
//---------------------------------------------------------------------------
BOOL KFile::Append(LPSTR FileName)
{
	char PathName[MAXPATH];
	
	// close prior file handle
	//if (m_hFile != INVALID_HANDLE_VALUE)
	//	Close();

	if (m_hFile != NULL)
		Close();
	
	// get full path name
	g_GetFullPath(PathName, FileName);

	// change file attribute for write
//	SetFileAttributes(PathName, FILE_ATTRIBUTE_NORMAL);

	// create file for write
/*	m_hFile = CreateFile(
		PathName,		// pointer to name of the file with path
		GENERIC_WRITE,	// access (read-write) mode
		FILE_SHARE_READ,// share mode
		NULL,			// pointer to security attributes
		OPEN_ALWAYS,	// Opens the file, if it exists. If the file does not exist, the function creates the file as if dwCreationDisposition were CREATE_NEW
		FILE_ATTRIBUTE_NORMAL, // file attributes
		NULL);			// template file*/
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return FALSE;
	
	// check file handle
#ifdef WIN32
	m_hFile = KFile_FopenWithFallback(PathName, "ab", L"ab");
#else
	m_hFile = fopen(PathName, "ab");
#endif
	if (m_hFile == NULL)
		return FALSE;

	Seek(0, FILE_END);
	return TRUE;
}

//---------------------------------------------------------------------------
// 函数:	Close
// 功能:	关闭打开的文件
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KFile::Close()
{
	//if (m_hFile != INVALID_HANDLE_VALUE)
		//CloseHandle(m_hFile);

	if (m_hFile)
		fclose((FILE*)m_hFile);
	
	//m_hFile	= (FILE *)INVALID_HANDLE_VALUE;
	m_hFile	= NULL;
	m_dwLen	= 0;
	m_dwPos	= 0;
}
//---------------------------------------------------------------------------
// 函数:	Read
// 功能:	读取文件数据
// 参数:	lpBuffer	读取数据存放的内存区域
//			dwReadBytes	读取数据的字节数
// 返回:	成功返回读取的字节数，失败返回0。
//---------------------------------------------------------------------------
DWORD KFile::Read(LPVOID lpBuffer, DWORD dwReadBytes)
{
	DWORD dwBytesRead;
	
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return 0;
	if (m_hFile == NULL)
		return 0;
	
//	ReadFile(m_hFile, lpBuffer, dwReadBytes, &dwBytesRead, NULL);
	dwBytesRead = fread(lpBuffer, 1, dwReadBytes, m_hFile);	
	m_dwPos += dwBytesRead;
	
	return dwBytesRead;
}
//---------------------------------------------------------------------------
// 函数:	write
// 功能:	写入文件数据
// 参数:	lpBuffer		写入数据存放的内存区域
//			dwWriteBytes	写入数据的字节数
// 返回:	成功返回写入的字节数，失败返回0。
//---------------------------------------------------------------------------
DWORD KFile::Write(LPVOID lpBuffer, DWORD dwWriteBytes)
{
	DWORD dwBytesWrite;
	
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return 0;

	if (m_hFile == NULL)
		return 0;

	dwBytesWrite = fwrite(lpBuffer, 1, dwWriteBytes, m_hFile);	
//	WriteFile(m_hFile, lpBuffer, dwWriteBytes, &dwBytesWrite, NULL);
	
	m_dwPos += dwBytesWrite;
	
	return dwBytesWrite;
}
//---------------------------------------------------------------------------
// 函数:	Seek
// 功能:	移动文件指针位置
// 参数:	lDistance		移动长度
//			dwMoveMethod	移动方法＝FILE_BEGIN，FILE_CURRENT，FILE_END
// 返回:	成功返回指针位置，失败返回SEEK_ERROR。
//---------------------------------------------------------------------------
DWORD KFile::Seek(LONG lDistance, DWORD dwMoveMethod)
{
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return SEEK_ERROR;

	if (m_hFile == NULL)
		return SEEK_ERROR;

//	m_dwPos = SetFilePointer(m_hFile, lDistance, NULL, dwMoveMethod);
	// the move has a return value and it was thrown away: on failure the C runtime leaves
	// the stream exactly where it was, and the count on the next line reported that OLD
	// position as though the move had happened.  SEEK_ERROR - the failure value this
	// function's own header promises - was therefore unreachable by any caller.
	if (0 != _fseeki64(m_hFile, (long long)lDistance, dwMoveMethod))
		return SEEK_ERROR;
	// `long' and `DWORD' are both 4 bytes at x86 and at x64 here, so the count and the box
	// share one ceiling and the -1 this call answers on failure would land in `m_dwPos' as
	// 4294967295.  A position the DWORD cannot report is refused, and the stream is wound
	// back to the one position that always fits so the next read is not left wherever the
	// runtime put it.
	long long llPos = _ftelli64(m_hFile);
	if (llPos < 0 || llPos > (long long)0xFFFFFFFF)
	{
		// the wind-back has an answer of its own, and the position this class caches has to
		// carry it: if the rewind did not happen the stream is not at 0, it is wherever the
		// oversized move left it, and `Tell' would go on reporting a 0 that nobody measured.
		// `SEEK_ERROR' is the word this class already uses for an unconfirmed position.
		if (0 != _fseeki64(m_hFile, 0, SEEK_SET))
		{
			m_dwPos = SEEK_ERROR;
			return SEEK_ERROR;
		}
		m_dwPos = 0;
		return SEEK_ERROR;
	}
	m_dwPos = (DWORD)llPos;
	return m_dwPos;
}
//---------------------------------------------------------------------------
// 函数:	Tell
// 功能:	取得文件指针位置
// 参数:	void
// 返回:	成功返回指针位置，失败返回SEEK_ERROR。
//---------------------------------------------------------------------------
DWORD KFile::Tell()
{
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return SEEK_ERROR;

	if (m_hFile == NULL)
		return SEEK_ERROR;

	return m_dwPos;
}
//---------------------------------------------------------------------------
// 函数:	Size
// 功能:	取得文件长度
// 参数:	void
// 返回:	成功返回文件长度，失败返回0。
//---------------------------------------------------------------------------
DWORD KFile::Size()
{
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return 0;

	if (m_hFile == NULL)
		return 0;

	if (m_dwLen == 0) {
		DWORD temp = m_dwPos;
		DWORD dwEnd = Seek(0, FILE_END);
		Seek(temp, FILE_BEGIN);
		// a length that could not be measured must not be cached: `m_dwLen' is read back as a
		// size by whoever asked, and 4294967295 is an allocation or a copy length somebody will
		// try to honour.  An unmeasured file answers 0, which is this function's documented
		// failure value.
		if (SEEK_ERROR == dwEnd)
			return 0;
		m_dwLen = dwEnd;
		//m_dwLen = GetFileSize(m_hFile, NULL);
	}

	return m_dwLen;
}
//---------------------------------------------------------------------------
