	TITLE	D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\KMissle.cpp
	.386P
include listing.inc
if @Version gt 510
.model FLAT
else
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT PARA USE32 PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT BYTE USE32 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT BYTE USE32 'DEBTYP'
$$TYPES	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS
;	COMDAT ??_C@_00A@?$AA@
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CRT$XCA	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCA	ENDS
CRT$XCU	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCU	ENDS
CRT$XCL	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCL	ENDS
CRT$XCC	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCC	ENDS
CRT$XCZ	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCZ	ENDS
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??4KMapMusic@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetGameTime@KSubWorldSet@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetGameVersion@KSubWorldSet@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetPing@KSubWorldSet@@QAEXK@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetPing@KSubWorldSet@@QAEKXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??4KSubWorldSet@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KSubWorldSet@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?g_DirSin@@YAHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?g_DirCos@@YAHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?g_GetDistance@@YAHHHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?g_GetDirIndex@@YAHHHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?g_Dir2DirIndex@@YAHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?g_DirIndex2Dir@@YAHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KSkillSpecialNode@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KSkillSpecialNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKSkillSpecialNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _wmemchr
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _wmemcmp
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _wmemcpy
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _wmemmove
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _wmemset
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1invalid_argument@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Doraise@invalid_argument@std@@MBEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_Ginvalid_argument@std@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0invalid_argument@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetMoney@KInventory@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetMoney@KInventory@@QAEHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetXu@KInventory@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetXu@KInventory@@QAEHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??4KInventory@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Release@CChatFriend@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GCChatFriend@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1CChatFriend@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GCChatApplyListNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1CChatApplyListNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Release@KTeamApplyList@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?NumOfEntries@KBasicPropertyTable@@QBEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetEquipment@KItemList@@QAEHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetMoney@KItemList@@QAEHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetXu@KItemList@@QAEHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Hand@KItemList@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetMaskLock@KItemList@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?IsLockOperation@KItemList@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??4KItemList@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KMissleSet@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??4KMissleSet@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KMissleSet@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?KSG_StringGetInt@@YAHPAPADH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?KSG_StringSkipSymbol@@YA_NPAPADH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKMissle@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?deallocate@?$allocator@D@std@@QAEXPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?allocate@?$allocator@D@std@@QAEPADIPBX@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?max_size@?$allocator@D@std@@QBEIXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??8std@@YA_NABV?$allocator@D@0@0@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Allocate@std@@YAPADHPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKIndexNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKWorldMsgNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KWorldMsgNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetSkillLevelUpScriptId@ISkill@@UBEKXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?IsPhysical@ISkill@@UBEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?IsAura@ISkill@@UBEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GISkill@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1ISkill@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7ISkill@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_GKCacheNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KCacheNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddTail@KList@@QAEXPAVKNode@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?InsertBefore@KNode@@QAEXPAV1@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetNext@KNode@@QAEPAV1@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetHead@KList@@QAEPAVKNode@@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Send@KWorldMsg@@QAEHKHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KWorldMsgNode@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7KWorldMsgNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ?ZAxisMove@KMissle@@AAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?TestBarrier@KMissle@@AAEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?FindNpc@KRegion@@QAEHHHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?IsMatch@KNpc@@QAEHK@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7KMissle@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??1KIndexNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KIndexNode@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7KIndexNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7CChatApplyListNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_7CChatFriend@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __CTA3?AVrange_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVrange_error@std@@@8??0range_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVrange_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVrange_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVunderflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVunderflow_error@std@@@8??0underflow_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVunderflow_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVunderflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVoverflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVoverflow_error@std@@@8??0overflow_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVoverflow_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVoverflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA2?AVruntime_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVruntime_error@std@@@8??0runtime_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVruntime_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI2?AVruntime_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVout_of_range@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVout_of_range@std@@@8??0out_of_range@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVout_of_range@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVout_of_range@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVlength_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVlength_error@std@@@8??0length_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVlength_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVlength_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVinvalid_argument@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVinvalid_argument@std@@@8??0invalid_argument@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVinvalid_argument@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVinvalid_argument@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_7invalid_argument@std@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __CTA3?AVdomain_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVdomain_error@std@@@8??0domain_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVdomain_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI3?AVdomain_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA2?AVlogic_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVlogic_error@std@@@8??0logic_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVlogic_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI2?AVlogic_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA2?AVbad_alloc@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVbad_alloc@std@@@8??0bad_alloc@std@@QAE@ABV01@@Z12
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVbad_alloc@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI2?AVbad_alloc@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA2?AVbad_exception@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVexception@@@8??0exception@@QAE@ABV0@@Z12
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVbad_exception@std@@@8??0bad_exception@std@@QAE@ABV01@@Z12
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVexception@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT ??_R0?AVbad_exception@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI2?AVbad_exception@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_7KSkillSpecialNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??1KNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7KNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??0KNode@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
FLAT	GROUP _DATA, CONST, _BSS, CRT$XCA, CRT$XCU, CRT$XCL, CRT$XCC, CRT$XCZ, xdata$x
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC	?Missle@@3PAVKMissle@@A				; Missle
PUBLIC	?g_MisslesLib@@3PAVKMissle@@A			; g_MisslesLib
PUBLIC	?g_nRandMissleTab@@3PAHA			; g_nRandMissleTab
PUBLIC	?g_CollisionMatrix@@3PAUTCollisionMatrix@@A	; g_CollisionMatrix
_BSS	SEGMENT
?Missle@@3PAVKMissle@@A DB 0166e30H DUP (?)		; Missle
?g_MisslesLib@@3PAVKMissle@@A DB 0166e30H DUP (?)	; g_MisslesLib
?g_nRandMissleTab@@3PAHA DD 064H DUP (?)		; g_nRandMissleTab
_BSS	ENDS
_DATA	SEGMENT
?g_CollisionMatrix@@3PAUTCollisionMatrix@@A DD 00H	; g_CollisionMatrix
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	0ffffffffH
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	00H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	01H
	DD	00H
	DD	01H
	DD	01H
	DD	01H
_DATA	ENDS
CRT$XCU	SEGMENT
_$S15	DD	FLAT:_$E14
_$S20	DD	FLAT:_$E19
CRT$XCU	ENDS
PUBLIC	??4KMapMusic@@QAEAAV0@ABV0@@Z			; KMapMusic::operator=
;	COMDAT ??4KMapMusic@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8
_this$ = -4
??4KMapMusic@@QAEAAV0@ABV0@@Z PROC NEAR			; KMapMusic::operator=, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	push	esi
	push	edi
	mov	DWORD PTR _this$[ebp], ecx
	mov	edi, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR ___that$[ebp]
	mov	ecx, 12351				; 0000303fH
	rep movsd
	mov	eax, DWORD PTR _this$[ebp]
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	4
??4KMapMusic@@QAEAAV0@ABV0@@Z ENDP			; KMapMusic::operator=
_TEXT	ENDS
PUBLIC	?GetGameTime@KSubWorldSet@@QAEHXZ		; KSubWorldSet::GetGameTime
;	COMDAT ?GetGameTime@KSubWorldSet@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?GetGameTime@KSubWorldSet@@QAEHXZ PROC NEAR		; KSubWorldSet::GetGameTime, COMDAT

; 33   : 	int		GetGameTime(){return m_nLoopRate;};

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	esp, ebp
	pop	ebp
	ret	0
?GetGameTime@KSubWorldSet@@QAEHXZ ENDP			; KSubWorldSet::GetGameTime
_TEXT	ENDS
PUBLIC	?GetGameVersion@KSubWorldSet@@QAEHXZ		; KSubWorldSet::GetGameVersion
;	COMDAT ?GetGameVersion@KSubWorldSet@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?GetGameVersion@KSubWorldSet@@QAEHXZ PROC NEAR		; KSubWorldSet::GetGameVersion, COMDAT

; 34   : 	int		GetGameVersion() { return m_nGameVersion; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+49440]
	mov	esp, ebp
	pop	ebp
	ret	0
?GetGameVersion@KSubWorldSet@@QAEHXZ ENDP		; KSubWorldSet::GetGameVersion
_TEXT	ENDS
PUBLIC	?SetPing@KSubWorldSet@@QAEXK@Z			; KSubWorldSet::SetPing
;	COMDAT ?SetPing@KSubWorldSet@@QAEXK@Z
_TEXT	SEGMENT
_dwTimer$ = 8
_this$ = -4
?SetPing@KSubWorldSet@@QAEXK@Z PROC NEAR		; KSubWorldSet::SetPing, COMDAT

; 42   : 	void	SetPing(DWORD dwTimer) { m_dwPing = dwTimer; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _dwTimer$[ebp]
	mov	DWORD PTR [eax+49444], ecx
	mov	esp, ebp
	pop	ebp
	ret	4
?SetPing@KSubWorldSet@@QAEXK@Z ENDP			; KSubWorldSet::SetPing
_TEXT	ENDS
PUBLIC	?GetPing@KSubWorldSet@@QAEKXZ			; KSubWorldSet::GetPing
;	COMDAT ?GetPing@KSubWorldSet@@QAEKXZ
_TEXT	SEGMENT
_this$ = -4
?GetPing@KSubWorldSet@@QAEKXZ PROC NEAR			; KSubWorldSet::GetPing, COMDAT

; 43   : 	DWORD	GetPing() { return m_dwPing; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+49444]
	mov	esp, ebp
	pop	ebp
	ret	0
?GetPing@KSubWorldSet@@QAEKXZ ENDP			; KSubWorldSet::GetPing
_TEXT	ENDS
PUBLIC	??4KSubWorldSet@@QAEAAV0@ABV0@@Z		; KSubWorldSet::operator=
;	COMDAT ??4KSubWorldSet@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8
_this$ = -4
??4KSubWorldSet@@QAEAAV0@ABV0@@Z PROC NEAR		; KSubWorldSet::operator=, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	push	esi
	push	edi
	mov	DWORD PTR _this$[ebp], ecx
	mov	edi, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR ___that$[ebp]
	mov	ecx, 12362				; 0000304aH
	rep movsd
	mov	eax, DWORD PTR _this$[ebp]
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	4
??4KSubWorldSet@@QAEAAV0@ABV0@@Z ENDP			; KSubWorldSet::operator=
_TEXT	ENDS
PUBLIC	??1KSubWorldSet@@QAE@XZ				; KSubWorldSet::~KSubWorldSet
EXTRN	??1KMapMusic@@QAE@XZ:NEAR			; KMapMusic::~KMapMusic
;	COMDAT ??1KSubWorldSet@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??1KSubWorldSet@@QAE@XZ PROC NEAR			; KSubWorldSet::~KSubWorldSet, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KMapMusic@@QAE@XZ			; KMapMusic::~KMapMusic
	mov	esp, ebp
	pop	ebp
	ret	0
??1KSubWorldSet@@QAE@XZ ENDP				; KSubWorldSet::~KSubWorldSet
_TEXT	ENDS
PUBLIC	?GetMoney@KInventory@@QAEHXZ			; KInventory::GetMoney
;	COMDAT ?GetMoney@KInventory@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?GetMoney@KInventory@@QAEHXZ PROC NEAR			; KInventory::GetMoney, COMDAT

; 29   : 	int		GetMoney() { return m_nMoney; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+4]
	mov	esp, ebp
	pop	ebp
	ret	0
?GetMoney@KInventory@@QAEHXZ ENDP			; KInventory::GetMoney
_TEXT	ENDS
PUBLIC	?SetMoney@KInventory@@QAEHH@Z			; KInventory::SetMoney
;	COMDAT ?SetMoney@KInventory@@QAEHH@Z
_TEXT	SEGMENT
_nMoney$ = 8
_this$ = -4
?SetMoney@KInventory@@QAEHH@Z PROC NEAR			; KInventory::SetMoney, COMDAT

; 30   : 	BOOL	SetMoney(int nMoney) { if (nMoney < 0) return FALSE; m_nMoney = nMoney; return TRUE; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	cmp	DWORD PTR _nMoney$[ebp], 0
	jge	SHORT $L102626
	xor	eax, eax
	jmp	SHORT $L102625
$L102626:
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nMoney$[ebp]
	mov	DWORD PTR [eax+4], ecx
	mov	eax, 1
$L102625:
	mov	esp, ebp
	pop	ebp
	ret	4
?SetMoney@KInventory@@QAEHH@Z ENDP			; KInventory::SetMoney
_TEXT	ENDS
PUBLIC	?GetXu@KInventory@@QAEHXZ			; KInventory::GetXu
;	COMDAT ?GetXu@KInventory@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?GetXu@KInventory@@QAEHXZ PROC NEAR			; KInventory::GetXu, COMDAT

; 33   : 	int		GetXu() { return m_nXu; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+16]
	mov	esp, ebp
	pop	ebp
	ret	0
?GetXu@KInventory@@QAEHXZ ENDP				; KInventory::GetXu
_TEXT	ENDS
PUBLIC	?SetXu@KInventory@@QAEHH@Z			; KInventory::SetXu
;	COMDAT ?SetXu@KInventory@@QAEHH@Z
_TEXT	SEGMENT
_nXu$ = 8
_this$ = -4
?SetXu@KInventory@@QAEHH@Z PROC NEAR			; KInventory::SetXu, COMDAT

; 34   : 	BOOL	SetXu(int nXu) { if (nXu < 0) return FALSE; m_nXu = nXu; return TRUE; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	cmp	DWORD PTR _nXu$[ebp], 0
	jge	SHORT $L102631
	xor	eax, eax
	jmp	SHORT $L102630
$L102631:
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nXu$[ebp]
	mov	DWORD PTR [eax+16], ecx
	mov	eax, 1
$L102630:
	mov	esp, ebp
	pop	ebp
	ret	4
?SetXu@KInventory@@QAEHH@Z ENDP				; KInventory::SetXu
_TEXT	ENDS
PUBLIC	??4KInventory@@QAEAAV0@ABV0@@Z			; KInventory::operator=
;	COMDAT ??4KInventory@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8
_this$ = -4
??4KInventory@@QAEAAV0@ABV0@@Z PROC NEAR		; KInventory::operator=, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	push	esi
	push	edi
	mov	DWORD PTR _this$[ebp], ecx
	mov	esi, DWORD PTR ___that$[ebp]
	mov	ecx, 5
	mov	edi, DWORD PTR _this$[ebp]
	rep movsd
	mov	eax, DWORD PTR _this$[ebp]
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	4
??4KInventory@@QAEAAV0@ABV0@@Z ENDP			; KInventory::operator=
_TEXT	ENDS
PUBLIC	?GetEquipment@KItemList@@QAEHH@Z		; KItemList::GetEquipment
;	COMDAT ?GetEquipment@KItemList@@QAEHH@Z
_TEXT	SEGMENT
_this$ = -4
_nIdx$ = 8
?GetEquipment@KItemList@@QAEHH@Z PROC NEAR		; KItemList::GetEquipment, COMDAT

; 68   : 	int			GetEquipment(int nIdx) { return m_EquipItem[nIdx]; }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _nIdx$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+eax*4+12]
	mov	esp, ebp
	pop	ebp
	ret	4
?GetEquipment@KItemList@@QAEHH@Z ENDP			; KItemList::GetEquipment
_TEXT	ENDS
PUBLIC	?GetMoney@KItemList@@QAEHH@Z			; KItemList::GetMoney
;	COMDAT ?GetMoney@KItemList@@QAEHH@Z
_TEXT	SEGMENT
_nRoom$ = 8
_this$ = -4
?GetMoney@KItemList@@QAEHH@Z PROC NEAR			; KItemList::GetMoney, COMDAT

; 94   : 	int			GetMoney(int nRoom) { return m_Room[nRoom].GetMoney(); }	// 取得空间的钱

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _nRoom$[ebp]
	imul	eax, 20					; 00000014H
	mov	ecx, DWORD PTR _this$[ebp]
	lea	ecx, DWORD PTR [ecx+eax+10012]
	call	?GetMoney@KInventory@@QAEHXZ		; KInventory::GetMoney
	mov	esp, ebp
	pop	ebp
	ret	4
?GetMoney@KItemList@@QAEHH@Z ENDP			; KItemList::GetMoney
_TEXT	ENDS
PUBLIC	?GetXu@KItemList@@QAEHH@Z			; KItemList::GetXu
;	COMDAT ?GetXu@KItemList@@QAEHH@Z
_TEXT	SEGMENT
_nRoom$ = 8
_this$ = -4
?GetXu@KItemList@@QAEHH@Z PROC NEAR			; KItemList::GetXu, COMDAT

; 96   : 	int			GetXu(int nRoom) { return m_Room[nRoom].GetXu(); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _nRoom$[ebp]
	imul	eax, 20					; 00000014H
	mov	ecx, DWORD PTR _this$[ebp]
	lea	ecx, DWORD PTR [ecx+eax+10012]
	call	?GetXu@KInventory@@QAEHXZ		; KInventory::GetXu
	mov	esp, ebp
	pop	ebp
	ret	4
?GetXu@KItemList@@QAEHH@Z ENDP				; KItemList::GetXu
_TEXT	ENDS
PUBLIC	?Hand@KItemList@@QAEHXZ				; KItemList::Hand
;	COMDAT ?Hand@KItemList@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?Hand@KItemList@@QAEHXZ PROC NEAR			; KItemList::Hand, COMDAT

; 106  : 	int			Hand() { return m_Hand; };

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+4]
	mov	esp, ebp
	pop	ebp
	ret	0
?Hand@KItemList@@QAEHXZ ENDP				; KItemList::Hand
_TEXT	ENDS
PUBLIC	?GetMaskLock@KItemList@@QAEHXZ			; KItemList::GetMaskLock
;	COMDAT ?GetMaskLock@KItemList@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?GetMaskLock@KItemList@@QAEHXZ PROC NEAR		; KItemList::GetMaskLock, COMDAT

; 119  : 	BOOL		GetMaskLock() {return m_nMaskLock;};	// mat na

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+10004]
	mov	esp, ebp
	pop	ebp
	ret	0
?GetMaskLock@KItemList@@QAEHXZ ENDP			; KItemList::GetMaskLock
_TEXT	ENDS
PUBLIC	?IsLockOperation@KItemList@@QAEHXZ		; KItemList::IsLockOperation
;	COMDAT ?IsLockOperation@KItemList@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?IsLockOperation@KItemList@@QAEHXZ PROC NEAR		; KItemList::IsLockOperation, COMDAT

; 159  : 	BOOL		IsLockOperation() { return m_bLockOperation; };

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+10008]
	mov	esp, ebp
	pop	ebp
	ret	0
?IsLockOperation@KItemList@@QAEHXZ ENDP			; KItemList::IsLockOperation
_TEXT	ENDS
PUBLIC	??4KItemList@@QAEAAV0@ABV0@@Z			; KItemList::operator=
;	COMDAT ??4KItemList@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8
_this$ = -4
??4KItemList@@QAEAAV0@ABV0@@Z PROC NEAR			; KItemList::operator=, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	push	esi
	push	edi
	mov	DWORD PTR _this$[ebp], ecx
	mov	edi, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR ___that$[ebp]
	mov	ecx, 2563				; 00000a03H
	rep movsd
	mov	eax, DWORD PTR _this$[ebp]
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	4
??4KItemList@@QAEAAV0@ABV0@@Z ENDP			; KItemList::operator=
_TEXT	ENDS
PUBLIC	??0KMissleSet@@QAE@XZ				; KMissleSet::KMissleSet
EXTRN	__imp_??0KLinkArray@@QAE@XZ:NEAR
EXTRN	__imp_??1KLinkArray@@QAE@XZ:NEAR
EXTRN	__except_list:DWORD
EXTRN	___CxxFrameHandler:NEAR
;	COMDAT xdata$x
xdata$x	SEGMENT
$T106076 DD	019930520H
	DD	01H
	DD	FLAT:$T106079
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T106079 DD	0ffffffffH
	DD	FLAT:$L106071
xdata$x	ENDS
;	COMDAT ??0KMissleSet@@QAE@XZ
_TEXT	SEGMENT
_this$ = -16
__$EHRec$ = -12
??0KMissleSet@@QAE@XZ PROC NEAR				; KMissleSet::KMissleSet, COMDAT
	push	ebp
	mov	ebp, esp
	push	-1
	push	$L106077
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	DWORD PTR __imp_??0KLinkArray@@QAE@XZ
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 8
	call	DWORD PTR __imp_??0KLinkArray@@QAE@XZ
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	0
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$L106071:
	mov	ecx, DWORD PTR _this$[ebp]
	call	DWORD PTR __imp_??1KLinkArray@@QAE@XZ
	ret	0
$L106077:
	mov	eax, OFFSET FLAT:$T106076
	jmp	___CxxFrameHandler
text$x	ENDS
??0KMissleSet@@QAE@XZ ENDP				; KMissleSet::KMissleSet
PUBLIC	??4KMissleSet@@QAEAAV0@ABV0@@Z			; KMissleSet::operator=
;	COMDAT ??4KMissleSet@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8
_this$ = -4
??4KMissleSet@@QAEAAV0@ABV0@@Z PROC NEAR		; KMissleSet::operator=, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR ___that$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR [ecx], edx
	mov	edx, DWORD PTR [eax+4]
	mov	DWORD PTR [ecx+4], edx
	mov	edx, DWORD PTR [eax+8]
	mov	DWORD PTR [ecx+8], edx
	mov	eax, DWORD PTR [eax+12]
	mov	DWORD PTR [ecx+12], eax
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??4KMissleSet@@QAEAAV0@ABV0@@Z ENDP			; KMissleSet::operator=
_TEXT	ENDS
PUBLIC	??1KMissleSet@@QAE@XZ				; KMissleSet::~KMissleSet
;	COMDAT xdata$x
xdata$x	SEGMENT
$T106090 DD	019930520H
	DD	01H
	DD	FLAT:$T106092
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T106092 DD	0ffffffffH
	DD	FLAT:$L106087
xdata$x	ENDS
;	COMDAT ??1KMissleSet@@QAE@XZ
_TEXT	SEGMENT
_this$ = -16
__$EHRec$ = -12
??1KMissleSet@@QAE@XZ PROC NEAR				; KMissleSet::~KMissleSet, COMDAT
	push	ebp
	mov	ebp, esp
	push	-1
	push	$L106091
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 8
	call	DWORD PTR __imp_??1KLinkArray@@QAE@XZ
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	ecx, DWORD PTR _this$[ebp]
	call	DWORD PTR __imp_??1KLinkArray@@QAE@XZ
	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	0
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$L106087:
	mov	ecx, DWORD PTR _this$[ebp]
	call	DWORD PTR __imp_??1KLinkArray@@QAE@XZ
	ret	0
$L106091:
	mov	eax, OFFSET FLAT:$T106090
	jmp	___CxxFrameHandler
text$x	ENDS
??1KMissleSet@@QAE@XZ ENDP				; KMissleSet::~KMissleSet
_TEXT	SEGMENT
_$E14	PROC NEAR
	push	ebp
	mov	ebp, esp
	call	_$E11
	call	_$E13
	pop	ebp
	ret	0
_$E14	ENDP
_TEXT	ENDS
PUBLIC	??0KMissle@@QAE@XZ				; KMissle::KMissle
PUBLIC	??1KMissle@@UAE@XZ				; KMissle::~KMissle
EXTRN	??_L@YGXPAXIHP6EX0@Z1@Z:NEAR			; `eh vector constructor iterator'
_TEXT	SEGMENT
_$E11	PROC NEAR

; 111  : KMissle g_MisslesLib[MAX_MISSLESTYLE];

	push	ebp
	mov	ebp, esp
	push	OFFSET FLAT:??1KMissle@@UAE@XZ		; KMissle::~KMissle
	push	OFFSET FLAT:??0KMissle@@QAE@XZ		; KMissle::KMissle
	push	500					; 000001f4H
	push	2940					; 00000b7cH
	push	OFFSET FLAT:?g_MisslesLib@@3PAVKMissle@@A ; g_MisslesLib
	call	??_L@YGXPAXIHP6EX0@Z1@Z			; `eh vector constructor iterator'
	pop	ebp
	ret	0
_$E11	ENDP
_TEXT	ENDS
EXTRN	_atexit:NEAR
_TEXT	SEGMENT
_$E13	PROC NEAR
	push	ebp
	mov	ebp, esp
	push	OFFSET FLAT:_$E12
	call	_atexit
	add	esp, 4
	pop	ebp
	ret	0
_$E13	ENDP
_TEXT	ENDS
EXTRN	??_M@YGXPAXIHP6EX0@Z@Z:NEAR			; `eh vector destructor iterator'
_TEXT	SEGMENT
_$E12	PROC NEAR
	push	ebp
	mov	ebp, esp
	push	OFFSET FLAT:??1KMissle@@UAE@XZ		; KMissle::~KMissle
	push	500					; 000001f4H
	push	2940					; 00000b7cH
	push	OFFSET FLAT:?g_MisslesLib@@3PAVKMissle@@A ; g_MisslesLib
	call	??_M@YGXPAXIHP6EX0@Z@Z			; `eh vector destructor iterator'
	pop	ebp
	ret	0
_$E12	ENDP
_$E19	PROC NEAR
	push	ebp
	mov	ebp, esp
	call	_$E16
	call	_$E18
	pop	ebp
	ret	0
_$E19	ENDP
_$E16	PROC NEAR

; 132  : CORE_API KMissle Missle[MAX_MISSLE];

	push	ebp
	mov	ebp, esp
	push	OFFSET FLAT:??1KMissle@@UAE@XZ		; KMissle::~KMissle
	push	OFFSET FLAT:??0KMissle@@QAE@XZ		; KMissle::KMissle
	push	500					; 000001f4H
	push	2940					; 00000b7cH
	push	OFFSET FLAT:?Missle@@3PAVKMissle@@A	; Missle
	call	??_L@YGXPAXIHP6EX0@Z1@Z			; `eh vector constructor iterator'
	pop	ebp
	ret	0
_$E16	ENDP
_$E18	PROC NEAR
	push	ebp
	mov	ebp, esp
	push	OFFSET FLAT:_$E17
	call	_atexit
	add	esp, 4
	pop	ebp
	ret	0
_$E18	ENDP
_$E17	PROC NEAR
	push	ebp
	mov	ebp, esp
	push	OFFSET FLAT:??1KMissle@@UAE@XZ		; KMissle::~KMissle
	push	500					; 000001f4H
	push	2940					; 00000b7cH
	push	OFFSET FLAT:?Missle@@3PAVKMissle@@A	; Missle
	call	??_M@YGXPAXIHP6EX0@Z@Z			; `eh vector destructor iterator'
	pop	ebp
	ret	0
_$E17	ENDP
_TEXT	ENDS
PUBLIC	??_7KMissle@@6B@				; KMissle::`vftable'
PUBLIC	??_GKMissle@@UAEPAXI@Z				; KMissle::`scalar deleting destructor'
PUBLIC	??_EKMissle@@UAEPAXI@Z				; KMissle::`vector deleting destructor'
PUBLIC	??0KIndexNode@@QAE@XZ				; KIndexNode::KIndexNode
PUBLIC	??1KIndexNode@@UAE@XZ				; KIndexNode::~KIndexNode
EXTRN	??0KMissleRes@@QAE@XZ:NEAR			; KMissleRes::KMissleRes
;	COMDAT ??_7KMissle@@6B@
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\KMissle.cpp
CONST	SEGMENT
??_7KMissle@@6B@ DD FLAT:??_EKMissle@@UAEPAXI@Z		; KMissle::`vftable'
CONST	ENDS
xdata$x	SEGMENT
$T106108 DD	019930520H
	DD	01H
	DD	FLAT:$T106110
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T106110 DD	0ffffffffH
	DD	FLAT:$L106105
xdata$x	ENDS
_TEXT	SEGMENT
__$EHRec$ = -12
_this$ = -16
??0KMissle@@QAE@XZ PROC NEAR				; KMissle::KMissle

; 138  : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L106109
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??0KIndexNode@@QAE@XZ			; KIndexNode::KIndexNode
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	??0KMissleRes@@QAE@XZ			; KMissleRes::KMissleRes
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KMissle@@6B@ ; KMissle::`vftable'

; 139  : 	m_nMissleId = -1;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+360], -1

; 140  : 	m_nCollideOrVanishTime = 0;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+216], 0

; 141  : 	m_ulDamageInterval = 0;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+200], 0

; 142  : 	m_nTempParam1 = 0;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+416], 0

; 143  : 	m_nTempParam2 = 0;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+420], 0

; 144  : 	m_nFirstReclaimTime = 0;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+408], 0

; 145  : 	m_nEndReclaimTime = 0;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+412], 0

; 146  : 	
; 147  : #ifdef _SERVER
; 148  : 	m_pMagicAttribsData = NULL;
; 149  : 	m_ulNextCalDamageTime = 0;
; 150  : #else
; 151  : 	m_bFollowNpcWhenCollid = 1;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+448], 1

; 152  : 	m_bRemoving	= FALSE;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+72], 0

; 153  : 	m_btRedLum = m_btGreenLum = m_btBlueLum = 0xff;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+460], 255		; 000000ffH
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+456], 255		; 000000ffH
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+452], 255		; 000000ffH

; 154  : 	m_usLightRadius = 50;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	WORD PTR [ecx+464], 50			; 00000032H

; 155  : #endif
; 156  : }

	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	0
_TEXT	ENDS
text$x	SEGMENT
$L106105:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KIndexNode@@UAE@XZ			; KIndexNode::~KIndexNode
	ret	0
$L106109:
	mov	eax, OFFSET FLAT:$T106108
	jmp	___CxxFrameHandler
text$x	ENDS
??0KMissle@@QAE@XZ ENDP					; KMissle::KMissle
PUBLIC	??0KNode@@QAE@XZ				; KNode::KNode
PUBLIC	??_7KIndexNode@@6B@				; KIndexNode::`vftable'
PUBLIC	??_GKIndexNode@@UAEPAXI@Z			; KIndexNode::`scalar deleting destructor'
PUBLIC	??_EKIndexNode@@UAEPAXI@Z			; KIndexNode::`vector deleting destructor'
;	COMDAT ??_7KIndexNode@@6B@
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\KIndexNode.h
CONST	SEGMENT
??_7KIndexNode@@6B@ DD FLAT:??_EKIndexNode@@UAEPAXI@Z	; KIndexNode::`vftable'
CONST	ENDS
;	COMDAT ??0KIndexNode@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0KIndexNode@@QAE@XZ PROC NEAR				; KIndexNode::KIndexNode, COMDAT

; 10   : 	KIndexNode() { m_nIndex = NULL; m_Ref = 0; };

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??0KNode@@QAE@XZ			; KNode::KNode
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KIndexNode@@6B@ ; KIndexNode::`vftable'
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+12], 0
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+16], 0
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0KIndexNode@@QAE@XZ ENDP				; KIndexNode::KIndexNode
_TEXT	ENDS
PUBLIC	??_7KNode@@6B@					; KNode::`vftable'
PUBLIC	??_GKNode@@UAEPAXI@Z				; KNode::`scalar deleting destructor'
PUBLIC	??_EKNode@@UAEPAXI@Z				; KNode::`vector deleting destructor'
;	COMDAT ??_7KNode@@6B@
; File ..\engine\src\KNode.h
CONST	SEGMENT
??_7KNode@@6B@ DD FLAT:??_EKNode@@UAEPAXI@Z		; KNode::`vftable'
CONST	ENDS
;	COMDAT ??0KNode@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0KNode@@QAE@XZ PROC NEAR				; KNode::KNode, COMDAT

; 43   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KNode@@6B@ ; KNode::`vftable'

; 44   : 	m_pNext = NULL;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+4], 0

; 45   : 	m_pPrev = NULL;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+8], 0

; 46   : }

	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0KNode@@QAE@XZ ENDP					; KNode::KNode
_TEXT	ENDS
PUBLIC	??1KNode@@UAE@XZ				; KNode::~KNode
EXTRN	??3@YAXPAX@Z:NEAR				; operator delete
;	COMDAT ??_GKNode@@UAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8
_this$ = -4
??_GKNode@@UAEPAXI@Z PROC NEAR				; KNode::`scalar deleting destructor', COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KNode@@UAE@XZ			; KNode::~KNode
	mov	eax, DWORD PTR ___flags$[ebp]
	and	eax, 1
	test	eax, eax
	je	SHORT $L83723
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L83723:
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??_GKNode@@UAEPAXI@Z ENDP				; KNode::`scalar deleting destructor'
_TEXT	ENDS
;	COMDAT ??1KNode@@UAE@XZ
_TEXT	SEGMENT
_this$ = -4
??1KNode@@UAE@XZ PROC NEAR				; KNode::~KNode, COMDAT

; 26   : 	virtual ~KNode(){};

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KNode@@6B@ ; KNode::`vftable'
	mov	esp, ebp
	pop	ebp
	ret	0
