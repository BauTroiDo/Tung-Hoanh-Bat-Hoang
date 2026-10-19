#ifndef KIndexNodeH
#define	KIndexNodeH

//class KIndexNode : public KNode
//{
//public:
//	int		m_nIndex;
//	int		m_Ref;
//public:
//	KIndexNode() { m_nIndex = NULL; m_Ref = 0; };
//	void	AddRef() { m_Ref++; };
//	void	Release() {
//		 if (m_Ref>0)
//		    m_Ref--; 
//		 else
//			m_Ref=0;
//
//	};
//};

class KIndexNode : public KNode
{
public:
	INT		m_nIndex;
	INT		m_Ref;
public:
	KIndexNode() { m_nIndex = 0; m_Ref = 0; };
	VOID	AddRef() { m_Ref++; };
	//VOID	Release() { ASSERT(m_Ref > 0); m_Ref--; };
	VOID	Release() {
		if (m_Ref>0)
			m_Ref--; 
		else
			m_Ref=0;
	};
};

#endif
