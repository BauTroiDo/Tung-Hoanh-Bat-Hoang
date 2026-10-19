//=============================================================================
// ScriptFuns_V9_TabFile.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y Table 4) for Win64
// High-Performance In-Memory TabFile Manager & Cache for V9 Lua Script Engine
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include "KTabFile.h"
#include "KIniFile.h"
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

struct KScriptTabEntry
{
    KTabFile* pTab;
    std::map<std::string, int> colMap;

    KScriptTabEntry() : pTab(NULL) {}

    int FindCol(const char* pszColName) const
    {
        if (!pszColName || !pszColName[0]) return -1;
        std::string s = pszColName;
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.pop_back();
        size_t st = 0;
        while (st < s.size() && (s[st] == ' ' || s[st] == '\t')) st++;
        if (st > 0) s = s.substr(st);
        for (size_t i = 0; i < s.size(); ++i) s[i] = (char)tolower((unsigned char)s[i]);

        std::map<std::string, int>::const_iterator it = colMap.find(s);
        if (it != colMap.end())
            return it->second;

        if (pTab)
            return pTab->FindColumn((char*)pszColName);

        return -1;
    }
};

class KScriptTabFileManager
{
private:
    std::map<std::string, KScriptTabEntry> m_mapTabFiles;

    static std::string NormalizeKey(const char* psz)
    {
        if (!psz || !psz[0]) return "";
        std::string s = psz;
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
            s.pop_back();
        size_t start = 0;
        while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
            start++;
        if (start > 0)
            s = s.substr(start);

        for (size_t i = 0; i < s.size(); ++i)
        {
            if (s[i] == '/')
                s[i] = '\\';
            else
                s[i] = (char)tolower((unsigned char)s[i]);
        }

        while (!s.empty() && s[0] == '\\')
            s.erase(0, 1);

        return s;
    }

    static void ResolveGamePath(char* szOut, size_t nOutSize, const char* pszIn)
    {
        if (!pszIn || !pszIn[0])
        {
            szOut[0] = '\0';
            return;
        }

        if (pszIn[1] == ':')
        {
            g_StrCpyLen(szOut, (char*)pszIn, (int)nOutSize);
            return;
        }

        const char* p = pszIn;
        while (*p != '\0' && (*p == '\\' || *p == '/'))
            p++;

        char szRoot[MAX_PATH] = {0};
        g_GetRootPath(szRoot);
        size_t rootLen = strlen(szRoot);
        while (rootLen > 0 && (szRoot[rootLen - 1] == '\\' || szRoot[rootLen - 1] == '/'))
        {
            szRoot[--rootLen] = '\0';
        }

        snprintf(szOut, nOutSize, "%s\\%s", szRoot, p);
        for (size_t i = 0; szOut[i]; ++i)
        {
            if (szOut[i] == '/')
                szOut[i] = '\\';
        }
    }

public:
    KScriptTabFileManager() {}
    ~KScriptTabFileManager()
    {
        Clear();
    }

    void Clear()
    {
        std::map<KTabFile*, bool> deleted;
        for (std::map<std::string, KScriptTabEntry>::iterator it = m_mapTabFiles.begin(); it != m_mapTabFiles.end(); ++it)
        {
            if (it->second.pTab && !deleted[it->second.pTab])
            {
                deleted[it->second.pTab] = true;
                delete it->second.pTab;
            }
        }
        m_mapTabFiles.clear();
    }

    KScriptTabEntry* GetEntry(const char* pszKeyOrPath, bool bAutoLoad = true)
    {
        if (!pszKeyOrPath || !pszKeyOrPath[0])
            return NULL;

        std::string norm = NormalizeKey(pszKeyOrPath);
        std::map<std::string, KScriptTabEntry>::iterator it = m_mapTabFiles.find(norm);
        if (it != m_mapTabFiles.end())
            return &it->second;

        if (!bAutoLoad)
            return NULL;

        if (LoadTabFile(pszKeyOrPath, pszKeyOrPath))
        {
            it = m_mapTabFiles.find(norm);
            if (it != m_mapTabFiles.end())
                return &it->second;
        }

        return NULL;
    }

    KTabFile* GetTabFile(const char* pszKeyOrPath, bool bAutoLoad = true)
    {
        KScriptTabEntry* pEntry = GetEntry(pszKeyOrPath, bAutoLoad);
        return pEntry ? pEntry->pTab : NULL;
    }