??1KNode@@UAE@XZ ENDP					; KNode::~KNode
_TEXT	ENDS
;	COMDAT ??_GKIndexNode@@UAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8
_this$ = -4
??_GKIndexNode@@UAEPAXI@Z PROC NEAR			; KIndexNode::`scalar deleting destructor', COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KIndexNode@@UAE@XZ			; KIndexNode::~KIndexNode
	mov	eax, DWORD PTR ___flags$[ebp]
	and	eax, 1
	test	eax, eax
	je	SHORT $L93083
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L93083:
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??_GKIndexNode@@UAEPAXI@Z ENDP				; KIndexNode::`scalar deleting destructor'
_TEXT	ENDS
;	COMDAT ??1KIndexNode@@UAE@XZ
_TEXT	SEGMENT
_this$ = -4
??1KIndexNode@@UAE@XZ PROC NEAR				; KIndexNode::~KIndexNode, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KNode@@UAE@XZ			; KNode::~KNode
	mov	esp, ebp
	pop	ebp
	ret	0
??1KIndexNode@@UAE@XZ ENDP				; KIndexNode::~KIndexNode
_TEXT	ENDS
;	COMDAT ??_GKMissle@@UAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8
_this$ = -4
??_GKMissle@@UAEPAXI@Z PROC NEAR			; KMissle::`scalar deleting destructor', COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KMissle@@UAE@XZ			; KMissle::~KMissle
	mov	eax, DWORD PTR ___flags$[ebp]
	and	eax, 1
	test	eax, eax
	je	SHORT $L105116
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L105116:
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??_GKMissle@@UAEPAXI@Z ENDP				; KMissle::`scalar deleting destructor'
_TEXT	ENDS
PUBLIC	?Release@KMissle@@AAEXXZ			; KMissle::Release
EXTRN	?g_ScenePlace@@3VKScenePlaceC@@A:BYTE		; g_ScenePlace
EXTRN	?Clear@KMissleRes@@QAEXXZ:NEAR			; KMissleRes::Clear
EXTRN	?RemoveObject@KScenePlaceC@@QAEXIHAAI@Z:NEAR	; KScenePlaceC::RemoveObject
_TEXT	SEGMENT
_this$ = -4
?Release@KMissle@@AAEXXZ PROC NEAR			; KMissle::Release

; 159  : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 160  : ///#pragma	message(ATTENTION("子弹消亡时，需更新发送者使用该技能时的当前使用次数，使之减一"))
; 161  : #ifndef _SERVER	
; 162  : 	g_ScenePlace.RemoveObject(CGOG_MISSLE, m_nMissleId, m_SceneID);

	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 2936				; 00000b78H
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+360]
	push	edx
	push	9
	mov	ecx, OFFSET FLAT:?g_ScenePlace@@3VKScenePlaceC@@A
	call	?RemoveObject@KScenePlaceC@@QAEXIHAAI@Z	; KScenePlaceC::RemoveObject

; 163  : 	m_MissleRes.Clear();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?Clear@KMissleRes@@QAEXXZ		; KMissleRes::Clear

; 164  : 	m_nMissleId = -1;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+360], -1

; 165  : 	m_nFollowNpcIdx = 0;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+328], 0

; 166  : #endif
; 167  : #ifdef _SERVER
; 168  : 	if (m_pMagicAttribsData)
; 169  : 		if (m_pMagicAttribsData->DelRef() == 0)
; 170  : 			delete m_pMagicAttribsData;
; 171  : 		m_pMagicAttribsData = NULL;
; 172  : #endif
; 173  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?Release@KMissle@@AAEXXZ ENDP				; KMissle::Release
_TEXT	ENDS
EXTRN	??1KMissleRes@@QAE@XZ:NEAR			; KMissleRes::~KMissleRes
xdata$x	SEGMENT
$T106132 DD	019930520H
	DD	01H
	DD	FLAT:$T106134
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T106134 DD	0ffffffffH
	DD	FLAT:$L106129
xdata$x	ENDS
_TEXT	SEGMENT
__$EHRec$ = -12
_this$ = -16
??1KMissle@@UAE@XZ PROC NEAR				; KMissle::~KMissle

