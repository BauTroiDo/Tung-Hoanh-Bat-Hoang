#ifndef KDROPSCRIPT_H
#define KDROPSCRIPT_H

#include "KCore.h"
#include "KLuaScript.h"
//排序二叉树，保存经过文件名转数字的脚本的id,并实现快速查找对应脚本

#include "KBinTreeNode.h"
#include "KBinsTree.h"

#ifdef _SERVER


class KDropScriptNode
{
	//char m_szFileName[128];
public:
	KItemDropRate * m_pItemDropRate;
	char m_szFileName[128];

	KDropScriptNode()
	{//构造函数	开始时使用 初始化个变量
		m_pItemDropRate = NULL;
	}
	~KDropScriptNode()
	{//析构函数 退出时调用 释放资源
		//	if (m_pItemDropRate)
		//		delete m_pItemDropRate;
		//	m_pItemDropRate = NULL;
	}
	
};

typedef	BinSTree<KDropScriptNode> KDropBinTree;
extern KDropBinTree g_DropBinTree;

#endif

#endif