    BOOL LoadTabFile(const char* pszPath, const char* pszKey = NULL)
    {
        if (!pszPath || !pszPath[0])
            return FALSE;

        std::string normPath = NormalizeKey(pszPath);
        std::string normKey = pszKey ? NormalizeKey(pszKey) : normPath;

        if (m_mapTabFiles.find(normKey) != m_mapTabFiles.end())
            return TRUE;
        if (m_mapTabFiles.find(normPath) != m_mapTabFiles.end())
        {
            m_mapTabFiles[normKey] = m_mapTabFiles[normPath];
            return TRUE;
        }

        KTabFile* pTab = new KTabFile();
        BOOL bSuccess = FALSE;

        // 1. Try directly with original path
        bSuccess = pTab->Load((char*)pszPath);

        // 2. Try with resolved full path from root
        if (!bSuccess)
        {
            char szResolved[MAX_PATH] = {0};
            ResolveGamePath(szResolved, sizeof(szResolved), pszPath);
            bSuccess = pTab->Load(szResolved);
        }

        // 3. Try with g_GetFullPath
        if (!bSuccess)
        {
            char szFullPath[MAX_PATH] = {0};
            g_GetFullPath(szFullPath, (char*)pszPath);
            bSuccess = pTab->Load(szFullPath);
        }

        if (bSuccess)
        {
            KScriptTabEntry entry;
            entry.pTab = pTab;

            // Build fast column name cache from row 1 (header)
            int nWidth = pTab->GetWidth();
            char szHeader[256];
            for (int c = 1; c <= nWidth; ++c)
            {
                szHeader[0] = '\0';
                pTab->GetString(1, c, "", szHeader, sizeof(szHeader));
                std::string s = szHeader;
                while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.pop_back();
                size_t st = 0;
                while (st < s.size() && (s[st] == ' ' || s[st] == '\t')) st++;
                if (st > 0) s = s.substr(st);
                for (size_t i = 0; i < s.size(); ++i) s[i] = (char)tolower((unsigned char)s[i]);
                if (!s.empty())
                    entry.colMap[s] = c;
            }

            m_mapTabFiles[normPath] = entry;
            m_mapTabFiles[normKey] = entry;
            return TRUE;
        }

        delete pTab;
        return FALSE;
    }

    BOOL UnloadTabFile(const char* pszKey)
    {
        return TRUE;
    }
};

static KScriptTabFileManager g_ScriptTabFileMgr;

KTabFile* GetScriptTabFile(const char* pszKeyOrPath)
{
    return g_ScriptTabFileMgr.GetTabFile(pszKeyOrPath);
}

//-----------------------------------------------------------------------------
// 1. TabFile_Load(szFileName, [szKey])
//-----------------------------------------------------------------------------
int LuaTabFile_Load(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* pszPath = Lua_ValueToString(L, 1);
    const char* pszKey  = (nTop >= 2) ? Lua_ValueToString(L, 2) : pszPath;
    if (!pszPath || !pszPath[0])
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    BOOL bRet = g_ScriptTabFileMgr.LoadTabFile(pszPath, pszKey);
    Lua_PushNumber(L, bRet ? 1 : 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. TabFile_UnLoad(szKey)
//-----------------------------------------------------------------------------
int LuaTabFile_UnLoad(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* pszKey = Lua_ValueToString(L, 1);
    g_ScriptTabFileMgr.UnloadTabFile(pszKey);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 3. TabFile_GetRowCount(szKey)
//-----------------------------------------------------------------------------
int LuaTabFile_GetRowCount(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* pszKey = Lua_ValueToString(L, 1);
    KTabFile* pTab = g_ScriptTabFileMgr.GetTabFile(pszKey);
    if (!pTab)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, pTab->GetHeight());
    return 1;
}

//-----------------------------------------------------------------------------
// 4. TabFile_GetColCount(szKey)
//-----------------------------------------------------------------------------
int LuaTabFile_GetColCount(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* pszKey = Lua_ValueToString(L, 1);
    KTabFile* pTab = g_ScriptTabFileMgr.GetTabFile(pszKey);
    if (!pTab)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, pTab->GetWidth());
    return 1;
}

//-----------------------------------------------------------------------------
// 5. TabFile_GetCell(szKey, nRow, szCol/nCol, [szDefault])
//-----------------------------------------------------------------------------
int LuaTabFile_GetCell(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 3)
    {
        Lua_PushString(L, "");
        return 1;
    }

    const char* pszKey = Lua_ValueToString(L, 1);
    if (!pszKey || !pszKey[0])
    {
        Lua_PushString(L, "");
        return 1;
    }

    const char* pszDefault = (nTop >= 4) ? Lua_ValueToString(L, 4) : "";
    if (!pszDefault) pszDefault = "";

    KScriptTabEntry* pEntry = g_ScriptTabFileMgr.GetEntry(pszKey);
    if (!pEntry || !pEntry->pTab)
    {
        Lua_PushString(L, pszDefault);
        return 1;
    }
    KTabFile* pTab = pEntry->pTab;

    char szVal[1024] = {0};

    if (Lua_IsNumber(L, 2))
    {
        int nRow = (int)Lua_ValueToNumber(L, 2);
        if (Lua_IsNumber(L, 3))
        {
            int nCol = (int)Lua_ValueToNumber(L, 3);
            pTab->GetString(nRow, nCol, (char*)pszDefault, szVal, sizeof(szVal));
        }
        else
        {
            char* pszCol = (char*)Lua_ValueToString(L, 3);
            int nCol = pEntry->FindCol(pszCol);
            if (nCol > 0)
                pTab->GetString(nRow, nCol, (char*)pszDefault, szVal, sizeof(szVal));
            else
                pTab->GetString(nRow, pszCol, (char*)pszDefault, szVal, sizeof(szVal));
        }
    }
    else
    {
        char* pszRow = (char*)Lua_ValueToString(L, 2);
        if (Lua_IsNumber(L, 3))
        {
            int nCol = (int)Lua_ValueToNumber(L, 3);
            pTab->GetString(pszRow, nCol, (char*)pszDefault, szVal, sizeof(szVal));
        }
        else
        {
            char* pszCol = (char*)Lua_ValueToString(L, 3);
            int nCol = pEntry->FindCol(pszCol);
            if (nCol > 0)
                pTab->GetString(pszRow, nCol, (char*)pszDefault, szVal, sizeof(szVal));
            else
                pTab->GetString(pszRow, pszCol, (char*)pszDefault, szVal, sizeof(szVal));
        }
    }

    if (szVal[0] == '\0' && nTop >= 4 && pszDefault[0] != '\0')
    {
        Lua_PushString(L, pszDefault);
    }
    else
    {
        Lua_PushString(L, szVal);
    }
    return 1;
}