; 176  : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L106133
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KMissle@@6B@ ; KMissle::`vftable'
	mov	DWORD PTR __$EHRec$[ebp+8], 0

; 177  : 	
; 178  : }

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	??1KMissleRes@@QAE@XZ			; KMissleRes::~KMissleRes
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KIndexNode@@UAE@XZ			; KIndexNode::~KIndexNode
	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	0
_TEXT	ENDS
text$x	SEGMENT
$L106129:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KIndexNode@@UAE@XZ			; KIndexNode::~KIndexNode
	ret	0
$L106133:
	mov	eax, OFFSET FLAT:$T106132
	jmp	___CxxFrameHandler
text$x	ENDS
??1KMissle@@UAE@XZ ENDP					; KMissle::~KMissle
PUBLIC	?GetInfoFromTabFile@KMissle@@QAEHH@Z		; KMissle::GetInfoFromTabFile
PUBLIC	?GetInfoFromTabFile@KMissle@@QAEHPAVKITabFile@@H@Z ; KMissle::GetInfoFromTabFile
EXTRN	?g_MisslesSetting@@3VKTabFile@@A:BYTE		; g_MisslesSetting
_TEXT	SEGMENT
_nMissleId$ = 8
_this$ = -8
_pITabFile$ = -4
?GetInfoFromTabFile@KMissle@@QAEHH@Z PROC NEAR		; KMissle::GetInfoFromTabFile

; 188  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx

; 189  : 	if (nMissleId <= 0 ) return FALSE;

	cmp	DWORD PTR _nMissleId$[ebp], 0
	jg	SHORT $L105127
	xor	eax, eax
	jmp	SHORT $L105126
$L105127:

; 190  : 	KITabFile * pITabFile = &g_MisslesSetting;

	mov	DWORD PTR _pITabFile$[ebp], OFFSET FLAT:?g_MisslesSetting@@3VKTabFile@@A ; g_MisslesSetting

; 191  : 	return GetInfoFromTabFile(pITabFile, nMissleId);

	mov	eax, DWORD PTR _nMissleId$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pITabFile$[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetInfoFromTabFile@KMissle@@QAEHPAVKITabFile@@H@Z ; KMissle::GetInfoFromTabFile
$L105126:

; 192  : }

	mov	esp, ebp
	pop	ebp
	ret	4
?GetInfoFromTabFile@KMissle@@QAEHH@Z ENDP		; KMissle::GetInfoFromTabFile
_TEXT	ENDS
EXTRN	__imp__sprintf:NEAR
EXTRN	__imp_?KSG_StringGetInt@@YAHPAPBDH@Z:NEAR
EXTRN	__imp_?KSG_StringSkipSymbol@@YA_NPAPBDH@Z:NEAR
_BSS	SEGMENT
$SG105136 DB	01H DUP (?)
	ALIGN	4

$SG105187 DB	01H DUP (?)
	ALIGN	4

$SG105188 DB	01H DUP (?)
	ALIGN	4

$SG105189 DB	01H DUP (?)
	ALIGN	4

$SG105193 DB	01H DUP (?)
	ALIGN	4

$SG105194 DB	01H DUP (?)
	ALIGN	4

$SG105195 DB	01H DUP (?)
_BSS	ENDS
_DATA	SEGMENT
$SG105137 DB	'MissleName', 00H
	ORG $+1
$SG105139 DB	'MissleHeight', 00H
	ORG $+3
$SG105140 DB	'LifeTime', 00H
	ORG $+3
$SG105141 DB	'Speed', 00H
	ORG $+2
$SG105142 DB	'ResponseSkill', 00H
	ORG $+2
$SG105143 DB	'CollidRange', 00H
$SG105144 DB	'ColVanish', 00H
	ORG $+2
$SG105145 DB	'CanColFriend', 00H
	ORG $+3
$SG105146 DB	'CanSlow', 00H
$SG105147 DB	'IsRangeDmg', 00H
	ORG $+1
$SG105148 DB	'DmgRange', 00H
	ORG $+3
$SG105150 DB	'MoveKind', 00H
	ORG $+3
$SG105152 DB	'FollowKind', 00H
	ORG $+1
$SG105154 DB	'Zacc', 00H
	ORG $+3
$SG105156 DB	'Zspeed', 00H
	ORG $+1
$SG105157 DB	'Param1', 00H
	ORG $+1
$SG105158 DB	'Param2', 00H
	ORG $+1
$SG105159 DB	'Param3', 00H
	ORG $+1
$SG105162 DB	'AutoExplode', 00H
$SG105164 DB	'DmgInterval', 00H
$SG105171 DB	'RedLum', 00H
	ORG $+1
$SG105173 DB	'GreenLum', 00H
	ORG $+3
$SG105175 DB	'BlueLum', 00H
$SG105178 DB	'LightRadius', 00H
$SG105179 DB	'MultiShow', 00H
	ORG $+2
$SG105184 DB	'AnimFile%d', 00H
	ORG $+1
$SG105185 DB	'SndFile%d', 00H
	ORG $+2
$SG105186 DB	'AnimFileInfo%d', 00H
	ORG $+1
$SG105190 DB	'AnimFileB%d', 00H
$SG105191 DB	'SndFileB%d', 00H
	ORG $+1
$SG105192 DB	'AnimFileInfoB%d', 00H
$SG105196 DB	'LoopPlay', 00H
	ORG $+3
$SG105197 DB	'SubLoop', 00H
$SG105198 DB	'SubStart', 00H
	ORG $+3
$SG105199 DB	'SubStop', 00H
$SG105201 DB	'ColFollowTarget', 00H
_DATA	ENDS
_TEXT	SEGMENT
_pMisslesSetting$ = 8
_nMissleId$ = 12
_this$ = -356
_nRow$ = -4
_nHeightOld$ = -244
_bAutoExplode$ = -352
_AnimFileCol$ = -240
_SndFileCol$ = -68
_AnimFileInfoCol$ = -344
_szAnimFileInfo$ = -176
_pcszTemp$ = -348
_nLightRadius$ = -72
_i$ = -76
?GetInfoFromTabFile@KMissle@@QAEHPAVKITabFile@@H@Z PROC NEAR ; KMissle::GetInfoFromTabFile

; 195  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 356				; 00000164H
	mov	DWORD PTR _this$[ebp], ecx

; 196  : 	if (nMissleId <= 0 ) return FALSE;

	cmp	DWORD PTR _nMissleId$[ebp], 0
	jg	SHORT $L105134
	xor	eax, eax
	jmp	$L105133
$L105134:

; 197  : 	m_nMissleId		= nMissleId;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nMissleId$[ebp]
	mov	DWORD PTR [eax+360], ecx

; 198  : 	int nRow = nMissleId;

	mov	edx, DWORD PTR _nMissleId$[ebp]
	mov	DWORD PTR _nRow$[ebp], edx

; 199  : 	
; 200  : 	pMisslesSetting->GetString(nRow, "MissleName",		   "", m_szMissleName,30, TRUE);

	push	1
	push	30					; 0000001eH
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 28					; 0000001cH
	push	eax
	push	OFFSET FLAT:$SG105136
	push	OFFSET FLAT:$SG105137
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+36]

; 201  : 	
; 202  : 	int nHeightOld ;
; 203  : 	pMisslesSetting->GetInteger(nRow, "MissleHeight",		0, &nHeightOld, TRUE);

	push	1
	lea	ecx, DWORD PTR _nHeightOld$[ebp]
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105139
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 204  : 	m_nHeight = nHeightOld << 10;

	mov	eax, DWORD PTR _nHeightOld$[ebp]
	shl	eax, 10					; 0000000aH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+88], eax

; 205  : 	
; 206  : 	pMisslesSetting->GetInteger(nRow, "LifeTime",			0, &m_nLifeTime, TRUE);

	push	1
	mov	edx, DWORD PTR _this$[ebp]
	add	edx, 96					; 00000060H
	push	edx
	push	0
	push	OFFSET FLAT:$SG105140
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 207  : 	pMisslesSetting->GetInteger(nRow, "Speed",				0, &m_nSpeed, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 100				; 00000064H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105141
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 208  : 	pMisslesSetting->GetInteger(nRow, "ResponseSkill",		0, &m_nSkillId, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 104				; 00000068H
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105142
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 209  : 	pMisslesSetting->GetInteger(nRow, "CollidRange",		0, &m_nCollideRange, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 148				; 00000094H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105143
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 210  : 	pMisslesSetting->GetInteger(nRow, "ColVanish",			0, &m_bCollideVanish, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 156				; 0000009cH
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105144
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 211  : 	pMisslesSetting->GetInteger(nRow, "CanColFriend",		0, &m_bCollideFriend, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 160				; 000000a0H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105145
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 212  : 	pMisslesSetting->GetInteger(nRow, "CanSlow",			0, &m_bCanSlow, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 164				; 000000a4H
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105146
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 213  : 	pMisslesSetting->GetInteger(nRow, "IsRangeDmg",		0, &m_bRangeDamage, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 108				; 0000006cH
	push	eax
	push	0
	push	OFFSET FLAT:$SG105147
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 214  : 	pMisslesSetting->GetInteger(nRow, "DmgRange",			0, &m_nDamageRange, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 152				; 00000098H
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105148
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 215  : 	pMisslesSetting->GetInteger(nRow, "MoveKind",			0, (int*)&m_eMoveKind, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 80					; 00000050H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105150
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 216  : 	pMisslesSetting->GetInteger(nRow, "FollowKind",		0, (int*)&m_eFollowKind, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 84					; 00000054H
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105152
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 217  : 	pMisslesSetting->GetInteger(nRow, "Zacc",				0,(int*)&m_nZAcceleration, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 352				; 00000160H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105154
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 218  : 	pMisslesSetting->GetInteger(nRow, "Zspeed",				0,(int*)&m_nHeightSpeed, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92					; 0000005cH
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105156
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 219  : 	pMisslesSetting->GetInteger(nRow, "Param1",			0, &m_nParam1, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 396				; 0000018cH
	push	eax
	push	0
	push	OFFSET FLAT:$SG105157
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 220  : 	pMisslesSetting->GetInteger(nRow, "Param2",			0, &m_nParam2, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 400				; 00000190H
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105158
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 221  : 	pMisslesSetting->GetInteger(nRow, "Param3",			0, &m_nParam3, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 404				; 00000194H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105159
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 222  : 	
; 223  : 	BOOL bAutoExplode = 0;

	mov	DWORD PTR _bAutoExplode$[ebp], 0

; 224  : 	pMisslesSetting->GetInteger(nRow, "AutoExplode",	0, (int*)&bAutoExplode, TRUE);

	push	1
	lea	ecx, DWORD PTR _bAutoExplode$[ebp]
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105162
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 225  : 	m_bAutoExplode = bAutoExplode;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _bAutoExplode$[ebp]
	mov	DWORD PTR [eax+116], ecx

; 226  : 	
; 227  : 	pMisslesSetting->GetInteger(nRow, "DmgInterval",	0, (int*)&m_ulDamageInterval, TRUE);

	push	1
	mov	edx, DWORD PTR _this$[ebp]
	add	edx, 200				; 000000c8H
	push	edx
	push	0
	push	OFFSET FLAT:$SG105164
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 228  : 	
; 229  : #ifndef _SERVER	
; 230  : 	char AnimFileCol[64];
; 231  : 	char SndFileCol[64];
; 232  : 	char AnimFileInfoCol[100];
; 233  : 	char szAnimFileInfo[100];
; 234  : 
; 235  :     const char *pcszTemp = NULL;

	mov	DWORD PTR _pcszTemp$[ebp], 0

; 236  : 	
; 237  : 	pMisslesSetting->GetInteger(nRow, "RedLum",	    255, (int*)&m_btRedLum, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 452				; 000001c4H
	push	eax
	push	255					; 000000ffH
	push	OFFSET FLAT:$SG105171
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 238  : 	pMisslesSetting->GetInteger(nRow, "GreenLum",	255, (int*)&m_btGreenLum, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 456				; 000001c8H
	push	ecx
	push	255					; 000000ffH
	push	OFFSET FLAT:$SG105173
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 239  : 	pMisslesSetting->GetInteger(nRow, "BlueLum",	255, (int*)&m_btBlueLum, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 460				; 000001ccH
	push	eax
	push	255					; 000000ffH
	push	OFFSET FLAT:$SG105175
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 240  : 	
; 241  : 	int nLightRadius = 0;

	mov	DWORD PTR _nLightRadius$[ebp], 0

; 242  : 	pMisslesSetting->GetInteger(nRow, "LightRadius", 50, (int*)&nLightRadius, TRUE);

	push	1
	lea	ecx, DWORD PTR _nLightRadius$[ebp]
	push	ecx
	push	50					; 00000032H
	push	OFFSET FLAT:$SG105178
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 243  : 	m_usLightRadius = nLightRadius;

	mov	eax, DWORD PTR _this$[ebp]
	mov	cx, WORD PTR _nLightRadius$[ebp]
	mov	WORD PTR [eax+464], cx

; 244  : 	
; 245  : 	pMisslesSetting->GetInteger(nRow, "MultiShow",		0, &m_bMultiShow, TRUE);

	push	1
	mov	edx, DWORD PTR _this$[ebp]
	add	edx, 444				; 000001bcH
	push	edx
	push	0
	push	OFFSET FLAT:$SG105179
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 246  : 	for (int i  = 0; i < MAX_MISSLE_STATUS; i++)

	mov	DWORD PTR _i$[ebp], 0
	jmp	SHORT $L105181
$L105182:
	mov	eax, DWORD PTR _i$[ebp]
	add	eax, 1
	mov	DWORD PTR _i$[ebp], eax
$L105181:
	cmp	DWORD PTR _i$[ebp], 4
	jge	$L105183

; 248  : 		sprintf(AnimFileCol, "AnimFile%d", i + 1);

	mov	ecx, DWORD PTR _i$[ebp]
	add	ecx, 1
	push	ecx
	push	OFFSET FLAT:$SG105184
	lea	edx, DWORD PTR _AnimFileCol$[ebp]
	push	edx
	call	DWORD PTR __imp__sprintf
	add	esp, 12					; 0000000cH

; 249  : 		sprintf(SndFileCol,  "SndFile%d", i + 1);

	mov	eax, DWORD PTR _i$[ebp]
	add	eax, 1
	push	eax
	push	OFFSET FLAT:$SG105185
	lea	ecx, DWORD PTR _SndFileCol$[ebp]
	push	ecx
	call	DWORD PTR __imp__sprintf
	add	esp, 12					; 0000000cH

; 250  : 		sprintf(AnimFileInfoCol, "AnimFileInfo%d", i + 1);

	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 1
	push	edx
	push	OFFSET FLAT:$SG105186
	lea	eax, DWORD PTR _AnimFileInfoCol$[ebp]
	push	eax
	call	DWORD PTR __imp__sprintf
	add	esp, 12					; 0000000cH

; 251  : 		
; 252  : 		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MissleRes.m_MissleRes[i].AnimFileName, 64, TRUE);

	push	1
	push	64					; 00000040H
	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _this$[ebp]
	lea	eax, DWORD PTR [edx+ecx+500]
	push	eax
	push	OFFSET FLAT:$SG105187
	lea	ecx, DWORD PTR _AnimFileCol$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+36]

; 253  : 		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MissleRes.m_MissleRes[i].SndFileName, 64, TRUE);

	push	1
	push	64					; 00000040H
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	lea	edx, DWORD PTR [ecx+eax+612]
	push	edx
	push	OFFSET FLAT:$SG105188
	lea	eax, DWORD PTR _SndFileCol$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+36]

; 254  : 		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, 100, TRUE);

	push	1
	push	100					; 00000064H
	lea	ecx, DWORD PTR _szAnimFileInfo$[ebp]
	push	ecx
	push	OFFSET FLAT:$SG105189
	lea	edx, DWORD PTR _AnimFileInfoCol$[ebp]
	push	edx
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+36]

; 255  : 		
; 256  : 		//m_MissleRes.m_MissleRes[i].nInterval = 1;
; 257  : 		//m_MissleRes.m_MissleRes[i].nDir = 16;
; 258  : 		//m_MissleRes.m_MissleRes[i].nTotalFrame = 100;
; 259  : 
; 260  :         pcszTemp = szAnimFileInfo;

	lea	eax, DWORD PTR _szAnimFileInfo$[ebp]
	mov	DWORD PTR _pcszTemp$[ebp], eax

; 261  :         m_MissleRes.m_MissleRes[i].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);

	push	100					; 00000064H
	lea	ecx, DWORD PTR _pcszTemp$[ebp]
	push	ecx
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8
	mov	edx, DWORD PTR _i$[ebp]
	imul	edx, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+edx+600], eax

; 262  :         KSG_StringSkipSymbol(&pcszTemp, ',');

	push	44					; 0000002cH
	lea	edx, DWORD PTR _pcszTemp$[ebp]
	push	edx
	call	DWORD PTR __imp_?KSG_StringSkipSymbol@@YA_NPAPBDH@Z
	add	esp, 8

; 263  :         m_MissleRes.m_MissleRes[i].nDir = KSG_StringGetInt(&pcszTemp, 16);

	push	16					; 00000010H
	lea	eax, DWORD PTR _pcszTemp$[ebp]
	push	eax
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8
	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+ecx+608], eax

; 264  :         KSG_StringSkipSymbol(&pcszTemp, ',');

	push	44					; 0000002cH
	lea	eax, DWORD PTR _pcszTemp$[ebp]
	push	eax
	call	DWORD PTR __imp_?KSG_StringSkipSymbol@@YA_NPAPBDH@Z
	add	esp, 8

; 265  :         m_MissleRes.m_MissleRes[i].nInterval = KSG_StringGetInt(&pcszTemp, 1);

	push	1
	lea	ecx, DWORD PTR _pcszTemp$[ebp]
	push	ecx
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8
	mov	edx, DWORD PTR _i$[ebp]
	imul	edx, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+edx+604], eax

; 266  : 		//sscanf(szAnimFileInfo, "%d,%d,%d", 
; 267  : 		//	&m_MissleRes.m_MissleRes[i].nTotalFrame,
; 268  : 		//	&m_MissleRes.m_MissleRes[i].nDir,
; 269  : 		//	&m_MissleRes.m_MissleRes[i].nInterval
; 270  :         //);
; 271  : 
; 272  : 		
; 273  : 		sprintf(AnimFileCol, "AnimFileB%d", i + 1);

	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 1
	push	edx
	push	OFFSET FLAT:$SG105190
	lea	eax, DWORD PTR _AnimFileCol$[ebp]
	push	eax
	call	DWORD PTR __imp__sprintf
	add	esp, 12					; 0000000cH

; 274  : 		sprintf(SndFileCol,  "SndFileB%d", i + 1);

	mov	ecx, DWORD PTR _i$[ebp]
	add	ecx, 1
	push	ecx
	push	OFFSET FLAT:$SG105191
	lea	edx, DWORD PTR _SndFileCol$[ebp]
	push	edx
	call	DWORD PTR __imp__sprintf
	add	esp, 12					; 0000000cH

; 275  : 		sprintf(AnimFileInfoCol, "AnimFileInfoB%d", i + 1);

	mov	eax, DWORD PTR _i$[ebp]
	add	eax, 1
	push	eax
	push	OFFSET FLAT:$SG105192
	lea	ecx, DWORD PTR _AnimFileInfoCol$[ebp]
	push	ecx
	call	DWORD PTR __imp__sprintf
	add	esp, 12					; 0000000cH

; 276  : 		
; 277  : 		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].AnimFileName, 64, TRUE);

	push	1
	push	64					; 00000040H
	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 4
	imul	edx, 212				; 000000d4H
	mov	eax, DWORD PTR _this$[ebp]
	lea	ecx, DWORD PTR [eax+edx+500]
	push	ecx
	push	OFFSET FLAT:$SG105193
	lea	edx, DWORD PTR _AnimFileCol$[ebp]
	push	edx
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+36]

; 278  : 		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].SndFileName, 64, TRUE);

	push	1
	push	64					; 00000040H
	mov	eax, DWORD PTR _i$[ebp]
	add	eax, 4
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	lea	edx, DWORD PTR [ecx+eax+612]
	push	edx
	push	OFFSET FLAT:$SG105194
	lea	eax, DWORD PTR _SndFileCol$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+36]

; 279  : 		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, 100, TRUE);

	push	1
	push	100					; 00000064H
	lea	ecx, DWORD PTR _szAnimFileInfo$[ebp]
	push	ecx
	push	OFFSET FLAT:$SG105195
	lea	edx, DWORD PTR _AnimFileInfoCol$[ebp]
	push	edx
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+36]

; 280  : 		
; 281  : 		//m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval = 1;
; 282  : 		//m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir = 16;
; 283  : 		//m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame = 100;
; 284  : 		
; 285  :         pcszTemp = szAnimFileInfo;

	lea	eax, DWORD PTR _szAnimFileInfo$[ebp]
	mov	DWORD PTR _pcszTemp$[ebp], eax

; 286  :         m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);

	push	100					; 00000064H
	lea	ecx, DWORD PTR _pcszTemp$[ebp]
	push	ecx
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8
	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 4
	imul	edx, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+edx+600], eax

; 287  :         KSG_StringSkipSymbol(&pcszTemp, ',');

	push	44					; 0000002cH
	lea	edx, DWORD PTR _pcszTemp$[ebp]
	push	edx
	call	DWORD PTR __imp_?KSG_StringSkipSymbol@@YA_NPAPBDH@Z
	add	esp, 8

; 288  :         m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir = KSG_StringGetInt(&pcszTemp, 16);

	push	16					; 00000010H
	lea	eax, DWORD PTR _pcszTemp$[ebp]
	push	eax
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8
	mov	ecx, DWORD PTR _i$[ebp]
	add	ecx, 4
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+ecx+608], eax

; 289  :         KSG_StringSkipSymbol(&pcszTemp, ',');

	push	44					; 0000002cH
	lea	eax, DWORD PTR _pcszTemp$[ebp]
	push	eax
	call	DWORD PTR __imp_?KSG_StringSkipSymbol@@YA_NPAPBDH@Z
	add	esp, 8

; 290  :         m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval = KSG_StringGetInt(&pcszTemp, 1);

	push	1
	lea	ecx, DWORD PTR _pcszTemp$[ebp]
	push	ecx
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8
	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 4
	imul	edx, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+edx+604], eax

; 291  : 
; 292  : 		//sscanf(szAnimFileInfo, "%d,%d,%d", 
; 293  : 		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame,
; 294  : 		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir,
; 295  : 		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval
; 296  :         //);
; 297  : 		
; 298  : 	}

	jmp	$L105182
$L105183:

; 299  : 	pMisslesSetting->GetInteger(nRow, "LoopPlay",			0, &m_MissleRes.m_bLoopAnim, TRUE);

	push	1
	mov	edx, DWORD PTR _this$[ebp]
	add	edx, 496				; 000001f0H
	push	edx
	push	0
	push	OFFSET FLAT:$SG105196
	mov	eax, DWORD PTR _nRow$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 300  : 	pMisslesSetting->GetInteger(nRow, "SubLoop",		0, &m_MissleRes.m_bSubLoop, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 2248				; 000008c8H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105197
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 301  : 	pMisslesSetting->GetInteger(nRow, "SubStart",		0, &m_MissleRes.m_nSubStart, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 2252				; 000008ccH
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105198
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 302  : 	pMisslesSetting->GetInteger(nRow, "SubStop",		0, &m_MissleRes.m_nSubStop, TRUE);

	push	1
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 2256				; 000008d0H
	push	eax
	push	0
	push	OFFSET FLAT:$SG105199
	mov	ecx, DWORD PTR _nRow$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pMisslesSetting$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [eax+48]

; 303  : 	pMisslesSetting->GetInteger(nRow, "ColFollowTarget",0, (int *)&m_bFollowNpcWhenCollid, TRUE);

	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 448				; 000001c0H
	push	ecx
	push	0
	push	OFFSET FLAT:$SG105201
	mov	edx, DWORD PTR _nRow$[ebp]
	push	edx
	mov	eax, DWORD PTR _pMisslesSetting$[ebp]
	mov	edx, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pMisslesSetting$[ebp]
	call	DWORD PTR [edx+48]

; 304  : #endif
; 305  : 	return TRUE;

	mov	eax, 1
$L105133:

; 306  : }

	mov	esp, ebp
	pop	ebp
	ret	8
?GetInfoFromTabFile@KMissle@@QAEHPAVKITabFile@@H@Z ENDP	; KMissle::GetInfoFromTabFile
_TEXT	ENDS
PUBLIC	?Init@KMissle@@AAEHHHHHH@Z			; KMissle::Init
EXTRN	?Init@KMissleRes@@QAEHXZ:NEAR			; KMissleRes::Init
_TEXT	SEGMENT
_this$ = -4
?Init@KMissle@@AAEHHHHHH@Z PROC NEAR			; KMissle::Init

; 309  : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 310  : #ifndef _SERVER
; 311  : 	m_MissleRes.Init();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?Init@KMissleRes@@QAEHXZ		; KMissleRes::Init

; 312  : #endif
; 313  : 	return	TRUE;

	mov	eax, 1

; 314  : }

	mov	esp, ebp
	pop	ebp
	ret	20					; 00000014H
?Init@KMissle@@AAEHHHHHH@Z ENDP				; KMissle::Init
_TEXT	ENDS
PUBLIC	?DoVanish@KMissle@@AAEXXZ			; KMissle::DoVanish
PUBLIC	?ProcessCollision@KMissle@@AAEHXZ		; KMissle::ProcessCollision
PUBLIC	?Activate@KMissle@@QAEHXZ			; KMissle::Activate
PUBLIC	?IsMatch@KNpc@@QAEHK@Z				; KNpc::IsMatch
PUBLIC	?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z	; KSkillManager::GetSkill
PUBLIC	?OnVanish@KMissle@@AAEXXZ			; KMissle::OnVanish
PUBLIC	?OnCollision@KMissle@@AAEXXZ			; KMissle::OnCollision
PUBLIC	?OnFly@KMissle@@AAEXXZ				; KMissle::OnFly
PUBLIC	?OnWait@KMissle@@AAEXXZ				; KMissle::OnWait
PUBLIC	?DoFly@KMissle@@AAEXXZ				; KMissle::DoFly
PUBLIC	?PrePareFly@KMissle@@AAEHXZ			; KMissle::PrePareFly
EXTRN	?Npc@@3PAVKNpc@@A:BYTE				; Npc
EXTRN	?FlyEvent@KSkill@@ABEXPAVKMissle@@@Z:NEAR	; KSkill::FlyEvent
EXTRN	?PlaySoundA@KMissleRes@@QAEXHHHH@Z:NEAR		; KMissleRes::PlaySoundA
EXTRN	?g_SkillManager@@3VKSkillManager@@A:BYTE	; g_SkillManager
EXTRN	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z:NEAR	; KSubWorld::Map2Mps
EXTRN	?SubWorld@@3PAVKSubWorld@@A:BYTE		; SubWorld
EXTRN	?MoveObject@KScenePlaceC@@QAEIIHHHHAAIH@Z:NEAR	; KScenePlaceC::MoveObject
_TEXT	SEGMENT
_this$ = -28
_eLastStatus$ = -4
_nSrcX2$105226 = -12
_nSrcY2$105227 = -8
_pOrdinSkill$105239 = -16
_nSrcX$105245 = -20
_nSrcY$105246 = -24
?Activate@KMissle@@QAEHXZ PROC NEAR			; KMissle::Activate

; 327  : {	

	push	ebp
	mov	ebp, esp
	sub	esp, 32					; 00000020H
	mov	DWORD PTR _this$[ebp], ecx

; 328  : 	if (m_nMissleId <= 0 || m_nRegionId < 0)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+360], 0
	jle	SHORT $L105214
	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+368], 0
	jge	SHORT $L105213
$L105214:

; 330  : 		return  0 ;

	xor	eax, eax
	jmp	$L105212
$L105213:

; 334  : 	if (m_nLauncher <= 0)

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+336], 0
	jg	SHORT $L105216

; 335  : 		return 0;

	xor	eax, eax
	jmp	$L105212
$L105216:

; 336  : 	
; 337  : 	//子弹的主人已经离开，So 子弹消亡
; 338  : 	if (!Npc[m_nLauncher].IsMatch(m_dwLauncherId) || Npc[m_nLauncher].m_SubWorldIndex != m_nSubWorldId || Npc[m_nLauncher].m_RegionIndex < 0)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+340]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+336]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?IsMatch@KNpc@@QAEHK@Z			; KNpc::IsMatch
	test	eax, eax
	je	SHORT $L105218
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+336]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3060]
	cmp	eax, DWORD PTR [edx+364]
	jne	SHORT $L105218
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+336]
	imul	edx, 33600				; 00008340H
	cmp	DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3064], 0
	jge	SHORT $L105217
$L105218:

; 340  : 		DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 341  : 		return 0;	

	xor	eax, eax
	jmp	$L105212
$L105217:

; 343  : 	
; 344  : 	//跟踪的目标人物已经不在该地图上时，自动清空
; 345  : 	if (m_nFollowNpcIdx > 0)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+328], 0
	jle	SHORT $L105220

; 347  : // 		if (!Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID) || Npc[m_nFollowNpcIdx].m_SubWorldIndex != m_nSubWorldId)
; 348  : // 		{
; 349  : // 			m_nFollowNpcIdx = 0;
; 350  : // 		}
; 351  : 		if (Npc[m_nFollowNpcIdx].m_SubWorldIndex != m_nSubWorldId)

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+328]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3060]
	cmp	ecx, DWORD PTR [eax+364]
	je	SHORT $L105220

; 353  : 			m_nFollowNpcIdx = 0;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+328], 0
$L105220:

; 356  : 	
; 357  : 	eMissleStatus eLastStatus = m_eMissleStatus;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+356]
	mov	DWORD PTR _eLastStatus$[ebp], ecx

; 361  : 		m_nCurrentLife >= m_nLifeTime 
; 362  : 		&& m_eMissleStatus != MS_DoVanish 
; 363  : 		&& m_eMissleStatus != MS_DoCollision
; 364  : 		)

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+208]
	cmp	ecx, DWORD PTR [eax+96]
	jl	SHORT $L105222
	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+356], 2
	je	SHORT $L105222
	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+356], 3
	je	SHORT $L105222

; 366  : 		if (m_bAutoExplode)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+116], 0
	je	SHORT $L105223

; 368  : 			ProcessCollision();//处理碰撞

	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessCollision@KMissle@@AAEHXZ	; KMissle::ProcessCollision
$L105223:

; 370  : 		DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish
$L105222:

; 376  : 	
; 377  : 	if (m_nCurrentLife == m_nStartLifeTime && m_eMissleStatus != MS_DoVanish)	

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+208]
	cmp	ecx, DWORD PTR [eax+212]
	jne	$L105228
	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+356], 2
	je	$L105228

; 379  : 		if (PrePareFly())

	mov	ecx, DWORD PTR _this$[ebp]
	call	?PrePareFly@KMissle@@AAEHXZ		; KMissle::PrePareFly
	test	eax, eax
	je	SHORT $L105225

; 381  : #ifndef _SERVER
; 382  : 			int nSrcX2 = 0 ;

	mov	DWORD PTR _nSrcX2$105226[ebp], 0

; 383  : 			int nSrcY2 = 0 ;

	mov	DWORD PTR _nSrcY2$105227[ebp], 0

; 384  : 			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);

	lea	eax, DWORD PTR _nSrcY2$105227[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSrcX2$105226[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+236]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 385  : 			m_MissleRes.PlaySound(MS_DoFly, nSrcX2, nSrcY2, 0);

	push	0
	mov	eax, DWORD PTR _nSrcY2$105227[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX2$105226[ebp]
	push	ecx
	push	1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?PlaySoundA@KMissleRes@@QAEXHHHH@Z	; KMissleRes::PlaySoundA

; 386  : 			//CreateSpecialEffect(MS_DoFly, nSrcX2, nSrcY2, m_nCurrentMapZ);
; 387  : #endif
; 388  : 			
; 389  : 			DoFly();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoFly@KMissle@@AAEXXZ			; KMissle::DoFly

; 391  : 		else

	jmp	SHORT $L105228
$L105225:

; 392  : 			DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish
$L105228:

; 396  : 	{

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+356]
	mov	DWORD PTR -32+[ebp], eax
	cmp	DWORD PTR -32+[ebp], 3
	ja	$L105230
	mov	ecx, DWORD PTR -32+[ebp]
	jmp	DWORD PTR $L106139[ecx*4]
$L105233:

; 399  : 			OnWait();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?OnWait@KMissle@@AAEXXZ			; KMissle::OnWait

; 401  : 		break;

	jmp	$L105230
$L105234:

; 404  : 			OnFly();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?OnFly@KMissle@@AAEXXZ			; KMissle::OnFly

; 405  : 			if (m_bFlyEvent)

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+176], 0
	je	SHORT $L105241

; 407  : 				if ( (m_nCurrentLife - m_nStartLifeTime) % m_nFlyEventTime == 0 )

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+208]
	sub	eax, DWORD PTR [ecx+212]
	mov	ecx, DWORD PTR _this$[ebp]
	cdq
	idiv	DWORD PTR [ecx+180]
	test	edx, edx
	jne	SHORT $L105241

; 409  : 					_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
; 410  : 					if (m_nLevel  <= 0 ) return 0;

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+324], 0
	jg	SHORT $L105238
	xor	eax, eax
	jmp	$L105212
$L105238:

; 411  : 					KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId , m_nLevel);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+324]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+104]
	push	eax
	mov	ecx, OFFSET FLAT:?g_SkillManager@@3VKSkillManager@@A
	call	?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z ; KSkillManager::GetSkill
	mov	DWORD PTR _pOrdinSkill$105239[ebp], eax

; 412  : 					if (pOrdinSkill)

	cmp	DWORD PTR _pOrdinSkill$105239[ebp], 0
	je	SHORT $L105241

; 414  : 						pOrdinSkill->FlyEvent(this);

	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	mov	ecx, DWORD PTR _pOrdinSkill$105239[ebp]
	call	?FlyEvent@KSkill@@ABEXPAVKMissle@@@Z	; KSkill::FlyEvent
$L105241:

; 419  : 		break;

	jmp	SHORT $L105230
$L105242:

; 422  : 			OnCollision();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?OnCollision@KMissle@@AAEXXZ		; KMissle::OnCollision

; 424  : 		break;

	jmp	SHORT $L105230
$L105243:

; 427  : 			OnVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?OnVanish@KMissle@@AAEXXZ		; KMissle::OnVanish
$L105230:

; 431  : 	
; 432  : #ifndef _SERVER
; 433  : 	//子弹未消亡掉
; 434  : 	if (m_nMissleId > 0)

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+360], 0
	jle	$L105248

; 436  : 		int nSrcX;
; 437  : 		int nSrcY;
; 438  : 		
; 439  : 		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);

	lea	eax, DWORD PTR _nSrcY$105246[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSrcX$105245[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+236]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 440  : 		if (m_usLightRadius && m_eMissleStatus != MS_DoWait)

	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax+464]
	test	ecx, ecx
	je	SHORT $L105247
	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+356], 0
	je	SHORT $L105247

; 441  : 			g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_LIGHT_PROP );

	push	10					; 0000000aH
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 2936				; 00000b78H
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY$105246[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX$105245[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+360]
	push	eax
	push	9
	mov	ecx, OFFSET FLAT:?g_ScenePlace@@3VKScenePlaceC@@A
	call	?MoveObject@KScenePlaceC@@QAEIIHHHHAAIH@Z ; KScenePlaceC::MoveObject

; 442  : 		else

	jmp	SHORT $L105248
$L105247:

; 443  : 			g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT);

	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 2936				; 00000b78H
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+228]
	push	eax
	mov	ecx, DWORD PTR _nSrcY$105246[ebp]
	push	ecx
	mov	edx, DWORD PTR _nSrcX$105245[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+360]
	push	ecx
	push	9
	mov	ecx, OFFSET FLAT:?g_ScenePlace@@3VKScenePlaceC@@A
	call	?MoveObject@KScenePlaceC@@QAEIIHHHHAAIH@Z ; KScenePlaceC::MoveObject
$L105248:

; 445  : 	
; 446  : #endif
; 447  : 	m_nCurrentLife ++;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+208]
	add	eax, 1
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+208], eax

; 448  : 	return 1;

	mov	eax, 1
$L105212:

; 449  : }

	mov	esp, ebp
	pop	ebp
	ret	0
$L106139:
	DD	$L105233
	DD	$L105234
	DD	$L105243
	DD	$L105242
?Activate@KMissle@@QAEHXZ ENDP				; KMissle::Activate
_TEXT	ENDS
;	COMDAT ?IsMatch@KNpc@@QAEHK@Z
_TEXT	SEGMENT
_dwID$ = 8
_this$ = -4
?IsMatch@KNpc@@QAEHK@Z PROC NEAR			; KNpc::IsMatch, COMDAT

; 599  : 	BOOL				IsMatch(DWORD dwID)	{ return dwID == m_dwID; };	// 是否ID与该Index匹配

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _dwID$[ebp]
	xor	edx, edx
	cmp	ecx, DWORD PTR [eax]
	sete	dl
	mov	eax, edx
	mov	esp, ebp
	pop	ebp
	ret	4
?IsMatch@KNpc@@QAEHK@Z ENDP				; KNpc::IsMatch
_TEXT	ENDS
EXTRN	?InstanceSkill@KSkillManager@@AAEPAVISkill@@KK@Z:NEAR ; KSkillManager::InstanceSkill
;	COMDAT ?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z
_TEXT	SEGMENT
_nSkillID$ = 8
_nSkillLevel$ = 12
_this$ = -12
_ulSkillID$ = -8
_ulSkillLevel$ = -4
?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z PROC NEAR	; KSkillManager::GetSkill, COMDAT

; 75   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	mov	DWORD PTR _this$[ebp], ecx

; 76   : //	_ASSERT((nSkillID <= MAX_SKILL)    && (nSkillID > 0));
; 77   : //	_ASSERT((nSkillLevel <= MAX_SKILLLEVEL) && (nSkillLevel > 0));
; 78   : 
; 79   : 	if (nSkillID > MAX_SKILL || nSkillID <= 0 || nSkillLevel <= 0 || nSkillLevel > MAX_SKILLLEVEL)

	cmp	DWORD PTR _nSkillID$[ebp], 2000		; 000007d0H
	jg	SHORT $L95818
	cmp	DWORD PTR _nSkillID$[ebp], 0
	jle	SHORT $L95818
	cmp	DWORD PTR _nSkillLevel$[ebp], 0
	jle	SHORT $L95818
	cmp	DWORD PTR _nSkillLevel$[ebp], 64	; 00000040H
	jle	SHORT $L95817
$L95818:

; 80   : 		return NULL;

	xor	eax, eax
	jmp	SHORT $L95816
$L95817:

; 81   : 
; 82   :     unsigned long ulSkillID = nSkillID;

	mov	eax, DWORD PTR _nSkillID$[ebp]
	mov	DWORD PTR _ulSkillID$[ebp], eax

; 83   :     unsigned long ulSkillLevel = nSkillLevel;

	mov	ecx, DWORD PTR _nSkillLevel$[ebp]
	mov	DWORD PTR _ulSkillLevel$[ebp], ecx

; 84   : 
; 85   :     if (m_pOrdinSkill[ulSkillID - 1][ulSkillLevel - 1])

	mov	edx, DWORD PTR _ulSkillID$[ebp]
	sub	edx, 1
	shl	edx, 8
	mov	eax, DWORD PTR _this$[ebp]
	lea	ecx, DWORD PTR [eax+edx+32000]
	mov	edx, DWORD PTR _ulSkillLevel$[ebp]
	cmp	DWORD PTR [ecx+edx*4-4], 0
	je	SHORT $L95821

; 87   :         return m_pOrdinSkill[ulSkillID - 1][ulSkillLevel - 1];

	mov	eax, DWORD PTR _ulSkillID$[ebp]
	sub	eax, 1
	shl	eax, 8
	mov	ecx, DWORD PTR _this$[ebp]
	lea	edx, DWORD PTR [ecx+eax+32000]
	mov	eax, DWORD PTR _ulSkillLevel$[ebp]
	mov	eax, DWORD PTR [edx+eax*4-4]
	jmp	SHORT $L95816
$L95821:

; 89   : 
; 90   : 	return InstanceSkill(ulSkillID , ulSkillLevel);

	mov	ecx, DWORD PTR _ulSkillLevel$[ebp]
	push	ecx
	mov	edx, DWORD PTR _ulSkillID$[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?InstanceSkill@KSkillManager@@AAEPAVISkill@@KK@Z ; KSkillManager::InstanceSkill
$L95816:

; 91   : }

	mov	esp, ebp
	pop	ebp
	ret	8
?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z ENDP	; KSkillManager::GetSkill
_TEXT	ENDS
_TEXT	SEGMENT
_this$ = -4
?OnWait@KMissle@@AAEXXZ PROC NEAR			; KMissle::OnWait

; 459  : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 460  : 	return;
; 461  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?OnWait@KMissle@@AAEXXZ ENDP				; KMissle::OnWait
_this$ = -4
?OnCollision@KMissle@@AAEXXZ PROC NEAR			; KMissle::OnCollision

; 470  : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 471  : 	return;	
; 472  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?OnCollision@KMissle@@AAEXXZ ENDP			; KMissle::OnCollision
_TEXT	ENDS
PUBLIC	?DoCollision@KMissle@@AAEXXZ			; KMissle::DoCollision
PUBLIC	?CheckNearestCollision@KMissle@@AAEHXZ		; KMissle::CheckNearestCollision
PUBLIC	?CheckCollision@KMissle@@AAEHXZ			; KMissle::CheckCollision
PUBLIC	?ProcessCollision@KMissle@@AAEHHHHHHH@Z		; KMissle::ProcessCollision
PUBLIC	?GetOffsetAxis@KMissle@@SAHHHHHHHAAH00@Z	; KMissle::GetOffsetAxis
PUBLIC	?FindNpc@KRegion@@QAEHHHHH@Z			; KRegion::FindNpc
_TEXT	SEGMENT
_this$ = -76
_nAbsX$ = -56
_nAbsY$ = -64
_nCellWidth$ = -20
_nCellHeight$ = -40
_nRMx$ = -32
_nRMy$ = -36
_nSearchRegion$ = -48
_nNpcIdx$ = -24
_nDX$ = -8
_nDY$ = -16
_nNpcOffsetX$ = -12
_nNpcOffsetY$ = -4
_bCollision$ = -28
_nColRegion$ = -52
_nColMapX$ = -44
_nColMapY$ = -60
_i$105286 = -68
_j$105290 = -72
?CheckCollision@KMissle@@AAEHXZ PROC NEAR		; KMissle::CheckCollision

; 476  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 76					; 0000004cH
	mov	DWORD PTR _this$[ebp], ecx

; 477  : #ifdef TOOLVERSION
; 478  : 	return FALSE;
; 479  : #endif
; 480  : 	
; 481  : 	if (m_nCurrentMapZ <= MISSLE_MIN_COLLISION_ZHEIGHT) 

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+228], 0
	jg	SHORT $L105258

; 483  : 		return -1;

	or	eax, -1
	jmp	$L105257
$L105258:

; 485  : 	
; 486  : 	//子弹在高于一定高度时，不处理越界碰撞问题
; 487  : 	if (m_nCurrentMapZ > MISSLE_MAX_COLLISION_ZHEIGHT) return 0;

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+228], 20			; 00000014H
	jle	SHORT $L105259
	xor	eax, eax
	jmp	$L105257
$L105259:

; 488  : 	
; 489  : 	if (m_nRegionId < 0) 

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+368], 0
	jge	SHORT $L105260

; 491  : 		return -1;

	or	eax, -1
	jmp	$L105257
$L105260:

; 493  : 
; 494  : 	int nAbsX = 0;

	mov	DWORD PTR _nAbsX$[ebp], 0

; 495  : 	int nAbsY = 0;

	mov	DWORD PTR _nAbsY$[ebp], 0

; 496  : 	int nCellWidth = CellWidth;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+148]
	shl	edx, 10					; 0000000aH
	mov	DWORD PTR _nCellWidth$[ebp], edx

; 497  : 	int nCellHeight = CellHeight;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+152]
	shl	edx, 10					; 0000000aH
	mov	DWORD PTR _nCellHeight$[ebp], edx

; 498  : 	_ASSERT(nCellWidth > 0 && nCellHeight > 0);
; 499  : 	int nRMx = 0;

	mov	DWORD PTR _nRMx$[ebp], 0

; 500  : 	int nRMy = 0;

	mov	DWORD PTR _nRMy$[ebp], 0

; 501  : 	int nSearchRegion = 0;

	mov	DWORD PTR _nSearchRegion$[ebp], 0

; 502  : 	int nNpcIdx = 0;

	mov	DWORD PTR _nNpcIdx$[ebp], 0

; 503  : 	int nDX = 0;

	mov	DWORD PTR _nDX$[ebp], 0

; 504  : 	int nDY = 0;

	mov	DWORD PTR _nDY$[ebp], 0

; 505  : 	int nNpcOffsetX = 0;

	mov	DWORD PTR _nNpcOffsetX$[ebp], 0

; 506  : 	int nNpcOffsetY = 0;

	mov	DWORD PTR _nNpcOffsetY$[ebp], 0

; 507  : 	BOOL bCollision = FALSE;

	mov	DWORD PTR _bCollision$[ebp], 0

; 508  : 	
; 509  : 	int nColRegion = m_nRegionId;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	mov	DWORD PTR _nColRegion$[ebp], ecx

; 510  : 	int nColMapX = m_nCurrentMapX;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	mov	DWORD PTR _nColMapX$[ebp], eax

; 511  : 	int nColMapY = m_nCurrentMapY;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	mov	DWORD PTR _nColMapY$[ebp], edx

; 512  : 		
; 513  : 	if (m_nCollideRange == 1)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+148], 1
	jne	$L105278

; 515  : 		if (m_bNeedReclaim && m_nCurrentLife >= m_nFirstReclaimTime && m_nCurrentLife <= m_nEndReclaimTime)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+260], 0
	je	SHORT $L105279
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+208]
	cmp	ecx, DWORD PTR [eax+408]
	jl	SHORT $L105279
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+208]
	cmp	ecx, DWORD PTR [eax+412]
	jg	SHORT $L105279

; 517  : 			if (m_nCurrentLife == m_nEndReclaimTime) 

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+208]
	cmp	ecx, DWORD PTR [eax+412]
	jne	SHORT $L105280

; 518  : 				m_bNeedReclaim = FALSE;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+260], 0
$L105280:

; 519  : 			nNpcIdx = 	CheckNearestCollision();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?CheckNearestCollision@KMissle@@AAEHXZ	; KMissle::CheckNearestCollision
	mov	DWORD PTR _nNpcIdx$[ebp], eax

; 521  : 		else

	jmp	SHORT $L105281
$L105279:

; 523  : 			nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nColRegion].FindNpc(nColMapX, nColMapY, m_nLauncher, m_eRelation);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+112]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	push	eax
	mov	ecx, DWORD PTR _nColMapY$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nColMapX$[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nColRegion$[ebp]
	imul	edx, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	add	ecx, edx
	call	?FindNpc@KRegion@@QAEHHHHH@Z		; KRegion::FindNpc
	mov	DWORD PTR _nNpcIdx$[ebp], eax
$L105281:

; 525  : 
; 526  : 		if (nNpcIdx > 0)

	cmp	DWORD PTR _nNpcIdx$[ebp], 0
	jle	SHORT $L105282

; 528  : 			if (m_nDamageRange == 1)//在目标Npc处碰撞

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+152], 1
	jne	SHORT $L105283

; 529  : 				ProcessCollision(m_nLauncher, Npc[nNpcIdx].m_RegionIndex , Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, m_nDamageRange , m_eRelation);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+112]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+152]
	push	ecx
	mov	edx, DWORD PTR _nNpcIdx$[ebp]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3544]
	push	eax
	mov	ecx, DWORD PTR _nNpcIdx$[ebp]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3540]
	push	edx
	mov	eax, DWORD PTR _nNpcIdx$[ebp]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3064]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessCollision@KMissle@@AAEHHHHHHH@Z	; KMissle::ProcessCollision

; 530  : 			else

	jmp	SHORT $L105284
$L105283:

; 531  : 				ProcessCollision();//在子弹位置处理碰撞

	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessCollision@KMissle@@AAEHXZ	; KMissle::ProcessCollision
$L105284:

; 532  : 			DoCollision();//子弹作碰撞后的效果

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoCollision@KMissle@@AAEXXZ		; KMissle::DoCollision

; 533  : 			return 1;

	mov	eax, 1
	jmp	$L105257
$L105282:

; 536  : 	else

	jmp	$L105289
$L105278:

; 538  : 		for (int i = -m_nCollideRange; i <= m_nCollideRange; i ++)

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+148]
	neg	edx
	mov	DWORD PTR _i$105286[ebp], edx
	jmp	SHORT $L105287
$L105288:
	mov	eax, DWORD PTR _i$105286[ebp]
	add	eax, 1
	mov	DWORD PTR _i$105286[ebp], eax
$L105287:
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _i$105286[ebp]
	cmp	edx, DWORD PTR [ecx+148]
	jg	$L105289

; 539  : 			for (int j = -m_nCollideRange; j <= m_nCollideRange; j ++)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+148]
	neg	ecx
	mov	DWORD PTR _j$105290[ebp], ecx
	jmp	SHORT $L105291
$L105292:
	mov	edx, DWORD PTR _j$105290[ebp]
	add	edx, 1
	mov	DWORD PTR _j$105290[ebp], edx
$L105291:
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _j$105290[ebp]
	cmp	ecx, DWORD PTR [eax+148]
	jg	$L105293

; 541  : 				if (!GetOffsetAxis(m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, i , j , nSearchRegion, nRMx, nRMy))

	lea	edx, DWORD PTR _nRMy$[ebp]
	push	edx
	lea	eax, DWORD PTR _nRMx$[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSearchRegion$[ebp]
	push	ecx
	mov	edx, DWORD PTR _j$105290[ebp]
	push	edx
	mov	eax, DWORD PTR _i$105286[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	push	edx
	call	?GetOffsetAxis@KMissle@@SAHHHHHHHAAH00@Z ; KMissle::GetOffsetAxis
	add	esp, 36					; 00000024H
	test	eax, eax
	jne	SHORT $L105294

; 542  : 					continue;

	jmp	SHORT $L105292
$L105294:

; 545  : 				nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+112]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	push	eax
	mov	ecx, DWORD PTR _nRMy$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nRMx$[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nSearchRegion$[ebp]
	imul	edx, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	add	ecx, edx
	call	?FindNpc@KRegion@@QAEHHHHH@Z		; KRegion::FindNpc
	mov	DWORD PTR _nNpcIdx$[ebp], eax

; 546  : 				if (nNpcIdx > 0)

	cmp	DWORD PTR _nNpcIdx$[ebp], 0
	jle	SHORT $L105296

; 548  : 					ProcessCollision();//处理碰撞

	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessCollision@KMissle@@AAEHXZ	; KMissle::ProcessCollision

; 549  : 					DoCollision();//子弹作碰撞后的效果

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoCollision@KMissle@@AAEXXZ		; KMissle::DoCollision

; 550  : 					return 1;

	mov	eax, 1
	jmp	SHORT $L105257
$L105296:

; 552  : 			}

	jmp	$L105292
$L105293:
	jmp	$L105288
$L105289:

; 554  : 	
; 555  : 	return 0;

	xor	eax, eax
$L105257:

; 556  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?CheckCollision@KMissle@@AAEHXZ ENDP			; KMissle::CheckCollision
_TEXT	ENDS
PUBLIC	?GetNext@KNode@@QAEPAV1@XZ			; KNode::GetNext
PUBLIC	?GetHead@KList@@QAEPAVKNode@@XZ			; KList::GetHead
EXTRN	?GetRelation@KNpcSet@@QAE?AW4NPC_RELATION@@HH@Z:NEAR ; KNpcSet::GetRelation
EXTRN	?NpcSet@@3VKNpcSet@@A:BYTE			; NpcSet
;	COMDAT ?FindNpc@KRegion@@QAEHHHHH@Z
_TEXT	SEGMENT
_nMapX$ = 8
_nMapY$ = 12
_nNpcIdx$ = 16
_nRelation$ = 20
_this$ = -8
_pNode$ = -4
?FindNpc@KRegion@@QAEHHHHH@Z PROC NEAR			; KRegion::FindNpc, COMDAT

; 136  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx

; 137  : 	if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nMapY$[ebp]
	imul	ecx, DWORD PTR [eax+176]
	add	ecx, DWORD PTR _nMapX$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+196]
	xor	edx, edx
	mov	dl, BYTE PTR [eax+ecx]
	test	edx, edx
	jne	SHORT $L94956

; 138  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L94955
$L94956:

; 139  : 
; 140  : 	KIndexNode *pNode = NULL;

	mov	DWORD PTR _pNode$[ebp], 0

; 141  : 	
; 142  : 	pNode = (KIndexNode *)m_NpcList.GetHead();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 8
	call	?GetHead@KList@@QAEPAVKNode@@XZ		; KList::GetHead
	mov	DWORD PTR _pNode$[ebp], eax
$L94960:

; 143  : 	
; 144  : 	while(pNode)

	cmp	DWORD PTR _pNode$[ebp], 0
	je	SHORT $L94961

; 146  : 		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)

	mov	eax, DWORD PTR _pNode$[ebp]
	mov	ecx, DWORD PTR [eax+12]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3540]
	cmp	edx, DWORD PTR _nMapX$[ebp]
	jne	SHORT $L94963
	mov	eax, DWORD PTR _pNode$[ebp]
	mov	ecx, DWORD PTR [eax+12]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3544]
	cmp	edx, DWORD PTR _nMapY$[ebp]
	jne	SHORT $L94963

; 148  : 			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation)

	mov	eax, DWORD PTR _pNode$[ebp]
	mov	ecx, DWORD PTR [eax+12]
	push	ecx
	mov	edx, DWORD PTR _nNpcIdx$[ebp]
	push	edx
	mov	ecx, OFFSET FLAT:?NpcSet@@3VKNpcSet@@A
	call	?GetRelation@KNpcSet@@QAE?AW4NPC_RELATION@@HH@Z ; KNpcSet::GetRelation
	and	eax, DWORD PTR _nRelation$[ebp]
	test	eax, eax
	je	SHORT $L94963

; 150  : 				return pNode->m_nIndex;

	mov	eax, DWORD PTR _pNode$[ebp]
	mov	eax, DWORD PTR [eax+12]
	jmp	SHORT $L94955
$L94963:

; 153  : 		pNode = (KIndexNode *)pNode->GetNext();

	mov	ecx, DWORD PTR _pNode$[ebp]
	call	?GetNext@KNode@@QAEPAV1@XZ		; KNode::GetNext
	mov	DWORD PTR _pNode$[ebp], eax

; 154  : 	}	

	jmp	SHORT $L94960
$L94961:

; 155  : 	return 0;

	xor	eax, eax
$L94955:

; 156  : }

	mov	esp, ebp
	pop	ebp
	ret	16					; 00000010H
?FindNpc@KRegion@@QAEHHHHH@Z ENDP			; KRegion::FindNpc
_TEXT	ENDS
;	COMDAT ?GetNext@KNode@@QAEPAV1@XZ
_TEXT	SEGMENT
_this$ = -4
?GetNext@KNode@@QAEPAV1@XZ PROC NEAR			; KNode::GetNext, COMDAT

; 54   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 55   : 	if (m_pNext->m_pNext)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+4]
	cmp	DWORD PTR [ecx+4], 0
	je	SHORT $L83732

; 56   : 		return m_pNext;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+4]
	jmp	SHORT $L83731
$L83732:

; 57   : 	return NULL;

	xor	eax, eax
$L83731:

; 58   : }

	mov	esp, ebp
	pop	ebp
	ret	0
?GetNext@KNode@@QAEPAV1@XZ ENDP				; KNode::GetNext
_TEXT	ENDS
;	COMDAT ?GetHead@KList@@QAEPAVKNode@@XZ
_TEXT	SEGMENT
_this$ = -4
?GetHead@KList@@QAEPAVKNode@@XZ PROC NEAR		; KList::GetHead, COMDAT

; 62   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 63   : 	return m_ListHead.GetNext();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetNext@KNode@@QAEPAV1@XZ		; KNode::GetNext

; 64   : }

	mov	esp, ebp
	pop	ebp
	ret	0
?GetHead@KList@@QAEPAVKNode@@XZ ENDP			; KList::GetHead
_TEXT	ENDS
PUBLIC	?CheckBeyondRegion@KMissle@@AAEHHH@Z		; KMissle::CheckBeyondRegion
PUBLIC	?ProcessDamage@KMissle@@AAEHH@Z			; KMissle::ProcessDamage
PUBLIC	?ZAxisMove@KMissle@@AAEXXZ			; KMissle::ZAxisMove
PUBLIC	?TestBarrier@KMissle@@AAEHXZ			; KMissle::TestBarrier
PUBLIC	?GetMpsPos@KMissle@@QAEXPAH0@Z			; KMissle::GetMpsPos
PUBLIC	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect
PUBLIC	?g_DirSin@@YAHHH@Z				; g_DirSin
PUBLIC	?g_DirCos@@YAHHH@Z				; g_DirCos
PUBLIC	?g_GetDirIndex@@YAHHHHH@Z			; g_GetDirIndex
PUBLIC	?Send@KWorldMsg@@QAEHKHHH@Z			; KWorldMsg::Send
PUBLIC	__real@8@3fffa666666666666800
EXTRN	?GetMpsPos@KNpc@@QAEXPAH0@Z:NEAR		; KNpc::GetMpsPos
EXTRN	__ftol:NEAR
EXTRN	__fltused:NEAR
EXTRN	?AddRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z:NEAR	; KRegion::AddRef
EXTRN	?DecRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z:NEAR	; KRegion::DecRef
EXTRN	?GetDistance@KSubWorld@@QAEHHHHH@Z:NEAR		; KSubWorld::GetDistance
;	COMDAT __real@8@3fffa666666666666800
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\KMissle.cpp
CONST	SEGMENT
__real@8@3fffa666666666666800 DQ 03ff4cccccccccccdr ; 1.3
CONST	ENDS
_TEXT	SEGMENT
_this$ = -164
_nPX$105305 = -12
_nPY$105306 = -16
_nSrcX2$105309 = -24
_nSrcY2$105310 = -20
_nSrcX3$105312 = -32
_nSrcY3$105313 = -28
_nDOffsetX$ = -8
_nDOffsetY$ = -4
_nDistance$105324 = -52
_nSrcMpsX$105325 = -48
_nSrcMpsY$105326 = -44
_nDesMpsX$105327 = -36
_nDesMpsY$105328 = -40
_nXFactor$105330 = -56
_nYFactor$105331 = -60
_dx$105332 = -64
_dy$105333 = -68
_nPreAngle$105343 = -72
_dx$105346 = -76
_dy$105347 = -80
_nOldRegion$105350 = -84
_nPreAngle$105357 = -88
_dx$105361 = -92
_dy$105362 = -96
_nOldRegion$105365 = -100
_nDistance$105371 = -120
_nSrcMpsX$105372 = -116
_nSrcMpsY$105373 = -112
_nDesMpsX$105374 = -104
_nDesMpsY$105375 = -108
_nXFactor$105377 = -124
_nYFactor$105378 = -128
_dx$105379 = -132
_dy$105380 = -136
_x$105386 = -140
_y$105387 = -144
_dx$105388 = -148
_dy$105389 = -152
_nSrcX4$105395 = -156
_nSrcY4$105396 = -160
?OnFly@KMissle@@AAEXXZ PROC NEAR			; KMissle::OnFly

; 564  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 184				; 000000b8H
	push	esi
	push	edi
	mov	DWORD PTR _this$[ebp], ecx

; 565  : 	if (m_nInteruptTypeWhenMove)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+132], 0
	je	$L105307

; 567  : 		//当发送者位置移动了，不仅正从do_wait状态到do_fly状态的新子弹被消失掉
; 568  : 		//而且已进入dofly状态的旧的所属子弹也要强制消失掉
; 569  : 		if (m_nInteruptTypeWhenMove == Interupt_EndOldMissleLifeWhenMove)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+132], 2
	jne	$L105307

; 571  : 			int nPX, nPY;
; 572  : 			Npc[m_nLauncher].GetMpsPos(&nPX, &nPY);

	lea	edx, DWORD PTR _nPY$105306[ebp]
	push	edx
	lea	eax, DWORD PTR _nPX$105305[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+336]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?GetMpsPos@KNpc@@QAEXPAH0@Z		; KNpc::GetMpsPos

; 573  : 			if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nPX$105305[ebp]
	cmp	eax, DWORD PTR [edx+140]
	jne	SHORT $L105308
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nPY$105306[ebp]
	cmp	edx, DWORD PTR [ecx+144]
	je	$L105307
$L105308:

; 575  : 				
; 576  : #ifndef _SERVER 
; 577  : 				int nSrcX2 = 0 ;

	mov	DWORD PTR _nSrcX2$105309[ebp], 0

; 578  : 				int nSrcY2 = 0 ;

	mov	DWORD PTR _nSrcY2$105310[ebp], 0

; 579  : 				SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);

	lea	eax, DWORD PTR _nSrcY2$105310[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSrcX2$105309[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+236]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 580  : 				CreateSpecialEffect(MS_DoVanish, nSrcX2, nSrcY2, m_nCurrentMapZ);

	push	0
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+228]
	push	ecx
	mov	edx, DWORD PTR _nSrcY2$105310[ebp]
	push	edx
	mov	eax, DWORD PTR _nSrcX2$105309[ebp]
	push	eax
	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 581  : #endif
; 582  : 				
; 583  : 				DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 584  : 				return ;

	jmp	$L105302
$L105307:

; 588  : 	
; 589  : 	//检测当前位置是否有障碍
; 590  : 	if (TestBarrier()) 

	mov	ecx, DWORD PTR _this$[ebp]
	call	?TestBarrier@KMissle@@AAEHXZ		; KMissle::TestBarrier
	test	eax, eax
	je	$L105311

; 592  : #ifndef _SERVER 
; 593  : 		int nSrcX3 = 0 ;

	mov	DWORD PTR _nSrcX3$105312[ebp], 0

; 594  : 		int nSrcY3 = 0 ;

	mov	DWORD PTR _nSrcY3$105313[ebp], 0

; 595  : 		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX3, &nSrcY3);

	lea	ecx, DWORD PTR _nSrcY3$105313[ebp]
	push	ecx
	lea	edx, DWORD PTR _nSrcX3$105312[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+232]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 596  : 		CreateSpecialEffect(MS_DoVanish, nSrcX3, nSrcY3, m_nCurrentMapZ);

	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY3$105313[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX3$105312[ebp]
	push	ecx
	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 597  : #endif
; 598  : 		DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 599  : 		return;

	jmp	$L105302
$L105311:

; 601  : 	
; 602  : 	int nDOffsetX = 0;

	mov	DWORD PTR _nDOffsetX$[ebp], 0

; 603  : 	int nDOffsetY = 0;

	mov	DWORD PTR _nDOffsetY$[ebp], 0

; 604  : 	
; 605  : 	ZAxisMove();			

	mov	ecx, DWORD PTR _this$[ebp]
	call	?ZAxisMove@KMissle@@AAEXXZ		; KMissle::ZAxisMove

; 607  : 	{

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+80]
	mov	DWORD PTR -168+[ebp], eax
	mov	ecx, DWORD PTR -168+[ebp]
	sub	ecx, 1
	mov	DWORD PTR -168+[ebp], ecx
	cmp	DWORD PTR -168+[ebp], 99		; 00000063H
	ja	$L105390
	mov	eax, DWORD PTR -168+[ebp]
	xor	edx, edx
	mov	dl, BYTE PTR $L106157[eax]
	jmp	DWORD PTR $L106158[edx*4]
$L105321:

; 616  : 			if(this->m_nFollowNpcIdx > 0 && this->m_eFollowKind == 2)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+328], 0
	jle	$L105322
	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+84], 2
	jne	$L105322

; 618  : 				//				m_nFollowNpcIdx = 2;
; 619  : 				if(m_nTempParam1 > 5)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+416], 5
	jle	$L105323

; 621  : 					int nDistance = 0;

	mov	DWORD PTR _nDistance$105324[ebp], 0

; 622  : 					int nSrcMpsX = 0;

	mov	DWORD PTR _nSrcMpsX$105325[ebp], 0

; 623  : 					int nSrcMpsY = 0;

	mov	DWORD PTR _nSrcMpsY$105326[ebp], 0

; 624  : 					int nDesMpsX = 0;

	mov	DWORD PTR _nDesMpsX$105327[ebp], 0

; 625  : 					int nDesMpsY = 0;

	mov	DWORD PTR _nDesMpsY$105328[ebp], 0

; 626  : 					
; 627  : 					GetMpsPos(&nSrcMpsX,&nSrcMpsY);

	lea	ecx, DWORD PTR _nSrcMpsY$105326[ebp]
	push	ecx
	lea	edx, DWORD PTR _nSrcMpsX$105325[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetMpsPos@KMissle@@QAEXPAH0@Z		; KMissle::GetMpsPos

; 628  : 					Npc[m_nFollowNpcIdx].GetMpsPos(&nDesMpsX,&nDesMpsY);

	lea	eax, DWORD PTR _nDesMpsY$105328[ebp]
	push	eax
	lea	ecx, DWORD PTR _nDesMpsX$105327[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+328]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?GetMpsPos@KNpc@@QAEXPAH0@Z		; KNpc::GetMpsPos

; 629  : 					nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX,nSrcMpsY,nDesMpsX,nDesMpsY);

	mov	eax, DWORD PTR _nDesMpsY$105328[ebp]
	push	eax
	mov	ecx, DWORD PTR _nDesMpsX$105327[ebp]
	push	ecx
	mov	edx, DWORD PTR _nSrcMpsY$105326[ebp]
	push	edx
	mov	eax, DWORD PTR _nSrcMpsX$105325[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?GetDistance@KSubWorld@@QAEHHHHH@Z	; KSubWorld::GetDistance
	mov	DWORD PTR _nDistance$105324[ebp], eax

; 630  : 					if (nDistance > 20)

	cmp	DWORD PTR _nDistance$105324[ebp], 20	; 00000014H
	jle	$L105329

; 632  : 						int nXFactor = ((nDesMpsX - nSrcMpsX) << 10)/nDistance;

	mov	eax, DWORD PTR _nDesMpsX$105327[ebp]
	sub	eax, DWORD PTR _nSrcMpsX$105325[ebp]
	shl	eax, 10					; 0000000aH
	cdq
	idiv	DWORD PTR _nDistance$105324[ebp]
	mov	DWORD PTR _nXFactor$105330[ebp], eax

; 633  : 						int nYFactor = ((nDesMpsY - nSrcMpsY) << 10)/nDistance;

	mov	eax, DWORD PTR _nDesMpsY$105328[ebp]
	sub	eax, DWORD PTR _nSrcMpsY$105326[ebp]
	shl	eax, 10					; 0000000aH
	cdq
	idiv	DWORD PTR _nDistance$105324[ebp]
	mov	DWORD PTR _nYFactor$105331[ebp], eax

; 634  : 						int dx = nXFactor * m_nSpeed / 1.3;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nXFactor$105330[ebp]
	imul	eax, DWORD PTR [edx+100]
	mov	DWORD PTR -172+[ebp], eax
	fild	DWORD PTR -172+[ebp]
	fdiv	QWORD PTR __real@8@3fffa666666666666800
	call	__ftol
	mov	DWORD PTR _dx$105332[ebp], eax

; 635  : 						int dy = nYFactor * m_nSpeed / 1.3;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nYFactor$105331[ebp]
	imul	edx, DWORD PTR [ecx+100]
	mov	DWORD PTR -176+[ebp], edx
	fild	DWORD PTR -176+[ebp]
	fdiv	QWORD PTR __real@8@3fffa666666666666800
	call	__ftol
	mov	DWORD PTR _dy$105333[ebp], eax

; 636  : 						nDOffsetX = dx;

	mov	eax, DWORD PTR _dx$105332[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], eax

; 637  : 						nDOffsetY = dy;

	mov	ecx, DWORD PTR _dy$105333[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], ecx

; 638  : 						m_nDir = g_GetDirIndex(nSrcMpsX,nSrcMpsY,nDesMpsX,nDesMpsY);

	mov	edx, DWORD PTR _nDesMpsY$105328[ebp]
	push	edx
	mov	eax, DWORD PTR _nDesMpsX$105327[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcMpsY$105326[ebp]
	push	ecx
	mov	edx, DWORD PTR _nSrcMpsX$105325[ebp]
	push	edx
	call	?g_GetDirIndex@@YAHHHHH@Z		; g_GetDirIndex
	add	esp, 16					; 00000010H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+428], eax

; 640  : 					else

	jmp	SHORT $L105334
$L105329:

; 642  : 						ProcessDamage(m_nFollowNpcIdx);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+328]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessDamage@KMissle@@AAEHH@Z		; KMissle::ProcessDamage

; 643  : 						DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 644  : 						return;

	jmp	$L105302
$L105334:

; 646  : 				}else{

	jmp	SHORT $L105335
$L105323:

; 647  : 					nDOffsetX    = (m_nSpeed * m_nXFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+316]
	mov	DWORD PTR _nDOffsetX$[ebp], eax

; 648  : 					nDOffsetY	 = (m_nSpeed * m_nYFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+320]
	mov	DWORD PTR _nDOffsetY$[ebp], eax

; 649  : 					m_nTempParam1 ++;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+416]
	add	edx, 1
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+416], edx
$L105335:

; 652  : 			else

	jmp	SHORT $L105336
$L105322:

; 654  : 				nDOffsetX    = (m_nSpeed * m_nXFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+316]
	mov	DWORD PTR _nDOffsetX$[ebp], eax

; 655  : 				nDOffsetY	 = (m_nSpeed * m_nYFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+320]
	mov	DWORD PTR _nDOffsetY$[ebp], eax
$L105336:

; 658  : 		break;

	jmp	$L105317
$L105337:

; 661  : 			if (!m_nTempParam1)	

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+416], 0
	jne	$L105340

; 663  : 				if (m_nTempParam2 <= m_nCurrentLife)

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+420]
	cmp	ecx, DWORD PTR [eax+208]
	jg	$L105340

; 665  : 					m_nXFactor = -m_nXFactor;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+316]
	neg	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+316], eax

; 666  : 					m_nYFactor = -m_nYFactor;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+320]
	neg	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+320], eax

; 667  : 					m_nTempParam1 = 1;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+416], 1

; 668  : 					m_nDir = m_nDir - MaxMissleDir / 2;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+428]
	sub	ecx, 32					; 00000020H
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+428], ecx

; 669  : 					if (m_nDir < 0) m_nDir += MaxMissleDir;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+428], 0
	jge	SHORT $L105340
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+428]
	add	edx, 64					; 00000040H
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+428], edx
$L105340:

; 672  : 
; 673  : 			nDOffsetX = (m_nSpeed * m_nXFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+316]
	mov	DWORD PTR _nDOffsetX$[ebp], eax

; 674  : 			nDOffsetY = (m_nSpeed * m_nYFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+320]
	mov	DWORD PTR _nDOffsetY$[ebp], eax

; 675  : 		}break;

	jmp	$L105317
$L105342:

; 687  : 			int nPreAngle = m_nAngle - 1;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+432]
	sub	edx, 1
	mov	DWORD PTR _nPreAngle$105343[ebp], edx

; 688  : 			if (nPreAngle < 0) nPreAngle = MaxMissleDir - 1;

	cmp	DWORD PTR _nPreAngle$105343[ebp], 0
	jge	SHORT $L105344
	mov	DWORD PTR _nPreAngle$105343[ebp], 63	; 0000003fH
$L105344:

; 689  : 			m_nDir = m_nAngle + (MaxMissleDir / 4);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+432]
	add	ecx, 16					; 00000010H
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+428], ecx

; 690  : 			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+428], 64			; 00000040H
	jl	SHORT $L105345
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+428]
	sub	edx, 64					; 00000040H
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+428], edx
$L105345:

; 691  : 			int dx = (m_nSpeed + 250)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle,MaxMissleDir)) ;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR [ecx+100]
	add	esi, 250				; 000000faH
	push	64					; 00000040H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+432]
	push	eax
	call	?g_DirCos@@YAHHH@Z			; g_DirCos
	add	esp, 8
	mov	edi, eax
	push	64					; 00000040H
	mov	ecx, DWORD PTR _nPreAngle$105343[ebp]
	push	ecx
	call	?g_DirCos@@YAHHH@Z			; g_DirCos
	add	esp, 8
	sub	edi, eax
	imul	esi, edi
	mov	DWORD PTR _dx$105346[ebp], esi

; 692  : 			int dy = (m_nSpeed + 50)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle, MaxMissleDir)) ; 

	mov	edx, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR [edx+100]
	add	esi, 50					; 00000032H
	push	64					; 00000040H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+432]
	push	ecx
	call	?g_DirSin@@YAHHH@Z			; g_DirSin
	add	esp, 8
	mov	edi, eax
	push	64					; 00000040H
	mov	edx, DWORD PTR _nPreAngle$105343[ebp]
	push	edx
	call	?g_DirSin@@YAHHH@Z			; g_DirSin
	add	esp, 8
	sub	edi, eax
	imul	esi, edi
	mov	DWORD PTR _dy$105347[ebp], esi

; 693  : 			
; 694  : 			if (m_nParam2) //原地转

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+400], 0
	je	SHORT $L105348

; 696  : 				nDOffsetX = dx;

	mov	ecx, DWORD PTR _dx$105346[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], ecx

; 697  : 				nDOffsetY = dy;

	mov	edx, DWORD PTR _dy$105347[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], edx

; 699  : 			else			// 围绕着发送者转

	jmp	$L105349
$L105348:

; 701  : 				int nOldRegion = m_nRegionId;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	mov	DWORD PTR _nOldRegion$105350[ebp], ecx

; 702  : 				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+224]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+220]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	add	ecx, eax
	call	?DecRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::DecRef

; 703  : 				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+336]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3064]
	mov	DWORD PTR [eax+368], ecx

; 704  : 				m_nCurrentMapX	= Npc[m_nLauncher].m_MapX;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3540]
	mov	DWORD PTR [ecx+220], edx

; 705  : 				m_nCurrentMapY	= Npc[m_nLauncher].m_MapY;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+336]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3544]
	mov	DWORD PTR [edx+224], eax

; 706  : 				m_nXOffset		= Npc[m_nLauncher].m_OffX;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+336]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3552]
	mov	DWORD PTR [eax+232], ecx

; 707  : 				m_nYOffset		= Npc[m_nLauncher].m_OffY;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3556]
	mov	DWORD PTR [ecx+236], edx

; 708  : 				CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	imul	ecx, 208				; 000000d0H
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	add	ecx, edx
	call	?AddRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::AddRef

; 709  : 				
; 710  : 				if (nOldRegion != m_nRegionId)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nOldRegion$105350[ebp]
	cmp	ecx, DWORD PTR [eax+368]
	je	SHORT $L105351

; 712  : 					SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+360]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	eax, DWORD PTR _nOldRegion$105350[ebp]
	push	eax
	push	4002					; 00000fa2H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A+172
	call	?Send@KWorldMsg@@QAEHKHHH@Z		; KWorldMsg::Send
$L105351:

; 714  : 				nDOffsetX = dx;

	mov	edx, DWORD PTR _dx$105346[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], edx

; 715  : 				nDOffsetY = dy;

	mov	eax, DWORD PTR _dy$105347[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], eax
$L105349:

; 717  : 			
; 718  : 			//顺时针还是逆时针
; 719  : 			if (m_nParam1)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+396], 0
	je	SHORT $L105352

; 721  : 				m_nAngle ++;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+432]
	add	eax, 1
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+432], eax

; 722  : 				if (m_nAngle >= MaxMissleDir)

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+432], 64			; 00000040H
	jl	SHORT $L105353

; 723  : 					m_nAngle = 0;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+432], 0
$L105353:

; 725  : 			else

	jmp	SHORT $L105355
$L105352:

; 727  : 				m_nAngle --;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+432]
	sub	edx, 1
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+432], edx

; 728  : 				if (m_nAngle < 0 )

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+432], 0
	jge	SHORT $L105355

; 729  : 					m_nAngle = MaxMissleDir - 1;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+432], 63			; 0000003fH
$L105355:

; 733  : 		break;

	jmp	$L105317
$L105356:

; 739  : 			int nPreAngle;
; 740  : 			if (m_nTempParam1 == 0)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+416], 0
	jne	SHORT $L105358

; 742  : // 				if ((Npc[m_nLauncher].m_Dir > 48 &&  Npc[m_nLauncher].m_Dir <= 64 ) 
; 743  : // 					||
; 744  : // 					 (Npc[m_nLauncher].m_Dir >=  0 && Npc[m_nLauncher].m_Dir <16))
; 745  : // 				{
; 746  : // 					m_nParam1 = 0;
; 747  : // 				} 
; 748  : // 				else if (Npc[m_nLauncher].m_Dir > 16 && Npc[m_nLauncher].m_Dir <= 48 )
; 749  : // 				{
; 750  : // 					m_nParam1 = 0;
; 751  : // 				}
; 752  : 				m_nParam1 = 0;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+396], 0

; 753  : 				m_nAngle = Npc[m_nLauncher].m_Dir;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3524]
	mov	DWORD PTR [ecx+432], edx

; 754  : 				m_nTempParam1 = 1;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+416], 1
$L105358:

; 756  : // 			if (m_nParam1)
; 757  : // 			{
; 758  : // 				nPreAngle = m_nAngle - 1;
; 759  : // 			}
; 760  : // 			else
; 761  : // 			{
; 762  : 				nPreAngle = m_nAngle + 1;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+432]
	add	edx, 1
	mov	DWORD PTR _nPreAngle$105357[ebp], edx

; 763  : //			}
; 764  : 			
; 765  : // 			if (nPreAngle < 0) 
; 766  : // 			{
; 767  : // 				nPreAngle = MaxMissleDir -1;
; 768  : // 			}
; 769  : 			if (nPreAngle > MaxMissleDir)

	cmp	DWORD PTR _nPreAngle$105357[ebp], 64	; 00000040H
	jle	SHORT $L105359

; 771  : 				nPreAngle = 0;

	mov	DWORD PTR _nPreAngle$105357[ebp], 0
$L105359:

; 773  : 
; 774  : 			m_nDir = m_nAngle + (MaxMissleDir / 4);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+432]
	add	ecx, 16					; 00000010H
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+428], ecx

; 775  : 
; 776  : 			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+428], 64			; 00000040H
	jl	SHORT $L105360
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+428]
	sub	edx, 64					; 00000040H
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+428], edx
$L105360:

; 777  : 
; 778  : 			int dx = (m_nSpeed + m_nCurrentLife + 275)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle, MaxMissleDir)) ;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+100]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+208]
	lea	esi, DWORD PTR [edx+ecx+275]
	push	64					; 00000040H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+432]
	push	eax
	call	?g_DirCos@@YAHHH@Z			; g_DirCos
	add	esp, 8
	mov	edi, eax
	push	64					; 00000040H
	mov	ecx, DWORD PTR _nPreAngle$105357[ebp]
	push	ecx
	call	?g_DirCos@@YAHHH@Z			; g_DirCos
	add	esp, 8
	sub	edi, eax
	imul	esi, edi
	mov	DWORD PTR _dx$105361[ebp], esi

; 779  : 			int dy = (m_nSpeed + m_nCurrentLife + 275)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle,MaxMissleDir)) ; 

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+100]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+208]
	lea	esi, DWORD PTR [eax+edx+275]
	push	64					; 00000040H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+432]
	push	ecx
	call	?g_DirSin@@YAHHH@Z			; g_DirSin
	add	esp, 8
	mov	edi, eax
	push	64					; 00000040H
	mov	edx, DWORD PTR _nPreAngle$105357[ebp]
	push	edx
	call	?g_DirSin@@YAHHH@Z			; g_DirSin
	add	esp, 8
	sub	edi, eax
	imul	esi, edi
	mov	DWORD PTR _dy$105362[ebp], esi

; 780  : 	
; 781  : 			
; 782  : 			if (m_nParam2) //原地转

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+400], 0
	je	SHORT $L105363

; 784  : 				nDOffsetX = dx;

	mov	ecx, DWORD PTR _dx$105361[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], ecx

; 785  : 				nDOffsetY = dy;

	mov	edx, DWORD PTR _dy$105362[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], edx

; 787  : 			else			// 围绕着发送者转

	jmp	$L105364
$L105363:

; 789  : 				int nOldRegion = m_nRegionId;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	mov	DWORD PTR _nOldRegion$105365[ebp], ecx

; 790  : 				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+224]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+220]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	add	ecx, eax
	call	?DecRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::DecRef

; 791  : 				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+336]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3064]
	mov	DWORD PTR [eax+368], ecx

; 792  : 				m_nCurrentMapX	= Npc[m_nLauncher].m_MapX;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3540]
	mov	DWORD PTR [ecx+220], edx

; 793  : 				m_nCurrentMapY	= Npc[m_nLauncher].m_MapY;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+336]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3544]
	mov	DWORD PTR [edx+224], eax

; 794  : 				m_nXOffset		= Npc[m_nLauncher].m_OffX + (35 * g_DirCos(Npc[m_nLauncher].m_Dir,64) + 100);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR [ecx+336]
	imul	esi, 33600				; 00008340H
	push	64					; 00000040H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3524]
	push	ecx
	call	?g_DirCos@@YAHHH@Z			; g_DirCos
	add	esp, 8
	imul	eax, 35					; 00000023H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[esi+3552]
	lea	eax, DWORD PTR [edx+eax+100]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+232], eax

; 795  : 				m_nYOffset		= Npc[m_nLauncher].m_OffY + (35 * g_DirSin(Npc[m_nLauncher].m_Dir,64) + 100);

	mov	edx, DWORD PTR _this$[ebp]
	mov	esi, DWORD PTR [edx+336]
	imul	esi, 33600				; 00008340H
	push	64					; 00000040H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+336]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3524]
	push	edx
	call	?g_DirSin@@YAHHH@Z			; g_DirSin
	add	esp, 8
	imul	eax, 35					; 00000023H
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[esi+3556]
	lea	edx, DWORD PTR [ecx+eax+100]
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+236], edx

; 796  : 				CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	imul	edx, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+8]
	add	ecx, edx
	call	?AddRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::AddRef

; 797  : 				
; 798  : 				if (nOldRegion != m_nRegionId)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nOldRegion$105365[ebp]
	cmp	ecx, DWORD PTR [eax+368]
	je	SHORT $L105366

; 800  : 					SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+360]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	eax, DWORD PTR _nOldRegion$105365[ebp]
	push	eax
	push	4002					; 00000fa2H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A+172
	call	?Send@KWorldMsg@@QAEHKHHH@Z		; KWorldMsg::Send
$L105366:

; 802  : 				nDOffsetX = dx;

	mov	edx, DWORD PTR _dx$105361[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], edx

; 803  : 				nDOffsetY = dy;

	mov	eax, DWORD PTR _dy$105362[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], eax
$L105364:

; 805  : 			
; 806  : // 			if (m_nParam1)
; 807  : // 			{
; 808  : 				m_nAngle ++;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+432]
	add	edx, 1
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+432], edx

; 809  : 				if (m_nAngle >= MaxMissleDir)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+432], 64			; 00000040H
	jl	SHORT $L105367

; 810  : 					m_nAngle = 0;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+432], 0
$L105367:

; 819  : 		break; 

	jmp	$L105317
$L105368:

; 822  : 			if(this->m_nFollowNpcIdx > 0)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+328], 0
	jle	$L105369

; 824  : 				//				m_nFollowNpcIdx = 2;
; 825  : 				if(m_nTempParam1 > 5)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+416], 5
	jle	$L105370

; 827  : 					int nDistance = 0;

	mov	DWORD PTR _nDistance$105371[ebp], 0

; 828  : 					int nSrcMpsX = 0;

	mov	DWORD PTR _nSrcMpsX$105372[ebp], 0

; 829  : 					int nSrcMpsY = 0;

	mov	DWORD PTR _nSrcMpsY$105373[ebp], 0

; 830  : 					int nDesMpsX = 0;

	mov	DWORD PTR _nDesMpsX$105374[ebp], 0

; 831  : 					int nDesMpsY = 0;

	mov	DWORD PTR _nDesMpsY$105375[ebp], 0

; 832  : 					
; 833  : 					GetMpsPos(&nSrcMpsX,&nSrcMpsY);

	lea	edx, DWORD PTR _nSrcMpsY$105373[ebp]
	push	edx
	lea	eax, DWORD PTR _nSrcMpsX$105372[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetMpsPos@KMissle@@QAEXPAH0@Z		; KMissle::GetMpsPos

; 834  : 					Npc[m_nFollowNpcIdx].GetMpsPos(&nDesMpsX,&nDesMpsY);

	lea	ecx, DWORD PTR _nDesMpsY$105375[ebp]
	push	ecx
	lea	edx, DWORD PTR _nDesMpsX$105374[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+328]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?GetMpsPos@KNpc@@QAEXPAH0@Z		; KNpc::GetMpsPos

; 835  : 					nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX,nSrcMpsY,nDesMpsX,nDesMpsY);

	mov	ecx, DWORD PTR _nDesMpsY$105375[ebp]
	push	ecx
	mov	edx, DWORD PTR _nDesMpsX$105374[ebp]
	push	edx
	mov	eax, DWORD PTR _nSrcMpsY$105373[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcMpsX$105372[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?GetDistance@KSubWorld@@QAEHHHHH@Z	; KSubWorld::GetDistance
	mov	DWORD PTR _nDistance$105371[ebp], eax

; 836  : 					if (nDistance > 20)

	cmp	DWORD PTR _nDistance$105371[ebp], 20	; 00000014H
	jle	$L105376

; 838  : 						int nXFactor = ((nDesMpsX - nSrcMpsX) << 10)/nDistance;

	mov	eax, DWORD PTR _nDesMpsX$105374[ebp]
	sub	eax, DWORD PTR _nSrcMpsX$105372[ebp]
	shl	eax, 10					; 0000000aH
	cdq
	idiv	DWORD PTR _nDistance$105371[ebp]
	mov	DWORD PTR _nXFactor$105377[ebp], eax

; 839  : 						int nYFactor = ((nDesMpsY - nSrcMpsY) << 10)/nDistance;

	mov	eax, DWORD PTR _nDesMpsY$105375[ebp]
	sub	eax, DWORD PTR _nSrcMpsY$105373[ebp]
	shl	eax, 10					; 0000000aH
	cdq
	idiv	DWORD PTR _nDistance$105371[ebp]
	mov	DWORD PTR _nYFactor$105378[ebp], eax

; 840  : 						int dx = nXFactor * m_nSpeed / 1.3;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nXFactor$105377[ebp]
	imul	ecx, DWORD PTR [eax+100]
	mov	DWORD PTR -180+[ebp], ecx
	fild	DWORD PTR -180+[ebp]
	fdiv	QWORD PTR __real@8@3fffa666666666666800
	call	__ftol
	mov	DWORD PTR _dx$105379[ebp], eax

; 841  : 						int dy = nYFactor * m_nSpeed / 1.3;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nYFactor$105378[ebp]
	imul	eax, DWORD PTR [edx+100]
	mov	DWORD PTR -184+[ebp], eax
	fild	DWORD PTR -184+[ebp]
	fdiv	QWORD PTR __real@8@3fffa666666666666800
	call	__ftol
	mov	DWORD PTR _dy$105380[ebp], eax

; 842  : 						nDOffsetX = dx;

	mov	ecx, DWORD PTR _dx$105379[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], ecx

; 843  : 						nDOffsetY = dy;

	mov	edx, DWORD PTR _dy$105380[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], edx

; 844  : 						m_nDir = g_GetDirIndex(nSrcMpsX,nSrcMpsY,nDesMpsX,nDesMpsY);

	mov	eax, DWORD PTR _nDesMpsY$105375[ebp]
	push	eax
	mov	ecx, DWORD PTR _nDesMpsX$105374[ebp]
	push	ecx
	mov	edx, DWORD PTR _nSrcMpsY$105373[ebp]
	push	edx
	mov	eax, DWORD PTR _nSrcMpsX$105372[ebp]
	push	eax
	call	?g_GetDirIndex@@YAHHHHH@Z		; g_GetDirIndex
	add	esp, 16					; 00000010H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+428], eax

; 846  : 					else

	jmp	SHORT $L105381
$L105376:

; 848  : 						ProcessDamage(m_nFollowNpcIdx);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+328]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessDamage@KMissle@@AAEHH@Z		; KMissle::ProcessDamage

; 849  : 						DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 850  : 						return;

	jmp	$L105302
$L105381:

; 852  : 				}else{

	jmp	SHORT $L105382
$L105370:

; 853  : 					nDOffsetX    = (m_nSpeed * m_nXFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+316]
	mov	DWORD PTR _nDOffsetX$[ebp], eax

; 854  : 					nDOffsetY	 = (m_nSpeed * m_nYFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+320]
	mov	DWORD PTR _nDOffsetY$[ebp], eax

; 855  : 					m_nTempParam1 ++;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+416]
	add	edx, 1
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+416], edx
$L105382:

; 858  : 			else

	jmp	SHORT $L105383
$L105369:

; 860  : 				nDOffsetX    = (m_nSpeed * m_nXFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+316]
	mov	DWORD PTR _nDOffsetX$[ebp], eax

; 861  : 				nDOffsetY	 = (m_nSpeed * m_nYFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+100]
	imul	eax, DWORD PTR [edx+320]
	mov	DWORD PTR _nDOffsetY$[ebp], eax
$L105383:

; 863  : 		}break;

	jmp	SHORT $L105317
$L105385:

; 872  : 			//单一必中类子弹，类式于传奇以及其它的同类网络游戏中的基本直线魔法			
; 873  : #ifdef _SERVER
; 874  : 			
; 875  : #else
; 876  : 			int x = m_nXOffset;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	mov	DWORD PTR _x$105386[ebp], edx

; 877  : 			int y = m_nYOffset;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	mov	DWORD PTR _y$105387[ebp], ecx

; 878  : 			int dx = (m_nSpeed * m_nXFactor);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+100]
	imul	ecx, DWORD PTR [eax+316]
	mov	DWORD PTR _dx$105388[ebp], ecx

; 879  : 			int dy = (m_nSpeed * m_nYFactor);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+100]
	imul	ecx, DWORD PTR [eax+320]
	mov	DWORD PTR _dy$105389[ebp], ecx

; 880  : 			nDOffsetX	=  dx;//* m_nCurrentLife;

	mov	edx, DWORD PTR _dx$105388[ebp]
	mov	DWORD PTR _nDOffsetX$[ebp], edx

; 881  : 			nDOffsetY	=  dy;//* m_nCurrentLife;

	mov	eax, DWORD PTR _dy$105389[ebp]
	mov	DWORD PTR _nDOffsetY$[ebp], eax
$L105390:
$L105317:

; 890  : 	
; 891  : 	//
; 892  : 	if (CheckBeyondRegion(nDOffsetX, nDOffsetY))

	mov	ecx, DWORD PTR _nDOffsetY$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nDOffsetX$[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CheckBeyondRegion@KMissle@@AAEHHH@Z	; KMissle::CheckBeyondRegion
	test	eax, eax
	je	$L105392

; 894  : 		if (CheckCollision() == -1) 

	mov	ecx, DWORD PTR _this$[ebp]
	call	?CheckCollision@KMissle@@AAEHXZ		; KMissle::CheckCollision
	cmp	eax, -1
	jne	$L105393

; 896  : 			if (m_bAutoExplode)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+116], 0
	je	SHORT $L105394

; 898  : 				ProcessCollision();//处理碰撞

	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessCollision@KMissle@@AAEHXZ	; KMissle::ProcessCollision
$L105394:

; 900  : #ifndef _SERVER 
; 901  : 			int nSrcX4 = 0 ;

	mov	DWORD PTR _nSrcX4$105395[ebp], 0

; 902  : 			int nSrcY4 = 0 ;

	mov	DWORD PTR _nSrcY4$105396[ebp], 0

; 903  : 			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX4, &nSrcY4);

	lea	ecx, DWORD PTR _nSrcY4$105396[ebp]
	push	ecx
	lea	edx, DWORD PTR _nSrcX4$105395[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+232]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 904  : 			CreateSpecialEffect(MS_DoVanish, nSrcX4, nSrcY4, m_nCurrentMapZ);

	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY4$105396[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX4$105395[ebp]
	push	ecx
	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 905  : #endif
; 906  : 			DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 907  : 			return;

	jmp	SHORT $L105302
$L105393:

; 910  : 	else//如果子弹飞行过程中进入了一个无效的Region则子弹自动消亡

	jmp	SHORT $L105397
$L105392:

; 912  : 		DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish
$L105397:
$L105302:

; 914  : }

	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	0
$L106158:
	DD	$L105321
	DD	$L105342
	DD	$L105356
	DD	$L105368
	DD	$L105385
	DD	$L105337
	DD	$L105390
$L106157:
	DB	0
	DB	6
	DB	1
	DB	2
	DB	3
	DB	6
	DB	0
	DB	4
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	6
	DB	5
?OnFly@KMissle@@AAEXXZ ENDP				; KMissle::OnFly
_TEXT	ENDS
EXTRN	?g_nSin@@3PAHA:DWORD				; g_nSin
EXTRN	?g_InternalDirSinCosCode@@3PAEA:DWORD		; g_InternalDirSinCosCode
;	COMDAT ?g_DirSin@@YAHHH@Z
_TEXT	SEGMENT
_nDir$ = 8
_nMaxDir$ = 12
?g_DirSin@@YAHHH@Z PROC NEAR				; g_DirSin, COMDAT

; 29   : {

	push	ebp
	mov	ebp, esp

; 30   :     return (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nSin, nDir, nMaxDir);

	mov	eax, DWORD PTR _nMaxDir$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nDir$[ebp]
	push	ecx
	mov	edx, DWORD PTR ?g_nSin@@3PAHA		; g_nSin
	push	edx
	call	DWORD PTR ?g_InternalDirSinCosCode@@3PAEA ; g_InternalDirSinCosCode
	add	esp, 12					; 0000000cH

; 31   : }

	pop	ebp
	ret	0
?g_DirSin@@YAHHH@Z ENDP					; g_DirSin
_TEXT	ENDS
EXTRN	?g_nCos@@3PAHA:DWORD				; g_nCos
;	COMDAT ?g_DirCos@@YAHHH@Z
_TEXT	SEGMENT
_nDir$ = 8
_nMaxDir$ = 12
?g_DirCos@@YAHHH@Z PROC NEAR				; g_DirCos, COMDAT

; 35   : {

	push	ebp
	mov	ebp, esp

; 36   :     return (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nCos, nDir, nMaxDir);

	mov	eax, DWORD PTR _nMaxDir$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nDir$[ebp]
	push	ecx
	mov	edx, DWORD PTR ?g_nCos@@3PAHA		; g_nCos
	push	edx
	call	DWORD PTR ?g_InternalDirSinCosCode@@3PAEA ; g_InternalDirSinCosCode
	add	esp, 12					; 0000000cH

; 37   : }

	pop	ebp
	ret	0
?g_DirCos@@YAHHH@Z ENDP					; g_DirCos
_TEXT	ENDS
PUBLIC	?g_GetDistance@@YAHHHHH@Z			; g_GetDistance
;	COMDAT ?g_GetDirIndex@@YAHHHHH@Z
_TEXT	SEGMENT
_nX1$ = 8
_nY1$ = 12
_nX2$ = 16
_nY2$ = 20
_nRet$ = -12
_nDistance$ = -20
_nYLength$ = -8
_nSin$ = -4
_i$ = -16
?g_GetDirIndex@@YAHHHHH@Z PROC NEAR			; g_GetDirIndex, COMDAT

; 68   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 20					; 00000014H

; 69   : 	int		nRet = -1;

	mov	DWORD PTR _nRet$[ebp], -1

; 70   : 
; 71   : 	if (nX1 == nX2 && nY1 == nY2)

	mov	eax, DWORD PTR _nX1$[ebp]
	cmp	eax, DWORD PTR _nX2$[ebp]
	jne	SHORT $L96610
	mov	ecx, DWORD PTR _nY1$[ebp]
	cmp	ecx, DWORD PTR _nY2$[ebp]
	jne	SHORT $L96610

; 72   : 		return -1;

	or	eax, -1
	jmp	$L96608
$L96610:

; 73   : 
; 74   : //	int		nDistance = g_GetDistance(nX1, nY1 * 2, nX2, nY2 * 2);
; 75   : 	int		nDistance = g_GetDistance(nX1, nY1, nX2, nY2);

	mov	edx, DWORD PTR _nY2$[ebp]
	push	edx
	mov	eax, DWORD PTR _nX2$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nY1$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nX1$[ebp]
	push	edx
	call	?g_GetDistance@@YAHHHHH@Z		; g_GetDistance
	add	esp, 16					; 00000010H
	mov	DWORD PTR _nDistance$[ebp], eax

; 76   : 	
; 77   : 	if (nDistance == 0 ) return -1;

	cmp	DWORD PTR _nDistance$[ebp], 0
	jne	SHORT $L96612
	or	eax, -1
	jmp	SHORT $L96608
$L96612:

; 78   : 	
; 79   : //	int		nYLength = (nY2 - nY1) * 2;
; 80   : 	int		nYLength = nY2 - nY1;

	mov	eax, DWORD PTR _nY2$[ebp]
	sub	eax, DWORD PTR _nY1$[ebp]
	mov	DWORD PTR _nYLength$[ebp], eax

; 81   : 	int		nSin = (nYLength << 10) / nDistance;	// 放大1024倍

	mov	eax, DWORD PTR _nYLength$[ebp]
	shl	eax, 10					; 0000000aH
	cdq
	idiv	DWORD PTR _nDistance$[ebp]
	mov	DWORD PTR _nSin$[ebp], eax

; 82   : 	
; 83   : 
; 84   : 	for (int i = 0; i < 32; i++)		// 顺时针方向 从270度到90度，sin值递减

	mov	DWORD PTR _i$[ebp], 0
	jmp	SHORT $L96616
$L96617:
	mov	ecx, DWORD PTR _i$[ebp]
	add	ecx, 1
	mov	DWORD PTR _i$[ebp], ecx
$L96616:
	cmp	DWORD PTR _i$[ebp], 32			; 00000020H
	jge	SHORT $L96618

; 86   : 		if (nSin > g_nSin[i])

	mov	edx, DWORD PTR _i$[ebp]
	mov	eax, DWORD PTR ?g_nSin@@3PAHA		; g_nSin
	mov	ecx, DWORD PTR _nSin$[ebp]
	cmp	ecx, DWORD PTR [eax+edx*4]
	jle	SHORT $L96619

; 87   : 			break;

	jmp	SHORT $L96618
$L96619:

; 88   : 		nRet = i;

	mov	edx, DWORD PTR _i$[ebp]
	mov	DWORD PTR _nRet$[ebp], edx

; 89   : 	}

	jmp	SHORT $L96617
$L96618:

; 90   : 
; 91   : 	if ((nX2 - nX1) > 0)

	mov	eax, DWORD PTR _nX2$[ebp]
	sub	eax, DWORD PTR _nX1$[ebp]
	test	eax, eax
	jle	SHORT $L96620

; 93   : 		nRet = 63 - nRet;

	mov	ecx, 63					; 0000003fH
	sub	ecx, DWORD PTR _nRet$[ebp]
	mov	DWORD PTR _nRet$[ebp], ecx
$L96620:

; 95   : 	return nRet;

	mov	eax, DWORD PTR _nRet$[ebp]
$L96608:

; 96   : }

	mov	esp, ebp
	pop	ebp
	ret	0
?g_GetDirIndex@@YAHHHHH@Z ENDP				; g_GetDirIndex
_TEXT	ENDS
EXTRN	_sqrt:NEAR
;	COMDAT ?g_GetDistance@@YAHHHHH@Z
_TEXT	SEGMENT
_nX1$ = 8
_nY1$ = 12
_nX2$ = 16
_nY2$ = 20
?g_GetDistance@@YAHHHHH@Z PROC NEAR			; g_GetDistance, COMDAT

; 62   : {

	push	ebp
	mov	ebp, esp
	push	ecx

; 63   : 	return (int)sqrt((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2));

	mov	eax, DWORD PTR _nX1$[ebp]
	sub	eax, DWORD PTR _nX2$[ebp]
	mov	ecx, DWORD PTR _nX1$[ebp]
	sub	ecx, DWORD PTR _nX2$[ebp]
	imul	eax, ecx
	mov	edx, DWORD PTR _nY1$[ebp]
	sub	edx, DWORD PTR _nY2$[ebp]
	mov	ecx, DWORD PTR _nY1$[ebp]
	sub	ecx, DWORD PTR _nY2$[ebp]
	imul	edx, ecx
	add	eax, edx
	mov	DWORD PTR -4+[ebp], eax
	fild	DWORD PTR -4+[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_sqrt
	add	esp, 8
	call	__ftol

; 64   : }

	mov	esp, ebp
	pop	ebp
	ret	0
?g_GetDistance@@YAHHHHH@Z ENDP				; g_GetDistance
_TEXT	ENDS
PUBLIC	??0KWorldMsgNode@@QAE@XZ			; KWorldMsgNode::KWorldMsgNode
PUBLIC	?AddTail@KList@@QAEXPAVKNode@@@Z		; KList::AddTail
EXTRN	??2@YAPAXI@Z:NEAR				; operator new
;	COMDAT xdata$x
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\KWorldMsg.h
xdata$x	SEGMENT
$T106175 DD	019930520H
	DD	01H
	DD	FLAT:$T106177
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T106177 DD	0ffffffffH
	DD	FLAT:$L106173
xdata$x	ENDS
;	COMDAT ?Send@KWorldMsg@@QAEHKHHH@Z
_TEXT	SEGMENT
_dwMsgType$ = 8
_nParam1$ = 12
_nParam2$ = 16
_nParam3$ = 20
_this$ = -28
_pNode$ = -16
$T106169 = -20
$T106170 = -24
__$EHRec$ = -12
?Send@KWorldMsg@@QAEHKHHH@Z PROC NEAR			; KWorldMsg::Send, COMDAT

; 71   : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L106176
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 20					; 00000014H
	mov	DWORD PTR _this$[ebp], ecx

; 72   : 	KWorldMsgNode *pNode = NULL;

	mov	DWORD PTR _pNode$[ebp], 0

; 73   : 
; 74   : 	pNode = new KWorldMsgNode;

	push	28					; 0000001cH
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T106170[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	cmp	DWORD PTR $T106170[ebp], 0
	je	SHORT $L106171
	mov	ecx, DWORD PTR $T106170[ebp]
	call	??0KWorldMsgNode@@QAE@XZ		; KWorldMsgNode::KWorldMsgNode
	mov	DWORD PTR -32+[ebp], eax
	jmp	SHORT $L106172
$L106171:
	mov	DWORD PTR -32+[ebp], 0
$L106172:
	mov	eax, DWORD PTR -32+[ebp]
	mov	DWORD PTR $T106169[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	ecx, DWORD PTR $T106169[ebp]
	mov	DWORD PTR _pNode$[ebp], ecx

; 75   : 	if (!pNode)

	cmp	DWORD PTR _pNode$[ebp], 0
	jne	SHORT $L94788

; 76   : 		return FALSE;

	xor	eax, eax
	jmp	SHORT $L94782
$L94788:

; 77   : 
; 78   : 	pNode->m_dwMsgType	= dwMsgType;

	mov	edx, DWORD PTR _pNode$[ebp]
	mov	eax, DWORD PTR _dwMsgType$[ebp]
	mov	DWORD PTR [edx+12], eax

; 79   : 	pNode->m_nParam[0]	= nParam1;

	mov	ecx, DWORD PTR _pNode$[ebp]
	mov	edx, DWORD PTR _nParam1$[ebp]
	mov	DWORD PTR [ecx+16], edx

; 80   : 	pNode->m_nParam[1]	= nParam2;

	mov	eax, DWORD PTR _pNode$[ebp]
	mov	ecx, DWORD PTR _nParam2$[ebp]
	mov	DWORD PTR [eax+20], ecx

; 81   : 	pNode->m_nParam[2]	= nParam3;

	mov	edx, DWORD PTR _pNode$[ebp]
	mov	eax, DWORD PTR _nParam3$[ebp]
	mov	DWORD PTR [edx+24], eax

; 82   : 
; 83   : 	m_LocalMsgQueue.AddTail(pNode);

	mov	ecx, DWORD PTR _pNode$[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?AddTail@KList@@QAEXPAVKNode@@@Z	; KList::AddTail

; 84   : 	return TRUE;

	mov	eax, 1
$L94782:

; 85   : }

	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	16					; 00000010H
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$L106173:
	mov	eax, DWORD PTR $T106170[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L106176:
	mov	eax, OFFSET FLAT:$T106175
	jmp	___CxxFrameHandler
text$x	ENDS
?Send@KWorldMsg@@QAEHKHHH@Z ENDP			; KWorldMsg::Send
PUBLIC	?InsertBefore@KNode@@QAEXPAV1@@Z		; KNode::InsertBefore
;	COMDAT ?AddTail@KList@@QAEXPAVKNode@@@Z
_TEXT	SEGMENT
_pNode$ = 8
_this$ = -4
?AddTail@KList@@QAEXPAVKNode@@@Z PROC NEAR		; KList::AddTail, COMDAT

; 92   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 93   : 	m_ListTail.InsertBefore(pNode);

	mov	eax, DWORD PTR _pNode$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 12					; 0000000cH
	call	?InsertBefore@KNode@@QAEXPAV1@@Z	; KNode::InsertBefore

; 94   : }

	mov	esp, ebp
	pop	ebp
	ret	4
?AddTail@KList@@QAEXPAVKNode@@@Z ENDP			; KList::AddTail
_TEXT	ENDS
;	COMDAT ?InsertBefore@KNode@@QAEXPAV1@@Z
_TEXT	SEGMENT
_pNode$ = 8
_this$ = -4
?InsertBefore@KNode@@QAEXPAV1@@Z PROC NEAR		; KNode::InsertBefore, COMDAT

; 78   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 79   : 	KASSERT(m_pPrev);
; 80   : 	if (!pNode || !m_pPrev)

	cmp	DWORD PTR _pNode$[ebp], 0
	je	SHORT $L83743
	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+8], 0
	jne	SHORT $L83742
$L83743:

; 81   : 		return;

	jmp	SHORT $L83741
$L83742:

; 82   : 
; 83   : 	pNode->m_pPrev = m_pPrev;

	mov	ecx, DWORD PTR _pNode$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+8]
	mov	DWORD PTR [ecx+8], eax

; 84   : 	pNode->m_pNext = this;

	mov	ecx, DWORD PTR _pNode$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+4], edx

; 85   : 	m_pPrev->m_pNext = pNode;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+8]
	mov	edx, DWORD PTR _pNode$[ebp]
	mov	DWORD PTR [ecx+4], edx

; 86   : 	m_pPrev = pNode;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _pNode$[ebp]
	mov	DWORD PTR [eax+8], ecx
$L83741:

; 87   : }

	mov	esp, ebp
	pop	ebp
	ret	4
?InsertBefore@KNode@@QAEXPAV1@@Z ENDP			; KNode::InsertBefore
_TEXT	ENDS
PUBLIC	??_7KWorldMsgNode@@6B@				; KWorldMsgNode::`vftable'
PUBLIC	??_GKWorldMsgNode@@UAEPAXI@Z			; KWorldMsgNode::`scalar deleting destructor'
PUBLIC	??_EKWorldMsgNode@@UAEPAXI@Z			; KWorldMsgNode::`vector deleting destructor'
;	COMDAT ??_7KWorldMsgNode@@6B@
CONST	SEGMENT
??_7KWorldMsgNode@@6B@ DD FLAT:??_EKWorldMsgNode@@UAEPAXI@Z ; KWorldMsgNode::`vftable'
CONST	ENDS
;	COMDAT ??0KWorldMsgNode@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0KWorldMsgNode@@QAE@XZ PROC NEAR			; KWorldMsgNode::KWorldMsgNode, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??0KNode@@QAE@XZ			; KNode::KNode
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KWorldMsgNode@@6B@ ; KWorldMsgNode::`vftable'
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0KWorldMsgNode@@QAE@XZ ENDP				; KWorldMsgNode::KWorldMsgNode
_TEXT	ENDS
PUBLIC	??1KWorldMsgNode@@UAE@XZ			; KWorldMsgNode::~KWorldMsgNode
;	COMDAT ??_GKWorldMsgNode@@UAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8
_this$ = -4
??_GKWorldMsgNode@@UAEPAXI@Z PROC NEAR			; KWorldMsgNode::`scalar deleting destructor', COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KWorldMsgNode@@UAE@XZ		; KWorldMsgNode::~KWorldMsgNode
	mov	eax, DWORD PTR ___flags$[ebp]
	and	eax, 1
	test	eax, eax
	je	SHORT $L94794
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L94794:
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??_GKWorldMsgNode@@UAEPAXI@Z ENDP			; KWorldMsgNode::`scalar deleting destructor'
_TEXT	ENDS
;	COMDAT ??1KWorldMsgNode@@UAE@XZ
_TEXT	SEGMENT
_this$ = -4
??1KWorldMsgNode@@UAE@XZ PROC NEAR			; KWorldMsgNode::~KWorldMsgNode, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KNode@@UAE@XZ			; KNode::~KNode
	mov	esp, ebp
	pop	ebp
	ret	0
??1KWorldMsgNode@@UAE@XZ ENDP				; KWorldMsgNode::~KWorldMsgNode
_TEXT	ENDS
;	COMDAT ?ZAxisMove@KMissle@@AAEXXZ
_TEXT	SEGMENT
_this$ = -4
?ZAxisMove@KMissle@@AAEXXZ PROC NEAR			; KMissle::ZAxisMove, COMDAT

; 202  : 	{

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 203  : 		if (m_nZAcceleration)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+352], 0
	je	SHORT $L95309

; 205  : 			m_nHeight += m_nHeightSpeed;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+88]
	mov	eax, DWORD PTR _this$[ebp]
	add	edx, DWORD PTR [eax+92]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+88], edx

; 206  : 			if (m_nHeight < 0) m_nHeight = 0;

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+88], 0
	jge	SHORT $L95310
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+88], 0
$L95310:

; 207  : 			m_nHeightSpeed -= m_nZAcceleration;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+92]
	sub	eax, DWORD PTR [edx+352]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+92], eax

; 208  : 			m_nCurrentMapZ = m_nHeight >> 10;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+88]
	sar	eax, 10					; 0000000aH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+228], eax
$L95309:

; 210  : 	}

	mov	esp, ebp
	pop	ebp
	ret	0
?ZAxisMove@KMissle@@AAEXXZ ENDP				; KMissle::ZAxisMove
_TEXT	ENDS
EXTRN	?TestBarrier@KSubWorld@@QAEEHHHHHHH@Z:NEAR	; KSubWorld::TestBarrier
;	COMDAT ?TestBarrier@KMissle@@AAEHXZ
_TEXT	SEGMENT
_this$ = -8
_nBarrierKind$ = -4
?TestBarrier@KMissle@@AAEHXZ PROC NEAR			; KMissle::TestBarrier, COMDAT