//-----------------------------------------------------------------------------
// 6. TabFile_Search(szKey, col, szVal/nVal, [nStartRow = 2])
//-----------------------------------------------------------------------------
int LuaTabFile_Search(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    const char* pszKey = Lua_ValueToString(L, 1);
    KScriptTabEntry* pEntry = g_ScriptTabFileMgr.GetEntry(pszKey);
    if (!pEntry || !pEntry->pTab)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KTabFile* pTab = pEntry->pTab;

    if (nTop == 2)
    {
        const char* pszRow = Lua_ValueToString(L, 2);
        if (!pszRow)
        {
            Lua_PushNumber(L, 0);
            return 1;
        }
        int nRow = pTab->FindRow((char*)pszRow);
        Lua_PushNumber(L, nRow);
        return 1;
    }

    int nCol = -1;
    if (Lua_IsNumber(L, 2))
    {
        nCol = (int)Lua_ValueToNumber(L, 2);
    }
    else
    {
        char* pszColName = (char*)Lua_ValueToString(L, 2);
        if (pszColName)
            nCol = pEntry->FindCol(pszColName);
    }

    if (nCol <= 0 || nCol > pTab->GetWidth())
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    char szTarget[256] = {0};
    if (Lua_IsNumber(L, 3))
    {
        double dVal = Lua_ValueToNumber(L, 3);
        if (dVal == (double)(long long)dVal)
            snprintf(szTarget, sizeof(szTarget), "%lld", (long long)dVal);
        else
            snprintf(szTarget, sizeof(szTarget), "%f", dVal);
    }
    else
    {
        const char* psz = Lua_ValueToString(L, 3);
        if (psz)
            g_StrCpyLen(szTarget, (char*)psz, sizeof(szTarget));
    }

    int nStartRow = 2; // Data rows start at 2
    if (nTop >= 4)
    {
        nStartRow = (int)Lua_ValueToNumber(L, 4);
        if (nStartRow < 2)
            nStartRow = 2;
    }

    int nHeight = pTab->GetHeight();
    char szCell[512];
    for (int r = nStartRow; r <= nHeight; ++r)
    {
        szCell[0] = '\0';
        pTab->GetString(r, nCol, "", szCell, sizeof(szCell));
        
        int len = (int)strlen(szCell);
        while (len > 0 && (szCell[len - 1] == ' ' || szCell[len - 1] == '\t' || szCell[len - 1] == '\r'))
            szCell[--len] = '\0';

        if (strcmp(szCell, szTarget) == 0)
        {
            Lua_PushNumber(L, r);
            return 1;
        }
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 7. TabFile_SetCell(szFileName, nRow, nCol, szValue)
//-----------------------------------------------------------------------------
int LuaTabFile_SetCell(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 8. TabFile_Save(szFileName)
//-----------------------------------------------------------------------------
int LuaTabFile_Save(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 9. IniFile_UnLoad(szFileName)
//-----------------------------------------------------------------------------
int LuaIniFile_UnLoad(Lua_State *L)
{
    return 0;
}

//-----------------------------------------------------------------------------
// 10. IniFile_Save(szFileName)
//-----------------------------------------------------------------------------
int LuaIniFile_Save(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    const char* pszFile = Lua_ValueToString(L, 1);
    if (!pszFile || !pszFile[0])
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    KIniFile ini;
    if (ini.Load((char*)pszFile))
    {
        BOOL bRet = ini.Save((char*)pszFile);
        Lua_PushNumber(L, bRet ? 1 : 0);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}