; 213  : 	{

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx

; 214  : 		int nBarrierKind = SubWorld[m_nSubWorldId].TestBarrier(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, 0, 0);

	push	0
	push	0
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+232]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A ; SubWorld
	call	?TestBarrier@KSubWorld@@QAEEHHHHHHH@Z	; KSubWorld::TestBarrier
	and	eax, 255				; 000000ffH
	mov	DWORD PTR _nBarrierKind$[ebp], eax

; 215  : 		if (nBarrierKind == Obstacle_Normal || nBarrierKind == Obstacle_Jump)

	cmp	DWORD PTR _nBarrierKind$[ebp], 1
	je	SHORT $L95315
	cmp	DWORD PTR _nBarrierKind$[ebp], 3
	jne	SHORT $L95314
$L95315:

; 217  : 			
; 218  : 			return TRUE;

	mov	eax, 1
	jmp	SHORT $L95312
$L95314:

; 220  : 		return FALSE;

	xor	eax, eax
$L95312:

; 221  : 	}

	mov	esp, ebp
	pop	ebp
	ret	0
?TestBarrier@KMissle@@AAEHXZ ENDP			; KMissle::TestBarrier
_TEXT	ENDS
_TEXT	SEGMENT
_this$ = -4
?OnVanish@KMissle@@AAEXXZ PROC NEAR			; KMissle::OnVanish

; 923  : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 924  : 	
; 925  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?OnVanish@KMissle@@AAEXXZ ENDP				; KMissle::OnVanish
_TEXT	ENDS
PUBLIC	?Paint@KMissle@@QAEXXZ				; KMissle::Paint
PUBLIC	?g_DirIndex2Dir@@YAHHH@Z			; g_DirIndex2Dir
PUBLIC	?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ	; KMissleRes::SpecialMovieIsAllEnd
EXTRN	?Draw@KMissleRes@@QAEHHHHHHHH@Z:NEAR		; KMissleRes::Draw
_TEXT	SEGMENT
_this$ = -20
_nSrcX$ = -4
_nSrcY$ = -8
_nDirIndex$105409 = -16
_nDir$105410 = -12
?Paint@KMissle@@QAEXXZ PROC NEAR			; KMissle::Paint

; 929  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 20					; 00000014H
	mov	DWORD PTR _this$[ebp], ecx

; 930  : 	if (m_nMissleId <= 0 ) return;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+360], 0
	jg	SHORT $L105404
	jmp	$L105403
$L105404:

; 932  : 	int nSrcY;
; 933  : 	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);

	lea	ecx, DWORD PTR _nSrcY$[ebp]
	push	ecx
	lea	edx, DWORD PTR _nSrcX$[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+232]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 934  : 	
; 935  : 	if (!m_nZAcceleration)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+352], 0
	jne	SHORT $L105407

; 937  : 		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nCurrentMapZ, m_nDir,m_nLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+208]
	sub	ecx, DWORD PTR [eax+212]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+96]
	sub	ecx, DWORD PTR [eax+212]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+428]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+356]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?Draw@KMissleRes@@QAEHHHHHHHH@Z		; KMissleRes::Draw

; 939  : 	else

	jmp	$L105408
$L105407:

; 941  : 		int nDirIndex = g_GetDirIndex(0,0,m_nXFactor, m_nYFactor);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+320]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+316]
	push	ecx
	push	0
	push	0
	call	?g_GetDirIndex@@YAHHHHH@Z		; g_GetDirIndex
	add	esp, 16					; 00000010H
	mov	DWORD PTR _nDirIndex$105409[ebp], eax

; 942  : 		int nDir = g_DirIndex2Dir(nDirIndex, 64);

	push	64					; 00000040H
	mov	edx, DWORD PTR _nDirIndex$105409[ebp]
	push	edx
	call	?g_DirIndex2Dir@@YAHHH@Z		; g_DirIndex2Dir
	add	esp, 8
	mov	DWORD PTR _nDir$105410[ebp], eax

; 943  : 		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nCurrentMapZ, nDir,m_nLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [eax+208]
	sub	edx, DWORD PTR [ecx+212]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [eax+96]
	sub	edx, DWORD PTR [ecx+212]
	push	edx
	mov	eax, DWORD PTR _nDir$105410[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+356]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?Draw@KMissleRes@@QAEHHHHHHHH@Z		; KMissleRes::Draw
$L105408:

; 945  : 	
; 946  : 	//对于客户端，直到子弹及其产生的效果全部播放完才终止并删除掉!
; 947  : 	if (m_MissleRes.m_bHaveEnd && (m_MissleRes.SpecialMovieIsAllEnd()))

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+492], 0
	je	SHORT $L105411
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ ; KMissleRes::SpecialMovieIsAllEnd
	test	eax, eax
	je	SHORT $L105411

; 948  : 		SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_DEL, m_nMissleId);

	push	0
	push	0
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+360]
	push	eax
	push	4001					; 00000fa1H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A+172
	call	?Send@KWorldMsg@@QAEHKHHH@Z		; KWorldMsg::Send
$L105411:
$L105403:

; 949  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?Paint@KMissle@@QAEXXZ ENDP				; KMissle::Paint
_TEXT	ENDS
;	COMDAT ?g_DirIndex2Dir@@YAHHH@Z
_TEXT	SEGMENT
_nDir$ = 8
_nMaxDir$ = 12
_nRet$ = -4
?g_DirIndex2Dir@@YAHHH@Z PROC NEAR			; g_DirIndex2Dir, COMDAT

; 110  : {

	push	ebp
	mov	ebp, esp
	push	ecx

; 111  : 	int		nRet = -1;

	mov	DWORD PTR _nRet$[ebp], -1

; 112  : 
; 113  : 	if (nMaxDir <= 0)

	cmp	DWORD PTR _nMaxDir$[ebp], 0
	jg	SHORT $L96632

; 114  : 		return nRet;

	mov	eax, DWORD PTR _nRet$[ebp]
	jmp	SHORT $L96630
$L96632:

; 115  : 
; 116  : 	nRet = (nMaxDir * nDir) >> 6;	// (nMaxDir / 64) * nDir

	mov	eax, DWORD PTR _nMaxDir$[ebp]
	imul	eax, DWORD PTR _nDir$[ebp]
	sar	eax, 6
	mov	DWORD PTR _nRet$[ebp], eax

; 117  : 	return nRet;

	mov	eax, DWORD PTR _nRet$[ebp]
$L96630:

; 118  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?g_DirIndex2Dir@@YAHHH@Z ENDP				; g_DirIndex2Dir
_TEXT	ENDS
;	COMDAT ?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ
_TEXT	SEGMENT
_this$ = -4
?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ PROC NEAR	; KMissleRes::SpecialMovieIsAllEnd, COMDAT

; 83   : 	{

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 84   : 		
; 85   : 		if (m_SkillSpecialList.GetHead())

	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetHead@KList@@QAEPAVKNode@@XZ		; KList::GetHead
	test	eax, eax
	je	SHORT $L92551

; 87   : 			return FALSE;

	xor	eax, eax
	jmp	SHORT $L92550
$L92551:

; 89   : 		
; 90   : 		return TRUE;

	mov	eax, 1
$L92550:

; 91   : 	};

	mov	esp, ebp
	pop	ebp
	ret	0
?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ ENDP		; KMissleRes::SpecialMovieIsAllEnd
_TEXT	ENDS
EXTRN	_abs:NEAR
_TEXT	SEGMENT
_nDOffsetX$ = 8
_nDOffsetY$ = 12
_this$ = -36
_nOldRegion$ = -4
_nNewXOffset$ = -24
_nNewYOffset$ = -32
_nNewMapX$ = -12
_nNewMapY$ = -8
_nNewRegion$ = -28
_nRegionWidth$ = -20
_nRegionHeight$ = -16
?CheckBeyondRegion@KMissle@@AAEHHH@Z PROC NEAR		; KMissle::CheckBeyondRegion

; 954  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 36					; 00000024H
	mov	DWORD PTR _this$[ebp], ecx

; 955  : 	if (m_nRegionId < 0) 

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+368], 0
	jge	SHORT $L105417

; 956  : 		return FALSE;

	xor	eax, eax
	jmp	$L105416
$L105417:

; 957  : 	//未动
; 958  : 	if (nDOffsetX == 0 && nDOffsetY == 0) return TRUE;

	cmp	DWORD PTR _nDOffsetX$[ebp], 0
	jne	SHORT $L105418
	cmp	DWORD PTR _nDOffsetY$[ebp], 0
	jne	SHORT $L105418
	mov	eax, 1
	jmp	$L105416
$L105418:

; 959  : 
; 960  : 	if (abs(nDOffsetX) > CellWidth) 

	mov	ecx, DWORD PTR _nDOffsetX$[ebp]
	push	ecx
	call	_abs
	add	esp, 4
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+148]
	shl	edx, 10					; 0000000aH
	cmp	eax, edx
	jle	SHORT $L105419

; 962  : 		return FALSE;

	xor	eax, eax
	jmp	$L105416
$L105419:

; 964  : 
; 965  : 	if (abs(nDOffsetY) > CellHeight) 

	mov	eax, DWORD PTR _nDOffsetY$[ebp]
	push	eax
	call	_abs
	add	esp, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+152]
	shl	ecx, 10					; 0000000aH
	cmp	eax, ecx
	jle	SHORT $L105420

; 967  : 		return FALSE;

	xor	eax, eax
	jmp	$L105416
$L105420:

; 969  : 
; 970  : 	int nOldRegion		= m_nRegionId;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	mov	DWORD PTR _nOldRegion$[ebp], eax

; 971  : 	int nNewXOffset		= m_nXOffset + nDOffsetX;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	add	edx, DWORD PTR _nDOffsetX$[ebp]
	mov	DWORD PTR _nNewXOffset$[ebp], edx

; 972  : 	int nNewYOffset		= m_nYOffset + nDOffsetY;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	add	ecx, DWORD PTR _nDOffsetY$[ebp]
	mov	DWORD PTR _nNewYOffset$[ebp], ecx

; 973  : 	int nNewMapX		= m_nCurrentMapX;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	mov	DWORD PTR _nNewMapX$[ebp], eax

; 974  : 	int nNewMapY		= m_nCurrentMapY;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	mov	DWORD PTR _nNewMapY$[ebp], edx

; 975  : 	int nNewRegion		= m_nRegionId;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	mov	DWORD PTR _nNewRegion$[ebp], ecx

; 976  : 	
; 977  : 	DWORD nRegionWidth = RegionWidth;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+140]
	mov	DWORD PTR _nRegionWidth$[ebp], ecx

; 978  : 	DWORD nRegionHeight = RegionHeight;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+144]
	mov	DWORD PTR _nRegionHeight$[ebp], ecx

; 979  : 	
; 980  : 	_ASSERT(abs(nNewXOffset) <= CellWidth * 2);
; 981  : 	_ASSERT(abs(nNewYOffset) <= CellHeight * 2);
; 982  : 	
; 983  : 	//	处理NPC的坐标变幻
; 984  : 	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
; 985  : 	
; 986  : 	if (nNewXOffset < 0)

	cmp	DWORD PTR _nNewXOffset$[ebp], 0
	jge	SHORT $L105431

; 988  : 		nNewMapX--;

	mov	edx, DWORD PTR _nNewMapX$[ebp]
	sub	edx, 1
	mov	DWORD PTR _nNewMapX$[ebp], edx

; 989  : 		nNewXOffset += CellWidth;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+148]
	shl	edx, 10					; 0000000aH
	mov	eax, DWORD PTR _nNewXOffset$[ebp]
	add	eax, edx
	mov	DWORD PTR _nNewXOffset$[ebp], eax

; 991  : 	else if (nNewXOffset > CellWidth)

	jmp	SHORT $L105433
$L105431:
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+148]
	shl	eax, 10					; 0000000aH
	cmp	DWORD PTR _nNewXOffset$[ebp], eax
	jle	SHORT $L105433

; 993  : 		nNewMapX++;

	mov	ecx, DWORD PTR _nNewMapX$[ebp]
	add	ecx, 1
	mov	DWORD PTR _nNewMapX$[ebp], ecx

; 994  : 		nNewXOffset -= CellWidth;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+148]
	shl	ecx, 10					; 0000000aH
	mov	edx, DWORD PTR _nNewXOffset$[ebp]
	sub	edx, ecx
	mov	DWORD PTR _nNewXOffset$[ebp], edx
$L105433:

; 996  : 	
; 997  : 	if (nNewYOffset < 0)

	cmp	DWORD PTR _nNewYOffset$[ebp], 0
	jge	SHORT $L105434

; 999  : 		nNewMapY--;

	mov	eax, DWORD PTR _nNewMapY$[ebp]
	sub	eax, 1
	mov	DWORD PTR _nNewMapY$[ebp], eax

; 1000 : 		nNewYOffset += CellHeight;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+152]
	shl	eax, 10					; 0000000aH
	mov	ecx, DWORD PTR _nNewYOffset$[ebp]
	add	ecx, eax
	mov	DWORD PTR _nNewYOffset$[ebp], ecx

; 1002 : 	else if (nNewYOffset > CellHeight)

	jmp	SHORT $L105436
$L105434:
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+152]
	shl	ecx, 10					; 0000000aH
	cmp	DWORD PTR _nNewYOffset$[ebp], ecx
	jle	SHORT $L105436

; 1004 : 		nNewMapY++;

	mov	edx, DWORD PTR _nNewMapY$[ebp]
	add	edx, 1
	mov	DWORD PTR _nNewMapY$[ebp], edx

; 1005 : 		nNewYOffset -= CellHeight;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+152]
	shl	edx, 10					; 0000000aH
	mov	eax, DWORD PTR _nNewYOffset$[ebp]
	sub	eax, edx
	mov	DWORD PTR _nNewYOffset$[ebp], eax
$L105436:

; 1007 : 	
; 1008 : 	if (nNewMapX < 0)

	cmp	DWORD PTR _nNewMapX$[ebp], 0
	jge	SHORT $L105437

; 1010 : 		nNewRegion = LeftRegion(m_nRegionId);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	imul	ecx, 208				; 000000d0H
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	mov	eax, DWORD PTR [edx+ecx+112]
	mov	DWORD PTR _nNewRegion$[ebp], eax

; 1011 : 		nNewMapX += nRegionWidth;

	mov	ecx, DWORD PTR _nNewMapX$[ebp]
	add	ecx, DWORD PTR _nRegionWidth$[ebp]
	mov	DWORD PTR _nNewMapX$[ebp], ecx

; 1013 : 	else if ((DWORD)nNewMapX >= nRegionWidth)

	jmp	SHORT $L105440
$L105437:
	mov	edx, DWORD PTR _nNewMapX$[ebp]
	cmp	edx, DWORD PTR _nRegionWidth$[ebp]
	jb	SHORT $L105440

; 1015 : 		nNewRegion = RightRegion(m_nRegionId);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	mov	edx, DWORD PTR [ecx+eax+128]
	mov	DWORD PTR _nNewRegion$[ebp], edx

; 1016 : 		nNewMapX -= nRegionWidth;

	mov	eax, DWORD PTR _nNewMapX$[ebp]
	sub	eax, DWORD PTR _nRegionWidth$[ebp]
	mov	DWORD PTR _nNewMapX$[ebp], eax
$L105440:

; 1018 : 
; 1019 : 	if (nNewRegion < 0) 

	cmp	DWORD PTR _nNewRegion$[ebp], 0
	jge	SHORT $L105441

; 1021 : 		return FALSE; 

	xor	eax, eax
	jmp	$L105416
$L105441:

; 1023 : 	
; 1024 : 	if (nNewMapY < 0)

	cmp	DWORD PTR _nNewMapY$[ebp], 0
	jge	SHORT $L105442

; 1026 : 		nNewRegion = UpRegion(nNewRegion);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nNewRegion$[ebp]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	mov	edx, DWORD PTR [ecx+eax+120]
	mov	DWORD PTR _nNewRegion$[ebp], edx

; 1027 : 		nNewMapY += nRegionHeight;

	mov	eax, DWORD PTR _nNewMapY$[ebp]
	add	eax, DWORD PTR _nRegionHeight$[ebp]
	mov	DWORD PTR _nNewMapY$[ebp], eax

; 1029 : 	else if (nNewMapY >= RegionHeight)

	jmp	SHORT $L105444
$L105442:
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nNewMapY$[ebp]
	cmp	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+144]
	jl	SHORT $L105444

; 1031 : 		nNewRegion = DownRegion(nNewRegion);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nNewRegion$[ebp]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	mov	edx, DWORD PTR [ecx+eax+104]
	mov	DWORD PTR _nNewRegion$[ebp], edx

; 1032 : 		nNewMapY -= nRegionHeight;

	mov	eax, DWORD PTR _nNewMapY$[ebp]
	sub	eax, DWORD PTR _nRegionHeight$[ebp]
	mov	DWORD PTR _nNewMapY$[ebp], eax
$L105444:

; 1034 : 	
; 1035 : 	//下一个位置为不合法位置，则消亡
; 1036 : 	if (nNewRegion < 0) 

	cmp	DWORD PTR _nNewRegion$[ebp], 0
	jge	SHORT $L105445

; 1038 : 		return FALSE; 

	xor	eax, eax
	jmp	$L105416
$L105445:

; 1042 : 		CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	imul	edx, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+8]
	add	ecx, edx
	call	?DecRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::DecRef

; 1043 : 		_ASSERT(m_nCurrentMapX >= 0  &&  m_nCurrentMapY >= 0);
; 1044 : 		
; 1045 : 		m_nRegionId	   = nNewRegion;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nNewRegion$[ebp]
	mov	DWORD PTR [eax+368], ecx

; 1046 : 		m_nCurrentMapX = nNewMapX;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nNewMapX$[ebp]
	mov	DWORD PTR [edx+220], eax

; 1047 : 		m_nCurrentMapY = nNewMapY;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nNewMapY$[ebp]
	mov	DWORD PTR [ecx+224], edx

; 1048 : 		m_nXOffset	   = nNewXOffset;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nNewXOffset$[ebp]
	mov	DWORD PTR [eax+232], ecx

; 1049 : 		m_nYOffset	   = nNewYOffset;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nNewYOffset$[ebp]
	mov	DWORD PTR [edx+236], eax

; 1050 : 		CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+364]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	imul	edx, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+8]
	add	ecx, edx
	call	?AddRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::AddRef

; 1051 : 		
; 1052 : 		if (nOldRegion != m_nRegionId)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nOldRegion$[ebp]
	cmp	ecx, DWORD PTR [eax+368]
	je	SHORT $L105448

; 1054 : 			SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+360]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	eax, DWORD PTR _nOldRegion$[ebp]
	push	eax
	push	4002					; 00000fa2H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A+172
	call	?Send@KWorldMsg@@QAEHKHHH@Z		; KWorldMsg::Send
$L105448:

; 1057 : 	return TRUE;

	mov	eax, 1
$L105416:

; 1058 : }

	mov	esp, ebp
	pop	ebp
	ret	8
?CheckBeyondRegion@KMissle@@AAEHHH@Z ENDP		; KMissle::CheckBeyondRegion
_TEXT	ENDS
PUBLIC	??4KMissle@@AAEAAV0@AAV0@@Z			; KMissle::operator=
EXTRN	__imp_?g_Random@@YAII@Z:NEAR
EXTRN	_memset:NEAR
EXTRN	_strcpy:NEAR
_TEXT	SEGMENT
_Missle$ = 8
_this$ = -12
_nOffset$ = -4
_t$ = -8
??4KMissle@@AAEAAV0@AAV0@@Z PROC NEAR			; KMissle::operator=

; 1061 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	push	esi
	mov	DWORD PTR _this$[ebp], ecx

; 1062 : 	Missle.m_nTempParam1	=	0;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [eax+416], 0

; 1063 : 	Missle.m_nTempParam2	=	0;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [ecx+420], 0

; 1064 : 	Missle.m_nDesMapX			=	0;

	mov	edx, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [edx+248], 0

; 1065 : 	Missle.m_nDesMapY			=	0;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [eax+252], 0

; 1066 : 	Missle.m_nDesRegion		=	0;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [ecx+256], 0

; 1067 : 	Missle.m_bNeedReclaim	=	FALSE;

	mov	edx, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [edx+260], 0

; 1068 : 	Missle.m_nFirstReclaimTime = 0;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [eax+408], 0

; 1069 : 	Missle.m_nEndReclaimTime = 0;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [ecx+412], 0

; 1070 : 	memset(Missle.m_NeedReclaimPos, 0, sizeof(m_NeedReclaimPos));

	push	48					; 00000030H
	push	0
	mov	edx, DWORD PTR _Missle$[ebp]
	add	edx, 264				; 00000108H
	push	edx
	call	_memset
	add	esp, 12					; 0000000cH

; 1071 : 
; 1072 : 	Missle.m_bCanSlow		=	m_bCanSlow;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+164]
	mov	DWORD PTR [eax+164], edx

; 1073 : 	Missle.m_bCollideEvent	=	m_bCollideEvent;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+192]
	mov	DWORD PTR [eax+192], edx

; 1074 : 	Missle.m_bCollideFriend =	m_bCollideFriend;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+160]
	mov	DWORD PTR [eax+160], edx

; 1075 : 	Missle.m_bCollideVanish	=	m_bCollideVanish;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+156]
	mov	DWORD PTR [eax+156], edx

; 1076 : 	Missle.m_bRangeDamage	=	m_bRangeDamage;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+108]
	mov	DWORD PTR [eax+108], edx

; 1077 : 	Missle.m_eFollowKind	=	m_eFollowKind;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+84]
	mov	DWORD PTR [eax+84], edx

; 1078 : 	Missle.m_eMoveKind		=	m_eMoveKind;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+80]
	mov	DWORD PTR [eax+80], edx

; 1079 : 	Missle.m_nAction		=	m_nAction;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+60]
	mov	DWORD PTR [eax+60], edx

; 1080 : 	Missle.m_nAngle			=	m_nAngle;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+432]
	mov	DWORD PTR [eax+432], edx

; 1081 : 	Missle.m_nCollideRange	=	m_nCollideRange;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+148]
	mov	DWORD PTR [eax+148], edx

; 1082 : 	Missle.m_nCurrentLife	=	0;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [eax+208], 0

; 1083 : 	Missle.m_nDamageRange	=	m_nDamageRange;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+152]
	mov	DWORD PTR [ecx+152], eax

; 1084 : 	Missle.m_nHeight		=	m_nHeight;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+88]
	mov	DWORD PTR [ecx+88], eax

; 1085 : 	Missle.m_nLifeTime		=	m_nLifeTime;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+96]
	mov	DWORD PTR [ecx+96], eax

; 1086 : 	Missle.m_nSpeed			=   m_nSpeed;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+100]
	mov	DWORD PTR [ecx+100], eax

; 1087 : 	Missle.m_nParam1		=	m_nParam1;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+396]
	mov	DWORD PTR [ecx+396], eax

; 1088 : 	Missle.m_nParam2		=	m_nParam2;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+400]
	mov	DWORD PTR [ecx+400], eax

; 1089 : 	Missle.m_nParam3		=	m_nParam3;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+404]
	mov	DWORD PTR [ecx+404], eax

; 1090 : 	Missle.m_nCurrentMapZ	=   m_nHeight >> 10;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+88]
	sar	edx, 10					; 0000000aH
	mov	eax, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [eax+228], edx

; 1091 : 	Missle.m_bFlyEvent		=	m_bFlyEvent;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+176]
	mov	DWORD PTR [ecx+176], eax

; 1092 : 	Missle.m_nFlyEventTime  =	m_nFlyEventTime;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+180]
	mov	DWORD PTR [ecx+180], eax

; 1093 : 	Missle.m_nZAcceleration =	m_nZAcceleration;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+352]
	mov	DWORD PTR [ecx+352], eax

; 1094 : 	Missle.m_nHeightSpeed	=	m_nHeightSpeed;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+92]
	mov	DWORD PTR [ecx+92], eax

; 1095 : 	Missle.m_bAutoExplode	=	m_bAutoExplode;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+116]
	mov	DWORD PTR [ecx+116], eax

; 1096 : 	Missle.m_ulDamageInterval = m_ulDamageInterval;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+200]
	mov	DWORD PTR [ecx+200], eax

; 1097 : 	strcpy(Missle.m_szMissleName	,	m_szMissleName);

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 28					; 0000001cH
	push	ecx
	mov	edx, DWORD PTR _Missle$[ebp]
	add	edx, 28					; 0000001cH
	push	edx
	call	_strcpy
	add	esp, 8

; 1098 : 	
; 1099 : #ifndef  _SERVER
; 1100 : 	Missle.m_bMultiShow		=  m_bMultiShow;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+444]
	mov	DWORD PTR [eax+444], edx

; 1101 : 	Missle.m_MissleRes.m_bLoopAnim = m_MissleRes.m_bLoopAnim;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+496]
	mov	DWORD PTR [eax+496], edx

; 1102 : 	Missle.m_MissleRes.m_bHaveEnd = FALSE;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	DWORD PTR [eax+492], 0

; 1103 : 	Missle.m_btRedLum		= m_btRedLum;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+452]
	mov	DWORD PTR [ecx+452], eax

; 1104 : 	Missle.m_btGreenLum		= m_btGreenLum;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+456]
	mov	DWORD PTR [ecx+456], eax

; 1105 : 	Missle.m_btBlueLum		= m_btBlueLum;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+460]
	mov	DWORD PTR [ecx+460], eax

; 1106 : 	Missle.m_usLightRadius	= m_usLightRadius;

	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	ax, WORD PTR [edx+464]
	mov	WORD PTR [ecx+464], ax

; 1107 : 	int nOffset = 0;

	mov	DWORD PTR _nOffset$[ebp], 0

; 1108 : 	
; 1109 : 	//如果是相同的子弹可以以不同方式显示时，则随机产生
; 1110 : 	if (m_bMultiShow)		

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+444], 0
	je	SHORT $L105456

; 1112 : 		if (g_Random(2) == 0)

	push	2
	call	DWORD PTR __imp_?g_Random@@YAII@Z
	add	esp, 4
	test	eax, eax
	jne	SHORT $L105455

; 1114 : 			nOffset = 0;

	mov	DWORD PTR _nOffset$[ebp], 0

; 1116 : 		else

	jmp	SHORT $L105456
$L105455:

; 1117 : 			nOffset = MAX_MISSLE_STATUS;

	mov	DWORD PTR _nOffset$[ebp], 4
$L105456:

; 1119 : 	
; 1120 : 	for (int t = 0; t < MAX_MISSLE_STATUS ; t++)

	mov	DWORD PTR _t$[ebp], 0
	jmp	SHORT $L105458
$L105459:
	mov	edx, DWORD PTR _t$[ebp]
	add	edx, 1
	mov	DWORD PTR _t$[ebp], edx
$L105458:
	cmp	DWORD PTR _t$[ebp], 4
	jge	$L105460

; 1122 : 		strcpy(Missle.m_MissleRes.m_MissleRes[t].AnimFileName,m_MissleRes.m_MissleRes[t + nOffset].AnimFileName);

	mov	eax, DWORD PTR _t$[ebp]
	add	eax, DWORD PTR _nOffset$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	lea	edx, DWORD PTR [ecx+eax+500]
	push	edx
	mov	eax, DWORD PTR _t$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _Missle$[ebp]
	lea	edx, DWORD PTR [ecx+eax+500]
	push	edx
	call	_strcpy
	add	esp, 8

; 1123 : 		
; 1124 : 		Missle.m_MissleRes.m_MissleRes[t].nTotalFrame = m_MissleRes.m_MissleRes[t + nOffset].nTotalFrame;

	mov	eax, DWORD PTR _t$[ebp]
	add	eax, DWORD PTR _nOffset$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _t$[ebp]
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _Missle$[ebp]
	mov	esi, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [esi+eax+600]
	mov	DWORD PTR [edx+ecx+600], eax

; 1125 : 		Missle.m_MissleRes.m_MissleRes[t].nDir = m_MissleRes.m_MissleRes[t + nOffset].nDir;

	mov	ecx, DWORD PTR _t$[ebp]
	add	ecx, DWORD PTR _nOffset$[ebp]
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _t$[ebp]
	imul	edx, 212				; 000000d4H
	mov	eax, DWORD PTR _Missle$[ebp]
	mov	esi, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [esi+ecx+608]
	mov	DWORD PTR [eax+edx+608], ecx

; 1126 : 		Missle.m_MissleRes.m_MissleRes[t].nInterval = m_MissleRes.m_MissleRes[t + nOffset].nInterval;

	mov	edx, DWORD PTR _t$[ebp]
	add	edx, DWORD PTR _nOffset$[ebp]
	imul	edx, 212				; 000000d4H
	mov	eax, DWORD PTR _t$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _Missle$[ebp]
	mov	esi, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [esi+edx+604]
	mov	DWORD PTR [ecx+eax+604], edx

; 1127 : 		
; 1128 : 		strcpy(Missle.m_MissleRes.m_MissleRes[t].SndFileName,m_MissleRes.m_MissleRes[t + nOffset].SndFileName);

	mov	eax, DWORD PTR _t$[ebp]
	add	eax, DWORD PTR _nOffset$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _this$[ebp]
	lea	edx, DWORD PTR [ecx+eax+612]
	push	edx
	mov	eax, DWORD PTR _t$[ebp]
	imul	eax, 212				; 000000d4H
	mov	ecx, DWORD PTR _Missle$[ebp]
	lea	edx, DWORD PTR [ecx+eax+612]
	push	edx
	call	_strcpy
	add	esp, 8

; 1129 : 	}

	jmp	$L105459
$L105460:

; 1130 : 	Missle.m_MissleRes.m_bSubLoop = m_MissleRes.m_bSubLoop;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+2248]
	mov	DWORD PTR [eax+2248], edx

; 1131 : 	Missle.m_MissleRes.m_nSubStart = m_MissleRes.m_nSubStart;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+2252]
	mov	DWORD PTR [eax+2252], edx

; 1132 : 	Missle.m_MissleRes.m_nSubStop = m_MissleRes.m_nSubStop;

	mov	eax, DWORD PTR _Missle$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+2256]
	mov	DWORD PTR [eax+2256], edx

; 1133 : #endif	
; 1134 : 	
; 1135 : 	return (Missle);

	mov	eax, DWORD PTR _Missle$[ebp]

; 1136 : }

	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	4
??4KMissle@@AAEAAV0@AAV0@@Z ENDP			; KMissle::operator=
_this$ = -4
?ProcessDamage@KMissle@@AAEHH@Z PROC NEAR		; KMissle::ProcessDamage

; 1147 : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 1148 : #ifdef _SERVER
; 1149 : 	bool bCalDamage = false;
; 1150 : 
; 1151 : 	_ASSERT (Npc[m_nLauncher].IsMatch(m_dwLauncherId));
; 1152 : 	
; 1153 : 	if (m_pMagicAttribsData) 
; 1154 : 	{
; 1155 : 		if (Npc[nNpcId].ReceiveDamage(m_nLauncher, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_bDoHurt,m_nEnChance))
; 1156 : 		{
; 1157 : 			if (m_pMagicAttribsData->m_nStateMagicAttribsNum > 0)
; 1158 : 				Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
; 1159 : 			
; 1160 : 			if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
; 1161 : 				Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
; 1162 : 		}
; 1163 : 		return TRUE;
; 1164 : 	}
; 1165 : #endif //_SERVER
; 1166 : 	return FALSE;

	xor	eax, eax

; 1167 : }

	mov	esp, ebp
	pop	ebp
	ret	4
?ProcessDamage@KMissle@@AAEHH@Z ENDP			; KMissle::ProcessDamage
_TEXT	ENDS
EXTRN	?Vanish@KSkill@@ABEXPAVKMissle@@@Z:NEAR		; KSkill::Vanish
_TEXT	SEGMENT
_this$ = -8
_pOrdinSkill$105471 = -4
?DoVanish@KMissle@@AAEXXZ PROC NEAR			; KMissle::DoVanish

; 1170 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx

; 1171 : 	if (m_eMissleStatus == MS_DoVanish) return ;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+356], 2
	jne	SHORT $L105468
	jmp	SHORT $L105467
$L105468:

; 1172 : 	
; 1173 : #ifndef _SERVER
; 1174 : 	m_MissleRes.m_bHaveEnd = TRUE;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+492], 1

; 1175 : 	m_nCollideOrVanishTime = m_nCurrentLife;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+208]
	mov	DWORD PTR [edx+216], ecx

; 1176 : #endif
; 1177 : 	if (m_bVanishedEvent)	

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+196], 0
	je	SHORT $L105473

; 1179 : 		_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
; 1180 : 		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId,m_nLevel);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+324]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+104]
	push	eax
	mov	ecx, OFFSET FLAT:?g_SkillManager@@3VKSkillManager@@A
	call	?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z ; KSkillManager::GetSkill
	mov	DWORD PTR _pOrdinSkill$105471[ebp], eax

; 1181 : 		if (pOrdinSkill)

	cmp	DWORD PTR _pOrdinSkill$105471[ebp], 0
	je	SHORT $L105473

; 1183 : 			pOrdinSkill->Vanish(this);

	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	mov	ecx, DWORD PTR _pOrdinSkill$105471[ebp]
	call	?Vanish@KSkill@@ABEXPAVKMissle@@@Z	; KSkill::Vanish
$L105473:

; 1186 : #ifdef _SERVER	//服务器端时子弹一旦进入消亡期则直接删除掉
; 1187 : 	SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_DEL, m_nMissleId);
; 1188 : 	m_eMissleStatus = MS_DoVanish;
; 1189 : 	return ;
; 1190 : #endif
; 1191 : 	m_eMissleStatus = MS_DoVanish;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+356], 2

; 1192 : #ifndef _SERVER 
; 1193 : 	if (m_nRegionId < 0)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+368], 0
	jge	SHORT $L105474

; 1195 : 		_ASSERT(0);
; 1196 : 		m_bRemoving = TRUE;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+72], 1
$L105474:
$L105467:

; 1199 : #endif
; 1200 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?DoVanish@KMissle@@AAEXXZ ENDP				; KMissle::DoVanish
_TEXT	ENDS
EXTRN	?Collidsion@KSkill@@ABEXPAVKMissle@@@Z:NEAR	; KSkill::Collidsion
_TEXT	SEGMENT
_this$ = -24
_nSrcX$ = -4
_nSrcY$ = -8
_pOrdinSkill$105484 = -12
_nSrcX5$105488 = -16
_nSrcY5$105489 = -20
?DoCollision@KMissle@@AAEXXZ PROC NEAR			; KMissle::DoCollision

; 1203 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 24					; 00000018H
	mov	DWORD PTR _this$[ebp], ecx

; 1204 : 	if (m_eMissleStatus == MS_DoCollision) return;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+356], 3
	jne	SHORT $L105479
	jmp	$L105478
$L105479:

; 1205 : 	
; 1206 : #ifndef _SERVER
; 1207 : 	int nSrcX = 0 ;

	mov	DWORD PTR _nSrcX$[ebp], 0

; 1208 : 	int nSrcY = 0 ;

	mov	DWORD PTR _nSrcY$[ebp], 0

; 1209 : 	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);

	lea	ecx, DWORD PTR _nSrcY$[ebp]
	push	ecx
	lea	edx, DWORD PTR _nSrcX$[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+232]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 1210 : #endif
; 1211 : 	
; 1212 : 	if (m_bCollideEvent)	

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+192], 0
	je	SHORT $L105486

; 1214 : 		_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
; 1215 : 		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_nSkillId, m_nLevel);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+324]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+104]
	push	edx
	mov	ecx, OFFSET FLAT:?g_SkillManager@@3VKSkillManager@@A
	call	?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z ; KSkillManager::GetSkill
	mov	DWORD PTR _pOrdinSkill$105484[ebp], eax

; 1216 : 		if (pOrdinSkill)

	cmp	DWORD PTR _pOrdinSkill$105484[ebp], 0
	je	SHORT $L105486

; 1218 : 			pOrdinSkill->Collidsion(this);

	mov	eax, DWORD PTR _this$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pOrdinSkill$105484[ebp]
	call	?Collidsion@KSkill@@ABEXPAVKMissle@@@Z	; KSkill::Collidsion
$L105486:

; 1221 : 	
; 1222 : 	if (m_bCollideVanish)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+156], 0
	je	$L105487

; 1224 : #ifndef _SERVER
; 1225 : 		m_MissleRes.m_bHaveEnd = TRUE;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+492], 1

; 1226 : #endif
; 1227 : 		
; 1228 : #ifndef _SERVER 
; 1229 : 		int nSrcX5 = 0 ;

	mov	DWORD PTR _nSrcX5$105488[ebp], 0

; 1230 : 		int nSrcY5 = 0 ;

	mov	DWORD PTR _nSrcY5$105489[ebp], 0

; 1231 : 		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX5, &nSrcY5);

	lea	eax, DWORD PTR _nSrcY5$105489[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSrcX5$105488[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+236]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 1232 : 		CreateSpecialEffect(MS_DoVanish, nSrcX5, nSrcY5, m_nCurrentMapZ);

	push	0
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+228]
	push	ecx
	mov	edx, DWORD PTR _nSrcY5$105489[ebp]
	push	edx
	mov	eax, DWORD PTR _nSrcX5$105488[ebp]
	push	eax
	push	2
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 1233 : #endif
; 1234 : 		
; 1235 : 		DoVanish();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?DoVanish@KMissle@@AAEXXZ		; KMissle::DoVanish

; 1237 : 	else 

	jmp	SHORT $L105490
$L105487:

; 1239 : #ifndef _SERVER		
; 1240 : 		//增加撞后的效果	
; 1241 : 		if (m_MissleRes.SpecialMovieIsAllEnd())

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?SpecialMovieIsAllEnd@KMissleRes@@QAEHXZ ; KMissleRes::SpecialMovieIsAllEnd
	test	eax, eax
	je	SHORT $L105491

; 1242 : 			CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);

	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX$[ebp]
	push	ecx
	push	3
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect
$L105491:

; 1243 : #endif
; 1244 : 		m_eMissleStatus = MS_DoFly;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+356], 1
$L105490:
$L105478:

; 1246 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?DoCollision@KMissle@@AAEXXZ ENDP			; KMissle::DoCollision
_this$ = -4
?DoFly@KMissle@@AAEXXZ PROC NEAR			; KMissle::DoFly

; 1249 : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 1250 : 	if (m_eMissleStatus == MS_DoFly) return ;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+356], 1
	jne	SHORT $L105495
	jmp	SHORT $L105494
$L105495:

; 1251 : 	//初始化贴图
; 1252 : 	m_eMissleStatus = MS_DoFly;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+356], 1
$L105494:

; 1253 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?DoFly@KMissle@@AAEXXZ ENDP				; KMissle::DoFly
_nSubWorld$ = 8
_nSrcRegionId$ = 12
_nSrcMapX$ = 16
_nSrcMapY$ = 20
_nOffsetMapX$ = 24
_nOffsetMapY$ = 28
_nDesRegionId$ = 32
_nDesMapX$ = 36
_nDesMapY$ = 40
_nSearchRegion$ = -4
?GetOffsetAxis@KMissle@@SAHHHHHHHAAH00@Z PROC NEAR	; KMissle::GetOffsetAxis

; 1258 : {

	push	ebp
	mov	ebp, esp
	push	ecx

; 1259 : 	nDesRegionId = -1;

	mov	eax, DWORD PTR _nDesRegionId$[ebp]
	mov	DWORD PTR [eax], -1

; 1260 : 	// 确定目标格子实际的REGION和坐标确定
; 1261 : 	nDesMapX = nSrcMapX + nOffsetMapX;

	mov	ecx, DWORD PTR _nSrcMapX$[ebp]
	add	ecx, DWORD PTR _nOffsetMapX$[ebp]
	mov	edx, DWORD PTR _nDesMapX$[ebp]
	mov	DWORD PTR [edx], ecx

; 1262 : 	nDesMapY = nSrcMapY + nOffsetMapY;

	mov	eax, DWORD PTR _nSrcMapY$[ebp]
	add	eax, DWORD PTR _nOffsetMapY$[ebp]
	mov	ecx, DWORD PTR _nDesMapY$[ebp]
	mov	DWORD PTR [ecx], eax

; 1263 : 	
; 1264 : 	if (nSrcRegionId < 0) 

	cmp	DWORD PTR _nSrcRegionId$[ebp], 0
	jge	SHORT $L105507

; 1265 : 		return FALSE;

	xor	eax, eax
	jmp	$L105506
$L105507:

; 1266 : 
; 1267 : 	int nSearchRegion = nSrcRegionId;

	mov	edx, DWORD PTR _nSrcRegionId$[ebp]
	mov	DWORD PTR _nSearchRegion$[ebp], edx

; 1268 : 	if (nDesMapX < 0)

	mov	eax, DWORD PTR _nDesMapX$[ebp]
	cmp	DWORD PTR [eax], 0
	jge	SHORT $L105509

; 1270 : 		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];

	mov	ecx, DWORD PTR _nSubWorld$[ebp]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nSearchRegion$[ebp]
	imul	edx, 208				; 000000d0H
	mov	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	mov	ecx, DWORD PTR [eax+edx+112]
	mov	DWORD PTR _nSearchRegion$[ebp], ecx

; 1271 : 		nDesMapX += SubWorld[nSubWorld].m_nRegionWidth;

	mov	edx, DWORD PTR _nSubWorld$[ebp]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nDesMapX$[ebp]
	mov	ecx, DWORD PTR [eax]
	add	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+140]
	mov	edx, DWORD PTR _nDesMapX$[ebp]
	mov	DWORD PTR [edx], ecx

; 1273 : 	else if (nDesMapX >= SubWorld[nSubWorld].m_nRegionWidth)

	jmp	SHORT $L105511
$L105509:
	mov	eax, DWORD PTR _nSubWorld$[ebp]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _nDesMapX$[ebp]
	mov	edx, DWORD PTR [ecx]
	cmp	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+140]
	jl	SHORT $L105511

; 1275 : 		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];

	mov	eax, DWORD PTR _nSubWorld$[ebp]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _nSearchRegion$[ebp]
	imul	ecx, 208				; 000000d0H
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+8]
	mov	eax, DWORD PTR [edx+ecx+128]
	mov	DWORD PTR _nSearchRegion$[ebp], eax

; 1276 : 		nDesMapX -= SubWorld[nSubWorld].m_nRegionWidth;

	mov	ecx, DWORD PTR _nSubWorld$[ebp]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nDesMapX$[ebp]
	mov	eax, DWORD PTR [edx]
	sub	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+140]
	mov	ecx, DWORD PTR _nDesMapX$[ebp]
	mov	DWORD PTR [ecx], eax
$L105511:

; 1278 : 	if (nSearchRegion < 0) 

	cmp	DWORD PTR _nSearchRegion$[ebp], 0
	jge	SHORT $L105512

; 1279 : 		return FALSE;

	xor	eax, eax
	jmp	$L105506
$L105512:

; 1280 : 	
; 1281 : 	if (nDesMapY < 0)

	mov	edx, DWORD PTR _nDesMapY$[ebp]
	cmp	DWORD PTR [edx], 0
	jge	SHORT $L105513

; 1283 : 		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];

	mov	eax, DWORD PTR _nSubWorld$[ebp]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _nSearchRegion$[ebp]
	imul	ecx, 208				; 000000d0H
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+8]
	mov	eax, DWORD PTR [edx+ecx+120]
	mov	DWORD PTR _nSearchRegion$[ebp], eax

; 1284 : 		nDesMapY += SubWorld[nSubWorld].m_nRegionHeight;

	mov	ecx, DWORD PTR _nSubWorld$[ebp]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nDesMapY$[ebp]
	mov	eax, DWORD PTR [edx]
	add	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+144]
	mov	ecx, DWORD PTR _nDesMapY$[ebp]
	mov	DWORD PTR [ecx], eax

; 1286 : 	else if (nDesMapY >= SubWorld[nSubWorld].m_nRegionHeight)

	jmp	SHORT $L105515
$L105513:
	mov	edx, DWORD PTR _nSubWorld$[ebp]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nDesMapY$[ebp]
	mov	ecx, DWORD PTR [eax]
	cmp	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+144]
	jl	SHORT $L105515

; 1288 : 		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];

	mov	edx, DWORD PTR _nSubWorld$[ebp]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nSearchRegion$[ebp]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	mov	edx, DWORD PTR [ecx+eax+104]
	mov	DWORD PTR _nSearchRegion$[ebp], edx

; 1289 : 		nDesMapY -= SubWorld[nSubWorld].m_nRegionHeight;

	mov	eax, DWORD PTR _nSubWorld$[ebp]
	imul	eax, 220				; 000000dcH
	mov	ecx, DWORD PTR _nDesMapY$[ebp]
	mov	edx, DWORD PTR [ecx]
	sub	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[eax+144]
	mov	eax, DWORD PTR _nDesMapY$[ebp]
	mov	DWORD PTR [eax], edx
$L105515:

; 1291 : 
; 1292 : 	if (nSearchRegion < 0) 

	cmp	DWORD PTR _nSearchRegion$[ebp], 0
	jge	SHORT $L105516

; 1293 : 		return FALSE;

	xor	eax, eax
	jmp	SHORT $L105506
$L105516:

; 1294 : 	nDesRegionId = nSearchRegion;

	mov	ecx, DWORD PTR _nDesRegionId$[ebp]
	mov	edx, DWORD PTR _nSearchRegion$[ebp]
	mov	DWORD PTR [ecx], edx

; 1295 : 	return TRUE;

	mov	eax, 1
$L105506:

; 1296 : 	// 从REGION的NPC列表中查找满足条件的NPC		
; 1297 : 	//int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nDesMapX, nDesMapY, nLauncherIdx, relation_all);
; 1298 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?GetOffsetAxis@KMissle@@SAHHHHHHHAAH00@Z ENDP		; KMissle::GetOffsetAxis
_nLauncherIdx$ = 8
_nRegionId$ = 12
_nMapX$ = 16
_nMapY$ = 20
_nRange$ = 24
_eRelation$ = 28
_this$ = -56
_nRangeX$ = -8
_nRangeY$ = -4
_nSubWorld$ = -28
_nRegion$ = -12
_nRet$ = -16
_nRMx$ = -24
_nRMy$ = -32
_nSearchRegion$ = -36
_i$ = -20
_j$105542 = -40
_nNpcIdx$105548 = -44
_nSrcX$105550 = -48
_nSrcY$105551 = -52
?ProcessCollision@KMissle@@AAEHHHHHHH@Z PROC NEAR	; KMissle::ProcessCollision

; 1314 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 56					; 00000038H
	mov	DWORD PTR _this$[ebp], ecx

; 1315 : #ifdef TOOLVERSION 
; 1316 : 	return 0;
; 1317 : #endif
; 1318 : #ifdef _SERVER
; 1319 : 	if (m_ulDamageInterval)
; 1320 : 	{
; 1321 : 		if (m_ulNextCalDamageTime > g_SubWorldSet.GetGameTime())
; 1322 : 		{
; 1323 : 			return FALSE;
; 1324 : 		}
; 1325 : 		else
; 1326 : 		{
; 1327 : 			// 6.29 romandou missledamage interval 
; 1328 : 			m_ulNextCalDamageTime = g_SubWorldSet.GetGameTime() + m_ulDamageInterval;
; 1329 : 		}
; 1330 : 	}
; 1331 : #endif
; 1332 : 	if (nLauncherIdx <= 0 ) return 0;

	cmp	DWORD PTR _nLauncherIdx$[ebp], 0
	jg	SHORT $L105526
	xor	eax, eax
	jmp	$L105525
$L105526:

; 1333 : 	if (nRange <= 0) return 0;

	cmp	DWORD PTR _nRange$[ebp], 0
	jg	SHORT $L105527
	xor	eax, eax
	jmp	$L105525
$L105527:

; 1334 : 	
; 1335 : 	int nRangeX = nRange / 2;

	mov	eax, DWORD PTR _nRange$[ebp]
	cdq
	sub	eax, edx
	sar	eax, 1
	mov	DWORD PTR _nRangeX$[ebp], eax

; 1336 : 	int	nRangeY = nRangeX;

	mov	eax, DWORD PTR _nRangeX$[ebp]
	mov	DWORD PTR _nRangeY$[ebp], eax

; 1337 : 	int	nSubWorld = Npc[nLauncherIdx].m_SubWorldIndex;

	mov	ecx, DWORD PTR _nLauncherIdx$[ebp]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3060]
	mov	DWORD PTR _nSubWorld$[ebp], edx

; 1338 : 	
; 1339 : 	_ASSERT(Npc[nLauncherIdx].m_SubWorldIndex >= 0);
; 1340 : 	_ASSERT(nRegionId >= 0);
; 1341 : 	
; 1342 : 	int	nRegion = nRegionId;

	mov	eax, DWORD PTR _nRegionId$[ebp]
	mov	DWORD PTR _nRegion$[ebp], eax

; 1343 : 	int	nRet = 0;

	mov	DWORD PTR _nRet$[ebp], 0

; 1344 : 	int	nRMx, nRMy, nSearchRegion;
; 1345 : 
; 1346 : 	// 检查范围内的格子里的NPC
; 1347 : 	for (int i = -nRangeX; i <= nRangeX; i++)

	mov	ecx, DWORD PTR _nRangeX$[ebp]
	neg	ecx
	mov	DWORD PTR _i$[ebp], ecx
	jmp	SHORT $L105539
$L105540:
	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 1
	mov	DWORD PTR _i$[ebp], edx
$L105539:
	mov	eax, DWORD PTR _i$[ebp]
	cmp	eax, DWORD PTR _nRangeX$[ebp]
	jg	$L105541

; 1349 : 		for (int j = -nRangeY; j <= nRangeY; j++)

	mov	ecx, DWORD PTR _nRangeY$[ebp]
	neg	ecx
	mov	DWORD PTR _j$105542[ebp], ecx
	jmp	SHORT $L105543
$L105544:
	mov	edx, DWORD PTR _j$105542[ebp]
	add	edx, 1
	mov	DWORD PTR _j$105542[ebp], edx
$L105543:
	mov	eax, DWORD PTR _j$105542[ebp]
	cmp	eax, DWORD PTR _nRangeY$[ebp]
	jg	$L105545

; 1351 : 			// 去掉边角几个格子，保证视野是椭圆形
; 1352 : 			//if ((i * i + j * j ) > nRangeX * nRangeX)
; 1353 : 			//continue;
; 1354 : 
; 1355 : 			if (!GetOffsetAxis(nSubWorld, nRegionId, nMapX, nMapY, i , j , nSearchRegion, nRMx, nRMy))

	lea	ecx, DWORD PTR _nRMy$[ebp]
	push	ecx
	lea	edx, DWORD PTR _nRMx$[ebp]
	push	edx
	lea	eax, DWORD PTR _nSearchRegion$[ebp]
	push	eax
	mov	ecx, DWORD PTR _j$105542[ebp]
	push	ecx
	mov	edx, DWORD PTR _i$[ebp]
	push	edx
	mov	eax, DWORD PTR _nMapY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nMapX$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nRegionId$[ebp]
	push	edx
	mov	eax, DWORD PTR _nSubWorld$[ebp]
	push	eax
	call	?GetOffsetAxis@KMissle@@SAHHHHHHHAAH00@Z ; KMissle::GetOffsetAxis
	add	esp, 36					; 00000024H
	test	eax, eax
	jne	SHORT $L105546

; 1356 : 				continue;

	jmp	SHORT $L105544
$L105546:

; 1359 : 
; 1360 : 			// 从REGION的NPC列表中查找满足条件的NPC		
; 1361 : 			int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation);

	mov	ecx, DWORD PTR _eRelation$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nLauncherIdx$[ebp]
	push	edx
	mov	eax, DWORD PTR _nRMy$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nRMx$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nSubWorld$[ebp]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _nSearchRegion$[ebp]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	add	ecx, eax
	call	?FindNpc@KRegion@@QAEHHHHH@Z		; KRegion::FindNpc
	mov	DWORD PTR _nNpcIdx$105548[ebp], eax

; 1362 : 			if (nNpcIdx > 0)	

	cmp	DWORD PTR _nNpcIdx$105548[ebp], 0
	jle	$L105553

; 1364 : 				nRet++;

	mov	ecx, DWORD PTR _nRet$[ebp]
	add	ecx, 1
	mov	DWORD PTR _nRet$[ebp], ecx

; 1365 : #ifndef _SERVER
; 1366 : 				int nSrcX = 0;

	mov	DWORD PTR _nSrcX$105550[ebp], 0

; 1367 : 				int nSrcY = 0;

	mov	DWORD PTR _nSrcY$105551[ebp], 0

; 1368 : 				SubWorld[0].Map2Mps(nSearchRegion, Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY,  &nSrcX, &nSrcY);

	lea	edx, DWORD PTR _nSrcY$105551[ebp]
	push	edx
	lea	eax, DWORD PTR _nSrcX$105550[ebp]
	push	eax
	mov	ecx, DWORD PTR _nNpcIdx$105548[ebp]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3556]
	push	edx
	mov	eax, DWORD PTR _nNpcIdx$105548[ebp]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3552]
	push	ecx
	mov	edx, DWORD PTR _nNpcIdx$105548[ebp]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3544]
	push	eax
	mov	ecx, DWORD PTR _nNpcIdx$105548[ebp]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3540]
	push	edx
	mov	eax, DWORD PTR _nSearchRegion$[ebp]
	push	eax
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 1369 : 				
; 1370 : 				if (m_bFollowNpcWhenCollid)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+448], 0
	je	SHORT $L105552

; 1371 : 					CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ, nNpcIdx);

	mov	edx, DWORD PTR _nNpcIdx$105548[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+228]
	push	ecx
	mov	edx, DWORD PTR _nSrcY$105551[ebp]
	push	edx
	mov	eax, DWORD PTR _nSrcX$105550[ebp]
	push	eax
	push	3
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 1372 : 				else 

	jmp	SHORT $L105553
$L105552:

; 1373 : 					CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);

	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY$105551[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX$105550[ebp]
	push	ecx
	push	3
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect
$L105553:

; 1378 : 		}

	jmp	$L105544
$L105545:

; 1379 : 	}

	jmp	$L105540
$L105541:

; 1380 : 	return nRet;

	mov	eax, DWORD PTR _nRet$[ebp]
$L105525:

; 1381 : }

	mov	esp, ebp
	pop	ebp
	ret	24					; 00000018H
?ProcessCollision@KMissle@@AAEHHHHHHH@Z ENDP		; KMissle::ProcessCollision
_this$ = -4
?ProcessCollision@KMissle@@AAEHXZ PROC NEAR		; KMissle::ProcessCollision

; 1385 : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 1386 : #ifdef TOOLVERSION
; 1387 : 	return 0;
; 1388 : #endif
; 1389 : 	if (m_bClientSend) return 0;

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+68], 0
	je	SHORT $L105557
	xor	eax, eax
	jmp	SHORT $L105556
$L105557:

; 1390 : 	return ProcessCollision(m_nLauncher, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nDamageRange , m_eRelation);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+112]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+152]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+224]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+220]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?ProcessCollision@KMissle@@AAEHHHHHHH@Z	; KMissle::ProcessCollision
$L105556:

; 1391 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?ProcessCollision@KMissle@@AAEHXZ ENDP			; KMissle::ProcessCollision
_TEXT	ENDS
PUBLIC	??0KSkillSpecialNode@@QAE@XZ			; KSkillSpecialNode::KSkillSpecialNode
EXTRN	??0KSkillSpecial@@QAE@XZ:NEAR			; KSkillSpecial::KSkillSpecial
EXTRN	?Init@KSkillSpecial@@QAEHXZ:NEAR		; KSkillSpecial::Init
EXTRN	?g_SubWorldSet@@3VKSubWorldSet@@A:BYTE		; g_SubWorldSet
xdata$x	SEGMENT
$T106224 DD	019930520H
	DD	02H
	DD	FLAT:$T106226
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T106226 DD	0ffffffffH
	DD	FLAT:$L106221
	DD	0ffffffffH
	DD	FLAT:$L106222
xdata$x	ENDS
_TEXT	SEGMENT
_eStatus$ = 8
_nPX$ = 12
_nPY$ = 16
_nPZ$ = 20
_nNpcIndex$ = 24
_this$ = -48
_pNode$ = -20
_pSkillSpecial$ = -16
_nSrcX$ = -24
_nSrcY$ = -28
$T106213 = -32
$T106214 = -36
$T106217 = -40
$T106218 = -44
__$EHRec$ = -12
?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z PROC NEAR ; KMissle::CreateSpecialEffect

; 1397 : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L106225
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 44					; 0000002cH
	push	esi
	mov	DWORD PTR _this$[ebp], ecx

; 1398 : 	
; 1399 : 	KSkillSpecialNode * pNode = NULL;

	mov	DWORD PTR _pNode$[ebp], 0

; 1400 : 	//同一颗子碟不能有几个爆炸效果在一个Npc身上
; 1401 : 	if (nNpcIndex > 0)

	cmp	DWORD PTR _nNpcIndex$[ebp], 0
	jle	SHORT $L105572

; 1403 : 		pNode = (KSkillSpecialNode*)m_MissleRes.m_SkillSpecialList.GetHead();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?GetHead@KList@@QAEPAVKNode@@XZ		; KList::GetHead
	mov	DWORD PTR _pNode$[ebp], eax
$L105571:

; 1404 : 		while(pNode)

	cmp	DWORD PTR _pNode$[ebp], 0
	je	SHORT $L105572

; 1406 : 			if (pNode->m_pSkillSpecial->m_dwMatchID == Npc[nNpcIndex].m_dwID) return FALSE;

	mov	eax, DWORD PTR _pNode$[ebp]
	mov	ecx, DWORD PTR [eax+12]
	mov	edx, DWORD PTR _nNpcIndex$[ebp]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR [ecx+44]
	cmp	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[edx]
	jne	SHORT $L105573
	xor	eax, eax
	jmp	$L105566
$L105573:

; 1407 : 			pNode = (KSkillSpecialNode*)pNode->GetNext();

	mov	ecx, DWORD PTR _pNode$[ebp]
	call	?GetNext@KNode@@QAEPAV1@XZ		; KNode::GetNext
	mov	DWORD PTR _pNode$[ebp], eax

; 1408 : 		}

	jmp	SHORT $L105571
$L105572:

; 1410 : 	m_MissleRes.PlaySound(eStatus, nPX, nPY, 0);

	push	0
	mov	ecx, DWORD PTR _nPY$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nPX$[ebp]
	push	edx
	mov	eax, DWORD PTR _eStatus$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?PlaySoundA@KMissleRes@@QAEXHHHH@Z	; KMissleRes::PlaySoundA

; 1411 : 	if (!m_MissleRes.m_MissleRes[eStatus].AnimFileName[0]) return FALSE; 

	mov	ecx, DWORD PTR _eStatus$[ebp]
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _this$[ebp]
	movsx	eax, BYTE PTR [edx+ecx+500]
	test	eax, eax
	jne	SHORT $L105575
	xor	eax, eax
	jmp	$L105566
$L105575:

; 1412 : 	pNode = new KSkillSpecialNode;

	push	16					; 00000010H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T106214[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	cmp	DWORD PTR $T106214[ebp], 0
	je	SHORT $L106215
	mov	ecx, DWORD PTR $T106214[ebp]
	call	??0KSkillSpecialNode@@QAE@XZ		; KSkillSpecialNode::KSkillSpecialNode
	mov	DWORD PTR -52+[ebp], eax
	jmp	SHORT $L106216
$L106215:
	mov	DWORD PTR -52+[ebp], 0
$L106216:
	mov	ecx, DWORD PTR -52+[ebp]
	mov	DWORD PTR $T106213[ebp], ecx
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	edx, DWORD PTR $T106213[ebp]
	mov	DWORD PTR _pNode$[ebp], edx

; 1413 : 	KSkillSpecial * pSkillSpecial = new KSkillSpecial;

	push	240					; 000000f0H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T106218[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], 1
	cmp	DWORD PTR $T106218[ebp], 0
	je	SHORT $L106219
	mov	ecx, DWORD PTR $T106218[ebp]
	call	??0KSkillSpecial@@QAE@XZ		; KSkillSpecial::KSkillSpecial
	mov	DWORD PTR -56+[ebp], eax
	jmp	SHORT $L106220
$L106219:
	mov	DWORD PTR -56+[ebp], 0
$L106220:
	mov	eax, DWORD PTR -56+[ebp]
	mov	DWORD PTR $T106217[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	ecx, DWORD PTR $T106217[ebp]
	mov	DWORD PTR _pSkillSpecial$[ebp], ecx

; 1414 : 	pNode->m_pSkillSpecial = pSkillSpecial;

	mov	edx, DWORD PTR _pNode$[ebp]
	mov	eax, DWORD PTR _pSkillSpecial$[ebp]
	mov	DWORD PTR [edx+12], eax

; 1415 : 	
; 1416 : 	int nSrcX = nPX;

	mov	ecx, DWORD PTR _nPX$[ebp]
	mov	DWORD PTR _nSrcX$[ebp], ecx

; 1417 : 	int nSrcY = nPY;

	mov	edx, DWORD PTR _nPY$[ebp]
	mov	DWORD PTR _nSrcY$[ebp], edx

; 1418 : 	
; 1419 : 	pSkillSpecial->m_nPX = nSrcX;

	mov	eax, DWORD PTR _pSkillSpecial$[ebp]
	mov	ecx, DWORD PTR _nSrcX$[ebp]
	mov	DWORD PTR [eax+24], ecx

; 1420 : 	pSkillSpecial->m_nPY = nSrcY - 5;// MISSLE_Y_OFFSET;

	mov	edx, DWORD PTR _nSrcY$[ebp]
	sub	edx, 5
	mov	eax, DWORD PTR _pSkillSpecial$[ebp]
	mov	DWORD PTR [eax+28], edx

; 1421 : 	pSkillSpecial->m_nPZ = nPZ;

	mov	ecx, DWORD PTR _pSkillSpecial$[ebp]
	mov	edx, DWORD PTR _nPZ$[ebp]
	mov	DWORD PTR [ecx+32], edx

; 1422 : 	pSkillSpecial->m_nNpcIndex = nNpcIndex;

	mov	eax, DWORD PTR _pSkillSpecial$[ebp]
	mov	ecx, DWORD PTR _nNpcIndex$[ebp]
	mov	DWORD PTR [eax+40], ecx

; 1423 : 	pSkillSpecial->m_dwMatchID = Npc[nNpcIndex].m_dwID;

	mov	edx, DWORD PTR _nNpcIndex$[ebp]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR _pSkillSpecial$[ebp]
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[edx]
	mov	DWORD PTR [eax+44], ecx

; 1424 : 	pSkillSpecial->m_pMissleRes = &m_MissleRes.m_MissleRes[eStatus];

	mov	edx, DWORD PTR _eStatus$[ebp]
	imul	edx, 212				; 000000d4H
	mov	eax, DWORD PTR _this$[ebp]
	lea	ecx, DWORD PTR [eax+edx+500]
	mov	edx, DWORD PTR _pSkillSpecial$[ebp]
	mov	DWORD PTR [edx+48], ecx

; 1425 : 	pSkillSpecial->m_nBeginTime = g_SubWorldSet.GetGameTime();

	mov	ecx, OFFSET FLAT:?g_SubWorldSet@@3VKSubWorldSet@@A
	call	?GetGameTime@KSubWorldSet@@QAEHXZ	; KSubWorldSet::GetGameTime
	mov	ecx, DWORD PTR _pSkillSpecial$[ebp]
	mov	DWORD PTR [ecx+52], eax

; 1426 : 	pSkillSpecial->m_nEndTime = g_SubWorldSet.GetGameTime() + (pSkillSpecial->m_pMissleRes->nInterval * pSkillSpecial->m_pMissleRes->nTotalFrame / pSkillSpecial->m_pMissleRes->nDir);

	mov	ecx, OFFSET FLAT:?g_SubWorldSet@@3VKSubWorldSet@@A
	call	?GetGameTime@KSubWorldSet@@QAEHXZ	; KSubWorldSet::GetGameTime
	mov	ecx, eax
	mov	edx, DWORD PTR _pSkillSpecial$[ebp]
	mov	eax, DWORD PTR [edx+48]
	mov	edx, DWORD PTR _pSkillSpecial$[ebp]
	mov	edx, DWORD PTR [edx+48]
	mov	eax, DWORD PTR [eax+104]
	imul	eax, DWORD PTR [edx+100]
	mov	edx, DWORD PTR _pSkillSpecial$[ebp]
	mov	esi, DWORD PTR [edx+48]
	cdq
	idiv	DWORD PTR [esi+108]
	add	ecx, eax
	mov	eax, DWORD PTR _pSkillSpecial$[ebp]
	mov	DWORD PTR [eax+56], ecx

; 1427 : 	pSkillSpecial->m_nCurDir = g_DirIndex2Dir(m_nDirIndex, m_MissleRes.m_MissleRes[eStatus].nDir);

	mov	ecx, DWORD PTR _eStatus$[ebp]
	imul	ecx, 212				; 000000d4H
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+ecx+608]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+424]
	push	edx
	call	?g_DirIndex2Dir@@YAHHH@Z		; g_DirIndex2Dir
	add	esp, 8
	mov	ecx, DWORD PTR _pSkillSpecial$[ebp]
	mov	DWORD PTR [ecx+36], eax

; 1428 : 	pSkillSpecial->Init();

	mov	ecx, DWORD PTR _pSkillSpecial$[ebp]
	call	?Init@KSkillSpecial@@QAEHXZ		; KSkillSpecial::Init

; 1429 : 	m_MissleRes.m_SkillSpecialList.AddTail(pNode);

	mov	edx, DWORD PTR _pNode$[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 468				; 000001d4H
	call	?AddTail@KList@@QAEXPAVKNode@@@Z	; KList::AddTail

; 1430 : 	
; 1431 : 	return TRUE;

	mov	eax, 1
$L105566:

; 1432 : }

	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	20					; 00000014H
_TEXT	ENDS
text$x	SEGMENT
$L106221:
	mov	eax, DWORD PTR $T106214[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L106222:
	mov	eax, DWORD PTR $T106218[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L106225:
	mov	eax, OFFSET FLAT:$T106224
	jmp	___CxxFrameHandler
text$x	ENDS
?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ENDP ; KMissle::CreateSpecialEffect
PUBLIC	??_7KSkillSpecialNode@@6B@			; KSkillSpecialNode::`vftable'
PUBLIC	??_GKSkillSpecialNode@@UAEPAXI@Z		; KSkillSpecialNode::`scalar deleting destructor'
PUBLIC	??_EKSkillSpecialNode@@UAEPAXI@Z		; KSkillSpecialNode::`vector deleting destructor'
;	COMDAT ??_7KSkillSpecialNode@@6B@
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\KSkillSpecial.h
CONST	SEGMENT
??_7KSkillSpecialNode@@6B@ DD FLAT:??_EKSkillSpecialNode@@UAEPAXI@Z ; KSkillSpecialNode::`vftable'
CONST	ENDS
;	COMDAT ??0KSkillSpecialNode@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0KSkillSpecialNode@@QAE@XZ PROC NEAR			; KSkillSpecialNode::KSkillSpecialNode, COMDAT

; 56   : 	KSkillSpecialNode(){m_pSkillSpecial = NULL;};

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??0KNode@@QAE@XZ			; KNode::KNode
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KSkillSpecialNode@@6B@ ; KSkillSpecialNode::`vftable'
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+12], 0
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0KSkillSpecialNode@@QAE@XZ ENDP			; KSkillSpecialNode::KSkillSpecialNode
_TEXT	ENDS
PUBLIC	??1KSkillSpecialNode@@UAE@XZ			; KSkillSpecialNode::~KSkillSpecialNode
;	COMDAT ??_GKSkillSpecialNode@@UAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8
_this$ = -4
??_GKSkillSpecialNode@@UAEPAXI@Z PROC NEAR		; KSkillSpecialNode::`scalar deleting destructor', COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KSkillSpecialNode@@UAE@XZ		; KSkillSpecialNode::~KSkillSpecialNode
	mov	eax, DWORD PTR ___flags$[ebp]
	and	eax, 1
	test	eax, eax
	je	SHORT $L96705
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L96705:
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??_GKSkillSpecialNode@@UAEPAXI@Z ENDP			; KSkillSpecialNode::`scalar deleting destructor'
_TEXT	ENDS
;	COMDAT ??1KSkillSpecialNode@@UAE@XZ
_TEXT	SEGMENT
_this$ = -8
$T106233 = -4
??1KSkillSpecialNode@@UAE@XZ PROC NEAR			; KSkillSpecialNode::~KSkillSpecialNode, COMDAT

; 57   : 	~KSkillSpecialNode(){delete m_pSkillSpecial;};

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KSkillSpecialNode@@6B@ ; KSkillSpecialNode::`vftable'
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+12]
	mov	DWORD PTR $T106233[ebp], edx
	mov	eax, DWORD PTR $T106233[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KNode@@UAE@XZ			; KNode::~KNode
	mov	esp, ebp
	pop	ebp
	ret	0
??1KSkillSpecialNode@@UAE@XZ ENDP			; KSkillSpecialNode::~KSkillSpecialNode
_TEXT	ENDS
PUBLIC	?CreateMissleForShow@KMissle@@SAHPAD00PAUTMissleForShow@@@Z ; KMissle::CreateMissleForShow
PUBLIC	?g_Dir2DirIndex@@YAHHH@Z			; g_Dir2DirIndex
PUBLIC	?KSG_StringGetInt@@YAHPAPADH@Z			; KSG_StringGetInt
PUBLIC	?KSG_StringSkipSymbol@@YA_NPAPADH@Z		; KSG_StringSkipSymbol
EXTRN	?LoadResource@KMissleRes@@QAEXHPAD0@Z:NEAR	; KMissleRes::LoadResource
EXTRN	?Player@@3PAVKPlayer@@A:BYTE			; Player
EXTRN	?Add@KMissleSet@@QAEHHHH@Z:NEAR			; KMissleSet::Add
EXTRN	?MissleSet@@3VKMissleSet@@A:BYTE		; MissleSet
_TEXT	SEGMENT
_szMovie$ = 8
_szFormat$ = 12
_szSound$ = 16
_pShowParam$ = 20
_nPX$ = -4
_nPY$ = -8
_nPZ$ = -12
_nSubWorldId$ = -20
_nMissleIndex$ = -16
_pcszTemp$ = -24
?CreateMissleForShow@KMissle@@SAHPAD00PAUTMissleForShow@@@Z PROC NEAR ; KMissle::CreateMissleForShow

; 1435 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 24					; 00000018H

; 1436 : 	if (!pShowParam || !szMovie || !szMovie[0])

	cmp	DWORD PTR _pShowParam$[ebp], 0
	je	SHORT $L105594
	cmp	DWORD PTR _szMovie$[ebp], 0
	je	SHORT $L105594
	mov	eax, DWORD PTR _szMovie$[ebp]
	movsx	ecx, BYTE PTR [eax]
	test	ecx, ecx
	jne	SHORT $L105593
$L105594:

; 1437 : 		return FALSE;

	xor	eax, eax
	jmp	$L105592
$L105593:

; 1438 : 	int nPX = 0;

	mov	DWORD PTR _nPX$[ebp], 0

; 1439 : 	int nPY = 0;

	mov	DWORD PTR _nPY$[ebp], 0

; 1440 : 	int nPZ = 0;

	mov	DWORD PTR _nPZ$[ebp], 0

; 1441 : 	
; 1442 : 	if (pShowParam->nNpcIndex > 0)

	mov	edx, DWORD PTR _pShowParam$[ebp]
	cmp	DWORD PTR [edx+12], 0
	jle	SHORT $L105598

; 1444 : 		Npc[pShowParam->nNpcIndex].GetMpsPos(&nPX, &nPY);

	lea	eax, DWORD PTR _nPY$[ebp]
	push	eax
	lea	ecx, DWORD PTR _nPX$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pShowParam$[ebp]
	mov	ecx, DWORD PTR [edx+12]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?GetMpsPos@KNpc@@QAEXPAH0@Z		; KNpc::GetMpsPos

; 1446 : 	else

	jmp	SHORT $L105599
$L105598:

; 1448 : 		nPX = pShowParam->nPX;

	mov	eax, DWORD PTR _pShowParam$[ebp]
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR _nPX$[ebp], ecx

; 1449 : 		nPY = pShowParam->nPY;

	mov	edx, DWORD PTR _pShowParam$[ebp]
	mov	eax, DWORD PTR [edx+4]
	mov	DWORD PTR _nPY$[ebp], eax
$L105599:

; 1451 : 
; 1452 : 	int nSubWorldId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex;

	mov	ecx, DWORD PTR ?Player@@3PAVKPlayer@@A+58996
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx+3060]
	mov	DWORD PTR _nSubWorldId$[ebp], edx

; 1453 : 	int nMissleIndex = MissleSet.Add(nSubWorldId , nPX , nPY);

	mov	eax, DWORD PTR _nPY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nPX$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nSubWorldId$[ebp]
	push	edx
	mov	ecx, OFFSET FLAT:?MissleSet@@3VKMissleSet@@A
	call	?Add@KMissleSet@@QAEHHHH@Z		; KMissleSet::Add
	mov	DWORD PTR _nMissleIndex$[ebp], eax

; 1454 : 	if (nMissleIndex < 0)	

	cmp	DWORD PTR _nMissleIndex$[ebp], 0
	jge	SHORT $L105602

; 1455 : 		return FALSE;

	xor	eax, eax
	jmp	$L105592
$L105602:

; 1456 : 	
; 1457 : 	Missle[nMissleIndex].m_nDir				= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Dir;

	mov	eax, DWORD PTR ?Player@@3PAVKPlayer@@A+58996
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3524]
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+428], edx

; 1458 : 	Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(Missle[nMissleIndex].m_nDir, MaxMissleDir);

	push	64					; 00000040H
	mov	eax, DWORD PTR _nMissleIndex$[ebp]
	imul	eax, 2940				; 00000b7cH
	mov	ecx, DWORD PTR ?Missle@@3PAVKMissle@@A[eax+428]
	push	ecx
	call	?g_Dir2DirIndex@@YAHHH@Z		; g_Dir2DirIndex
	add	esp, 8
	mov	edx, DWORD PTR _nMissleIndex$[ebp]
	imul	edx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[edx+424], eax

; 1459 : 
; 1460 : 	Missle[nMissleIndex].m_nFollowNpcIdx	= 0;

	mov	eax, DWORD PTR _nMissleIndex$[ebp]
	imul	eax, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[eax+328], 0

; 1461 : 
; 1462 : 	Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;

	mov	ecx, DWORD PTR _nSubWorldId$[ebp]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nMissleIndex$[ebp]
	imul	edx, 2940				; 00000b7cH
	mov	eax, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+168]
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[edx+436], eax

; 1463 : 	Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;

	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	edx, DWORD PTR _nSubWorldId$[ebp]
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+364], edx

; 1464 : 	Missle[nMissleIndex].m_nLauncher		= pShowParam->nLauncherIndex;

	mov	eax, DWORD PTR _nMissleIndex$[ebp]
	imul	eax, 2940				; 00000b7cH
	mov	ecx, DWORD PTR _pShowParam$[ebp]
	mov	edx, DWORD PTR [ecx+16]
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[eax+336], edx

; 1465 : 	Missle[nMissleIndex].m_dwLauncherId		= Npc[pShowParam->nLauncherIndex].m_dwID;

	mov	eax, DWORD PTR _pShowParam$[ebp]
	mov	ecx, DWORD PTR [eax+16]
	imul	ecx, 33600				; 00008340H
	mov	edx, DWORD PTR _nMissleIndex$[ebp]
	imul	edx, 2940				; 00000b7cH
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[ecx]
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[edx+340], eax

; 1466 : 	
; 1467 : 	Missle[nMissleIndex].m_nParentMissleIndex = 0;

	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+344], 0

; 1468 : 	Missle[nMissleIndex].m_nEnChance		= 0;

	mov	edx, DWORD PTR _nMissleIndex$[ebp]
	imul	edx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[edx+24], 0

; 1469 : 	Missle[nMissleIndex].m_nSkillId			= 0;

	mov	eax, DWORD PTR _nMissleIndex$[ebp]
	imul	eax, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[eax+104], 0

; 1470 : 	Missle[nMissleIndex].m_nStartLifeTime	= 0;

	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+212], 0

; 1471 : 	Missle[nMissleIndex].m_nLifeTime		= 1;

	mov	edx, DWORD PTR _nMissleIndex$[ebp]
	imul	edx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[edx+96], 1

; 1472 : 	Missle[nMissleIndex].m_nRefPX			= 0;

	mov	eax, DWORD PTR _nMissleIndex$[ebp]
	imul	eax, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[eax+240], 0

; 1473 : 	Missle[nMissleIndex].m_nRefPY			= 0;

	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+244], 0

; 1474 : 	Missle[nMissleIndex].m_MissleRes.Clear();

	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	add	ecx, OFFSET FLAT:?Missle@@3PAVKMissle@@A+468
	call	?Clear@KMissleRes@@QAEXXZ		; KMissleRes::Clear

; 1475 : 
; 1476 : 	Missle[nMissleIndex].m_MissleRes.LoadResource(MS_DoWait, szMovie, szSound);

	mov	edx, DWORD PTR _szSound$[ebp]
	push	edx
	mov	eax, DWORD PTR _szMovie$[ebp]
	push	eax
	push	0
	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	add	ecx, OFFSET FLAT:?Missle@@3PAVKMissle@@A+468
	call	?LoadResource@KMissleRes@@QAEXHPAD0@Z	; KMissleRes::LoadResource

; 1477 : 	char * pcszTemp = szFormat;

	mov	ecx, DWORD PTR _szFormat$[ebp]
	mov	DWORD PTR _pcszTemp$[ebp], ecx

; 1478 : 	Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);

	push	100					; 00000064H
	lea	edx, DWORD PTR _pcszTemp$[ebp]
	push	edx
	call	?KSG_StringGetInt@@YAHPAPADH@Z		; KSG_StringGetInt
	add	esp, 8
	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+600], eax

; 1479 : 	KSG_StringSkipSymbol(&pcszTemp, ',');

	push	44					; 0000002cH
	lea	edx, DWORD PTR _pcszTemp$[ebp]
	push	edx
	call	?KSG_StringSkipSymbol@@YA_NPAPADH@Z	; KSG_StringSkipSymbol
	add	esp, 8

; 1480 : 	Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nDir = KSG_StringGetInt(&pcszTemp, 16);

	push	16					; 00000010H
	lea	eax, DWORD PTR _pcszTemp$[ebp]
	push	eax
	call	?KSG_StringGetInt@@YAHPAPADH@Z		; KSG_StringGetInt
	add	esp, 8
	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+608], eax

; 1481 : 	KSG_StringSkipSymbol(&pcszTemp, ',');

	push	44					; 0000002cH
	lea	edx, DWORD PTR _pcszTemp$[ebp]
	push	edx
	call	?KSG_StringSkipSymbol@@YA_NPAPADH@Z	; KSG_StringSkipSymbol
	add	esp, 8

; 1482 :     Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nInterval = KSG_StringGetInt(&pcszTemp, 1);

	push	1
	lea	eax, DWORD PTR _pcszTemp$[ebp]
	push	eax
	call	?KSG_StringGetInt@@YAHPAPADH@Z		; KSG_StringGetInt
	add	esp, 8
	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	mov	DWORD PTR ?Missle@@3PAVKMissle@@A[ecx+604], eax

; 1483 : 
; 1484 : 	Missle[nMissleIndex].CreateSpecialEffect(MS_DoWait, nPX, nPY, nPZ, pShowParam->nNpcIndex);

	mov	edx, DWORD PTR _pShowParam$[ebp]
	mov	eax, DWORD PTR [edx+12]
	push	eax
	mov	ecx, DWORD PTR _nPZ$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nPY$[ebp]
	push	edx
	mov	eax, DWORD PTR _nPX$[ebp]
	push	eax
	push	0
	mov	ecx, DWORD PTR _nMissleIndex$[ebp]
	imul	ecx, 2940				; 00000b7cH
	add	ecx, OFFSET FLAT:?Missle@@3PAVKMissle@@A ; Missle
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 1485 : 	return TRUE;

	mov	eax, 1
$L105592:

; 1486 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?CreateMissleForShow@KMissle@@SAHPAD00PAUTMissleForShow@@@Z ENDP ; KMissle::CreateMissleForShow
_TEXT	ENDS
;	COMDAT ?g_Dir2DirIndex@@YAHHH@Z
_TEXT	SEGMENT
_nDir$ = 8
_nMaxDir$ = 12
_nRet$ = -4
?g_Dir2DirIndex@@YAHHH@Z PROC NEAR			; g_Dir2DirIndex, COMDAT

; 99   : {

	push	ebp
	mov	ebp, esp
	push	ecx

; 100  : 	int nRet = -1;

	mov	DWORD PTR _nRet$[ebp], -1

; 101  : 
; 102  : 	if (nMaxDir <= 0)

	cmp	DWORD PTR _nMaxDir$[ebp], 0
	jg	SHORT $L96626

; 103  : 		return nRet;

	mov	eax, DWORD PTR _nRet$[ebp]
	jmp	SHORT $L96624
$L96626:

; 104  : 
; 105  : 	nRet = (nDir << 6) / nMaxDir;

	mov	eax, DWORD PTR _nDir$[ebp]
	shl	eax, 6
	cdq
	idiv	DWORD PTR _nMaxDir$[ebp]
	mov	DWORD PTR _nRet$[ebp], eax

; 106  : 	return nRet;

	mov	eax, DWORD PTR _nRet$[ebp]
$L96624:

; 107  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?g_Dir2DirIndex@@YAHHH@Z ENDP				; g_Dir2DirIndex
_TEXT	ENDS
;	COMDAT ?KSG_StringGetInt@@YAHPAPADH@Z
_TEXT	SEGMENT
_ppszString$ = 8
_nDefaultValue$ = 12
?KSG_StringGetInt@@YAHPAPADH@Z PROC NEAR		; KSG_StringGetInt, COMDAT

; 19   : {

	push	ebp
	mov	ebp, esp

; 20   :     return KSG_StringGetInt((const char **)ppszString, nDefaultValue);    

	mov	eax, DWORD PTR _nDefaultValue$[ebp]
	push	eax
	mov	ecx, DWORD PTR _ppszString$[ebp]
	push	ecx
	call	DWORD PTR __imp_?KSG_StringGetInt@@YAHPAPBDH@Z
	add	esp, 8

; 21   : }

	pop	ebp
	ret	0
?KSG_StringGetInt@@YAHPAPADH@Z ENDP			; KSG_StringGetInt
_TEXT	ENDS
;	COMDAT ?KSG_StringSkipSymbol@@YA_NPAPADH@Z
_TEXT	SEGMENT
_ppszString$ = 8
_nSymbol$ = 12
?KSG_StringSkipSymbol@@YA_NPAPADH@Z PROC NEAR		; KSG_StringSkipSymbol, COMDAT

; 24   : {

	push	ebp
	mov	ebp, esp

; 25   :     return KSG_StringSkipSymbol((const char **)ppszString, nSymbol);

	mov	eax, DWORD PTR _nSymbol$[ebp]
	push	eax
	mov	ecx, DWORD PTR _ppszString$[ebp]
	push	ecx
	call	DWORD PTR __imp_?KSG_StringSkipSymbol@@YA_NPAPBDH@Z
	add	esp, 8

; 26   : }

	pop	ebp
	ret	0
?KSG_StringSkipSymbol@@YA_NPAPADH@Z ENDP		; KSG_StringSkipSymbol
_TEXT	ENDS
PUBLIC	?GetLightInfo@KMissle@@QAEXPAUKLightInfo@@@Z	; KMissle::GetLightInfo
_TEXT	SEGMENT
_pLightInfo$ = 8
_this$ = -16
_nPX$ = -4
_nPY$ = -8
_nPZ$ = -12
?GetLightInfo@KMissle@@QAEXPAUKLightInfo@@@Z PROC NEAR	; KMissle::GetLightInfo

; 1489 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx

; 1490 : 	if (!pLightInfo) 

	cmp	DWORD PTR _pLightInfo$[ebp], 0
	jne	SHORT $L105608

; 1492 : 		return ;

	jmp	SHORT $L105607
$L105608:

; 1496 : 	GetMpsPos(&nPX, &nPY);

	lea	eax, DWORD PTR _nPY$[ebp]
	push	eax
	lea	ecx, DWORD PTR _nPX$[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetMpsPos@KMissle@@QAEXPAH0@Z		; KMissle::GetMpsPos

; 1497 : 	nPZ = m_nCurrentMapZ;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+228]
	mov	DWORD PTR _nPZ$[ebp], eax

; 1498 : 	
; 1499 : 	pLightInfo->oPosition.nX = nPX;

	mov	ecx, DWORD PTR _pLightInfo$[ebp]
	mov	edx, DWORD PTR _nPX$[ebp]
	mov	DWORD PTR [ecx], edx

; 1500 : 	pLightInfo->oPosition.nY = nPY;

	mov	eax, DWORD PTR _pLightInfo$[ebp]
	mov	ecx, DWORD PTR _nPY$[ebp]
	mov	DWORD PTR [eax+4], ecx

; 1501 : 	pLightInfo->oPosition.nZ = nPZ;

	mov	edx, DWORD PTR _pLightInfo$[ebp]
	mov	eax, DWORD PTR _nPZ$[ebp]
	mov	DWORD PTR [edx+8], eax

; 1502 : 	pLightInfo->dwColor = 0xff000000 | m_btRedLum << 16 | m_btGreenLum << 8 | m_btBlueLum;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+452]
	shl	edx, 16					; 00000010H
	or	edx, -16777216				; ff000000H
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+456]
	shl	ecx, 8
	or	edx, ecx
	mov	eax, DWORD PTR _this$[ebp]
	or	edx, DWORD PTR [eax+460]
	mov	ecx, DWORD PTR _pLightInfo$[ebp]
	mov	DWORD PTR [ecx+12], edx

; 1503 : 	pLightInfo->nRadius = m_usLightRadius;

	mov	edx, DWORD PTR _this$[ebp]
	xor	eax, eax
	mov	ax, WORD PTR [edx+464]
	mov	ecx, DWORD PTR _pLightInfo$[ebp]
	mov	DWORD PTR [ecx+16], eax
$L105607:

; 1504 : }

	mov	esp, ebp
	pop	ebp
	ret	4
?GetLightInfo@KMissle@@QAEXPAUKLightInfo@@@Z ENDP	; KMissle::GetLightInfo
_TEXT	ENDS
PUBLIC	?DoWait@KMissle@@AAEXXZ				; KMissle::DoWait
_TEXT	SEGMENT
_this$ = -12
_nSrcX$ = -4
_nSrcY$ = -8
?DoWait@KMissle@@AAEXXZ PROC NEAR			; KMissle::DoWait

; 1508 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	mov	DWORD PTR _this$[ebp], ecx

; 1509 : 	//	if (m_eMissleStatus == MS_DoWait) return;
; 1510 : 	m_eMissleStatus = MS_DoWait;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+356], 0

; 1511 : 	
; 1512 : #ifndef _SERVER 
; 1513 : 	int nSrcX = 0 ;

	mov	DWORD PTR _nSrcX$[ebp], 0

; 1514 : 	int nSrcY = 0 ;

	mov	DWORD PTR _nSrcY$[ebp], 0

; 1515 : 	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);

	lea	ecx, DWORD PTR _nSrcY$[ebp]
	push	ecx
	lea	edx, DWORD PTR _nSrcX$[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+236]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+232]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 1516 : 	CreateSpecialEffect(MS_DoWait, nSrcX, nSrcY, m_nCurrentMapZ);

	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+228]
	push	edx
	mov	eax, DWORD PTR _nSrcY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _nSrcX$[ebp]
	push	ecx
	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CreateSpecialEffect@KMissle@@QAEHW4eMissleStatus@@HHHH@Z ; KMissle::CreateSpecialEffect

; 1517 : #endif
; 1518 : 	
; 1519 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?DoWait@KMissle@@AAEXXZ ENDP				; KMissle::DoWait
_TEXT	ENDS
EXTRN	?Mps2Map@KSubWorld@@QAEXHHPAH0000@Z:NEAR	; KSubWorld::Mps2Map
_TEXT	SEGMENT
_this$ = -56
_nPX$105622 = -4
_nPY$105623 = -8
_nNewPX$105627 = -16
_nNewPY$105628 = -20
_nParentPX$105632 = -36
_nParentPY$105633 = -28
_nSrcPX$105634 = -24
_nSrcPY$105635 = -32
_nParentPX$105638 = -52
_nParentPY$105639 = -44
_nSrcPX$105640 = -40
_nSrcPY$105641 = -48
_nOldRegion$105642 = -12
?PrePareFly@KMissle@@AAEHXZ PROC NEAR			; KMissle::PrePareFly

; 1522 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 56					; 00000038H
	mov	DWORD PTR _this$[ebp], ecx

; 1523 : 	if (m_eMoveKind == MISSLE_MMK_RollBack)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+80], 100			; 00000064H
	jne	SHORT $L105620

; 1524 : 		m_nTempParam2 =  m_nStartLifeTime + (m_nLifeTime - m_nStartLifeTime ) / 2;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+96]
	sub	eax, DWORD PTR [edx+212]
	cdq
	sub	eax, edx
	sar	eax, 1
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+212]
	add	edx, eax
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+420], edx
$L105620:

; 1525 : 
; 1526 : 	//是否会随发送者的移动而中断，类式魔兽3中大型法术
; 1527 : 	if (m_nInteruptTypeWhenMove)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+132], 0
	je	SHORT $L105624

; 1529 : 		int nPX, nPY;
; 1530 : 		Npc[m_nLauncher].GetMpsPos(&nPX, &nPY);

	lea	edx, DWORD PTR _nPY$105623[ebp]
	push	edx
	lea	eax, DWORD PTR _nPX$105622[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+336]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?GetMpsPos@KNpc@@QAEXPAH0@Z		; KNpc::GetMpsPos

; 1531 : 		if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nPX$105622[ebp]
	cmp	eax, DWORD PTR [edx+140]
	jne	SHORT $L105625
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nPY$105623[ebp]
	cmp	edx, DWORD PTR [ecx+144]
	je	SHORT $L105624
$L105625:

; 1533 : 			return false;

	xor	eax, eax
	jmp	$L105619
$L105624:

; 1536 : 	
; 1537 : 	//子碟位置需要更正为到适当的位置（子弹的出现总是以某个可能位置在不断变化的物体为参照物）
; 1538 : 	if (m_bHeelAtParent)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+136], 0
	je	$L105643

; 1540 : 		int nNewPX = 0;

	mov	DWORD PTR _nNewPX$105627[ebp], 0

; 1541 : 		int nNewPY = 0;

	mov	DWORD PTR _nNewPY$105628[ebp], 0

; 1542 : 		
; 1543 : 		if (m_nParentMissleIndex) // 参考点为母子弹

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+344], 0
	je	$L105629

; 1545 : 			if (Missle[m_nParentMissleIndex].m_dwLauncherId != m_dwLauncherId)

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+344]
	imul	eax, 2940				; 00000b7cH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR ?Missle@@3PAVKMissle@@A[eax+340]
	cmp	edx, DWORD PTR [ecx+340]
	je	SHORT $L105630

; 1547 : 				return false;

	xor	eax, eax
	jmp	$L105619
$L105630:

; 1551 : 				int nParentPX, nParentPY;
; 1552 : 				int nSrcPX, nSrcPY;
; 1553 : 				Missle[m_nParentMissleIndex].GetMpsPos(&nParentPX, &nParentPY);

	lea	eax, DWORD PTR _nParentPY$105633[ebp]
	push	eax
	lea	ecx, DWORD PTR _nParentPX$105632[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+344]
	imul	ecx, 2940				; 00000b7cH
	add	ecx, OFFSET FLAT:?Missle@@3PAVKMissle@@A ; Missle
	call	?GetMpsPos@KMissle@@QAEXPAH0@Z		; KMissle::GetMpsPos

; 1554 : 				GetMpsPos(&nSrcPX, &nSrcPY);

	lea	eax, DWORD PTR _nSrcPY$105635[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSrcPX$105634[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetMpsPos@KMissle@@QAEXPAH0@Z		; KMissle::GetMpsPos

; 1555 : 				nNewPX = nSrcPX + (nParentPX - m_nRefPX);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nParentPX$105632[ebp]
	sub	eax, DWORD PTR [edx+240]
	mov	ecx, DWORD PTR _nSrcPX$105634[ebp]
	add	ecx, eax
	mov	DWORD PTR _nNewPX$105627[ebp], ecx

; 1556 : 				nNewPY = nSrcPY + (nParentPY - m_nRefPY);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _nParentPY$105633[ebp]
	sub	eax, DWORD PTR [edx+244]
	mov	ecx, DWORD PTR _nSrcPY$105635[ebp]
	add	ecx, eax
	mov	DWORD PTR _nNewPY$105628[ebp], ecx

; 1559 : 		else

	jmp	SHORT $L105636
$L105629:

; 1562 : 			_ASSERT(m_nLauncher > 0);
; 1563 : 			int nParentPX, nParentPY;
; 1564 : 			int nSrcPX, nSrcPY;
; 1565 : 			
; 1566 : 			Npc[m_nLauncher].GetMpsPos(&nParentPX, &nParentPY);

	lea	edx, DWORD PTR _nParentPY$105639[ebp]
	push	edx
	lea	eax, DWORD PTR _nParentPX$105638[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+336]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?GetMpsPos@KNpc@@QAEXPAH0@Z		; KNpc::GetMpsPos

; 1567 : 			GetMpsPos(&nSrcPX, &nSrcPY);

	lea	edx, DWORD PTR _nSrcPY$105641[ebp]
	push	edx
	lea	eax, DWORD PTR _nSrcPX$105640[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?GetMpsPos@KMissle@@QAEXPAH0@Z		; KMissle::GetMpsPos

; 1568 : 			
; 1569 : 			nNewPX = nSrcPX + (nParentPX - m_nRefPX);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nParentPX$105638[ebp]
	sub	edx, DWORD PTR [ecx+240]
	mov	eax, DWORD PTR _nSrcPX$105640[ebp]
	add	eax, edx
	mov	DWORD PTR _nNewPX$105627[ebp], eax

; 1570 : 			nNewPY = nSrcPY + (nParentPY - m_nRefPY);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nParentPY$105639[ebp]
	sub	edx, DWORD PTR [ecx+244]
	mov	eax, DWORD PTR _nSrcPY$105641[ebp]
	add	eax, edx
	mov	DWORD PTR _nNewPY$105628[ebp], eax
$L105636:

; 1572 : 		
; 1573 : 		int nOldRegion = m_nRegionId;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	mov	DWORD PTR _nOldRegion$105642[ebp], edx

; 1574 : 		CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	imul	edx, 220				; 000000dcH
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+368]
	imul	ecx, 208				; 000000d0H
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[edx+8]
	add	ecx, edx
	call	?DecRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::DecRef

; 1575 : 		SubWorld[m_nSubWorldId].Mps2Map(nNewPX, nNewPY, &m_nRegionId, &m_nCurrentMapX, &m_nCurrentMapY, &m_nXOffset, &m_nYOffset);

	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 236				; 000000ecH
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 232				; 000000e8H
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	add	edx, 224				; 000000e0H
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 220				; 000000dcH
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 368				; 00000170H
	push	ecx
	mov	edx, DWORD PTR _nNewPY$105628[ebp]
	push	edx
	mov	eax, DWORD PTR _nNewPX$105627[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A
	call	?Mps2Map@KSubWorld@@QAEXHHPAH0000@Z	; KSubWorld::Mps2Map

; 1576 : 		CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	push	2
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+224]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+220]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	imul	eax, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	add	ecx, eax
	call	?AddRef@KRegion@@QAEHHHW4MOVE_OBJ_KIND@@@Z ; KRegion::AddRef

; 1577 : 		
; 1578 : 		if (nOldRegion != m_nRegionId)

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nOldRegion$105642[ebp]
	cmp	edx, DWORD PTR [ecx+368]
	je	SHORT $L105643

; 1580 : 			SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+360]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, DWORD PTR _nOldRegion$105642[ebp]
	push	ecx
	push	4002					; 00000fa2H
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A+172
	call	?Send@KWorldMsg@@QAEHKHHH@Z		; KWorldMsg::Send
$L105643:

; 1584 : 	
; 1585 : 	return true;

	mov	eax, 1
$L105619:

; 1586 : 	
; 1587 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?PrePareFly@KMissle@@AAEHXZ ENDP			; KMissle::PrePareFly
_this$ = -64
_nSearchRegion$ = -48
_nRMx$ = -36
_nRMy$ = -44
_bCollision$ = -32
_nNpcIdx$ = -24
_nDX$ = -8
_nDY$ = -16
_nNpcOffsetX$ = -12
_nNpcOffsetY$ = -4
_nAbsX$ = -52
_nAbsY$ = -56
_nCellWidth$ = -20
_nCellHeight$ = -40
_i$ = -28
_j$105665 = -60
?CheckNearestCollision@KMissle@@AAEHXZ PROC NEAR	; KMissle::CheckNearestCollision

; 1590 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	mov	DWORD PTR _this$[ebp], ecx

; 1591 : 	int nSearchRegion = 0;

	mov	DWORD PTR _nSearchRegion$[ebp], 0

; 1592 : 	int nRMx = 0;

	mov	DWORD PTR _nRMx$[ebp], 0

; 1593 : 	int nRMy = 0;

	mov	DWORD PTR _nRMy$[ebp], 0

; 1594 : 	BOOL bCollision = TRUE;

	mov	DWORD PTR _bCollision$[ebp], 1

; 1595 : 	int nNpcIdx = 0;

	mov	DWORD PTR _nNpcIdx$[ebp], 0

; 1596 : 	int nDX = 0;

	mov	DWORD PTR _nDX$[ebp], 0

; 1597 : 	int nDY = 0;

	mov	DWORD PTR _nDY$[ebp], 0

; 1598 : 	int nNpcOffsetX = 0;

	mov	DWORD PTR _nNpcOffsetX$[ebp], 0

; 1599 : 	int nNpcOffsetY = 0;

	mov	DWORD PTR _nNpcOffsetY$[ebp], 0

; 1600 : 	int nAbsX = 0;

	mov	DWORD PTR _nAbsX$[ebp], 0

; 1601 : 	int nAbsY = 0;

	mov	DWORD PTR _nAbsY$[ebp], 0

; 1602 : 	int nCellWidth = CellWidth;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+148]
	shl	edx, 10					; 0000000aH
	mov	DWORD PTR _nCellWidth$[ebp], edx

; 1603 : 	int nCellHeight = CellHeight;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+152]
	shl	edx, 10					; 0000000aH
	mov	DWORD PTR _nCellHeight$[ebp], edx

; 1604 : 	_ASSERT(nCellWidth > 0 && nCellHeight > 0);
; 1605 : 	
; 1606 : 	for (int i = -1; i <= 1; i ++)

	mov	DWORD PTR _i$[ebp], -1
	jmp	SHORT $L105662
$L105663:
	mov	eax, DWORD PTR _i$[ebp]
	add	eax, 1
	mov	DWORD PTR _i$[ebp], eax
$L105662:
	cmp	DWORD PTR _i$[ebp], 1
	jg	$L105664

; 1607 : 		for (int j = -1; j <= 1; j ++)

	mov	DWORD PTR _j$105665[ebp], -1
	jmp	SHORT $L105666
$L105667:
	mov	ecx, DWORD PTR _j$105665[ebp]
	add	ecx, 1
	mov	DWORD PTR _j$105665[ebp], ecx
$L105666:
	cmp	DWORD PTR _j$105665[ebp], 1
	jg	$L105668

; 1610 : 				m_nSubWorldId,
; 1611 : 				m_nRegionId, 
; 1612 : 				m_nCurrentMapX, 
; 1613 : 				m_nCurrentMapY, 
; 1614 : 				i , 
; 1615 : 				j , 
; 1616 : 				nSearchRegion, 
; 1617 : 				nRMx, 
; 1618 : 				nRMy
; 1619 : 				))

	lea	edx, DWORD PTR _nRMy$[ebp]
	push	edx
	lea	eax, DWORD PTR _nRMx$[ebp]
	push	eax
	lea	ecx, DWORD PTR _nSearchRegion$[ebp]
	push	ecx
	mov	edx, DWORD PTR _j$105665[ebp]
	push	edx
	mov	eax, DWORD PTR _i$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+220]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+368]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+364]
	push	edx
	call	?GetOffsetAxis@KMissle@@SAHHHHHHHAAH00@Z ; KMissle::GetOffsetAxis
	add	esp, 36					; 00000024H
	test	eax, eax
	jne	SHORT $L105669

; 1620 : 				continue;

	jmp	SHORT $L105667
$L105669:

; 1623 : 			
; 1624 : 			nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+112]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+336]
	push	eax
	mov	ecx, DWORD PTR _nRMy$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nRMx$[ebp]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	mov	edx, DWORD PTR _nSearchRegion$[ebp]
	imul	edx, 208				; 000000d0H
	mov	ecx, DWORD PTR ?SubWorld@@3PAVKSubWorld@@A[ecx+8]
	add	ecx, edx
	call	?FindNpc@KRegion@@QAEHHHHH@Z		; KRegion::FindNpc
	mov	DWORD PTR _nNpcIdx$[ebp], eax

; 1625 : 			
; 1626 : 			if (nNpcIdx > 0)

	cmp	DWORD PTR _nNpcIdx$[ebp], 0
	jle	$L105689

; 1628 : 				bCollision = TRUE;

	mov	DWORD PTR _bCollision$[ebp], 1

; 1629 : 				nDX = m_nCurrentMapX - Npc[nNpcIdx].m_MapX;

	mov	eax, DWORD PTR _nNpcIdx$[ebp]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+220]
	sub	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3540]
	mov	DWORD PTR _nDX$[ebp], edx

; 1630 : 				nDY = m_nCurrentMapY - Npc[nNpcIdx].m_MapY;

	mov	eax, DWORD PTR _nNpcIdx$[ebp]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+224]
	sub	edx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3544]
	mov	DWORD PTR _nDY$[ebp], edx

; 1631 : 				nNpcOffsetX = Npc[nNpcIdx].m_OffX;

	mov	eax, DWORD PTR _nNpcIdx$[ebp]
	imul	eax, 33600				; 00008340H
	mov	ecx, DWORD PTR ?Npc@@3PAVKNpc@@A[eax+3552]
	mov	DWORD PTR _nNpcOffsetX$[ebp], ecx

; 1632 : 				nNpcOffsetY = Npc[nNpcIdx].m_OffY;

	mov	edx, DWORD PTR _nNpcIdx$[ebp]
	imul	edx, 33600				; 00008340H
	mov	eax, DWORD PTR ?Npc@@3PAVKNpc@@A[edx+3556]
	mov	DWORD PTR _nNpcOffsetY$[ebp], eax

; 1633 : 				nAbsX = abs(nDX);

	mov	ecx, DWORD PTR _nDX$[ebp]
	push	ecx
	call	_abs
	add	esp, 4
	mov	DWORD PTR _nAbsX$[ebp], eax

; 1634 : 				nAbsY = abs(nDY);

	mov	edx, DWORD PTR _nDY$[ebp]
	push	edx
	call	_abs
	add	esp, 4
	mov	DWORD PTR _nAbsY$[ebp], eax

; 1635 : 				
; 1636 : 				if (nAbsX)

	cmp	DWORD PTR _nAbsX$[ebp], 0
	je	SHORT $L105679

; 1638 : 					if (nDX < 0)

	cmp	DWORD PTR _nDX$[ebp], 0
	jge	SHORT $L105673

; 1640 : 						if (nCellWidth - m_nXOffset + nNpcOffsetX > nCellWidth)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nCellWidth$[ebp]
	sub	ecx, DWORD PTR [eax+232]
	add	ecx, DWORD PTR _nNpcOffsetX$[ebp]
	cmp	ecx, DWORD PTR _nCellWidth$[ebp]
	jle	SHORT $L105674

; 1642 : 							bCollision = FALSE;

	mov	DWORD PTR _bCollision$[ebp], 0

; 1643 : 							goto CheckCollision;

	jmp	$L105676
$L105674:

; 1646 : 					else if (nDX > 0)

	jmp	SHORT $L105679
$L105673:
	cmp	DWORD PTR _nDX$[ebp], 0
	jle	SHORT $L105679

; 1648 : 						if (nCellWidth - nNpcOffsetX + m_nXOffset > nCellWidth)

	mov	edx, DWORD PTR _nCellWidth$[ebp]
	sub	edx, DWORD PTR _nNpcOffsetX$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	add	edx, DWORD PTR [eax+232]
	cmp	edx, DWORD PTR _nCellWidth$[ebp]
	jle	SHORT $L105679

; 1650 : 							bCollision = FALSE;

	mov	DWORD PTR _bCollision$[ebp], 0

; 1651 : 							goto CheckCollision;

	jmp	SHORT $L105680
$L105679:

; 1655 : 				
; 1656 : 				if (nAbsY)

	cmp	DWORD PTR _nAbsY$[ebp], 0
	je	SHORT $CheckCollision$105675

; 1658 : 					if (nDY <0)

	cmp	DWORD PTR _nDY$[ebp], 0
	jge	SHORT $L105682

; 1660 : 						if (nCellHeight - m_nYOffset + nNpcOffsetY > nCellHeight)

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nCellHeight$[ebp]
	sub	edx, DWORD PTR [ecx+236]
	add	edx, DWORD PTR _nNpcOffsetY$[ebp]
	cmp	edx, DWORD PTR _nCellHeight$[ebp]
	jle	SHORT $L105683

; 1662 : 							bCollision = FALSE;

	mov	DWORD PTR _bCollision$[ebp], 0

; 1663 : 							goto CheckCollision;

	jmp	SHORT $L105684
$L105683:

; 1666 : 					else if (nDY >0)

	jmp	SHORT $CheckCollision$105675
$L105682:
	cmp	DWORD PTR _nDY$[ebp], 0
	jle	SHORT $CheckCollision$105675

; 1668 : 						if (nCellHeight - nNpcOffsetY + m_nYOffset > nCellHeight)

	mov	eax, DWORD PTR _nCellHeight$[ebp]
	sub	eax, DWORD PTR _nNpcOffsetY$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	add	eax, DWORD PTR [ecx+236]
	cmp	eax, DWORD PTR _nCellHeight$[ebp]
	jle	SHORT $CheckCollision$105675

; 1670 : 							bCollision = FALSE;

	mov	DWORD PTR _bCollision$[ebp], 0

; 1671 : 							goto CheckCollision;

	jmp	SHORT $L105688
$CheckCollision$105675:

; 1678 : 				if (bCollision)

	cmp	DWORD PTR _bCollision$[ebp], 0
	je	SHORT $L105689

; 1679 : 					return nNpcIdx;

	mov	eax, DWORD PTR _nNpcIdx$[ebp]
	jmp	SHORT $L105646
$L105689:

; 1681 : 		}

	jmp	$L105667
$L105668:
	jmp	$L105663
$L105664:

; 1682 : 		
; 1683 : 		return 0;

	xor	eax, eax
	jmp	SHORT $L105646
$L105688:

; 1684 : }

	jmp	SHORT $CheckCollision$105675
$L105684:
	jmp	SHORT $CheckCollision$105675
$L105680:
	jmp	SHORT $CheckCollision$105675
$L105676:
	jmp	SHORT $CheckCollision$105675
$L105646:
	mov	esp, ebp
	pop	ebp
	ret	0
?CheckNearestCollision@KMissle@@AAEHXZ ENDP		; KMissle::CheckNearestCollision
_pPosX$ = 8
_pPosY$ = 12
_this$ = -4
?GetMpsPos@KMissle@@QAEXPAH0@Z PROC NEAR		; KMissle::GetMpsPos

; 1687 : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 1688 : 	SubWorld[m_nSubWorldId].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, pPosX, pPosY);

	mov	eax, DWORD PTR _pPosY$[ebp]
	push	eax
	mov	ecx, DWORD PTR _pPosX$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+236]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+232]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+224]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+220]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+368]
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+364]
	imul	ecx, 220				; 000000dcH
	add	ecx, OFFSET FLAT:?SubWorld@@3PAVKSubWorld@@A ; SubWorld
	call	?Map2Mps@KSubWorld@@QAEXHHHHHPAH0@Z	; KSubWorld::Map2Mps

; 1689 : };

	mov	esp, ebp
	pop	ebp
	ret	8
?GetMpsPos@KMissle@@QAEXPAH0@Z ENDP			; KMissle::GetMpsPos
_TEXT	ENDS
END
