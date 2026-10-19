	TITLE	D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
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
_BSS	SEGMENT DWORD USE32 PUBLIC 'BSS'
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
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT _IsEqualGUID
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _==
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KNode@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetNext@KNode@@QAEPAV1@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetPrev@KNode@@QAEPAV1@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?InsertBefore@KNode@@QAEXPAV1@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?InsertAfter@KNode@@QAEXPAV1@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Remove@KNode@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetHead@KList@@QAEPAVKNode@@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddTail@KList@@QAEXPAVKNode@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKCacheNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KCacheNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Release@tagSHOW_MSG_SYNC@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Release@SOBJ_LIGHT_DATA@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Release@OBJ_SKILL_DATA@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKIndexNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KIndexNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Clear@SACTTABLEOFF@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Clear@SECTINFO@KNpcResNode@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KWorldMsgNode@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKWorldMsgNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KWorldMsgNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Remove@KMissleMagicAttribsData@@AAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1ISkill@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?IsAura@ISkill@@UBEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?IsPhysical@ISkill@@UBEHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetSkillLevelUpScriptId@ISkill@@UBEKXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GISkill@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetColor@KLColor@@QAEXK@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Scale@KLColor@@QAEXM@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetColor@KLColor@@QAEKXZ
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
;	COMDAT ??2@YAPAXIPAX@Z
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
;	COMDAT ?Breath@KLightBase@@UAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KLightBase@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKIpotBranch@@QAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@ABV?$allocator@PAVKLightBase@@@1@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?size@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEIXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?clear@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?insert@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@ABQAVKLightBase@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?destroy@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??8iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?deallocate@?$allocator@D@std@@QAEXPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Charalloc@?$allocator@PAVKLightBase@@@std@@QAEPADI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?deallocate@?$allocator@PAVKLightBase@@@std@@QAEXPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?construct@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@ABQAV3@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z
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
;	COMDAT ?_Destroy@std@@YAXPAPAVKLightBase@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Allocate@std@@YAPADHPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?_Construct@std@@YAXPAPAVKLightBase@@ABQAV2@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_7KNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_R0?AVruntime_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __CTA2?AVlogic_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVrange_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVout_of_range@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT ??_7KIndexNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __CT??_R0?AVoverflow_error@std@@@8??0overflow_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA2?AVruntime_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI3?AVunderflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVoverflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVrange_error@std@@@8??0range_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVbad_alloc@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT ??_7KWorldMsgNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __CT??_R0?AVlength_error@std@@@8??0length_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI3?AVlength_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVlength_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __CTA3?AVdomain_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA2?AVbad_exception@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI2?AVruntime_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI3?AVout_of_range@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVunderflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI3?AVrange_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVoverflow_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __CTA2?AVbad_alloc@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_7invalid_argument@std@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __TI3?AVinvalid_argument@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVbad_alloc@std@@@8??0bad_alloc@std@@QAE@ABV01@@Z12
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVlogic_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT ??_R0?AVexception@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __CT??_R0?AVout_of_range@std@@@8??0out_of_range@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI3?AVdomain_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CTA3?AVinvalid_argument@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_7KCacheNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __CT??_R0?AVexception@@@8??0exception@@QAE@ABV0@@Z12
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVunderflow_error@std@@@8??0underflow_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVdomain_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __CTA3?AVlength_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_7ISkill@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_R0?AVunderflow_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT ??_7KLightBase@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT __CT??_R0?AVinvalid_argument@std@@@8??0invalid_argument@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVbad_exception@std@@@8??0bad_exception@std@@QAE@ABV01@@Z12
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI2?AVlogic_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI3?AVoverflow_error@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVdomain_error@std@@@8??0domain_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVruntime_error@std@@@8??0runtime_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_R0?AVinvalid_argument@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT ??_R0?AVrange_error@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __TI2?AVbad_exception@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __CT??_R0?AVlogic_error@std@@@8??0logic_error@std@@QAE@ABV01@@Z28
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT __TI2?AVbad_alloc@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
;	COMDAT ??_7KSelfBreathLight@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_R0?AVbad_exception@std@@@8
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
;	COMDAT __CTA3?AVout_of_range@std@@
xdata$x	SEGMENT DWORD USE32 PUBLIC 'CONST'
xdata$x	ENDS
FLAT	GROUP _DATA, CONST, _BSS, xdata$x
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
_DATA	SEGMENT
_?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA DD 01H
_DATA	ENDS
PUBLIC	??0KLightBase@@QAE@XZ				; KLightBase::KLightBase
PUBLIC	??0KSelfBreathLight@@QAE@XZ			; KSelfBreathLight::KSelfBreathLight
PUBLIC	?Breath@KSelfBreathLight@@UAEXXZ		; KSelfBreathLight::Breath
PUBLIC	??_7KSelfBreathLight@@6B@			; KSelfBreathLight::`vftable'
EXTRN	__imp__timeGetTime@0:NEAR
;	COMDAT ??_7KSelfBreathLight@@6B@
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
??_7KSelfBreathLight@@6B@ DD FLAT:?Breath@KSelfBreathLight@@UAEXXZ ; KSelfBreathLight::`vftable'
CONST	ENDS
_TEXT	SEGMENT
_this$ = -4
??0KSelfBreathLight@@QAE@XZ PROC NEAR			; KSelfBreathLight::KSelfBreathLight

; 25   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??0KLightBase@@QAE@XZ			; KLightBase::KLightBase
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KSelfBreathLight@@6B@ ; KSelfBreathLight::`vftable'

; 26   : 	nDir = 1;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+44], 1

; 27   : 	dwLastBreathTime = timeGetTime();

	call	DWORD PTR __imp__timeGetTime@0
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+48], eax

; 28   : }

	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0KSelfBreathLight@@QAE@XZ ENDP			; KSelfBreathLight::KSelfBreathLight
_TEXT	ENDS
PUBLIC	?Breath@KLightBase@@UAEXXZ			; KLightBase::Breath
PUBLIC	??_7KLightBase@@6B@				; KLightBase::`vftable'
;	COMDAT ??_7KLightBase@@6B@
CONST	SEGMENT
??_7KLightBase@@6B@ DD FLAT:?Breath@KLightBase@@UAEXXZ	; KLightBase::`vftable'
CONST	ENDS
;	COMDAT ??0KLightBase@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0KLightBase@@QAE@XZ PROC NEAR				; KLightBase::KLightBase, COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:??_7KLightBase@@6B@ ; KLightBase::`vftable'
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0KLightBase@@QAE@XZ ENDP				; KLightBase::KLightBase
_TEXT	ENDS
;	COMDAT ?Breath@KLightBase@@UAEXXZ
_TEXT	SEGMENT
_this$ = -4
?Breath@KLightBase@@UAEXXZ PROC NEAR			; KLightBase::Breath, COMDAT

; 50   : 	virtual void Breath(){}

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	esp, ebp
	pop	ebp
	ret	0
?Breath@KLightBase@@UAEXXZ ENDP				; KLightBase::Breath
_TEXT	ENDS
EXTRN	__ftol:NEAR
EXTRN	__fltused:NEAR
_TEXT	SEGMENT
_this$ = -8
_dwCurTime$ = -4
?Breath@KSelfBreathLight@@UAEXXZ PROC NEAR		; KSelfBreathLight::Breath

; 31   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx

; 32   : 	DWORD dwCurTime = timeGetTime();

	call	DWORD PTR __imp__timeGetTime@0
	mov	DWORD PTR _dwCurTime$[ebp], eax

; 33   : 	
; 34   : 	fRadius += (dwCurTime - dwLastBreathTime) / fCycle * (fMaxRange - fMinRange) * nDir * 2;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _dwCurTime$[ebp]
	sub	ecx, DWORD PTR [eax+48]
	mov	DWORD PTR -16+[ebp], ecx
	mov	DWORD PTR -16+[ebp+4], 0
	fild	QWORD PTR -16+[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	fdiv	DWORD PTR [edx+40]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	fld	DWORD PTR [eax+32]
	fsub	DWORD PTR [ecx+28]
	fmulp	ST(1), ST(0)
	mov	edx, DWORD PTR _this$[ebp]
	fimul	DWORD PTR [edx+44]
	fadd	ST(0), ST(0)
	mov	eax, DWORD PTR _this$[ebp]
	fadd	DWORD PTR [eax+36]
	mov	ecx, DWORD PTR _this$[ebp]
	fstp	DWORD PTR [ecx+36]

; 35   : 	if(nDir == 1 && fRadius > fMaxRange)

	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+44], 1
	jne	SHORT $L102149
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	fld	DWORD PTR [eax+36]
	fcomp	DWORD PTR [ecx+32]
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L102149

; 37   : 		nDir = -1;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+44], -1

; 38   : 		fRadius = fMaxRange;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+32]
	mov	DWORD PTR [eax+36], edx

; 40   : 	else if(nDir == -1 && fRadius < fMinRange)

	jmp	SHORT $L102151
$L102149:
	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+44], -1
	jne	SHORT $L102151
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	fld	DWORD PTR [ecx+36]
	fcomp	DWORD PTR [edx+28]
	fnstsw	ax
	test	ah, 1
	je	SHORT $L102151

; 42   : 		nDir = 1;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+44], 1

; 43   : 		fRadius = fMinRange;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+28]
	mov	DWORD PTR [ecx+36], eax
$L102151:

; 45   : 	m_nRadius = (int)fRadius;

	mov	ecx, DWORD PTR _this$[ebp]
	fld	DWORD PTR [ecx+36]
	call	__ftol
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+24], eax

; 46   : 	dwLastBreathTime = dwCurTime;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _dwCurTime$[ebp]
	mov	DWORD PTR [eax+48], ecx

; 47   : }

	mov	esp, ebp
	pop	ebp
	ret	0
?Breath@KSelfBreathLight@@UAEXXZ ENDP			; KSelfBreathLight::Breath
_TEXT	ENDS
PUBLIC	??0KIpoTree@@QAE@XZ				; KIpoTree::KIpoTree
PUBLIC	??0?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@ABV?$allocator@PAVKLightBase@@@1@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::list<KLightBase *,std::allocator<KLightBase *> >
PUBLIC	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
EXTRN	??0KIpotBranch@@QAE@XZ:NEAR			; KIpotBranch::KIpotBranch
EXTRN	??1KIpotBranch@@QAE@XZ:NEAR			; KIpotBranch::~KIpotBranch
EXTRN	??2@YAPAXI@Z:NEAR				; operator new
EXTRN	__except_list:DWORD
EXTRN	___CxxFrameHandler:NEAR
EXTRN	_memset:NEAR
xdata$x	SEGMENT
$T103146 DD	019930520H
	DD	02H
	DD	FLAT:$T103149
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T103149 DD	0ffffffffH
	DD	FLAT:$L103141
	DD	00H
	DD	FLAT:$L103142
xdata$x	ENDS
_TEXT	SEGMENT
$T103138 = -20
$T103139 = -24
$T103140 = -28
__$EHRec$ = -12
_this$ = -32
_ulAddress$ = -16
??0KIpoTree@@QAE@XZ PROC NEAR				; KIpoTree::KIpoTree

; 51   : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L103147
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 20					; 00000014H
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??0KIpotBranch@@QAE@XZ			; KIpotBranch::KIpotBranch
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	lea	eax, DWORD PTR $T103138[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	??0?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@ABV?$allocator@PAVKLightBase@@@1@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::list<KLightBase *,std::allocator<KLightBase *> >
	mov	BYTE PTR __$EHRec$[ebp+8], 1

; 52   : 	m_pMainBranch = NULL;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx], 0

; 53   : 	m_bProcessBioLights = true;

	mov	edx, DWORD PTR _this$[ebp]
	mov	BYTE PTR [edx+89], 1

; 54   : 	m_bDynamicLighting = true;

	mov	eax, DWORD PTR _this$[ebp]
	mov	BYTE PTR [eax+90], 1

; 55   : 	m_nCurrentTime = 0;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+84], 0

; 56   : 	m_bIsIndoor = false;

	mov	edx, DWORD PTR _this$[ebp]
	mov	BYTE PTR [edx+88], 0

; 57   : 	m_dwAmbient = 0xff000000;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+92], -16777216		; ff000000H

; 58   : 	memset(&m_PermanentLeaf, 0, sizeof(m_PermanentLeaf));

	push	24					; 00000018H
	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 52					; 00000034H
	push	ecx
	call	_memset
	add	esp, 12					; 0000000cH

; 59   : 
; 60   :     ptrdiff_t ulAddress;
; 61   : 
; 62   :     m_pbyLColorBase = new unsigned char [(sizeof(KLColor) * LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT) + 255];

	push	37119					; 000090ffH
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T103139[ebp], eax
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR $T103139[ebp]
	mov	DWORD PTR [edx+73828], eax

; 63   :     _ASSERT(m_pbyLColorBase);
; 64   :     ulAddress = (ptrdiff_t)m_pbyLColorBase;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73828]
	mov	DWORD PTR _ulAddress$[ebp], edx

; 65   :     ulAddress = (ulAddress + 255) / 256 * 256;

	mov	eax, DWORD PTR _ulAddress$[ebp]
	add	eax, 255				; 000000ffH
	cdq
	and	edx, 255				; 000000ffH
	add	eax, edx
	sar	eax, 8
	shl	eax, 8
	mov	DWORD PTR _ulAddress$[ebp], eax

; 66   :     pLColor = (KLColor *)ulAddress;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _ulAddress$[ebp]
	mov	DWORD PTR [eax+73824], ecx

; 67   : 
; 68   :     m_pby_ptpBase = new unsigned char [(sizeof(KLColor) * LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT) + 255];

	push	37119					; 000090ffH
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T103140[ebp], eax
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR $T103140[ebp]
	mov	DWORD PTR [edx+73836], eax

; 69   :     _ASSERT(m_pby_ptpBase);
; 70   :     ulAddress = (ptrdiff_t)m_pby_ptpBase;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73836]
	mov	DWORD PTR _ulAddress$[ebp], edx

; 71   :     ulAddress = (ulAddress + 255) / 256 * 256;

	mov	eax, DWORD PTR _ulAddress$[ebp]
	add	eax, 255				; 000000ffH
	cdq
	and	edx, 255				; 000000ffH
	add	eax, edx
	sar	eax, 8
	shl	eax, 8
	mov	DWORD PTR _ulAddress$[ebp], eax

; 72   :     ptp = (KLColor *)ulAddress;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _ulAddress$[ebp]
	mov	DWORD PTR [eax+73832], ecx

; 73   : }

	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	0
_TEXT	ENDS
text$x	SEGMENT
$L103141:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	ret	0
$L103142:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
	ret	0
$L103147:
	mov	eax, OFFSET FLAT:$T103146
	jmp	___CxxFrameHandler
text$x	ENDS
??0KIpoTree@@QAE@XZ ENDP				; KIpoTree::KIpoTree
PUBLIC	??1KIpoTree@@QAE@XZ				; KIpoTree::~KIpoTree
PUBLIC	?Clear@KIpoTree@@QAEXXZ				; KIpoTree::Clear
EXTRN	??3@YAXPAX@Z:NEAR				; operator delete
xdata$x	SEGMENT
$T103157 DD	019930520H
	DD	02H
	DD	FLAT:$T103159
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T103159 DD	0ffffffffH
	DD	FLAT:$L103154
	DD	00H
	DD	FLAT:$L103155
xdata$x	ENDS
_TEXT	SEGMENT
$T103152 = -16
$T103153 = -20
__$EHRec$ = -12
_this$ = -24
??1KIpoTree@@QAE@XZ PROC NEAR				; KIpoTree::~KIpoTree

; 77   : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L103158
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 12					; 0000000cH
	mov	DWORD PTR _this$[ebp], ecx
	mov	DWORD PTR __$EHRec$[ebp+8], 1

; 78   : 	Clear();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?Clear@KIpoTree@@QAEXXZ			; KIpoTree::Clear

; 79   : 
; 80   :     if (m_pby_ptpBase)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+73836], 0
	je	SHORT $L102174

; 82   :         delete []m_pby_ptpBase;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73836]
	mov	DWORD PTR $T103152[ebp], edx
	mov	eax, DWORD PTR $T103152[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 83   :         m_pby_ptpBase = NULL;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+73836], 0

; 84   :         ptp = NULL;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+73832], 0
$L102174:

; 86   : 
; 87   :     if (m_pbyLColorBase)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+73828], 0
	je	SHORT $L102176

; 89   :         delete []m_pbyLColorBase;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73828]
	mov	DWORD PTR $T103153[ebp], edx
	mov	eax, DWORD PTR $T103153[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 90   :         m_pbyLColorBase = NULL;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+73828], 0

; 91   : 
; 92   :         pLColor = NULL;

	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+73824], 0
$L102176:

; 94   : }

	mov	BYTE PTR __$EHRec$[ebp+8], 0
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	0
_TEXT	ENDS
text$x	SEGMENT
$L103154:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	ret	0
$L103155:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
	ret	0
$L103158:
	mov	eax, OFFSET FLAT:$T103157
	jmp	___CxxFrameHandler
text$x	ENDS
??1KIpoTree@@QAE@XZ ENDP				; KIpoTree::~KIpoTree
PUBLIC	?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z ; KIpoTree::Paint
PUBLIC	?RenderLightMap@KIpoTree@@QAEXXZ		; KIpoTree::RenderLightMap
EXTRN	?PaintObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@@Z:NEAR ; KIpotBranch::PaintObjectLayer
EXTRN	?PaintNoneObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@H@Z:NEAR ; KIpotBranch::PaintNoneObjectLayer
EXTRN	?g_pRepresent@@3PAUiRepresentShell@@A:DWORD	; g_pRepresent
_TEXT	SEGMENT
_pRepresentArea$ = 8
_eLayer$ = 12
_this$ = -4
?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z PROC NEAR ; KIpoTree::Paint

; 98   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 99   : 	if(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting)

	cmp	DWORD PTR _eLayer$[ebp], 1
	jne	SHORT $L102183
	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cl, BYTE PTR [eax+90]
	test	ecx, ecx
	je	SHORT $L102183

; 101  : 		// 渲染光照图
; 102  : 		RenderLightMap();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?RenderLightMap@KIpoTree@@QAEXXZ	; KIpoTree::RenderLightMap

; 103  : 		// 设置表现模块的光照信息
; 104  : 		g_pRepresent->SetLightInfo(m_nLeftTopX, m_nLeftTopY, (unsigned int*)pLightingArray);

	mov	edx, DWORD PTR _this$[ebp]
	add	edx, 73840				; 00012070H
	push	edx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+80]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+76]
	push	eax
	mov	ecx, DWORD PTR ?g_pRepresent@@3PAUiRepresentShell@@A ; g_pRepresent
	mov	edx, DWORD PTR [ecx]
	mov	ecx, DWORD PTR ?g_pRepresent@@3PAUiRepresentShell@@A ; g_pRepresent
	call	DWORD PTR [edx+104]
$L102183:

; 106  : 	
; 107  : 	if (eLayer == IPOT_RL_OBJECT)

	cmp	DWORD PTR _eLayer$[ebp], 2
	jne	SHORT $L102185

; 109  : 		if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102186

; 110  : 			m_pMainBranch->PaintObjectLayer(pRepresentArea);

	mov	ecx, DWORD PTR _pRepresentArea$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx]
	call	?PaintObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@@Z ; KIpotBranch::PaintObjectLayer

; 111  : 		else

	jmp	SHORT $L102187
$L102186:

; 112  : 			m_DefaultBranch.PaintObjectLayer(pRepresentArea);

	mov	eax, DWORD PTR _pRepresentArea$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?PaintObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@@Z ; KIpotBranch::PaintObjectLayer
$L102187:

; 114  : 	else if (m_pMainBranch)

	jmp	SHORT $L102190
$L102185:
	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx], 0
	je	SHORT $L102189

; 115  : 		m_pMainBranch->PaintNoneObjectLayer(pRepresentArea, eLayer);

	mov	edx, DWORD PTR _eLayer$[ebp]
	push	edx
	mov	eax, DWORD PTR _pRepresentArea$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx]
	call	?PaintNoneObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@H@Z ; KIpotBranch::PaintNoneObjectLayer

; 116  : 	else

	jmp	SHORT $L102190
$L102189:

; 117  : 		m_DefaultBranch.PaintNoneObjectLayer(pRepresentArea, eLayer);

	mov	edx, DWORD PTR _eLayer$[ebp]
	push	edx
	mov	eax, DWORD PTR _pRepresentArea$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?PaintNoneObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@H@Z ; KIpotBranch::PaintNoneObjectLayer
$L102190:

; 118  : }

	mov	esp, ebp
	pop	ebp
	ret	8
?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z ENDP ; KIpoTree::Paint
_TEXT	ENDS
PUBLIC	?Breathe@KIpoTree@@QAEXXZ			; KIpoTree::Breathe
PUBLIC	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
PUBLIC	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
PUBLIC	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
PUBLIC	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
PUBLIC	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
PUBLIC	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
_TEXT	SEGMENT
$T103162 = -8
$T103163 = -12
_this$ = -16
_i$ = -4
?Breathe@KIpoTree@@QAEXXZ PROC NEAR			; KIpoTree::Breathe

; 121  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 20					; 00000014H
	mov	DWORD PTR _this$[ebp], ecx

; 122  : 	list<KLightBase*>::iterator i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator

; 123  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	lea	eax, DWORD PTR $T103162[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR _i$[ebp], ecx
	jmp	SHORT $L102198
$L102199:
	lea	ecx, DWORD PTR _i$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
$L102198:
	lea	edx, DWORD PTR $T103163[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	push	eax
	lea	ecx, DWORD PTR _i$[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	SHORT $L102200

; 125  : 		if((*i)->m_pParent == NULL)

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	eax, DWORD PTR [eax]
	cmp	DWORD PTR [eax+4], 0
	jne	SHORT $L102201

; 126  : 			(*i)->Breath();

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR -20+[ebp], ecx
	mov	edx, DWORD PTR -20+[ebp]
	mov	eax, DWORD PTR [edx]
	mov	ecx, DWORD PTR -20+[ebp]
	call	DWORD PTR [eax]
$L102201:

; 127  : 	}

	jmp	SHORT $L102199
$L102200:

; 128  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?Breathe@KIpoTree@@QAEXXZ ENDP				; KIpoTree::Breathe
_TEXT	ENDS
PUBLIC	??_GKIpotBranch@@QAEPAXI@Z			; KIpotBranch::`scalar deleting destructor'
PUBLIC	?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ ; KIpoTree::RemoveRtoGroupWithPermanentLeaf
PUBLIC	?clear@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXXZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::clear
EXTRN	?Clear@KIpotBranch@@QAEXXZ:NEAR			; KIpotBranch::Clear
_TEXT	SEGMENT
$T103165 = -8
$T103166 = -12
$T103169 = -16
$T103170 = -20
$T103171 = -24
_this$ = -28
_i$ = -4
?Clear@KIpoTree@@QAEXXZ PROC NEAR			; KIpoTree::Clear

; 132  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 32					; 00000020H
	mov	DWORD PTR _this$[ebp], ecx

; 133  : 	if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102205

; 135  : 		delete (m_pMainBranch);

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	DWORD PTR $T103166[ebp], edx
	mov	eax, DWORD PTR $T103166[ebp]
	mov	DWORD PTR $T103165[ebp], eax
	cmp	DWORD PTR $T103165[ebp], 0
	je	SHORT $L103167
	push	1
	mov	ecx, DWORD PTR $T103165[ebp]
	call	??_GKIpotBranch@@QAEPAXI@Z		; KIpotBranch::`scalar deleting destructor'
	mov	DWORD PTR -32+[ebp], eax
	jmp	SHORT $L103168
$L103167:
	mov	DWORD PTR -32+[ebp], 0
$L103168:

; 136  : 		m_pMainBranch = NULL;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx], 0
$L102205:

; 138  : 	m_DefaultBranch.Clear();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?Clear@KIpotBranch@@QAEXXZ		; KIpotBranch::Clear

; 139  : 	RemoveRtoGroupWithPermanentLeaf();

	mov	ecx, DWORD PTR _this$[ebp]
	call	?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ ; KIpoTree::RemoveRtoGroupWithPermanentLeaf

; 140  : 	
; 141  : 	//clear all dyna light
; 142  : 	list<KLightBase*>::iterator i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator

; 143  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	lea	edx, DWORD PTR $T103169[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	eax, DWORD PTR [eax]
	mov	DWORD PTR _i$[ebp], eax
	jmp	SHORT $L102212
$L102213:
	lea	ecx, DWORD PTR _i$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
$L102212:
	lea	ecx, DWORD PTR $T103170[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	push	eax
	lea	ecx, DWORD PTR _i$[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	SHORT $L102214

; 145  : 		delete (*i);

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR $T103171[ebp], edx
	mov	eax, DWORD PTR $T103171[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 146  : 	}

	jmp	SHORT $L102213
$L102214:

; 147  : 	m_LightList.clear();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?clear@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXXZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::clear

; 148  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?Clear@KIpoTree@@QAEXXZ ENDP				; KIpoTree::Clear
_TEXT	ENDS
;	COMDAT ??_GKIpotBranch@@QAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8
_this$ = -4
??_GKIpotBranch@@QAEPAXI@Z PROC NEAR			; KIpotBranch::`scalar deleting destructor', COMDAT
	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	mov	eax, DWORD PTR ___flags$[ebp]
	and	eax, 1
	test	eax, eax
	je	SHORT $L102219
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L102219:
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??_GKIpotBranch@@QAEPAXI@Z ENDP				; KIpotBranch::`scalar deleting destructor'
_TEXT	ENDS
_TEXT	SEGMENT
_this$ = -8
_pRto$ = -4
?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ PROC NEAR ; KIpoTree::RemoveRtoGroupWithPermanentLeaf

; 151  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx
$L102225:

; 152  : 	KIpotRuntimeObj* pRto;
; 153  : 	while(pRto = (KIpotRuntimeObj*)m_PermanentLeaf.pBrother)

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+56]
	mov	DWORD PTR _pRto$[ebp], ecx
	cmp	DWORD PTR _pRto$[ebp], 0
	je	SHORT $L102226

; 155  : 		m_PermanentLeaf.pBrother = pRto->pBrother;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR _pRto$[ebp]
	mov	ecx, DWORD PTR [eax+4]
	mov	DWORD PTR [edx+56], ecx

; 156  : 		pRto->pAheadBrother = NULL;

	mov	edx, DWORD PTR _pRto$[ebp]
	mov	DWORD PTR [edx+48], 0

; 157  : 		pRto->pBrother = NULL;

	mov	eax, DWORD PTR _pRto$[ebp]
	mov	DWORD PTR [eax+4], 0

; 158  : 	}

	jmp	SHORT $L102225
$L102226:

; 159  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ ENDP	; KIpoTree::RemoveRtoGroupWithPermanentLeaf
_TEXT	ENDS
PUBLIC	?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z	; KIpoTree::AddBranch
EXTRN	?AddBranch@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z:NEAR ; KIpotBranch::AddBranch
EXTRN	?AddAObject@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z:NEAR ; KIpotBranch::AddAObject
xdata$x	SEGMENT
$T103182 DD	019930520H
	DD	01H
	DD	FLAT:$T103184
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T103184 DD	0ffffffffH
	DD	FLAT:$L103180
xdata$x	ENDS
_TEXT	SEGMENT
$T103176 = -16
$T103177 = -20
__$EHRec$ = -12
_pBranchObj$ = 8
_this$ = -24
?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z PROC NEAR ; KIpoTree::AddBranch

; 172  : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L103183
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx

; 173  : 	if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102232

; 174  : 		m_pMainBranch->AddBranch(pBranchObj);

	mov	ecx, DWORD PTR _pBranchObj$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx]
	call	?AddBranch@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddBranch

; 175  : 	else if (pBranchObj)

	jmp	SHORT $L102234
$L102232:
	cmp	DWORD PTR _pBranchObj$[ebp], 0
	je	SHORT $L102234

; 177  : 		m_pMainBranch = new KIpotBranch;

	push	48					; 00000030H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T103177[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	cmp	DWORD PTR $T103177[ebp], 0
	je	SHORT $L103178
	mov	ecx, DWORD PTR $T103177[ebp]
	call	??0KIpotBranch@@QAE@XZ			; KIpotBranch::KIpotBranch
	mov	DWORD PTR -28+[ebp], eax
	jmp	SHORT $L103179
$L103178:
	mov	DWORD PTR -28+[ebp], 0
$L103179:
	mov	eax, DWORD PTR -28+[ebp]
	mov	DWORD PTR $T103176[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR $T103176[ebp]
	mov	DWORD PTR [ecx], edx

; 178  : 		m_pMainBranch->AddAObject(pBranchObj);

	mov	eax, DWORD PTR _pBranchObj$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [ecx]
	call	?AddAObject@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddAObject
$L102234:

; 180  : }

	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	4
_TEXT	ENDS
text$x	SEGMENT
$L103180:
	mov	eax, DWORD PTR $T103177[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L103183:
	mov	eax, OFFSET FLAT:$T103182
	jmp	___CxxFrameHandler
text$x	ENDS
?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z ENDP	; KIpoTree::AddBranch
PUBLIC	?Fell@KIpoTree@@QAEXXZ				; KIpoTree::Fell
PUBLIC	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
EXTRN	?RemoveAllRtoLeafs@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z:NEAR ; KIpotBranch::RemoveAllRtoLeafs
_TEXT	SEGMENT
$T103186 = -8
$T103187 = -12
$T103190 = -16
$T103191 = -20
$T103192 = -24
$T103193 = -28
_this$ = -32
_i$ = -4
?Fell@KIpoTree@@QAEXXZ PROC NEAR			; KIpoTree::Fell

; 184  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 36					; 00000024H
	mov	DWORD PTR _this$[ebp], ecx

; 185  : 	if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102242

; 187  : 		m_pMainBranch->RemoveAllRtoLeafs(&m_PermanentLeaf);

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 52					; 00000034H
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx]
	call	?RemoveAllRtoLeafs@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::RemoveAllRtoLeafs

; 188  : 		delete(m_pMainBranch);

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR $T103187[ebp], ecx
	mov	edx, DWORD PTR $T103187[ebp]
	mov	DWORD PTR $T103186[ebp], edx
	cmp	DWORD PTR $T103186[ebp], 0
	je	SHORT $L103188
	push	1
	mov	ecx, DWORD PTR $T103186[ebp]
	call	??_GKIpotBranch@@QAEPAXI@Z		; KIpotBranch::`scalar deleting destructor'
	mov	DWORD PTR -36+[ebp], eax
	jmp	SHORT $L103189
$L103188:
	mov	DWORD PTR -36+[ebp], 0
$L103189:

; 189  : 		m_pMainBranch = NULL;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax], 0
$L102242:

; 191  : 	m_DefaultBranch.RemoveAllRtoLeafs(&m_PermanentLeaf);

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 52					; 00000034H
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?RemoveAllRtoLeafs@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::RemoveAllRtoLeafs

; 192  : 	m_DefaultBranch.Clear();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?Clear@KIpotBranch@@QAEXXZ		; KIpotBranch::Clear

; 193  : 
; 194  : 	//清除所有内建对象的光源
; 195  : 	list<KLightBase*>::iterator i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator

; 196  : 	for (i = m_LightList.begin(); i != m_LightList.end(); )

	lea	edx, DWORD PTR $T103190[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	eax, DWORD PTR [eax]
	mov	DWORD PTR _i$[ebp], eax
$L102250:
	lea	ecx, DWORD PTR $T103191[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	push	eax
	lea	ecx, DWORD PTR _i$[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	SHORT $L102251

; 198  : 		if((*i)->m_pParent == NULL)

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	edx, DWORD PTR [eax]
	cmp	DWORD PTR [edx+4], 0
	jne	SHORT $L102252

; 200  : 			delete (*i);

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	eax, DWORD PTR [eax]
	mov	DWORD PTR $T103192[ebp], eax
	mov	ecx, DWORD PTR $T103192[ebp]
	push	ecx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 201  : 			i = m_LightList.erase(i);

	mov	edx, DWORD PTR _i$[ebp]
	push	edx
	lea	eax, DWORD PTR $T103193[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR _i$[ebp], ecx

; 202  : 			continue;

	jmp	SHORT $L102250
$L102252:

; 204  : 		++i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++

; 205  : 	}

	jmp	SHORT $L102250
$L102251:

; 206  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?Fell@KIpoTree@@QAEXXZ ENDP				; KIpoTree::Fell
_TEXT	ENDS
PUBLIC	?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z ; KIpoTree::AddLeafLine
EXTRN	?AddLeafLine@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z:NEAR ; KIpotBranch::AddLeafLine
_TEXT	SEGMENT
_pLeaf$ = 8
_this$ = -4
?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z PROC NEAR ; KIpoTree::AddLeafLine

; 221  : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 222  : 	if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102259

; 223  : 		m_pMainBranch->AddLeafLine(pLeaf);

	mov	ecx, DWORD PTR _pLeaf$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx]
	call	?AddLeafLine@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddLeafLine

; 224  : 	else

	jmp	SHORT $L102260
$L102259:

; 225  : 		m_DefaultBranch.AddLeafLine(pLeaf);

	mov	eax, DWORD PTR _pLeaf$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?AddLeafLine@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddLeafLine
$L102260:

; 226  : }

	mov	esp, ebp
	pop	ebp
	ret	4
?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z ENDP	; KIpoTree::AddLeafLine
_TEXT	ENDS
PUBLIC	?SetColor@KLColor@@QAEXK@Z			; KLColor::SetColor
PUBLIC	?Scale@KLColor@@QAEXM@Z				; KLColor::Scale
PUBLIC	?GetColor@KLColor@@QAEKXZ			; KLColor::GetColor
PUBLIC	?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z	; KIpoTree::AddLeafPoint
PUBLIC	__real@4@4007b400000000000000
PUBLIC	?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::push_back
EXTRN	?IsPlayer@KNpc@@QAEHXZ:NEAR			; KNpc::IsPlayer
EXTRN	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z:NEAR ; KIpotBranch::AddLeafPoint
EXTRN	?Npc@@3PAVKNpc@@A:BYTE				; Npc
EXTRN	?CoreGetGameObjLightInfo@@YAXIIPAUKLightInfo@@@Z:NEAR ; CoreGetGameObjLightInfo
;	COMDAT __real@4@4007b400000000000000
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@4@4007b400000000000000 DD 043b40000r	; 360
CONST	ENDS
xdata$x	SEGMENT
$T103203 DD	019930520H
	DD	01H
	DD	FLAT:$T103207
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T103207 DD	0ffffffffH
	DD	FLAT:$L103201
xdata$x	ENDS
_TEXT	SEGMENT
$T103196 = -60
$T103197 = -64
$T103200 = -68
__$EHRec$ = -12
_pLeaf$ = 8
_this$ = -72
_pRtoLeaf$102268 = -16
_pLight$102271 = -20
_lightInfo$102277 = -40
_n$102283 = -56
_f$102289 = -52
_color$102290 = -48
?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z PROC NEAR	; KIpoTree::AddLeafPoint

; 230  : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L103206
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 64					; 00000040H
	mov	DWORD PTR _this$[ebp], ecx

; 231  : 	if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102265

; 232  : 		m_pMainBranch->AddLeafPoint(pLeaf);

	mov	ecx, DWORD PTR _pLeaf$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx]
	call	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::AddLeafPoint

; 233  : 	else

	jmp	SHORT $L102266
$L102265:

; 234  : 		m_DefaultBranch.AddLeafPoint(pLeaf);

	mov	eax, DWORD PTR _pLeaf$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::AddLeafPoint
$L102266:

; 235  : 	// 
; 236  : 	if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)

	mov	ecx, DWORD PTR _pLeaf$[ebp]
	cmp	DWORD PTR [ecx], 1
	jne	$L102292

; 238  : 		KIpotRuntimeObj* pRtoLeaf = (KIpotRuntimeObj*)pLeaf;

	mov	edx, DWORD PTR _pLeaf$[ebp]
	mov	DWORD PTR _pRtoLeaf$102268[ebp], edx

; 239  : 		if (pRtoLeaf->eLayerParam & IPOT_RL_LIGHT_PROP)

	mov	eax, DWORD PTR _pRtoLeaf$102268[ebp]
	mov	ecx, DWORD PTR [eax+36]
	and	ecx, 8
	test	ecx, ecx
	je	$L102292

; 241  : 			// 增加一个光源
; 242  : 			KLightBase *pLight = new KLightBase;

	push	28					; 0000001cH
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T103197[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	cmp	DWORD PTR $T103197[ebp], 0
	je	SHORT $L103198
	mov	ecx, DWORD PTR $T103197[ebp]
	call	??0KLightBase@@QAE@XZ			; KLightBase::KLightBase
	mov	DWORD PTR -76+[ebp], eax
	jmp	SHORT $L103199
$L103198:
	mov	DWORD PTR -76+[ebp], 0
$L103199:
	mov	edx, DWORD PTR -76+[ebp]
	mov	DWORD PTR $T103196[ebp], edx
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	eax, DWORD PTR $T103196[ebp]
	mov	DWORD PTR _pLight$102271[ebp], eax

; 243  : 			if(pLight)

	cmp	DWORD PTR _pLight$102271[ebp], 0
	je	$L102292

; 245  : 				KLightInfo lightInfo;
; 246  : 				CoreGetGameObjLightInfo(pRtoLeaf->uGenre, pRtoLeaf->nId, &lightInfo);

	lea	ecx, DWORD PTR _lightInfo$102277[ebp]
	push	ecx
	mov	edx, DWORD PTR _pRtoLeaf$102268[ebp]
	mov	eax, DWORD PTR [edx+28]
	push	eax
	mov	ecx, DWORD PTR _pRtoLeaf$102268[ebp]
	mov	edx, DWORD PTR [ecx+24]
	push	edx
	call	?CoreGetGameObjLightInfo@@YAXIIPAUKLightInfo@@@Z ; CoreGetGameObjLightInfo
	add	esp, 12					; 0000000cH

; 247  : 				pLight->m_dwColor = lightInfo.dwColor;

	mov	eax, DWORD PTR _pLight$102271[ebp]
	mov	ecx, DWORD PTR _lightInfo$102277[ebp+12]
	mov	DWORD PTR [eax+20], ecx

; 248  : 				pLight->m_oPosition = lightInfo.oPosition;

	mov	edx, DWORD PTR _pLight$102271[ebp]
	add	edx, 8
	mov	eax, DWORD PTR _lightInfo$102277[ebp]
	mov	DWORD PTR [edx], eax
	mov	ecx, DWORD PTR _lightInfo$102277[ebp+4]
	mov	DWORD PTR [edx+4], ecx
	mov	eax, DWORD PTR _lightInfo$102277[ebp+8]
	mov	DWORD PTR [edx+8], eax

; 249  : 				pLight->m_nRadius = lightInfo.nRadius;

	mov	ecx, DWORD PTR _pLight$102271[ebp]
	mov	edx, DWORD PTR _lightInfo$102277[ebp+16]
	mov	DWORD PTR [ecx+24], edx

; 250  : 				pLight->m_pParent = pLeaf;

	mov	eax, DWORD PTR _pLight$102271[ebp]
	mov	ecx, DWORD PTR _pLeaf$[ebp]
	mov	DWORD PTR [eax+4], ecx

; 251  : 				
; 252  : 				if (pRtoLeaf->uGenre == CGOG_NPC && Npc[pRtoLeaf->nId].IsPlayer())

	mov	edx, DWORD PTR _pRtoLeaf$102268[ebp]
	cmp	DWORD PTR [edx+24], 6
	jne	$L102282
	mov	eax, DWORD PTR _pRtoLeaf$102268[ebp]
	mov	ecx, DWORD PTR [eax+28]
	imul	ecx, 33600				; 00008340H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?IsPlayer@KNpc@@QAEHXZ			; KNpc::IsPlayer
	test	eax, eax
	je	$L102282

; 254  : 					pLight->m_dwColor = 0xffffffff;

	mov	ecx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [ecx+20], -1

; 255  : 					if(m_bIsIndoor)

	mov	edx, DWORD PTR _this$[ebp]
	xor	eax, eax
	mov	al, BYTE PTR [edx+88]
	test	eax, eax
	je	SHORT $L102279

; 257  : 						if(m_dwAmbient == 0xff404040)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+92], -12566464		; ff404040H
	jne	SHORT $L102280

; 258  : 							pLight->m_nRadius = 0;

	mov	edx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [edx+24], 0

; 259  : 						else

	jmp	SHORT $L102281
$L102280:

; 260  : 							pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;

	mov	eax, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [eax+24], 320			; 00000140H
$L102281:

; 262  : 					else

	jmp	$L102282
$L102279:

; 264  : 						// 是主角，按时间调整光源亮度
; 265  : 						int n = 0;

	mov	DWORD PTR _n$102283[ebp], 0

; 266  : 						if(m_nCurrentTime >= 480 && m_nCurrentTime <= 960)

	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+84], 480			; 000001e0H
	jl	SHORT $L102284
	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+84], 960			; 000003c0H
	jg	SHORT $L102284

; 267  : 							pLight->m_nRadius = 0;

	mov	eax, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [eax+24], 0

; 268  : 						else if(m_nCurrentTime < 480)

	jmp	SHORT $L102287
$L102284:
	mov	ecx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [ecx+84], 480			; 000001e0H
	jge	SHORT $L102286

; 270  : 							n = 480 - m_nCurrentTime;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, 480				; 000001e0H
	sub	eax, DWORD PTR [edx+84]
	mov	DWORD PTR _n$102283[ebp], eax

; 271  : 							pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;

	mov	ecx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [ecx+24], 320			; 00000140H

; 273  : 						else

	jmp	SHORT $L102287
$L102286:

; 275  : 							n = m_nCurrentTime - 960;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+84]
	sub	eax, 960				; 000003c0H
	mov	DWORD PTR _n$102283[ebp], eax

; 276  : 							pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;

	mov	ecx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [ecx+24], 320			; 00000140H
$L102287:

; 278  : 
; 279  : 						if(n > 360)

	cmp	DWORD PTR _n$102283[ebp], 360		; 00000168H
	jle	SHORT $L102288

; 280  : 							n = 360;

	mov	DWORD PTR _n$102283[ebp], 360		; 00000168H
$L102288:

; 281  : 						float f = n / 360.0f;

	fild	DWORD PTR _n$102283[ebp]
	fdiv	DWORD PTR __real@4@4007b400000000000000
	fst	DWORD PTR _f$102289[ebp]

; 282  : 						f = f * f;

	fmul	DWORD PTR _f$102289[ebp]
	fstp	DWORD PTR _f$102289[ebp]

; 283  : 						KLColor color;
; 284  : 						color.SetColor(pLight->m_dwColor);

	mov	edx, DWORD PTR _pLight$102271[ebp]
	mov	eax, DWORD PTR [edx+20]
	push	eax
	lea	ecx, DWORD PTR _color$102290[ebp]
	call	?SetColor@KLColor@@QAEXK@Z		; KLColor::SetColor

; 285  : 						color.Scale(f);

	mov	ecx, DWORD PTR _f$102289[ebp]
	push	ecx
	lea	ecx, DWORD PTR _color$102290[ebp]
	call	?Scale@KLColor@@QAEXM@Z			; KLColor::Scale

; 286  : 						pLight->m_dwColor = color.GetColor();

	lea	ecx, DWORD PTR _color$102290[ebp]
	call	?GetColor@KLColor@@QAEKXZ		; KLColor::GetColor
	mov	edx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [edx+20], eax
$L102282:

; 289  : 
; 290  : 				pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;

	mov	eax, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [eax+24], 320			; 00000140H

; 291  : 				pLight->m_dwColor = 0xff808080;

	mov	ecx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR [ecx+20], -8355712		; ff808080H

; 292  : 
; 293  : 				if(pLight->m_nRadius)

	mov	edx, DWORD PTR _pLight$102271[ebp]
	cmp	DWORD PTR [edx+24], 0
	je	SHORT $L102291

; 295  : 					m_LightList.push_back(pLight);

	lea	eax, DWORD PTR _pLight$102271[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::push_back

; 297  :                 else

	jmp	SHORT $L102292
$L102291:

; 299  :                     delete pLight;

	mov	ecx, DWORD PTR _pLight$102271[ebp]
	mov	DWORD PTR $T103200[ebp], ecx
	mov	edx, DWORD PTR $T103200[ebp]
	push	edx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 300  :                     pLight = NULL;

	mov	DWORD PTR _pLight$102271[ebp], 0
$L102292:

; 305  : }

	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	mov	esp, ebp
	pop	ebp
	ret	4
_TEXT	ENDS
text$x	SEGMENT
$L103201:
	mov	eax, DWORD PTR $T103197[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L103206:
	mov	eax, OFFSET FLAT:$T103203
	jmp	___CxxFrameHandler
text$x	ENDS
?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z ENDP	; KIpoTree::AddLeafPoint
;	COMDAT ?SetColor@KLColor@@QAEXK@Z
_TEXT	SEGMENT
_dwColor$ = 8
_this$ = -4
?SetColor@KLColor@@QAEXK@Z PROC NEAR			; KLColor::SetColor, COMDAT

; 372  : 	{

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 373  : 		r = (unsigned short)((dwColor>>16) & 0xff);

	mov	eax, DWORD PTR _dwColor$[ebp]
	shr	eax, 16					; 00000010H
	and	eax, 255				; 000000ffH
	mov	ecx, DWORD PTR _this$[ebp]
	mov	WORD PTR [ecx], ax

; 374  : 		g = (unsigned short)((dwColor>>8) & 0xff);

	mov	edx, DWORD PTR _dwColor$[ebp]
	shr	edx, 8
	and	edx, 255				; 000000ffH
	mov	eax, DWORD PTR _this$[ebp]
	mov	WORD PTR [eax+2], dx

; 375  : 		b = (unsigned short)(dwColor & 0xff);

	mov	ecx, DWORD PTR _dwColor$[ebp]
	and	ecx, 255				; 000000ffH
	mov	edx, DWORD PTR _this$[ebp]
	mov	WORD PTR [edx+4], cx

; 376  : 	}

	mov	esp, ebp
	pop	ebp
	ret	4
?SetColor@KLColor@@QAEXK@Z ENDP				; KLColor::SetColor
_TEXT	ENDS
;	COMDAT ?Scale@KLColor@@QAEXM@Z
_TEXT	SEGMENT
_f$ = 8
_this$ = -4
?Scale@KLColor@@QAEXM@Z PROC NEAR			; KLColor::Scale, COMDAT

; 378  : 	{

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx

; 379  : 		r = (int)(r * f);

	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax]
	mov	DWORD PTR -8+[ebp], ecx
	fild	DWORD PTR -8+[ebp]
	fmul	DWORD PTR _f$[ebp]
	call	__ftol
	mov	edx, DWORD PTR _this$[ebp]
	mov	WORD PTR [edx], ax

; 380  : 		g = (int)(g * f);

	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax+2]
	mov	DWORD PTR -12+[ebp], ecx
	fild	DWORD PTR -12+[ebp]
	fmul	DWORD PTR _f$[ebp]
	call	__ftol
	mov	edx, DWORD PTR _this$[ebp]
	mov	WORD PTR [edx+2], ax

; 381  : 		b = (int)(b * f);

	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax+4]
	mov	DWORD PTR -16+[ebp], ecx
	fild	DWORD PTR -16+[ebp]
	fmul	DWORD PTR _f$[ebp]
	call	__ftol
	mov	edx, DWORD PTR _this$[ebp]
	mov	WORD PTR [edx+4], ax

; 382  : 	}

	mov	esp, ebp
	pop	ebp
	ret	4
?Scale@KLColor@@QAEXM@Z ENDP				; KLColor::Scale
_TEXT	ENDS
;	COMDAT ?GetColor@KLColor@@QAEKXZ
_TEXT	SEGMENT
_this$ = -4
?GetColor@KLColor@@QAEKXZ PROC NEAR			; KLColor::GetColor, COMDAT

; 384  : 	{

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 385  : 		return 0xff000000 | (r<<16) | (g<<8) | b;

	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax]
	mov	eax, ecx
	shl	eax, 16					; 00000010H
	or	eax, -16777216				; ff000000H
	mov	edx, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [edx+2]
	shl	ecx, 8
	or	eax, ecx
	mov	edx, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [edx+4]
	or	eax, ecx

; 386  : 	}

	mov	esp, ebp
	pop	ebp
	ret	0
?GetColor@KLColor@@QAEKXZ ENDP				; KLColor::GetColor
_TEXT	ENDS
PUBLIC	?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z	; KIpoTree::PluckRto
EXTRN	?Pluck@KIpotRuntimeObj@@QAEXXZ:NEAR		; KIpotRuntimeObj::Pluck
_TEXT	SEGMENT
$T103215 = -8
$T103216 = -12
$T103217 = -16
$T103218 = -20
_pLeaf$ = 8
_this$ = -24
_i$ = -4
?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z PROC NEAR ; KIpoTree::PluckRto

; 308  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 24					; 00000018H
	mov	DWORD PTR _this$[ebp], ecx

; 309  : 	pLeaf->Pluck();

	mov	ecx, DWORD PTR _pLeaf$[ebp]
	call	?Pluck@KIpotRuntimeObj@@QAEXXZ		; KIpotRuntimeObj::Pluck

; 310  : 	list<KLightBase*>::iterator i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator

; 311  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	lea	eax, DWORD PTR $T103215[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR _i$[ebp], ecx
	jmp	SHORT $L102302
$L102303:
	lea	ecx, DWORD PTR _i$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
$L102302:
	lea	edx, DWORD PTR $T103216[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	push	eax
	lea	ecx, DWORD PTR _i$[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	SHORT $L102304

; 313  : 		if((*i)->m_pParent == pLeaf)

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR [eax+4]
	cmp	ecx, DWORD PTR _pLeaf$[ebp]
	jne	SHORT $L102305

; 315  : 			delete (*i);

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR $T103217[ebp], edx
	mov	eax, DWORD PTR $T103217[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 316  : 			m_LightList.erase(i);

	mov	ecx, DWORD PTR _i$[ebp]
	push	ecx
	lea	edx, DWORD PTR $T103218[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase

; 317  : 			break;

	jmp	SHORT $L102304
$L102305:

; 319  : 	}

	jmp	SHORT $L102303
$L102304:

; 320  : }

	mov	esp, ebp
	pop	ebp
	ret	4
?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z ENDP	; KIpoTree::PluckRto
_TEXT	ENDS
PUBLIC	?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z	; KIpoTree::SetPermanentBranchPos
EXTRN	?SetLine@KIpotBranch@@QAEXPAUtagPOINT@@0@Z:NEAR	; KIpotBranch::SetLine
_TEXT	SEGMENT
_nLeftX$ = 8
_nRightX$ = 12
_y$ = 16
_this$ = -20
_p1$ = -8
_p2$ = -16
?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z PROC NEAR	; KIpoTree::SetPermanentBranchPos

; 323  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 20					; 00000014H
	mov	DWORD PTR _this$[ebp], ecx

; 324  : 	POINT	p1, p2;
; 325  : 	if (nLeftX < nRightX)

	mov	eax, DWORD PTR _nLeftX$[ebp]
	cmp	eax, DWORD PTR _nRightX$[ebp]
	jge	SHORT $L102316

; 327  : 		p1.x = nLeftX;

	mov	ecx, DWORD PTR _nLeftX$[ebp]
	mov	DWORD PTR _p1$[ebp], ecx

; 328  : 		p2.x = nRightX;

	mov	edx, DWORD PTR _nRightX$[ebp]
	mov	DWORD PTR _p2$[ebp], edx

; 330  : 	else if (nLeftX > nRightX)

	jmp	SHORT $L102319
$L102316:
	mov	eax, DWORD PTR _nLeftX$[ebp]
	cmp	eax, DWORD PTR _nRightX$[ebp]
	jle	SHORT $L102318

; 332  : 		p2.x = nLeftX;

	mov	ecx, DWORD PTR _nLeftX$[ebp]
	mov	DWORD PTR _p2$[ebp], ecx

; 333  : 		p1.x = nRightX;

	mov	edx, DWORD PTR _nRightX$[ebp]
	mov	DWORD PTR _p1$[ebp], edx

; 335  : 	else

	jmp	SHORT $L102319
$L102318:

; 337  : 		p1.x = nLeftX;

	mov	eax, DWORD PTR _nLeftX$[ebp]
	mov	DWORD PTR _p1$[ebp], eax

; 338  : 		p2.x = nRightX + 2048;

	mov	ecx, DWORD PTR _nRightX$[ebp]
	add	ecx, 2048				; 00000800H
	mov	DWORD PTR _p2$[ebp], ecx
$L102319:

; 340  : 	p2.y = p1.y = y;

	mov	edx, DWORD PTR _y$[ebp]
	mov	DWORD PTR _p1$[ebp+4], edx
	mov	eax, DWORD PTR _p1$[ebp+4]
	mov	DWORD PTR _p2$[ebp+4], eax

; 341  : 	m_DefaultBranch.SetLine(&p1, &p2);

	lea	ecx, DWORD PTR _p2$[ebp]
	push	ecx
	lea	edx, DWORD PTR _p1$[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?SetLine@KIpotBranch@@QAEXPAUtagPOINT@@0@Z ; KIpotBranch::SetLine

; 342  : }

	mov	esp, ebp
	pop	ebp
	ret	12					; 0000000cH
?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z ENDP		; KIpoTree::SetPermanentBranchPos
_TEXT	ENDS
PUBLIC	?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z	; KIpoTree::StrewRtoLeafs
PUBLIC	?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ; KIpoTree::ObjectsCallback
EXTRN	?EnumerateObjects@KIpotBranch@@QAEXPAXP6AX0PAUKIpotLeaf@@@Z@Z:NEAR ; KIpotBranch::EnumerateObjects
_TEXT	SEGMENT
_KeepRtoArea$ = 8
_this$ = -16
_pBranch$ = -8
_pRto$ = -4
_j$102336 = -12
?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z PROC NEAR	; KIpoTree::StrewRtoLeafs

; 345  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx

; 346  : 	KIpotBranch* pBranch;
; 347  : 	if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102325

; 348  : 		pBranch = m_pMainBranch;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	DWORD PTR _pBranch$[ebp], edx

; 349  : 	else

	jmp	SHORT $L102326
$L102325:

; 350  : 		pBranch = &m_DefaultBranch;

	mov	eax, DWORD PTR _this$[ebp]
	add	eax, 4
	mov	DWORD PTR _pBranch$[ebp], eax
$L102326:

; 351  : 
; 352  : 	KIpotRuntimeObj* pRto;
; 353  : 	while(pRto = (KIpotRuntimeObj*)m_PermanentLeaf.pBrother)

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+56]
	mov	DWORD PTR _pRto$[ebp], edx
	cmp	DWORD PTR _pRto$[ebp], 0
	je	SHORT $L102330

; 355  : 		m_PermanentLeaf.pBrother = pRto->pBrother;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _pRto$[ebp]
	mov	edx, DWORD PTR [ecx+4]
	mov	DWORD PTR [eax+56], edx

; 356  : 		pRto->pAheadBrother = NULL;

	mov	eax, DWORD PTR _pRto$[ebp]
	mov	DWORD PTR [eax+48], 0

; 357  : 		pRto->pBrother = NULL;

	mov	ecx, DWORD PTR _pRto$[ebp]
	mov	DWORD PTR [ecx+4], 0

; 360  : 			pRto->oPosition.x <  KeepRtoArea.right ||
; 361  : 			pRto->oPosition.y >= KeepRtoArea.top   ||
; 362  : 			pRto->oPosition.y <  KeepRtoArea.bottom)

	mov	edx, DWORD PTR _pRto$[ebp]
	mov	eax, DWORD PTR _KeepRtoArea$[ebp]
	mov	ecx, DWORD PTR [edx+16]
	cmp	ecx, DWORD PTR [eax]
	jge	SHORT $L102333
	mov	edx, DWORD PTR _pRto$[ebp]
	mov	eax, DWORD PTR _KeepRtoArea$[ebp]
	mov	ecx, DWORD PTR [edx+16]
	cmp	ecx, DWORD PTR [eax+8]
	jl	SHORT $L102333
	mov	edx, DWORD PTR _pRto$[ebp]
	mov	eax, DWORD PTR _KeepRtoArea$[ebp]
	mov	ecx, DWORD PTR [edx+20]
	cmp	ecx, DWORD PTR [eax+4]
	jge	SHORT $L102333
	mov	edx, DWORD PTR _pRto$[ebp]
	mov	eax, DWORD PTR _KeepRtoArea$[ebp]
	mov	ecx, DWORD PTR [edx+20]
	cmp	ecx, DWORD PTR [eax+12]
	jge	SHORT $L102332
$L102333:

; 364  : 			pBranch->AddLeafPoint(pRto);

	mov	edx, DWORD PTR _pRto$[ebp]
	push	edx
	mov	ecx, DWORD PTR _pBranch$[ebp]
	call	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::AddLeafPoint

; 366  : 		else

	jmp	SHORT $L102334
$L102332:

; 368  : 			PluckRto(pRto);				

	mov	eax, DWORD PTR _pRto$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z ; KIpoTree::PluckRto
$L102334:

; 370  : 	}

	jmp	$L102326
$L102330:

; 371  : 
; 372  : 	if(m_bDynamicLighting)

	mov	ecx, DWORD PTR _this$[ebp]
	xor	edx, edx
	mov	dl, BYTE PTR [ecx+90]
	test	edx, edx
	je	SHORT $L102341

; 374  : 		// 清空遮挡信息
; 375  : 		for(int j=0; j<LIGHTING_GRID_WIDTH*LIGHTING_GRID_HEIGHT; j++)

	mov	DWORD PTR _j$102336[ebp], 0
	jmp	SHORT $L102337
$L102338:
	mov	eax, DWORD PTR _j$102336[ebp]
	add	eax, 1
	mov	DWORD PTR _j$102336[ebp], eax
$L102337:
	cmp	DWORD PTR _j$102336[ebp], 4608		; 00001200H
	jge	SHORT $L102339

; 377  : 			pObstacle[j].nObstacle = 0;

	mov	ecx, DWORD PTR _j$102336[ebp]
	shl	ecx, 4
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+ecx+96], 0

; 378  : 		}

	jmp	SHORT $L102338
$L102339:

; 379  : 
; 380  : 		// 遍历树，计算遮挡信息
; 381  : 		if (m_pMainBranch)

	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax], 0
	je	SHORT $L102340

; 382  : 			m_pMainBranch->EnumerateObjects(this, ObjectsCallback);

	push	OFFSET FLAT:?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ; KIpoTree::ObjectsCallback
	mov	ecx, DWORD PTR _this$[ebp]
	push	ecx
	mov	edx, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [edx]
	call	?EnumerateObjects@KIpotBranch@@QAEXPAXP6AX0PAUKIpotLeaf@@@Z@Z ; KIpotBranch::EnumerateObjects

; 383  : 		else

	jmp	SHORT $L102341
$L102340:

; 384  : 			m_DefaultBranch.EnumerateObjects(this, ObjectsCallback);

	push	OFFSET FLAT:?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ; KIpoTree::ObjectsCallback
	mov	eax, DWORD PTR _this$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	call	?EnumerateObjects@KIpotBranch@@QAEXPAXP6AX0PAUKIpotLeaf@@@Z@Z ; KIpotBranch::EnumerateObjects
$L102341:

; 386  : }

	mov	esp, ebp
	pop	ebp
	ret	4
?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z ENDP	; KIpoTree::StrewRtoLeafs
_TEXT	ENDS
PUBLIC	?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z		; KIpoTree::FillLineObstacle
_TEXT	SEGMENT
_p$ = 8
_pLeaf$ = 12
_pTree$ = -8
_pObj$ = -4
_pBio$ = -12
_nX$102361 = -20
_nY$102362 = -24
_nIdx$102363 = -16
?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z PROC NEAR ; KIpoTree::ObjectsCallback

; 390  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 24					; 00000018H

; 391  : 	_ASSERT(p);
; 392  : 	_ASSERT(pLeaf);
; 393  : 	static int nObstacleID = 1;
; 394  : 	nObstacleID++;

	mov	eax, DWORD PTR _?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA
	add	eax, 1
	mov	DWORD PTR _?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA, eax

; 395  : 
; 396  : 	if(pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)

	mov	ecx, DWORD PTR _pLeaf$[ebp]
	cmp	DWORD PTR [ecx], 1
	jne	SHORT $L102350

; 397  : 		return;

	jmp	$L102345
$L102350:

; 398  : 
; 399  : 	KIpoTree *pTree = (KIpoTree*)p;

	mov	edx, DWORD PTR _p$[ebp]
	mov	DWORD PTR _pTree$[ebp], edx

; 400  : 	KIpotBuildinObj* pObj = (KIpotBuildinObj*)pLeaf;

	mov	eax, DWORD PTR _pLeaf$[ebp]
	mov	DWORD PTR _pObj$[ebp], eax

; 401  : 	KBuildinObj* pBio = ((KIpotBuildinObj*)pLeaf)->pBio;

	mov	ecx, DWORD PTR _pLeaf$[ebp]
	mov	edx, DWORD PTR [ecx+48]
	mov	DWORD PTR _pBio$[ebp], edx

; 402  : 	if((pBio->Props & SPBIO_P_BLOCK_LIGHT_MASK) == SPBIO_P_BLOCK_LIGHT_NONE)

	mov	eax, DWORD PTR _pBio$[ebp]
	mov	ecx, DWORD PTR [eax]
	and	ecx, 48					; 00000030H
	test	ecx, ecx
	jne	SHORT $L102357

; 403  : 		return;

	jmp	$L102345
$L102357:

; 404  : 
; 405  : 	if((pBio->Props & SPBIO_P_BLOCK_LIGHT_MASK) == SPBIO_P_BLOCK_LIGHT_BOTTOM)

	mov	edx, DWORD PTR _pBio$[ebp]
	mov	eax, DWORD PTR [edx]
	and	eax, 48					; 00000030H
	cmp	eax, 16					; 00000010H
	jne	SHORT $L102358

; 407  : 		// 按线段处理遮挡
; 408  : 		pTree->FillLineObstacle(pObj->oPosition.x - pTree->m_nLeftTopX, pObj->oPosition.y - pTree->m_nLeftTopY, 
; 409  : 								pObj->oEndPos.x - pTree->m_nLeftTopX, pObj->oEndPos.y - pTree->m_nLeftTopY, nObstacleID);

	mov	ecx, DWORD PTR _?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA
	push	ecx
	mov	edx, DWORD PTR _pObj$[ebp]
	mov	eax, DWORD PTR _pTree$[ebp]
	mov	ecx, DWORD PTR [edx+32]
	sub	ecx, DWORD PTR [eax+80]
	push	ecx
	mov	edx, DWORD PTR _pObj$[ebp]
	mov	eax, DWORD PTR _pTree$[ebp]
	mov	ecx, DWORD PTR [edx+28]
	sub	ecx, DWORD PTR [eax+76]
	push	ecx
	mov	edx, DWORD PTR _pObj$[ebp]
	mov	eax, DWORD PTR _pTree$[ebp]
	mov	ecx, DWORD PTR [edx+20]
	sub	ecx, DWORD PTR [eax+80]
	push	ecx
	mov	edx, DWORD PTR _pObj$[ebp]
	mov	eax, DWORD PTR _pTree$[ebp]
	mov	ecx, DWORD PTR [edx+16]
	sub	ecx, DWORD PTR [eax+76]
	push	ecx
	mov	ecx, DWORD PTR _pTree$[ebp]
	call	?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z	; KIpoTree::FillLineObstacle

; 411  : 	else if((pBio->Props & SPBIO_P_BLOCK_LIGHT_MASK) == SPBIO_P_BLOCK_LIGHT_CIRCLE)

	jmp	$L102366
$L102358:
	mov	edx, DWORD PTR _pBio$[ebp]
	mov	eax, DWORD PTR [edx]
	and	eax, 48					; 00000030H
	cmp	eax, 32					; 00000020H
	jne	$L102366

; 413  : 		// 按圆处理光线遮挡，忽略半径，都按一个格子处理
; 414  : 		int nX = pObj->oPosition.x;

	mov	ecx, DWORD PTR _pObj$[ebp]
	mov	edx, DWORD PTR [ecx+16]
	mov	DWORD PTR _nX$102361[ebp], edx

; 415  : 		int nY = pObj->oPosition.y;

	mov	eax, DWORD PTR _pObj$[ebp]
	mov	ecx, DWORD PTR [eax+20]
	mov	DWORD PTR _nY$102362[ebp], ecx

; 418  : 				nX >= pTree->m_nLeftTopX + AREGION_WIDTH * 3 || nY >= pTree->m_nLeftTopY + AREGION_HEIGHT * 3)

	mov	edx, DWORD PTR _pTree$[ebp]
	mov	eax, DWORD PTR _nX$102361[ebp]
	cmp	eax, DWORD PTR [edx+76]
	jl	SHORT $L102365
	mov	ecx, DWORD PTR _pTree$[ebp]
	mov	edx, DWORD PTR _nY$102362[ebp]
	cmp	edx, DWORD PTR [ecx+80]
	jl	SHORT $L102365
	mov	eax, DWORD PTR _pTree$[ebp]
	mov	ecx, DWORD PTR [eax+76]
	add	ecx, 1536				; 00000600H
	cmp	DWORD PTR _nX$102361[ebp], ecx
	jge	SHORT $L102365
	mov	edx, DWORD PTR _pTree$[ebp]
	mov	eax, DWORD PTR [edx+80]
	add	eax, 3072				; 00000c00H
	cmp	DWORD PTR _nY$102362[ebp], eax
	jl	SHORT $L102364
$L102365:

; 419  : 			return;

	jmp	SHORT $L102345
$L102364:

; 422  : 			nX = (nX - pTree->m_nLeftTopX) / LIGHTING_GRID_SIZEX;

	mov	ecx, DWORD PTR _pTree$[ebp]
	mov	eax, DWORD PTR _nX$102361[ebp]
	sub	eax, DWORD PTR [ecx+76]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nX$102361[ebp], eax

; 423  : 			nY = (nY - pTree->m_nLeftTopY) / LIGHTING_GRID_SIZEY;

	mov	edx, DWORD PTR _pTree$[ebp]
	mov	eax, DWORD PTR _nY$102362[ebp]
	sub	eax, DWORD PTR [edx+80]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nY$102362[ebp], eax

; 424  : 			nIdx = nY * LIGHTING_GRID_WIDTH + nX;

	mov	eax, DWORD PTR _nY$102362[ebp]
	imul	eax, 48					; 00000030H
	add	eax, DWORD PTR _nX$102361[ebp]
	mov	DWORD PTR _nIdx$102363[ebp], eax

; 425  : 			pTree->pObstacle[nIdx].nObstacle = nObstacleID;

	mov	ecx, DWORD PTR _nIdx$102363[ebp]
	shl	ecx, 4
	mov	edx, DWORD PTR _pTree$[ebp]
	mov	eax, DWORD PTR _?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA
	mov	DWORD PTR [edx+ecx+96], eax

; 426  : 			// 将方向的x分量设为0，表示不考虑方向
; 427  : 			pTree->pObstacle[nIdx].vDir.fX = 0.0f;

	mov	ecx, DWORD PTR _nIdx$102363[ebp]
	shl	ecx, 4
	mov	edx, DWORD PTR _pTree$[ebp]
	mov	DWORD PTR [edx+ecx+100], 0
$L102366:
$L102345:

; 430  : }

	mov	esp, ebp
	pop	ebp
	ret	0
?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ENDP	; KIpoTree::ObjectsCallback
_TEXT	ENDS
PUBLIC	__real@4@40048000000000000000
EXTRN	_fabs:NEAR
;	COMDAT __real@4@40048000000000000000
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@4@40048000000000000000 DD 042000000r	; 32
CONST	ENDS
_TEXT	SEGMENT
_fFlope1$ = -56
_fFlope2$ = -60
_nx$102411 = -88
_ny$102413 = -92
_nx$102432 = -96
_ny$102434 = -100
_x1$ = 8
_y1$ = 12
_x2$ = 16
_y2$ = 20
_nObstacleID$ = 24
_this$ = -104
_fx1$ = -40
_fy1$ = -68
_fx2$ = -52
_fy2$ = -84
_fdx$ = -8
_fdy$ = -20
_fStepX$ = -4
_fStepY$ = -16
_gStepX$ = -12
_gStepY$ = -28
_gx$ = -36
_gy$ = -48
_nIdx$ = -24
_gx1$ = -32
_gy1$ = -64
_gx2$ = -44
_gy2$ = -80
_vDir$ = -76
?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z PROC NEAR	; KIpoTree::FillLineObstacle

; 433  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 136				; 00000088H
	mov	DWORD PTR _this$[ebp], ecx

; 434  : 	if(x1 >= x2)

	mov	eax, DWORD PTR _x1$[ebp]
	cmp	eax, DWORD PTR _x2$[ebp]
	jl	SHORT $L102375

; 435  : 		return;

	jmp	$L102374
$L102375:

; 438  : 	int gStepX, gStepY, gx, gy, nIdx, gx1, gy1, gx2, gy2;
; 439  : 	fx1 = (float)x1, fy1 = (float)y1, fx2 = (float)x2, fy2 = (float)y2;

	fild	DWORD PTR _x1$[ebp]
	fstp	DWORD PTR _fx1$[ebp]
	fild	DWORD PTR _y1$[ebp]
	fstp	DWORD PTR _fy1$[ebp]
	fild	DWORD PTR _x2$[ebp]
	fstp	DWORD PTR _fx2$[ebp]
	fild	DWORD PTR _y2$[ebp]
	fstp	DWORD PTR _fy2$[ebp]

; 440  : 	fdx = (float)fabs(fx2 - fx1);

	fld	DWORD PTR _fx2$[ebp]
	fsub	DWORD PTR _fx1$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	add	esp, 8
	fstp	DWORD PTR _fdx$[ebp]

; 441  : 	fdy = (float)fabs(fy2 - fy1);

	fld	DWORD PTR _fy2$[ebp]
	fsub	DWORD PTR _fy1$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	add	esp, 8
	fstp	DWORD PTR _fdy$[ebp]

; 442  : 	KVector2 vDir;
; 443  : 	vDir.fX = fdx;

	mov	ecx, DWORD PTR _fdx$[ebp]
	mov	DWORD PTR _vDir$[ebp], ecx

; 444  : 	vDir.fY = fdy;

	mov	edx, DWORD PTR _fdy$[ebp]
	mov	DWORD PTR _vDir$[ebp+4], edx

; 445  : 	float fFlope1, fFlope2;
; 446  : 	gx1 = x1 / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _x1$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gx1$[ebp], eax

; 447  : 	gy1 = y1 / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _y1$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gy1$[ebp], eax

; 448  : 	gx2 = x2 / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _x2$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gx2$[ebp], eax

; 449  : 	gy2 = y2 / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _y2$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gy2$[ebp], eax

; 450  : 
; 451  : 	fFlope1 = fdy / fdx;

	fld	DWORD PTR _fdy$[ebp]
	fdiv	DWORD PTR _fdx$[ebp]
	fstp	DWORD PTR _fFlope1$[ebp]

; 452  : 
; 453  : 	// 斜率小于45度的情况
; 454  : 	if(fdx >= fdy)

	fld	DWORD PTR _fdx$[ebp]
	fcomp	DWORD PTR _fdy$[ebp]
	fnstsw	ax
	test	ah, 1
	jne	$L102402

; 456  : 		if(gx1 == gx2)

	mov	eax, DWORD PTR _gx1$[ebp]
	cmp	eax, DWORD PTR _gx2$[ebp]
	jne	SHORT $L102403

; 457  : 			return;

	jmp	$L102374
$L102403:

; 458  : 
; 459  : 		if(x2 > x1)

	mov	ecx, DWORD PTR _x2$[ebp]
	cmp	ecx, DWORD PTR _x1$[ebp]
	jle	SHORT $L102404

; 461  : 			gStepX = 1;

	mov	DWORD PTR _gStepX$[ebp], 1

; 462  : 			fStepX = LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepX$[ebp], 1107296256	; 42000000H

; 464  : 		else

	jmp	SHORT $L102405
$L102404:

; 466  : 			gStepX = -1;

	mov	DWORD PTR _gStepX$[ebp], -1

; 467  : 			fStepX = -LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepX$[ebp], -1040187392	; c2000000H
$L102405:

; 469  : 		if(y2 > y1)

	mov	edx, DWORD PTR _y2$[ebp]
	cmp	edx, DWORD PTR _y1$[ebp]
	jle	SHORT $L102406

; 471  : 			gStepY = 1;

	mov	DWORD PTR _gStepY$[ebp], 1

; 472  : 			fStepY = fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdy$[ebp]
	fdiv	DWORD PTR _fdx$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepY$[ebp]

; 474  : 		else

	jmp	SHORT $L102407
$L102406:

; 476  : 			gStepY = -1;

	mov	DWORD PTR _gStepY$[ebp], -1

; 477  : 			fStepY = -fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdy$[ebp]
	fchs
	fdiv	DWORD PTR _fdx$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepY$[ebp]
$L102407:

; 479  : 		while(1)

	mov	eax, 1
	test	eax, eax
	je	$L102410

; 481  : 			int nx = (int)fx1;

	fld	DWORD PTR _fx1$[ebp]
	call	__ftol
	mov	DWORD PTR _nx$102411[ebp], eax

; 482  : 			int ny = (int)fy1;

	fld	DWORD PTR _fy1$[ebp]
	call	__ftol
	mov	DWORD PTR _ny$102413[ebp], eax

; 483  : 
; 484  : 			gx = nx / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nx$102411[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gx$[ebp], eax

; 485  : 			gy = ny / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _ny$102413[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gy$[ebp], eax

; 486  : 
; 487  : 			if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	cmp	DWORD PTR _gx$[ebp], 0
	jle	$L102418
	cmp	DWORD PTR _gx$[ebp], 47			; 0000002fH
	jge	$L102418
	cmp	DWORD PTR _gy$[ebp], 0
	jle	$L102418
	cmp	DWORD PTR _gy$[ebp], 95			; 0000005fH
	jge	$L102418

; 489  : 				nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	mov	ecx, DWORD PTR _gy$[ebp]
	imul	ecx, 48					; 00000030H
	add	ecx, DWORD PTR _gx$[ebp]
	mov	DWORD PTR _nIdx$[ebp], ecx

; 490  : 				pObstacle[nIdx].nObstacle = nObstacleID;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [eax+edx+96], ecx

; 491  : 				pObstacle[nIdx].vDir = vDir;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [eax+edx+100], ecx
	mov	ecx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [eax+edx+104], ecx

; 492  : 
; 493  : 				fFlope2 = (float)(fabs((gy + gStepY) * 32.0f - fy1) / fabs((gx + gStepX) * 32.0f - fx1));

	mov	edx, DWORD PTR _gy$[ebp]
	add	edx, DWORD PTR _gStepY$[ebp]
	mov	DWORD PTR -108+[ebp], edx
	fild	DWORD PTR -108+[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	DWORD PTR _fy1$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	fstp	QWORD PTR -116+[ebp]
	add	esp, 8
	mov	eax, DWORD PTR _gx$[ebp]
	add	eax, DWORD PTR _gStepX$[ebp]
	mov	DWORD PTR -120+[ebp], eax
	fild	DWORD PTR -120+[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	DWORD PTR _fx1$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	add	esp, 8
	fdivr	QWORD PTR -116+[ebp]
	fstp	DWORD PTR _fFlope2$[ebp]

; 494  : 				if(fFlope2 > fFlope1)

	fld	DWORD PTR _fFlope2$[ebp]
	fcomp	DWORD PTR _fFlope1$[ebp]
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L102417

; 496  : 					nIdx = (gy + gStepY) * LIGHTING_GRID_WIDTH + gx;

	mov	ecx, DWORD PTR _gy$[ebp]
	add	ecx, DWORD PTR _gStepY$[ebp]
	imul	ecx, 48					; 00000030H
	add	ecx, DWORD PTR _gx$[ebp]
	mov	DWORD PTR _nIdx$[ebp], ecx

; 497  : 					pObstacle[nIdx].nObstacle = nObstacleID;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [eax+edx+96], ecx

; 498  : 					pObstacle[nIdx].vDir = vDir;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [eax+edx+100], ecx
	mov	ecx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [eax+edx+104], ecx

; 500  : 				else

	jmp	SHORT $L102418
$L102417:

; 502  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx + gStepX;

	mov	edx, DWORD PTR _gy$[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _gx$[ebp]
	add	edx, DWORD PTR _gStepX$[ebp]
	mov	DWORD PTR _nIdx$[ebp], edx

; 503  : 					pObstacle[nIdx].nObstacle = nObstacleID;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [ecx+eax+96], edx

; 504  : 					pObstacle[nIdx].vDir = vDir;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [ecx+eax+100], edx
	mov	edx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [ecx+eax+104], edx
$L102418:

; 507  : 
; 508  : 			fx1 += fStepX;

	fld	DWORD PTR _fx1$[ebp]
	fadd	DWORD PTR _fStepX$[ebp]
	fstp	DWORD PTR _fx1$[ebp]

; 509  : 			fy1 += fStepY;

	fld	DWORD PTR _fy1$[ebp]
	fadd	DWORD PTR _fStepY$[ebp]
	fstp	DWORD PTR _fy1$[ebp]

; 510  : 			gx1 += gStepX;

	mov	eax, DWORD PTR _gx1$[ebp]
	add	eax, DWORD PTR _gStepX$[ebp]
	mov	DWORD PTR _gx1$[ebp], eax

; 511  : 			if(gx1 == gx2)

	mov	ecx, DWORD PTR _gx1$[ebp]
	cmp	ecx, DWORD PTR _gx2$[ebp]
	jne	SHORT $L102419

; 513  : 				gx = ((int)fx1) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fx1$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gx$[ebp], eax

; 514  : 				gy = ((int)fy1) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fy1$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gy$[ebp], eax

; 515  : 				if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	cmp	DWORD PTR _gx$[ebp], 0
	jle	SHORT $L102422
	cmp	DWORD PTR _gx$[ebp], 47			; 0000002fH
	jge	SHORT $L102422
	cmp	DWORD PTR _gy$[ebp], 0
	jle	SHORT $L102422
	cmp	DWORD PTR _gy$[ebp], 95			; 0000005fH
	jge	SHORT $L102422

; 517  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	mov	edx, DWORD PTR _gy$[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _gx$[ebp]
	mov	DWORD PTR _nIdx$[ebp], edx

; 518  : 					pObstacle[nIdx].nObstacle = nObstacleID;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [ecx+eax+96], edx

; 519  : 					pObstacle[nIdx].vDir = vDir;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [ecx+eax+100], edx
	mov	edx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [ecx+eax+104], edx
$L102422:

; 521  : 				return;

	jmp	$L102374
$L102419:

; 523  : 		}

	jmp	$L102407
$L102410:

; 525  : 	// 斜率大于45度的情况
; 526  : 	else

	jmp	$L102431
$L102402:

; 528  : 		if(gy1 == gy2)

	mov	eax, DWORD PTR _gy1$[ebp]
	cmp	eax, DWORD PTR _gy2$[ebp]
	jne	SHORT $L102424

; 529  : 			return;

	jmp	$L102374
$L102424:

; 530  : 
; 531  : 		if(y2 > y1)

	mov	ecx, DWORD PTR _y2$[ebp]
	cmp	ecx, DWORD PTR _y1$[ebp]
	jle	SHORT $L102425

; 533  : 			gStepY = 1;

	mov	DWORD PTR _gStepY$[ebp], 1

; 534  : 			fStepY = LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepY$[ebp], 1107296256	; 42000000H

; 536  : 		else

	jmp	SHORT $L102426
$L102425:

; 538  : 			gStepY = -1;

	mov	DWORD PTR _gStepY$[ebp], -1

; 539  : 			fStepY = -LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepY$[ebp], -1040187392	; c2000000H
$L102426:

; 541  : 		if(x2 > x1)

	mov	edx, DWORD PTR _x2$[ebp]
	cmp	edx, DWORD PTR _x1$[ebp]
	jle	SHORT $L102427

; 543  : 			gStepX = 1;

	mov	DWORD PTR _gStepX$[ebp], 1

; 544  : 			fStepX = fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[ebp]
	fdiv	DWORD PTR _fdy$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepX$[ebp]

; 546  : 		else

	jmp	SHORT $L102428
$L102427:

; 548  : 			gStepX = -1;

	mov	DWORD PTR _gStepX$[ebp], -1

; 549  : 			fStepX = -fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[ebp]
	fchs
	fdiv	DWORD PTR _fdy$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepX$[ebp]
$L102428:

; 551  : 		while(1)

	mov	eax, 1
	test	eax, eax
	je	$L102431

; 553  : 			int nx = (int)fx1;

	fld	DWORD PTR _fx1$[ebp]
	call	__ftol
	mov	DWORD PTR _nx$102432[ebp], eax

; 554  : 			int ny = (int)fy1;

	fld	DWORD PTR _fy1$[ebp]
	call	__ftol
	mov	DWORD PTR _ny$102434[ebp], eax

; 555  : 
; 556  : 			gx = nx / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nx$102432[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gx$[ebp], eax

; 557  : 			gy = ny / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _ny$102434[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gy$[ebp], eax

; 558  : 
; 559  : 			if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	cmp	DWORD PTR _gx$[ebp], 0
	jle	$L102439
	cmp	DWORD PTR _gx$[ebp], 47			; 0000002fH
	jge	$L102439
	cmp	DWORD PTR _gy$[ebp], 0
	jle	$L102439
	cmp	DWORD PTR _gy$[ebp], 95			; 0000005fH
	jge	$L102439

; 561  : 				nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	mov	ecx, DWORD PTR _gy$[ebp]
	imul	ecx, 48					; 00000030H
	add	ecx, DWORD PTR _gx$[ebp]
	mov	DWORD PTR _nIdx$[ebp], ecx

; 562  : 				pObstacle[nIdx].nObstacle = nObstacleID;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [eax+edx+96], ecx

; 563  : 				pObstacle[nIdx].vDir = vDir;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [eax+edx+100], ecx
	mov	ecx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [eax+edx+104], ecx

; 564  : 
; 565  : 				fFlope2 = (float)(fabs((gy + gStepY) * 32.0f - fy1) / fabs((gx + gStepX) * 32.0f - fx1));

	mov	edx, DWORD PTR _gy$[ebp]
	add	edx, DWORD PTR _gStepY$[ebp]
	mov	DWORD PTR -124+[ebp], edx
	fild	DWORD PTR -124+[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	DWORD PTR _fy1$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	fstp	QWORD PTR -132+[ebp]
	add	esp, 8
	mov	eax, DWORD PTR _gx$[ebp]
	add	eax, DWORD PTR _gStepX$[ebp]
	mov	DWORD PTR -136+[ebp], eax
	fild	DWORD PTR -136+[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	DWORD PTR _fx1$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	add	esp, 8
	fdivr	QWORD PTR -132+[ebp]
	fstp	DWORD PTR _fFlope2$[ebp]

; 566  : 				if(fFlope2 > fFlope1)

	fld	DWORD PTR _fFlope2$[ebp]
	fcomp	DWORD PTR _fFlope1$[ebp]
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L102438

; 568  : 					nIdx = (gy + gStepY) * LIGHTING_GRID_WIDTH + gx;

	mov	ecx, DWORD PTR _gy$[ebp]
	add	ecx, DWORD PTR _gStepY$[ebp]
	imul	ecx, 48					; 00000030H
	add	ecx, DWORD PTR _gx$[ebp]
	mov	DWORD PTR _nIdx$[ebp], ecx

; 569  : 					pObstacle[nIdx].nObstacle = nObstacleID;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [eax+edx+96], ecx

; 570  : 					pObstacle[nIdx].vDir = vDir;

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [eax+edx+100], ecx
	mov	ecx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [eax+edx+104], ecx

; 572  : 				else

	jmp	SHORT $L102439
$L102438:

; 574  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx + gStepX;

	mov	edx, DWORD PTR _gy$[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _gx$[ebp]
	add	edx, DWORD PTR _gStepX$[ebp]
	mov	DWORD PTR _nIdx$[ebp], edx

; 575  : 					pObstacle[nIdx].nObstacle = nObstacleID;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [ecx+eax+96], edx

; 576  : 					pObstacle[nIdx].vDir = vDir;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [ecx+eax+100], edx
	mov	edx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [ecx+eax+104], edx
$L102439:

; 579  : 
; 580  : 			fx1 += fStepX;

	fld	DWORD PTR _fx1$[ebp]
	fadd	DWORD PTR _fStepX$[ebp]
	fstp	DWORD PTR _fx1$[ebp]

; 581  : 			fy1 += fStepY;

	fld	DWORD PTR _fy1$[ebp]
	fadd	DWORD PTR _fStepY$[ebp]
	fstp	DWORD PTR _fy1$[ebp]

; 582  : 			gy1 += gStepY;

	mov	eax, DWORD PTR _gy1$[ebp]
	add	eax, DWORD PTR _gStepY$[ebp]
	mov	DWORD PTR _gy1$[ebp], eax

; 583  : 			if(gy1 == gy2)

	mov	ecx, DWORD PTR _gy1$[ebp]
	cmp	ecx, DWORD PTR _gy2$[ebp]
	jne	SHORT $L102440

; 585  : 				gx = ((int)fx1) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fx1$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gx$[ebp], eax

; 586  : 				gy = ((int)fy1) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fy1$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _gy$[ebp], eax

; 587  : 				if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	cmp	DWORD PTR _gx$[ebp], 0
	jle	SHORT $L102443
	cmp	DWORD PTR _gx$[ebp], 47			; 0000002fH
	jge	SHORT $L102443
	cmp	DWORD PTR _gy$[ebp], 0
	jle	SHORT $L102443
	cmp	DWORD PTR _gy$[ebp], 95			; 0000005fH
	jge	SHORT $L102443

; 589  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	mov	edx, DWORD PTR _gy$[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _gx$[ebp]
	mov	DWORD PTR _nIdx$[ebp], edx

; 590  : 					pObstacle[nIdx].nObstacle = nObstacleID;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _nObstacleID$[ebp]
	mov	DWORD PTR [ecx+eax+96], edx

; 591  : 					pObstacle[nIdx].vDir = vDir;

	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR _vDir$[ebp]
	mov	DWORD PTR [ecx+eax+100], edx
	mov	edx, DWORD PTR _vDir$[ebp+4]
	mov	DWORD PTR [ecx+eax+104], edx
$L102443:

; 593  : 				return;

	jmp	SHORT $L102374
$L102440:

; 595  : 		}

	jmp	$L102428
$L102431:
$L102374:

; 597  : }

	mov	esp, ebp
	pop	ebp
	ret	20					; 00000014H
?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z ENDP		; KIpoTree::FillLineObstacle
_TEXT	ENDS
PUBLIC	?size@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEIXZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::size
PUBLIC	__real@8@3fff8000000000000000
PUBLIC	__real@8@400f8000000000000000
PUBLIC	?CanLighting@KIpoTree@@AAE_NHHHH@Z		; KIpoTree::CanLighting
EXTRN	_sqrt:NEAR
;	COMDAT __real@8@3fff8000000000000000
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@8@3fff8000000000000000 DQ 03ff0000000000000r ; 1
CONST	ENDS
;	COMDAT __real@8@400f8000000000000000
CONST	SEGMENT
__real@8@400f8000000000000000 DQ 040f0000000000000r ; 65536
CONST	ENDS
_TEXT	SEGMENT
_this$ = -180
_nn$ = -60
_j$ = -44
_nLightID$ = -16
_dwR$ = -28
_dwG$ = -52
_dwB$ = -24
_pTempColor$ = -4
_i$ = -40
_pLight$102464 = -100
_nLightPosX$102465 = -104
_nLightPosY$102466 = -108
_nLightGridX$102467 = -96
_nLightGridY$102468 = -92
_nLightGridCenterX$102471 = -144
_nLightGridCenterY$102472 = -124
_ur$102473 = -152
_ug$102474 = -88
_ub$102475 = -148
_j$102476 = -136
_x$102477 = -120
_y$102478 = -128
_gx$102479 = -132
_gy$102480 = -140
_nGridRadius$102481 = -116
_fLightRadiusDenom$102482 = -76
_uDistance65536$102484 = -80
_pCurPos$102485 = -112
_r$102486 = -84
_f$102495 = -156
_f$102507 = -160
_f$102518 = -164
_f$102529 = -168
_m$ = -64
_n$ = -72
_pdwLight$ = -68
_pO$ = -20
_pL$ = -12
_pR$ = -36
_pU$ = -56
_pD$ = -48
_pDest$ = -32
_nIdx$ = -8
$T103226 = -172
$T103227 = -176
?RenderLightMap@KIpoTree@@QAEXXZ PROC NEAR		; KIpoTree::RenderLightMap

; 934  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 224				; 000000e0H
	mov	DWORD PTR _this$[ebp], ecx

; 935  : 	int nn = m_LightList.size();

	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?size@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEIXZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::size
	mov	DWORD PTR _nn$[ebp], eax

; 936  : 	int j, nLightID=1;

	mov	DWORD PTR _nLightID$[ebp], 1

; 937  : 
; 938  : 	m_dwAmbient = 0xff101010;

	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+92], -15724528		; ff101010H

; 939  : 
; 940  : 	// 按环境光清空光照图
; 941  : 	DWORD dwR, dwG, dwB;
; 942  : 	dwR = (m_dwAmbient & 0x00ff0000) >>16;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+92]
	and	edx, 16711680				; 00ff0000H
	shr	edx, 16					; 00000010H
	mov	DWORD PTR _dwR$[ebp], edx

; 943  : 	dwG = (m_dwAmbient & 0x0000ff00) >>8;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+92]
	and	ecx, 65280				; 0000ff00H
	shr	ecx, 8
	mov	DWORD PTR _dwG$[ebp], ecx

; 944  : 	dwB = m_dwAmbient & 0x000000ff;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+92]
	and	eax, 255				; 000000ffH
	mov	DWORD PTR _dwB$[ebp], eax

; 945  :     KLColor *pTempColor = pLColor;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73824]
	mov	DWORD PTR _pTempColor$[ebp], edx

; 946  :     
; 947  :     // 如果每一个颜色分量是2个字节,可以考虑多一个项,然后用一个MMX的寄存器存放
; 948  :     // 正好8个字节
; 949  : 	for(j=0; j < LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT / 4; j++)

	mov	DWORD PTR _j$[ebp], 0
	jmp	SHORT $L102454
$L102455:
	mov	eax, DWORD PTR _j$[ebp]
	add	eax, 1
	mov	DWORD PTR _j$[ebp], eax
$L102454:
	cmp	DWORD PTR _j$[ebp], 1152		; 00000480H
	jge	$L102456

; 951  : 			pTempColor[0].r = dwR;

	mov	ecx, DWORD PTR _pTempColor$[ebp]
	mov	dx, WORD PTR _dwR$[ebp]
	mov	WORD PTR [ecx], dx

; 952  : 			pTempColor[1].r = dwR;

	mov	eax, DWORD PTR _pTempColor$[ebp]
	mov	cx, WORD PTR _dwR$[ebp]
	mov	WORD PTR [eax+8], cx

; 953  : 			pTempColor[2].r = dwR;

	mov	edx, DWORD PTR _pTempColor$[ebp]
	mov	ax, WORD PTR _dwR$[ebp]
	mov	WORD PTR [edx+16], ax

; 954  : 			pTempColor[3].r = dwR;

	mov	ecx, DWORD PTR _pTempColor$[ebp]
	mov	dx, WORD PTR _dwR$[ebp]
	mov	WORD PTR [ecx+24], dx

; 955  : 
; 956  : 			pTempColor[0].g = dwG;

	mov	eax, DWORD PTR _pTempColor$[ebp]
	mov	cx, WORD PTR _dwG$[ebp]
	mov	WORD PTR [eax+2], cx

; 957  : 			pTempColor[1].g = dwG;

	mov	edx, DWORD PTR _pTempColor$[ebp]
	mov	ax, WORD PTR _dwG$[ebp]
	mov	WORD PTR [edx+10], ax

; 958  : 			pTempColor[2].g = dwG;

	mov	ecx, DWORD PTR _pTempColor$[ebp]
	mov	dx, WORD PTR _dwG$[ebp]
	mov	WORD PTR [ecx+18], dx

; 959  : 			pTempColor[3].g = dwG;

	mov	eax, DWORD PTR _pTempColor$[ebp]
	mov	cx, WORD PTR _dwG$[ebp]
	mov	WORD PTR [eax+26], cx

; 960  : 
; 961  : 
; 962  : 			pTempColor[0].b = dwB;

	mov	edx, DWORD PTR _pTempColor$[ebp]
	mov	ax, WORD PTR _dwB$[ebp]
	mov	WORD PTR [edx+4], ax

; 963  : 			pTempColor[1].b = dwB;

	mov	ecx, DWORD PTR _pTempColor$[ebp]
	mov	dx, WORD PTR _dwB$[ebp]
	mov	WORD PTR [ecx+12], dx

; 964  : 			pTempColor[2].b = dwB;

	mov	eax, DWORD PTR _pTempColor$[ebp]
	mov	cx, WORD PTR _dwB$[ebp]
	mov	WORD PTR [eax+20], cx

; 965  : 			pTempColor[3].b = dwB;

	mov	edx, DWORD PTR _pTempColor$[ebp]
	mov	ax, WORD PTR _dwB$[ebp]
	mov	WORD PTR [edx+28], ax

; 966  :             
; 967  :             pTempColor += 4;

	mov	ecx, DWORD PTR _pTempColor$[ebp]
	add	ecx, 32					; 00000020H
	mov	DWORD PTR _pTempColor$[ebp], ecx

; 968  : 	}

	jmp	$L102455
$L102456:

; 969  : 
; 970  : 	list<KLightBase*>::iterator i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator

; 971  : 	// 计算每一个光源的光照结果
; 972  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	lea	edx, DWORD PTR $T103226[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	eax, DWORD PTR [eax]
	mov	DWORD PTR _i$[ebp], eax
	jmp	SHORT $L102461
$L102462:
	lea	ecx, DWORD PTR _i$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
$L102461:
	lea	ecx, DWORD PTR $T103227[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	push	eax
	lea	ecx, DWORD PTR _i$[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	$L102463

; 974  : 		nLightID++;

	mov	edx, DWORD PTR _nLightID$[ebp]
	add	edx, 1
	mov	DWORD PTR _nLightID$[ebp], edx

; 975  : 		KLightBase *pLight = *i;

	lea	ecx, DWORD PTR _i$[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	eax, DWORD PTR [eax]
	mov	DWORD PTR _pLight$102464[ebp], eax

; 976  : 		// 光源相对于九区域左上角的坐标
; 977  : 		int nLightPosX = pLight->m_oPosition.nX - m_nLeftTopX;

	mov	ecx, DWORD PTR _pLight$102464[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+8]
	sub	eax, DWORD PTR [edx+76]
	mov	DWORD PTR _nLightPosX$102465[ebp], eax

; 978  : 		int nLightPosY = pLight->m_oPosition.nY - m_nLeftTopY;

	mov	ecx, DWORD PTR _pLight$102464[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [ecx+12]
	sub	eax, DWORD PTR [edx+80]
	mov	DWORD PTR _nLightPosY$102466[ebp], eax

; 979  : 		// 光源的格子坐标
; 980  : 		int nLightGridX = nLightPosX / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nLightPosX$102465[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nLightGridX$102467[ebp], eax

; 981  : 		int nLightGridY = nLightPosY / LIGHTING_GRID_SIZEY;

	mov	eax, DWORD PTR _nLightPosY$102466[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nLightGridY$102468[ebp], eax

; 983  : 			nLightGridY < 0 || nLightGridY >= LIGHTING_GRID_HEIGHT)

	cmp	DWORD PTR _nLightGridX$102467[ebp], 0
	jl	SHORT $L102470
	cmp	DWORD PTR _nLightGridX$102467[ebp], 48	; 00000030H
	jge	SHORT $L102470
	cmp	DWORD PTR _nLightGridY$102468[ebp], 0
	jl	SHORT $L102470
	cmp	DWORD PTR _nLightGridY$102468[ebp], 96	; 00000060H
	jl	SHORT $L102469
$L102470:

; 984  : 			continue;

	jmp	$L102462
$L102469:

; 985  : 
; 986  : 		// 光源所在格子中心的坐标
; 987  : 		int nLightGridCenterX = nLightGridX * LIGHTING_GRID_SIZEX + LIGHTING_GRID_SIZEX / 2;

	mov	ecx, DWORD PTR _nLightGridX$102467[ebp]
	shl	ecx, 5
	add	ecx, 16					; 00000010H
	mov	DWORD PTR _nLightGridCenterX$102471[ebp], ecx

; 988  : 		int nLightGridCenterY = nLightGridY * LIGHTING_GRID_SIZEY + LIGHTING_GRID_SIZEY / 2;

	mov	edx, DWORD PTR _nLightGridY$102468[ebp]
	shl	edx, 5
	add	edx, 16					; 00000010H
	mov	DWORD PTR _nLightGridCenterY$102472[ebp], edx

; 989  : 
; 990  : 		// 获取光源的r，g，b分量
; 991  : 		unsigned int ur, ug, ub;
; 992  : 
; 993  : 		ur = (pLight->m_dwColor & 0x00ff0000) >> 18;

	mov	eax, DWORD PTR _pLight$102464[ebp]
	mov	ecx, DWORD PTR [eax+20]
	and	ecx, 16711680				; 00ff0000H
	shr	ecx, 18					; 00000012H
	mov	DWORD PTR _ur$102473[ebp], ecx

; 994  : 		ug = (pLight->m_dwColor & 0x0000ff00) >> 10;

	mov	edx, DWORD PTR _pLight$102464[ebp]
	mov	eax, DWORD PTR [edx+20]
	and	eax, 65280				; 0000ff00H
	shr	eax, 10					; 0000000aH
	mov	DWORD PTR _ug$102474[ebp], eax

; 995  : 		ub = (pLight->m_dwColor & 0x000000ff) >> 2;

	mov	ecx, DWORD PTR _pLight$102464[ebp]
	mov	edx, DWORD PTR [ecx+20]
	and	edx, 255				; 000000ffH
	shr	edx, 2
	mov	DWORD PTR _ub$102475[ebp], edx

; 996  : 
; 997  : 		//float fr = (float)pLight->m_nRadius * (float)pLight->m_nRadius;
; 998  : 
; 999  : 		int j, x, y, gx, gy;
; 1000 : 		int nGridRadius = pLight->m_nRadius / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _pLight$102464[ebp]
	mov	eax, DWORD PTR [eax+24]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nGridRadius$102481[ebp], eax

; 1001 :         float fLightRadiusDenom =  1.0 / ((float)pLight->m_nRadius);

	mov	ecx, DWORD PTR _pLight$102464[ebp]
	fild	DWORD PTR [ecx+24]
	fdivr	QWORD PTR __real@8@3fff8000000000000000
	fstp	DWORD PTR _fLightRadiusDenom$102482[ebp]

; 1002 :         //fLightRadiusDenom *= fLightRadiusDenom;
; 1003 :         unsigned int uDistance65536 = 0;

	mov	DWORD PTR _uDistance65536$102484[ebp], 0

; 1004 :         KLColor *pCurPos = NULL;

	mov	DWORD PTR _pCurPos$102485[ebp], 0

; 1005 : 
; 1006 : 		
; 1007 : 		gy = nLightGridY - (nGridRadius - 1);

	mov	edx, DWORD PTR _nGridRadius$102481[ebp]
	sub	edx, 1
	mov	eax, DWORD PTR _nLightGridY$102468[ebp]
	sub	eax, edx
	mov	DWORD PTR _gy$102480[ebp], eax

; 1008 : 		// 从中心点向外扩展计算光照强度
; 1009 : 		for(int r = 0; r < nGridRadius; r++)

	mov	DWORD PTR _r$102486[ebp], 0
	jmp	SHORT $L102487
$L102488:
	mov	ecx, DWORD PTR _r$102486[ebp]
	add	ecx, 1
	mov	DWORD PTR _r$102486[ebp], ecx
$L102487:
	mov	edx, DWORD PTR _r$102486[ebp]
	cmp	edx, DWORD PTR _nGridRadius$102481[ebp]
	jge	$L102489

; 1011 : 			gx = nLightGridX - r;

	mov	eax, DWORD PTR _nLightGridX$102467[ebp]
	sub	eax, DWORD PTR _r$102486[ebp]
	mov	DWORD PTR _gx$102479[ebp], eax

; 1012 : 			gy = nLightGridY - r;

	mov	ecx, DWORD PTR _nLightGridY$102468[ebp]
	sub	ecx, DWORD PTR _r$102486[ebp]
	mov	DWORD PTR _gy$102480[ebp], ecx

; 1014 : 				((gx < 0) || (gy < 0))
; 1015 : 			)

	cmp	DWORD PTR _gx$102479[ebp], 0
	jl	$L102493
	cmp	DWORD PTR _gy$102480[ebp], 0
	jl	$L102493

; 1017 : 				x = nLightGridCenterX - r * LIGHTING_GRID_SIZEX;

	mov	edx, DWORD PTR _r$102486[ebp]
	shl	edx, 5
	mov	eax, DWORD PTR _nLightGridCenterX$102471[ebp]
	sub	eax, edx
	mov	DWORD PTR _x$102477[ebp], eax

; 1018 : 				y = nLightGridCenterY - r * LIGHTING_GRID_SIZEY;

	mov	ecx, DWORD PTR _r$102486[ebp]
	shl	ecx, 5
	mov	edx, DWORD PTR _nLightGridCenterY$102472[ebp]
	sub	edx, ecx
	mov	DWORD PTR _y$102478[ebp], edx

; 1019 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	eax, DWORD PTR _gy$102480[ebp]
	imul	eax, 48					; 00000030H
	add	eax, DWORD PTR _gx$102479[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73824]
	lea	eax, DWORD PTR [edx+eax*8]
	mov	DWORD PTR _pCurPos$102485[ebp], eax

; 1020 : 				for(j=0; j<(r * 2 + 1); j++)

	mov	DWORD PTR _j$102476[ebp], 0
	jmp	SHORT $L102491
$L102492:
	mov	ecx, DWORD PTR _j$102476[ebp]
	add	ecx, 1
	mov	DWORD PTR _j$102476[ebp], ecx
$L102491:
	mov	edx, DWORD PTR _r$102486[ebp]
	lea	eax, DWORD PTR [edx+edx+1]
	cmp	DWORD PTR _j$102476[ebp], eax
	jge	$L102493

; 1022 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	ecx, DWORD PTR _nLightPosY$102466[ebp]
	push	ecx
	mov	edx, DWORD PTR _nLightPosX$102465[ebp]
	push	edx
	mov	eax, DWORD PTR _y$102478[ebp]
	push	eax
	mov	ecx, DWORD PTR _x$102477[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	$L102499

; 1024 : 						//float f = 65536.0 - ((float)(
; 1025 : 						//    (
; 1026 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1027 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1028 : 						//    ) * 
; 1029 : 						//    fLightRadiusDenom *
; 1030 : 						//    65536.0
; 1031 : 						//));
; 1032 : 						float f = 65536.0 - ((float)(
; 1033 : 							sqrt(
; 1034 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1035 : 								(y - nLightPosY) * (y - nLightPosY)
; 1036 : 							) * 
; 1037 : 							fLightRadiusDenom * 
; 1038 : 							65536.0
; 1039 : 						));

	mov	edx, DWORD PTR _x$102477[ebp]
	sub	edx, DWORD PTR _nLightPosX$102465[ebp]
	mov	eax, DWORD PTR _x$102477[ebp]
	sub	eax, DWORD PTR _nLightPosX$102465[ebp]
	imul	edx, eax
	mov	ecx, DWORD PTR _y$102478[ebp]
	sub	ecx, DWORD PTR _nLightPosY$102466[ebp]
	mov	eax, DWORD PTR _y$102478[ebp]
	sub	eax, DWORD PTR _nLightPosY$102466[ebp]
	imul	ecx, eax
	add	edx, ecx
	mov	DWORD PTR -184+[ebp], edx
	fild	DWORD PTR -184+[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_sqrt
	add	esp, 8
	fmul	DWORD PTR _fLightRadiusDenom$102482[ebp]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR -188+[ebp]
	fsubr	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR _f$102495[ebp]

; 1040 : 
; 1041 : 						uDistance65536 = (int)(f);

	call	__ftol
	mov	DWORD PTR _uDistance65536$102484[ebp], eax

; 1042 : 
; 1043 : 						if (((int)uDistance65536) > 0)

	cmp	DWORD PTR _uDistance65536$102484[ebp], 0
	jle	SHORT $L102499

; 1045 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	ecx, DWORD PTR _ur$102473[ebp]
	imul	ecx, DWORD PTR _uDistance65536$102484[ebp]
	shr	ecx, 16					; 00000010H
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	ax, WORD PTR [edx]
	add	ax, cx
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [ecx], ax

; 1046 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	edx, DWORD PTR _ug$102474[ebp]
	imul	edx, DWORD PTR _uDistance65536$102484[ebp]
	shr	edx, 16					; 00000010H
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	cx, WORD PTR [eax+2]
	add	cx, dx
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [edx+2], cx

; 1047 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	mov	eax, DWORD PTR _ub$102475[ebp]
	imul	eax, DWORD PTR _uDistance65536$102484[ebp]
	shr	eax, 16					; 00000010H
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	dx, WORD PTR [ecx+4]
	add	dx, ax
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [eax+4], dx
$L102499:

; 1050 : 
; 1051 : 					gx++;

	mov	ecx, DWORD PTR _gx$102479[ebp]
	add	ecx, 1
	mov	DWORD PTR _gx$102479[ebp], ecx

; 1052 :                     if (gx >= LIGHTING_GRID_WIDTH)

	cmp	DWORD PTR _gx$102479[ebp], 48		; 00000030H
	jl	SHORT $L102500

; 1053 :                         break;

	jmp	SHORT $L102493
$L102500:

; 1054 : 
; 1055 : 					x += LIGHTING_GRID_SIZEX;

	mov	edx, DWORD PTR _x$102477[ebp]
	add	edx, 32					; 00000020H
	mov	DWORD PTR _x$102477[ebp], edx

; 1056 : 					pCurPos++;

	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	add	eax, 8
	mov	DWORD PTR _pCurPos$102485[ebp], eax

; 1057 : 				}

	jmp	$L102492
$L102493:

; 1059 : 
; 1060 : 			if(r == 0)

	cmp	DWORD PTR _r$102486[ebp], 0
	jne	SHORT $L102501

; 1061 : 				continue;

	jmp	$L102488
$L102501:

; 1062 : 
; 1063 : 			gx = nLightGridX - r;

	mov	ecx, DWORD PTR _nLightGridX$102467[ebp]
	sub	ecx, DWORD PTR _r$102486[ebp]
	mov	DWORD PTR _gx$102479[ebp], ecx

; 1064 : 			gy = nLightGridY + r;

	mov	edx, DWORD PTR _nLightGridY$102468[ebp]
	add	edx, DWORD PTR _r$102486[ebp]
	mov	DWORD PTR _gy$102480[ebp], edx

; 1066 : 					((gx < 0) || (gy >= LIGHTING_GRID_HEIGHT))
; 1067 : 			)

	cmp	DWORD PTR _gx$102479[ebp], 0
	jl	$L102505
	cmp	DWORD PTR _gy$102480[ebp], 96		; 00000060H
	jge	$L102505

; 1069 : 				x = nLightGridCenterX - r * LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _r$102486[ebp]
	shl	eax, 5
	mov	ecx, DWORD PTR _nLightGridCenterX$102471[ebp]
	sub	ecx, eax
	mov	DWORD PTR _x$102477[ebp], ecx

; 1070 : 				y = nLightGridCenterY + r * LIGHTING_GRID_SIZEY;

	mov	edx, DWORD PTR _r$102486[ebp]
	shl	edx, 5
	mov	eax, DWORD PTR _nLightGridCenterY$102472[ebp]
	add	eax, edx
	mov	DWORD PTR _y$102478[ebp], eax

; 1071 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	ecx, DWORD PTR _gy$102480[ebp]
	imul	ecx, 48					; 00000030H
	add	ecx, DWORD PTR _gx$102479[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+73824]
	lea	ecx, DWORD PTR [eax+ecx*8]
	mov	DWORD PTR _pCurPos$102485[ebp], ecx

; 1072 : 				for(j=0; j<(r * 2 + 1); j++)

	mov	DWORD PTR _j$102476[ebp], 0
	jmp	SHORT $L102503
$L102504:
	mov	edx, DWORD PTR _j$102476[ebp]
	add	edx, 1
	mov	DWORD PTR _j$102476[ebp], edx
$L102503:
	mov	eax, DWORD PTR _r$102486[ebp]
	lea	ecx, DWORD PTR [eax+eax+1]
	cmp	DWORD PTR _j$102476[ebp], ecx
	jge	$L102505

; 1074 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	edx, DWORD PTR _nLightPosY$102466[ebp]
	push	edx
	mov	eax, DWORD PTR _nLightPosX$102465[ebp]
	push	eax
	mov	ecx, DWORD PTR _y$102478[ebp]
	push	ecx
	mov	edx, DWORD PTR _x$102477[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	$L102511

; 1076 : 						//float f = 65536.0 - ((float)(
; 1077 : 						//    (
; 1078 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1079 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1080 : 						//    ) * 
; 1081 : 						//    fLightRadiusDenom *
; 1082 : 						//    65536.0
; 1083 : 						//));
; 1084 : 						float f = 65536.0 - ((float)(
; 1085 : 							sqrt(
; 1086 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1087 : 								(y - nLightPosY) * (y - nLightPosY)
; 1088 : 							) * 
; 1089 : 							fLightRadiusDenom * 
; 1090 : 							65536.0
; 1091 : 						));

	mov	eax, DWORD PTR _x$102477[ebp]
	sub	eax, DWORD PTR _nLightPosX$102465[ebp]
	mov	ecx, DWORD PTR _x$102477[ebp]
	sub	ecx, DWORD PTR _nLightPosX$102465[ebp]
	imul	eax, ecx
	mov	edx, DWORD PTR _y$102478[ebp]
	sub	edx, DWORD PTR _nLightPosY$102466[ebp]
	mov	ecx, DWORD PTR _y$102478[ebp]
	sub	ecx, DWORD PTR _nLightPosY$102466[ebp]
	imul	edx, ecx
	add	eax, edx
	mov	DWORD PTR -192+[ebp], eax
	fild	DWORD PTR -192+[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_sqrt
	add	esp, 8
	fmul	DWORD PTR _fLightRadiusDenom$102482[ebp]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR -196+[ebp]
	fsubr	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR _f$102507[ebp]

; 1092 : 
; 1093 : 						uDistance65536 = (int)(f);

	call	__ftol
	mov	DWORD PTR _uDistance65536$102484[ebp], eax

; 1094 : 
; 1095 : 						if (((int)uDistance65536) > 0)

	cmp	DWORD PTR _uDistance65536$102484[ebp], 0
	jle	SHORT $L102511

; 1097 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	edx, DWORD PTR _ur$102473[ebp]
	imul	edx, DWORD PTR _uDistance65536$102484[ebp]
	shr	edx, 16					; 00000010H
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	cx, WORD PTR [eax]
	add	cx, dx
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [edx], cx

; 1098 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	eax, DWORD PTR _ug$102474[ebp]
	imul	eax, DWORD PTR _uDistance65536$102484[ebp]
	shr	eax, 16					; 00000010H
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	dx, WORD PTR [ecx+2]
	add	dx, ax
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [eax+2], dx

; 1099 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	mov	ecx, DWORD PTR _ub$102475[ebp]
	imul	ecx, DWORD PTR _uDistance65536$102484[ebp]
	shr	ecx, 16					; 00000010H
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	ax, WORD PTR [edx+4]
	add	ax, cx
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [ecx+4], ax
$L102511:

; 1102 : 
; 1103 : 					gx++;

	mov	edx, DWORD PTR _gx$102479[ebp]
	add	edx, 1
	mov	DWORD PTR _gx$102479[ebp], edx

; 1104 :                     if (gx >= LIGHTING_GRID_WIDTH)

	cmp	DWORD PTR _gx$102479[ebp], 48		; 00000030H
	jl	SHORT $L102512

; 1105 :                         break;

	jmp	SHORT $L102505
$L102512:

; 1106 : 
; 1107 : 					x += LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _x$102477[ebp]
	add	eax, 32					; 00000020H
	mov	DWORD PTR _x$102477[ebp], eax

; 1108 : 					pCurPos++;

	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	add	ecx, 8
	mov	DWORD PTR _pCurPos$102485[ebp], ecx

; 1109 : 				}

	jmp	$L102504
$L102505:

; 1111 : 
; 1112 : 			gx = nLightGridX - r;

	mov	edx, DWORD PTR _nLightGridX$102467[ebp]
	sub	edx, DWORD PTR _r$102486[ebp]
	mov	DWORD PTR _gx$102479[ebp], edx

; 1113 : 			gy = nLightGridY - (r - 1);

	mov	eax, DWORD PTR _r$102486[ebp]
	sub	eax, 1
	mov	ecx, DWORD PTR _nLightGridY$102468[ebp]
	sub	ecx, eax
	mov	DWORD PTR _gy$102480[ebp], ecx

; 1115 : 				((gx < 0) || (gy < 0))
; 1116 : 			)

	cmp	DWORD PTR _gx$102479[ebp], 0
	jl	$L102516
	cmp	DWORD PTR _gy$102480[ebp], 0
	jl	$L102516

; 1118 : 				x = nLightGridCenterX - r * LIGHTING_GRID_SIZEX;

	mov	edx, DWORD PTR _r$102486[ebp]
	shl	edx, 5
	mov	eax, DWORD PTR _nLightGridCenterX$102471[ebp]
	sub	eax, edx
	mov	DWORD PTR _x$102477[ebp], eax

; 1119 : 				y = nLightGridCenterY - (r - 1) * LIGHTING_GRID_SIZEY;

	mov	ecx, DWORD PTR _r$102486[ebp]
	sub	ecx, 1
	shl	ecx, 5
	mov	edx, DWORD PTR _nLightGridCenterY$102472[ebp]
	sub	edx, ecx
	mov	DWORD PTR _y$102478[ebp], edx

; 1120 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	eax, DWORD PTR _gy$102480[ebp]
	imul	eax, 48					; 00000030H
	add	eax, DWORD PTR _gx$102479[ebp]
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73824]
	lea	eax, DWORD PTR [edx+eax*8]
	mov	DWORD PTR _pCurPos$102485[ebp], eax

; 1121 : 				for(j=0; j<(r * 2 - 1); j++)

	mov	DWORD PTR _j$102476[ebp], 0
	jmp	SHORT $L102514
$L102515:
	mov	ecx, DWORD PTR _j$102476[ebp]
	add	ecx, 1
	mov	DWORD PTR _j$102476[ebp], ecx
$L102514:
	mov	edx, DWORD PTR _r$102486[ebp]
	lea	eax, DWORD PTR [edx+edx-1]
	cmp	DWORD PTR _j$102476[ebp], eax
	jge	$L102516

; 1123 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	ecx, DWORD PTR _nLightPosY$102466[ebp]
	push	ecx
	mov	edx, DWORD PTR _nLightPosX$102465[ebp]
	push	edx
	mov	eax, DWORD PTR _y$102478[ebp]
	push	eax
	mov	ecx, DWORD PTR _x$102477[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	$L102522

; 1125 : 						//float f = 65536.0 - ((float)(
; 1126 : 						//    (
; 1127 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1128 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1129 : 						//    ) * 
; 1130 : 						//    fLightRadiusDenom *
; 1131 : 						//    65536.0
; 1132 : 						//));
; 1133 : 						float f = 65536.0 - ((float)(
; 1134 : 							sqrt(
; 1135 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1136 : 								(y - nLightPosY) * (y - nLightPosY)
; 1137 : 							) * 
; 1138 : 							fLightRadiusDenom * 
; 1139 : 							65536.0
; 1140 : 						));

	mov	edx, DWORD PTR _x$102477[ebp]
	sub	edx, DWORD PTR _nLightPosX$102465[ebp]
	mov	eax, DWORD PTR _x$102477[ebp]
	sub	eax, DWORD PTR _nLightPosX$102465[ebp]
	imul	edx, eax
	mov	ecx, DWORD PTR _y$102478[ebp]
	sub	ecx, DWORD PTR _nLightPosY$102466[ebp]
	mov	eax, DWORD PTR _y$102478[ebp]
	sub	eax, DWORD PTR _nLightPosY$102466[ebp]
	imul	ecx, eax
	add	edx, ecx
	mov	DWORD PTR -200+[ebp], edx
	fild	DWORD PTR -200+[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_sqrt
	add	esp, 8
	fmul	DWORD PTR _fLightRadiusDenom$102482[ebp]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR -204+[ebp]
	fsubr	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR _f$102518[ebp]

; 1141 : 
; 1142 : 						uDistance65536 = (int)(f);

	call	__ftol
	mov	DWORD PTR _uDistance65536$102484[ebp], eax

; 1143 : 
; 1144 : 						if (((int)uDistance65536) > 0)

	cmp	DWORD PTR _uDistance65536$102484[ebp], 0
	jle	SHORT $L102522

; 1146 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	ecx, DWORD PTR _ur$102473[ebp]
	imul	ecx, DWORD PTR _uDistance65536$102484[ebp]
	shr	ecx, 16					; 00000010H
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	ax, WORD PTR [edx]
	add	ax, cx
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [ecx], ax

; 1147 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	edx, DWORD PTR _ug$102474[ebp]
	imul	edx, DWORD PTR _uDistance65536$102484[ebp]
	shr	edx, 16					; 00000010H
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	cx, WORD PTR [eax+2]
	add	cx, dx
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [edx+2], cx

; 1148 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	mov	eax, DWORD PTR _ub$102475[ebp]
	imul	eax, DWORD PTR _uDistance65536$102484[ebp]
	shr	eax, 16					; 00000010H
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	dx, WORD PTR [ecx+4]
	add	dx, ax
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [eax+4], dx
$L102522:

; 1151 : 
; 1152 : 					gy++;

	mov	ecx, DWORD PTR _gy$102480[ebp]
	add	ecx, 1
	mov	DWORD PTR _gy$102480[ebp], ecx

; 1153 :                     if (gy >= LIGHTING_GRID_HEIGHT)

	cmp	DWORD PTR _gy$102480[ebp], 96		; 00000060H
	jl	SHORT $L102523

; 1154 :                         break;

	jmp	SHORT $L102516
$L102523:

; 1155 : 
; 1156 : 					y += LIGHTING_GRID_SIZEX;

	mov	edx, DWORD PTR _y$102478[ebp]
	add	edx, 32					; 00000020H
	mov	DWORD PTR _y$102478[ebp], edx

; 1157 : 					pCurPos += LIGHTING_GRID_WIDTH;

	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	add	eax, 384				; 00000180H
	mov	DWORD PTR _pCurPos$102485[ebp], eax

; 1158 : 				}

	jmp	$L102515
$L102516:

; 1160 : 
; 1161 : 			
; 1162 : 			gx = nLightGridX + r;

	mov	ecx, DWORD PTR _nLightGridX$102467[ebp]
	add	ecx, DWORD PTR _r$102486[ebp]
	mov	DWORD PTR _gx$102479[ebp], ecx

; 1163 : 			gy = nLightGridY - (r - 1);

	mov	edx, DWORD PTR _r$102486[ebp]
	sub	edx, 1
	mov	eax, DWORD PTR _nLightGridY$102468[ebp]
	sub	eax, edx
	mov	DWORD PTR _gy$102480[ebp], eax

; 1165 : 				((gx >= LIGHTING_GRID_WIDTH ) || (gy < 0))
; 1166 : 			)

	cmp	DWORD PTR _gx$102479[ebp], 48		; 00000030H
	jge	$L102527
	cmp	DWORD PTR _gy$102480[ebp], 0
	jl	$L102527

; 1168 : 				x = nLightGridCenterX + r * LIGHTING_GRID_SIZEX;

	mov	ecx, DWORD PTR _r$102486[ebp]
	shl	ecx, 5
	mov	edx, DWORD PTR _nLightGridCenterX$102471[ebp]
	add	edx, ecx
	mov	DWORD PTR _x$102477[ebp], edx

; 1169 : 				y = nLightGridCenterY - (r - 1) * LIGHTING_GRID_SIZEY;

	mov	eax, DWORD PTR _r$102486[ebp]
	sub	eax, 1
	shl	eax, 5
	mov	ecx, DWORD PTR _nLightGridCenterY$102472[ebp]
	sub	ecx, eax
	mov	DWORD PTR _y$102478[ebp], ecx

; 1170 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	edx, DWORD PTR _gy$102480[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _gx$102479[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+73824]
	lea	edx, DWORD PTR [ecx+edx*8]
	mov	DWORD PTR _pCurPos$102485[ebp], edx

; 1171 : 				for(j=0; j<(r * 2 - 1); j++)

	mov	DWORD PTR _j$102476[ebp], 0
	jmp	SHORT $L102525
$L102526:
	mov	eax, DWORD PTR _j$102476[ebp]
	add	eax, 1
	mov	DWORD PTR _j$102476[ebp], eax
$L102525:
	mov	ecx, DWORD PTR _r$102486[ebp]
	lea	edx, DWORD PTR [ecx+ecx-1]
	cmp	DWORD PTR _j$102476[ebp], edx
	jge	$L102527

; 1173 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	eax, DWORD PTR _nLightPosY$102466[ebp]
	push	eax
	mov	ecx, DWORD PTR _nLightPosX$102465[ebp]
	push	ecx
	mov	edx, DWORD PTR _y$102478[ebp]
	push	edx
	mov	eax, DWORD PTR _x$102477[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	$L102533

; 1175 : 						//float f = 65536.0 - ((float)(
; 1176 : 						//    (
; 1177 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1178 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1179 : 						//    ) * 
; 1180 : 						//    fLightRadiusDenom *
; 1181 : 						//    65536.0
; 1182 : 						//));
; 1183 : 						float f = 65536.0 - ((float)(
; 1184 : 							sqrt(
; 1185 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1186 : 								(y - nLightPosY) * (y - nLightPosY)
; 1187 : 							) * 
; 1188 : 							fLightRadiusDenom * 
; 1189 : 							65536.0
; 1190 : 						));

	mov	ecx, DWORD PTR _x$102477[ebp]
	sub	ecx, DWORD PTR _nLightPosX$102465[ebp]
	mov	edx, DWORD PTR _x$102477[ebp]
	sub	edx, DWORD PTR _nLightPosX$102465[ebp]
	imul	ecx, edx
	mov	eax, DWORD PTR _y$102478[ebp]
	sub	eax, DWORD PTR _nLightPosY$102466[ebp]
	mov	edx, DWORD PTR _y$102478[ebp]
	sub	edx, DWORD PTR _nLightPosY$102466[ebp]
	imul	eax, edx
	add	ecx, eax
	mov	DWORD PTR -208+[ebp], ecx
	fild	DWORD PTR -208+[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_sqrt
	add	esp, 8
	fmul	DWORD PTR _fLightRadiusDenom$102482[ebp]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR -212+[ebp]
	fsubr	QWORD PTR __real@8@400f8000000000000000
	fst	DWORD PTR _f$102529[ebp]

; 1191 : 
; 1192 : 						uDistance65536 = (int)(f);

	call	__ftol
	mov	DWORD PTR _uDistance65536$102484[ebp], eax

; 1193 : 
; 1194 : 						if (((int)uDistance65536) > 0)

	cmp	DWORD PTR _uDistance65536$102484[ebp], 0
	jle	SHORT $L102533

; 1196 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	eax, DWORD PTR _ur$102473[ebp]
	imul	eax, DWORD PTR _uDistance65536$102484[ebp]
	shr	eax, 16					; 00000010H
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	dx, WORD PTR [ecx]
	add	dx, ax
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [eax], dx

; 1197 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	ecx, DWORD PTR _ug$102474[ebp]
	imul	ecx, DWORD PTR _uDistance65536$102484[ebp]
	shr	ecx, 16					; 00000010H
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	ax, WORD PTR [edx+2]
	add	ax, cx
	mov	ecx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [ecx+2], ax

; 1198 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	mov	edx, DWORD PTR _ub$102475[ebp]
	imul	edx, DWORD PTR _uDistance65536$102484[ebp]
	shr	edx, 16					; 00000010H
	mov	eax, DWORD PTR _pCurPos$102485[ebp]
	mov	cx, WORD PTR [eax+4]
	add	cx, dx
	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	mov	WORD PTR [edx+4], cx
$L102533:

; 1201 : 
; 1202 : 					gy++;

	mov	eax, DWORD PTR _gy$102480[ebp]
	add	eax, 1
	mov	DWORD PTR _gy$102480[ebp], eax

; 1203 :                     if (gy >= LIGHTING_GRID_HEIGHT)

	cmp	DWORD PTR _gy$102480[ebp], 96		; 00000060H
	jl	SHORT $L102534

; 1204 :                         break;

	jmp	SHORT $L102527
$L102534:

; 1205 : 
; 1206 : 					y += LIGHTING_GRID_SIZEX;

	mov	ecx, DWORD PTR _y$102478[ebp]
	add	ecx, 32					; 00000020H
	mov	DWORD PTR _y$102478[ebp], ecx

; 1207 : 					pCurPos += LIGHTING_GRID_WIDTH;

	mov	edx, DWORD PTR _pCurPos$102485[ebp]
	add	edx, 384				; 00000180H
	mov	DWORD PTR _pCurPos$102485[ebp], edx

; 1208 : 				}

	jmp	$L102526
$L102527:

; 1210 : 		}

	jmp	$L102488
$L102489:

; 1211 : 	}

	jmp	$L102462
$L102463:

; 1212 : 
; 1213 :     //memcpy(ptp, pLColor, sizeof(ptp));
; 1214 : 
; 1215 :     
; 1216 : 	// 对光照图进行过滤，使阴影边沿平滑
; 1217 :     
; 1218 : 
; 1219 : 	int m;
; 1220 :     int n;
; 1221 :     DWORD   *pdwLight;
; 1222 :     KLColor *pO, *pL, *pR, *pU, *pD, *pDest;
; 1223 :     int nIdx = 1 * LIGHTING_GRID_WIDTH + 1;

	mov	DWORD PTR _nIdx$[ebp], 49		; 00000031H

; 1224 : 	for(m=1; m<LIGHTING_GRID_HEIGHT-1;m++)

	mov	DWORD PTR _m$[ebp], 1
	jmp	SHORT $L102545
$L102546:
	mov	eax, DWORD PTR _m$[ebp]
	add	eax, 1
	mov	DWORD PTR _m$[ebp], eax
$L102545:
	cmp	DWORD PTR _m$[ebp], 95			; 0000005fH
	jge	$L102547

; 1226 : 		pDest = ptp + nIdx;

	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+73832]
	mov	eax, DWORD PTR _nIdx$[ebp]
	lea	ecx, DWORD PTR [edx+eax*8]
	mov	DWORD PTR _pDest$[ebp], ecx

; 1227 :         pdwLight = pLightingArray + nIdx;

	mov	edx, DWORD PTR _nIdx$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	lea	ecx, DWORD PTR [eax+edx*4+73840]
	mov	DWORD PTR _pdwLight$[ebp], ecx

; 1228 : 		pO = pLColor + nIdx;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+73824]
	mov	ecx, DWORD PTR _nIdx$[ebp]
	lea	edx, DWORD PTR [eax+ecx*8]
	mov	DWORD PTR _pO$[ebp], edx

; 1229 : 		pL = pO - 1;

	mov	eax, DWORD PTR _pO$[ebp]
	sub	eax, 8
	mov	DWORD PTR _pL$[ebp], eax

; 1230 : 		pR = pO + 1;

	mov	ecx, DWORD PTR _pO$[ebp]
	add	ecx, 8
	mov	DWORD PTR _pR$[ebp], ecx

; 1231 : 		pU = pO - LIGHTING_GRID_WIDTH;

	mov	edx, DWORD PTR _pO$[ebp]
	sub	edx, 384				; 00000180H
	mov	DWORD PTR _pU$[ebp], edx

; 1232 : 		pD = pO + LIGHTING_GRID_WIDTH;

	mov	eax, DWORD PTR _pO$[ebp]
	add	eax, 384				; 00000180H
	mov	DWORD PTR _pD$[ebp], eax

; 1233 : 		for(n=1; n<LIGHTING_GRID_WIDTH-1;n++)

	mov	DWORD PTR _n$[ebp], 1
	jmp	SHORT $L102548
$L102549:
	mov	ecx, DWORD PTR _n$[ebp]
	add	ecx, 1
	mov	DWORD PTR _n$[ebp], ecx
$L102548:
	cmp	DWORD PTR _n$[ebp], 47			; 0000002fH
	jge	$L102550

; 1235 : 			//pDest->r = (pO->r + pR->r + pL->r + pU->r + pD->r) / 5;
; 1236 : 			//pDest->g = (pO->g + pR->g + pL->g + pU->g + pD->g) / 5;
; 1237 : 			//pDest->b = (pO->b + pR->b + pL->b + pU->b + pD->b) / 5;
; 1238 : 			//pDest->r = (pO->r + pR->r + pL->r + (pU->r + pD->r) / 2) / 4;
; 1239 : 			//pDest->g = (pO->g + pR->g + pL->g + (pU->g + pD->g) / 2) / 4;
; 1240 : 			//pDest->b = (pO->b + pR->b + pL->b + (pU->b + pD->b) / 2) / 4;
; 1241 : 			//pDest->r = (pO->r + (pR->r + pL->r) / 2 + pU->r + pD->r) / 4;
; 1242 : 			//pDest->g = (pO->g + (pR->g + pL->g) / 2 + pU->g + pD->g) / 4;
; 1243 : 			//pDest->b = (pO->b + (pR->b + pL->b) / 2 + pU->b + pD->b) / 4;
; 1244 : 			
; 1245 :             pDest->r = (pO->r + pL->r + pU->r + pD->r) / 4;

	mov	edx, DWORD PTR _pO$[ebp]
	xor	eax, eax
	mov	ax, WORD PTR [edx]
	mov	ecx, DWORD PTR _pL$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx]
	add	eax, edx
	mov	ecx, DWORD PTR _pU$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx]
	add	eax, edx
	mov	ecx, DWORD PTR _pD$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx]
	add	eax, edx
	cdq
	and	edx, 3
	add	eax, edx
	sar	eax, 2
	mov	ecx, DWORD PTR _pDest$[ebp]
	mov	WORD PTR [ecx], ax

; 1246 : 		    pDest->g = (pO->g + pL->g + pU->g + pD->g) / 4;

	mov	edx, DWORD PTR _pO$[ebp]
	xor	eax, eax
	mov	ax, WORD PTR [edx+2]
	mov	ecx, DWORD PTR _pL$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+2]
	add	eax, edx
	mov	ecx, DWORD PTR _pU$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+2]
	add	eax, edx
	mov	ecx, DWORD PTR _pD$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+2]
	add	eax, edx
	cdq
	and	edx, 3
	add	eax, edx
	sar	eax, 2
	mov	ecx, DWORD PTR _pDest$[ebp]
	mov	WORD PTR [ecx+2], ax

; 1247 : 			pDest->b = (pO->b + pL->b + pU->b + pD->b) / 4;

	mov	edx, DWORD PTR _pO$[ebp]
	xor	eax, eax
	mov	ax, WORD PTR [edx+4]
	mov	ecx, DWORD PTR _pL$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+4]
	add	eax, edx
	mov	ecx, DWORD PTR _pU$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+4]
	add	eax, edx
	mov	ecx, DWORD PTR _pD$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+4]
	add	eax, edx
	cdq
	and	edx, 3
	add	eax, edx
	sar	eax, 2
	mov	ecx, DWORD PTR _pDest$[ebp]
	mov	WORD PTR [ecx+4], ax

; 1248 : 
; 1249 :             *pdwLight = 0xff000000 |
; 1250 :                 (((pDest->r > 0xff) ? 0xff : pDest->r) << 16) |
; 1251 :                 (((pDest->g > 0xff) ? 0xff : pDest->g) << 8) |
; 1252 :                 (((pDest->b > 0xff) ? 0xff : pDest->b));

	mov	edx, DWORD PTR _pDest$[ebp]
	xor	eax, eax
	mov	ax, WORD PTR [edx]
	cmp	eax, 255				; 000000ffH
	jle	SHORT $L103228
	mov	DWORD PTR -216+[ebp], 255		; 000000ffH
	jmp	SHORT $L103229
$L103228:
	mov	ecx, DWORD PTR _pDest$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx]
	mov	DWORD PTR -216+[ebp], edx
$L103229:
	mov	eax, DWORD PTR _pDest$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax+2]
	cmp	ecx, 255				; 000000ffH
	jle	SHORT $L103230
	mov	DWORD PTR -220+[ebp], 255		; 000000ffH
	jmp	SHORT $L103231
$L103230:
	mov	edx, DWORD PTR _pDest$[ebp]
	xor	eax, eax
	mov	ax, WORD PTR [edx+2]
	mov	DWORD PTR -220+[ebp], eax
$L103231:
	mov	ecx, DWORD PTR _pDest$[ebp]
	xor	edx, edx
	mov	dx, WORD PTR [ecx+4]
	cmp	edx, 255				; 000000ffH
	jle	SHORT $L103232
	mov	DWORD PTR -224+[ebp], 255		; 000000ffH
	jmp	SHORT $L103233
$L103232:
	mov	eax, DWORD PTR _pDest$[ebp]
	xor	ecx, ecx
	mov	cx, WORD PTR [eax+4]
	mov	DWORD PTR -224+[ebp], ecx
$L103233:
	mov	edx, DWORD PTR -216+[ebp]
	shl	edx, 16					; 00000010H
	or	edx, -16777216				; ff000000H
	mov	eax, DWORD PTR -220+[ebp]
	shl	eax, 8
	or	edx, eax
	or	edx, DWORD PTR -224+[ebp]
	mov	ecx, DWORD PTR _pdwLight$[ebp]
	mov	DWORD PTR [ecx], edx

; 1253 : 
; 1254 : 
; 1255 :             //pDest->r = pO->r;
; 1256 : 		    //pDest->g = pO->g;
; 1257 : 			//pDest->b = pO->b;
; 1258 : 
; 1259 : 			pDest++, pO++, pL++, pR++, pU++, pD++, pdwLight++;

	mov	edx, DWORD PTR _pDest$[ebp]
	add	edx, 8
	mov	DWORD PTR _pDest$[ebp], edx
	mov	eax, DWORD PTR _pO$[ebp]
	add	eax, 8
	mov	DWORD PTR _pO$[ebp], eax
	mov	ecx, DWORD PTR _pL$[ebp]
	add	ecx, 8
	mov	DWORD PTR _pL$[ebp], ecx
	mov	edx, DWORD PTR _pR$[ebp]
	add	edx, 8
	mov	DWORD PTR _pR$[ebp], edx
	mov	eax, DWORD PTR _pU$[ebp]
	add	eax, 8
	mov	DWORD PTR _pU$[ebp], eax
	mov	ecx, DWORD PTR _pD$[ebp]
	add	ecx, 8
	mov	DWORD PTR _pD$[ebp], ecx
	mov	edx, DWORD PTR _pdwLight$[ebp]
	add	edx, 4
	mov	DWORD PTR _pdwLight$[ebp], edx

; 1260 : 		}

	jmp	$L102549
$L102550:

; 1261 : 		nIdx += LIGHTING_GRID_WIDTH;

	mov	eax, DWORD PTR _nIdx$[ebp]
	add	eax, 48					; 00000030H
	mov	DWORD PTR _nIdx$[ebp], eax

; 1262 : 	}

	jmp	$L102546
$L102547:

; 1263 :     
; 1264 : 
; 1265 : 	// 将r，g，b的值限制在0xff之内，防止色彩错误
; 1266 : //	for(j = 0; j < LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT; j++)
; 1267 : //	{
; 1268 : //        //pLightingArray[j] = 0xff000000 |
; 1269 : //        //    ((((-(ptp[j].r > 0xff)) | ptp[j].r) & 0xff) << 16) |
; 1270 : //        //    ((((-(ptp[j].g > 0xff)) | ptp[j].g) & 0xff) << 8) |
; 1271 : //        //    ((((-(ptp[j].b > 0xff)) | ptp[j].b) & 0xff));
; 1272 : //
; 1273 : //		//if(ptp[j].r > 0xff)
; 1274 : //		//	ptp[j].r = 0xff;
; 1275 : //        
; 1276 : //		//if(ptp[j].g > 0xff)
; 1277 : //		//	ptp[j].g = 0xff;
; 1278 : //		//if(ptp[j].b > 0xff)
; 1279 : //		//	ptp[j].b = 0xff;
; 1280 : //		//pLightingArray[j] = 0xff000000 | (ptp[j].r<<16) | (ptp[j].g<<8) | ptp[j].b;
; 1281 : //
; 1282 : //        pLightingArray[j] = 0xff000000 |
; 1283 : //            (((ptp[j].r > 0xff) ? 0xff : ptp[j].r) << 16) |
; 1284 : //            (((ptp[j].g > 0xff) ? 0xff : ptp[j].g) << 8) |
; 1285 : //            (((ptp[j].b > 0xff) ? 0xff : ptp[j].b));
; 1286 : //
; 1287 : //	}
; 1288 :     for (m = 0; m < LIGHTING_GRID_HEIGHT; m++)

	mov	DWORD PTR _m$[ebp], 0
	jmp	SHORT $L102551
$L102552:
	mov	ecx, DWORD PTR _m$[ebp]
	add	ecx, 1
	mov	DWORD PTR _m$[ebp], ecx
$L102551:
	cmp	DWORD PTR _m$[ebp], 96			; 00000060H
	jge	SHORT $L102553

; 1290 :         pLightingArray[m * LIGHTING_GRID_WIDTH + 0] = m_dwAmbient | 0xff000000;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+92]
	or	eax, -16777216				; ff000000H
	mov	ecx, DWORD PTR _m$[ebp]
	imul	ecx, 48					; 00000030H
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+ecx*4+73840], eax

; 1291 :         pLightingArray[m * LIGHTING_GRID_WIDTH + LIGHTING_GRID_WIDTH - 1] = m_dwAmbient | 0xff000000;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+92]
	or	ecx, -16777216				; ff000000H
	mov	edx, DWORD PTR _m$[ebp]
	imul	edx, 48					; 00000030H
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+edx*4+74028], ecx

; 1292 :     }

	jmp	SHORT $L102552
$L102553:

; 1293 : 
; 1294 :     for (n = 0; n < LIGHTING_GRID_WIDTH; n++)

	mov	DWORD PTR _n$[ebp], 0
	jmp	SHORT $L102554
$L102555:
	mov	ecx, DWORD PTR _n$[ebp]
	add	ecx, 1
	mov	DWORD PTR _n$[ebp], ecx
$L102554:
	cmp	DWORD PTR _n$[ebp], 48			; 00000030H
	jge	SHORT $L102556

; 1296 :         pLightingArray[n] = m_dwAmbient | 0xff000000;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+92]
	or	eax, -16777216				; ff000000H
	mov	ecx, DWORD PTR _n$[ebp]
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+ecx*4+73840], eax

; 1297 :         pLightingArray[(LIGHTING_GRID_HEIGHT - 1) * LIGHTING_GRID_WIDTH + n] = m_dwAmbient | 0xff000000;

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+92]
	or	ecx, -16777216				; ff000000H
	mov	edx, DWORD PTR _n$[ebp]
	mov	eax, DWORD PTR _this$[ebp]
	mov	DWORD PTR [eax+edx*4+92080], ecx

; 1298 :     }

	jmp	SHORT $L102555
$L102556:

; 1299 :     //pLightingArray[0] = m_dwAmbient;    // 设置环境光
; 1300 : }

	mov	esp, ebp
	pop	ebp
	ret	0
?RenderLightMap@KIpoTree@@QAEXXZ ENDP			; KIpoTree::RenderLightMap
_TEXT	ENDS
PUBLIC	__real@4@00000000000000000000
;	COMDAT __real@4@00000000000000000000
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@4@00000000000000000000 DD 000000000r	; 0
CONST	ENDS
_TEXT	SEGMENT
_x$ = 8
_y$ = 12
_nLightPosX$ = 16
_nLightPosY$ = 20
_this$ = -88
_nGridX$ = -36
_nGridY$ = -48
_nGridLightPosX$ = -72
_nGridLightPosY$ = -76
_nGridStep$ = -20
_nCurrentGridX$ = -52
_nCurrentGridY$ = -68
_nIdx$ = -24
_nObstacle$ = -64
_vDir$ = -84
_vLight$ = -60
_fx$ = -32
_fy$ = -44
_fLightPosX$ = -40
_fLightPosY$ = -28
_fdx$ = -8
_fdy$ = -16
_fStepX$ = -4
_fStepY$ = -12
?CanLighting@KIpoTree@@AAE_NHHHH@Z PROC NEAR		; KIpoTree::CanLighting

; 1304 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 96					; 00000060H
	mov	DWORD PTR _this$[ebp], ecx

; 1305 : 	int nGridX, nGridY, nGridLightPosX, nGridLightPosY, nGridStep, nCurrentGridX, nCurrentGridY, nIdx;
; 1306 : 	nGridX = x / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _x$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nGridX$[ebp], eax

; 1307 : 	nGridY = y / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _y$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nGridY$[ebp], eax

; 1308 : 	nGridLightPosX = nLightPosX / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nLightPosX$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nGridLightPosX$[ebp], eax

; 1309 : 	nGridLightPosY = nLightPosY / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nLightPosY$[ebp]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nGridLightPosY$[ebp], eax

; 1310 : 	if(nGridX == nGridLightPosX && nGridY == nGridLightPosY)

	mov	eax, DWORD PTR _nGridX$[ebp]
	cmp	eax, DWORD PTR _nGridLightPosX$[ebp]
	jne	SHORT $L102572
	mov	ecx, DWORD PTR _nGridY$[ebp]
	cmp	ecx, DWORD PTR _nGridLightPosY$[ebp]
	jne	SHORT $L102572

; 1311 : 		return true;

	mov	al, 1
	jmp	$L102615
$L102572:

; 1312 : 
; 1313 : 	int nObstacle = pObstacle[nGridY * LIGHTING_GRID_WIDTH + nGridX].nObstacle;

	mov	edx, DWORD PTR _nGridY$[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _nGridX$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+edx+96]
	mov	DWORD PTR _nObstacle$[ebp], ecx

; 1314 : 	KVector2 vDir = pObstacle[nGridY * LIGHTING_GRID_WIDTH + nGridX].vDir;

	mov	edx, DWORD PTR _nGridY$[ebp]
	imul	edx, 48					; 00000030H
	add	edx, DWORD PTR _nGridX$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+edx+100]
	mov	edx, DWORD PTR [eax+edx+104]
	mov	DWORD PTR _vDir$[ebp], ecx
	mov	DWORD PTR _vDir$[ebp+4], edx

; 1315 : 	KVector2 vLight;
; 1316 : 	vLight.fX = (float)(x - nLightPosX);

	mov	eax, DWORD PTR _x$[ebp]
	sub	eax, DWORD PTR _nLightPosX$[ebp]
	mov	DWORD PTR -92+[ebp], eax
	fild	DWORD PTR -92+[ebp]
	fstp	DWORD PTR _vLight$[ebp]

; 1317 : 	vLight.fY = (float)(y - nLightPosY);

	mov	ecx, DWORD PTR _y$[ebp]
	sub	ecx, DWORD PTR _nLightPosY$[ebp]
	mov	DWORD PTR -96+[ebp], ecx
	fild	DWORD PTR -96+[ebp]
	fstp	DWORD PTR _vLight$[ebp+4]

; 1318 : 
; 1319 : 	float fx, fy, fLightPosX, fLightPosY, fdx, fdy, fStepX, fStepY;
; 1320 : 	fx = (float)x, fy =(float) y, fLightPosX = (float)nLightPosX, fLightPosY = (float)nLightPosY;

	fild	DWORD PTR _x$[ebp]
	fstp	DWORD PTR _fx$[ebp]
	fild	DWORD PTR _y$[ebp]
	fstp	DWORD PTR _fy$[ebp]
	fild	DWORD PTR _nLightPosX$[ebp]
	fstp	DWORD PTR _fLightPosX$[ebp]
	fild	DWORD PTR _nLightPosY$[ebp]
	fstp	DWORD PTR _fLightPosY$[ebp]

; 1321 : 	fdx = (float)fabs(fLightPosX - fx);

	fld	DWORD PTR _fLightPosX$[ebp]
	fsub	DWORD PTR _fx$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	add	esp, 8
	fstp	DWORD PTR _fdx$[ebp]

; 1322 : 	fdy = (float)fabs(fLightPosY - fy);

	fld	DWORD PTR _fLightPosY$[ebp]
	fsub	DWORD PTR _fy$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_fabs
	add	esp, 8
	fstp	DWORD PTR _fdy$[ebp]

; 1323 : 
; 1324 : 	// 斜率小于45度的情况
; 1325 : 	if(fdx >= fdy)

	fld	DWORD PTR _fdx$[ebp]
	fcomp	DWORD PTR _fdy$[ebp]
	fnstsw	ax
	test	ah, 1
	jne	$L102592

; 1327 : 		// 计算线段前进累加值
; 1328 : 		if(nLightPosX > x)

	mov	edx, DWORD PTR _nLightPosX$[ebp]
	cmp	edx, DWORD PTR _x$[ebp]
	jle	SHORT $L102593

; 1330 : 			nGridStep = 1;

	mov	DWORD PTR _nGridStep$[ebp], 1

; 1331 : 			fStepX = LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepX$[ebp], 1107296256	; 42000000H

; 1333 : 		else

	jmp	SHORT $L102594
$L102593:

; 1335 : 			nGridStep = -1;

	mov	DWORD PTR _nGridStep$[ebp], -1

; 1336 : 			fStepX = -LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepX$[ebp], -1040187392	; c2000000H
$L102594:

; 1338 : 		if(nLightPosY > y)

	mov	eax, DWORD PTR _nLightPosY$[ebp]
	cmp	eax, DWORD PTR _y$[ebp]
	jle	SHORT $L102595

; 1339 : 			fStepY = fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdy$[ebp]
	fdiv	DWORD PTR _fdx$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepY$[ebp]

; 1340 : 		else

	jmp	SHORT $L102596
$L102595:

; 1341 : 			fStepY = -fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdy$[ebp]
	fchs
	fdiv	DWORD PTR _fdx$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepY$[ebp]
$L102596:

; 1342 : 
; 1343 : 		while(1)

	mov	ecx, 1
	test	ecx, ecx
	je	$L102599

; 1345 : 			fx += fStepX;

	fld	DWORD PTR _fx$[ebp]
	fadd	DWORD PTR _fStepX$[ebp]
	fstp	DWORD PTR _fx$[ebp]

; 1346 : 			fy += fStepY;

	fld	DWORD PTR _fy$[ebp]
	fadd	DWORD PTR _fStepY$[ebp]
	fstp	DWORD PTR _fy$[ebp]

; 1347 : 			nGridX += nGridStep;

	mov	edx, DWORD PTR _nGridX$[ebp]
	add	edx, DWORD PTR _nGridStep$[ebp]
	mov	DWORD PTR _nGridX$[ebp], edx

; 1348 : 			if(nGridX == nGridLightPosX)

	mov	eax, DWORD PTR _nGridX$[ebp]
	cmp	eax, DWORD PTR _nGridLightPosX$[ebp]
	jne	SHORT $L102604

; 1350 : 				// 没有碰到障碍，根据这点本身是否障碍和障碍方向判断是否受光
; 1351 : 				if(nObstacle && vDir.fX != 0.0f)

	cmp	DWORD PTR _nObstacle$[ebp], 0
	je	SHORT $L102601
	fld	DWORD PTR _vDir$[ebp]
	fcomp	DWORD PTR __real@4@00000000000000000000
	fnstsw	ax
	test	ah, 64					; 00000040H
	jne	SHORT $L102601

; 1353 : 					if(vDir.fX * vLight.fY - vDir.fY * vLight.fX < 0.0f)

	fld	DWORD PTR _vDir$[ebp]
	fmul	DWORD PTR _vLight$[ebp+4]
	fld	DWORD PTR _vDir$[ebp+4]
	fmul	DWORD PTR _vLight$[ebp]
	fsubp	ST(1), ST(0)
	fcomp	DWORD PTR __real@4@00000000000000000000
	fnstsw	ax
	test	ah, 1
	je	SHORT $L102602

; 1354 : 						return true;

	mov	al, 1
	jmp	$L102615
$L102602:

; 1356 : 						return false;

	xor	al, al
	jmp	$L102615
$L102601:

; 1359 : 					return true;

	mov	al, 1
	jmp	$L102615
$L102604:

; 1361 : 
; 1362 : 			nCurrentGridX = ((int)fx) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fx$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nCurrentGridX$[ebp], eax

; 1363 : 			nCurrentGridY = ((int)fy) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fy$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nCurrentGridY$[ebp], eax

; 1364 : 			nIdx = nCurrentGridY * LIGHTING_GRID_WIDTH + nCurrentGridX;

	mov	ecx, DWORD PTR _nCurrentGridY$[ebp]
	imul	ecx, 48					; 00000030H
	add	ecx, DWORD PTR _nCurrentGridX$[ebp]
	mov	DWORD PTR _nIdx$[ebp], ecx

; 1365 : 			// 检查是否碰到挡光障碍
; 1366 : 			if(pObstacle[nIdx].nObstacle != 0 && pObstacle[nIdx].nObstacle != nObstacle)

	mov	edx, DWORD PTR _nIdx$[ebp]
	shl	edx, 4
	mov	eax, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [eax+edx+96], 0
	je	SHORT $L102607
	mov	ecx, DWORD PTR _nIdx$[ebp]
	shl	ecx, 4
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+ecx+96]
	cmp	eax, DWORD PTR _nObstacle$[ebp]
	je	SHORT $L102607

; 1367 : 				return false;

	xor	al, al
	jmp	$L102615
$L102607:

; 1368 : 		}

	jmp	$L102596
$L102599:

; 1370 : 	// 斜率大于45度的情况
; 1371 : 	else

	jmp	$L102615
$L102592:

; 1373 : 		if(nLightPosY > y)

	mov	ecx, DWORD PTR _nLightPosY$[ebp]
	cmp	ecx, DWORD PTR _y$[ebp]
	jle	SHORT $L102609

; 1375 : 			nGridStep = 1;

	mov	DWORD PTR _nGridStep$[ebp], 1

; 1376 : 			fStepY = LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepY$[ebp], 1107296256	; 42000000H

; 1378 : 		else

	jmp	SHORT $L102610
$L102609:

; 1380 : 			nGridStep = -1;

	mov	DWORD PTR _nGridStep$[ebp], -1

; 1381 : 			fStepY = -LIGHTING_GRID_SIZEX;

	mov	DWORD PTR _fStepY$[ebp], -1040187392	; c2000000H
$L102610:

; 1383 : 		if(nLightPosX > x)

	mov	edx, DWORD PTR _nLightPosX$[ebp]
	cmp	edx, DWORD PTR _x$[ebp]
	jle	SHORT $L102611

; 1384 : 			fStepX = fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[ebp]
	fdiv	DWORD PTR _fdy$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepX$[ebp]

; 1385 : 		else

	jmp	SHORT $L102612
$L102611:

; 1386 : 			fStepX = -fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[ebp]
	fchs
	fdiv	DWORD PTR _fdy$[ebp]
	fmul	DWORD PTR __real@4@40048000000000000000
	fstp	DWORD PTR _fStepX$[ebp]
$L102612:

; 1387 : 		while(1)

	mov	eax, 1
	test	eax, eax
	je	$L102615

; 1389 : 			fx += fStepX;

	fld	DWORD PTR _fx$[ebp]
	fadd	DWORD PTR _fStepX$[ebp]
	fstp	DWORD PTR _fx$[ebp]

; 1390 : 			fy += fStepY;

	fld	DWORD PTR _fy$[ebp]
	fadd	DWORD PTR _fStepY$[ebp]
	fstp	DWORD PTR _fy$[ebp]

; 1391 : 			nGridY += nGridStep;

	mov	ecx, DWORD PTR _nGridY$[ebp]
	add	ecx, DWORD PTR _nGridStep$[ebp]
	mov	DWORD PTR _nGridY$[ebp], ecx

; 1392 : 			if(nGridY == nGridLightPosY)

	mov	edx, DWORD PTR _nGridY$[ebp]
	cmp	edx, DWORD PTR _nGridLightPosY$[ebp]
	jne	SHORT $L102620

; 1394 : 				// 没有碰到障碍，根据这点本身是否障碍和障碍方向判断是否受光
; 1395 : 				if(nObstacle && vDir.fX != 0.0f)

	cmp	DWORD PTR _nObstacle$[ebp], 0
	je	SHORT $L102617
	fld	DWORD PTR _vDir$[ebp]
	fcomp	DWORD PTR __real@4@00000000000000000000
	fnstsw	ax
	test	ah, 64					; 00000040H
	jne	SHORT $L102617

; 1397 : 					if(vDir.fX * vLight.fY - vDir.fY * vLight.fX < 0.0f)

	fld	DWORD PTR _vDir$[ebp]
	fmul	DWORD PTR _vLight$[ebp+4]
	fld	DWORD PTR _vDir$[ebp+4]
	fmul	DWORD PTR _vLight$[ebp]
	fsubp	ST(1), ST(0)
	fcomp	DWORD PTR __real@4@00000000000000000000
	fnstsw	ax
	test	ah, 1
	je	SHORT $L102618

; 1398 : 						return true;

	mov	al, 1
	jmp	SHORT $L102615
$L102618:

; 1400 : 						return false;

	xor	al, al
	jmp	SHORT $L102615
$L102617:

; 1403 : 					return true;

	mov	al, 1
	jmp	SHORT $L102615
$L102620:

; 1405 : 
; 1406 : 			nCurrentGridX = ((int)fx) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fx$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nCurrentGridX$[ebp], eax

; 1407 : 			nCurrentGridY = ((int)fy) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fy$[ebp]
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	mov	DWORD PTR _nCurrentGridY$[ebp], eax

; 1408 : 			nIdx = nCurrentGridY * LIGHTING_GRID_WIDTH + nCurrentGridX;

	mov	eax, DWORD PTR _nCurrentGridY$[ebp]
	imul	eax, 48					; 00000030H
	add	eax, DWORD PTR _nCurrentGridX$[ebp]
	mov	DWORD PTR _nIdx$[ebp], eax

; 1409 : 			// 检查是否碰到挡光障碍
; 1410 : 			if(pObstacle[nIdx].nObstacle != 0 && pObstacle[nIdx].nObstacle != nObstacle)

	mov	ecx, DWORD PTR _nIdx$[ebp]
	shl	ecx, 4
	mov	edx, DWORD PTR _this$[ebp]
	cmp	DWORD PTR [edx+ecx+96], 0
	je	SHORT $L102623
	mov	eax, DWORD PTR _nIdx$[ebp]
	shl	eax, 4
	mov	ecx, DWORD PTR _this$[ebp]
	mov	edx, DWORD PTR [ecx+eax+96]
	cmp	edx, DWORD PTR _nObstacle$[ebp]
	je	SHORT $L102623

; 1411 : 				return false;

	xor	al, al
	jmp	SHORT $L102615
$L102623:

; 1412 : 		}

	jmp	$L102612
$L102615:

; 1414 : }

	mov	esp, ebp
	pop	ebp
	ret	16					; 00000010H
?CanLighting@KIpoTree@@AAE_NHHHH@Z ENDP			; KIpoTree::CanLighting
_TEXT	ENDS
PUBLIC	?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z ; KIpoTree::AddBuildinLight
PUBLIC	__real@4@4005af00000000000000
EXTRN	__imp_?g_Random@@YAII@Z:NEAR
;	COMDAT __real@4@4005af00000000000000
; File D:\Build Source\SwordOnline\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@4@4005af00000000000000 DD 042af0000r	; 87.5
CONST	ENDS
xdata$x	SEGMENT
$T103249 DD	019930520H
	DD	01H
	DD	FLAT:$T103253
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T103253 DD	0ffffffffH
	DD	FLAT:$L103247
xdata$x	ENDS
_TEXT	SEGMENT
_pLights$ = 8
_nCount$ = 12
_this$ = -44
_nMinRange$ = -24
_nMaxRange$ = -16
_i$ = -20
_pLight$102636 = -28
$T103242 = -32
$T103243 = -36
$T103246 = -40
__$EHRec$ = -12
?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z PROC NEAR ; KIpoTree::AddBuildinLight

; 1531 : {

	push	ebp
	mov	ebp, esp
	push	-1
	push	$L103252
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 44					; 0000002cH
	push	esi
	mov	DWORD PTR _this$[ebp], ecx

; 1532 : 	if(!m_bProcessBioLights)

	mov	eax, DWORD PTR _this$[ebp]
	xor	ecx, ecx
	mov	cl, BYTE PTR [eax+89]
	test	ecx, ecx
	jne	SHORT $L102629

; 1533 : 		return;

	jmp	$L102628
$L102629:

; 1536 : 
; 1537 : 	for(int i=0; i<nCount; i++)

	mov	DWORD PTR _i$[ebp], 0
	jmp	SHORT $L102633
$L102634:
	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 1
	mov	DWORD PTR _i$[ebp], edx
$L102633:
	mov	eax, DWORD PTR _i$[ebp]
	cmp	eax, DWORD PTR _nCount$[ebp]
	jge	$L102635

; 1539 : 		// 增加一个光源
; 1540 : 		KSelfBreathLight *pLight = new KSelfBreathLight;

	push	52					; 00000034H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T103243[ebp], eax
	mov	DWORD PTR __$EHRec$[ebp+8], 0
	cmp	DWORD PTR $T103243[ebp], 0
	je	SHORT $L103244
	mov	ecx, DWORD PTR $T103243[ebp]
	call	??0KSelfBreathLight@@QAE@XZ		; KSelfBreathLight::KSelfBreathLight
	mov	DWORD PTR -48+[ebp], eax
	jmp	SHORT $L103245
$L103244:
	mov	DWORD PTR -48+[ebp], 0
$L103245:
	mov	ecx, DWORD PTR -48+[ebp]
	mov	DWORD PTR $T103242[ebp], ecx
	mov	DWORD PTR __$EHRec$[ebp+8], -1
	mov	edx, DWORD PTR $T103242[ebp]
	mov	DWORD PTR _pLight$102636[ebp], edx

; 1541 : 		if(pLight)

	cmp	DWORD PTR _pLight$102636[ebp], 0
	je	$L102641

; 1543 : 			// 保证nMaxRange大于nMinRange
; 1544 : 			if(pLights[i].nMaxRange >= pLights[i].nMinRange)

	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 28					; 0000001cH
	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 28					; 0000001cH
	mov	edx, DWORD PTR _pLights$[ebp]
	mov	esi, DWORD PTR _pLights$[ebp]
	mov	eax, DWORD PTR [edx+eax+20]
	cmp	eax, DWORD PTR [esi+ecx+16]
	jl	SHORT $L102642

; 1545 : 				nMaxRange = pLights[i].nMaxRange, nMinRange = pLights[i].nMinRange;

	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 28					; 0000001cH
	mov	edx, DWORD PTR _pLights$[ebp]
	mov	eax, DWORD PTR [edx+ecx+20]
	mov	DWORD PTR _nMaxRange$[ebp], eax
	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 28					; 0000001cH
	mov	edx, DWORD PTR _pLights$[ebp]
	mov	eax, DWORD PTR [edx+ecx+16]
	mov	DWORD PTR _nMinRange$[ebp], eax

; 1546 : 			else

	jmp	SHORT $L102643
$L102642:

; 1547 : 				nMaxRange = pLights[i].nMinRange, nMinRange = pLights[i].nMaxRange;

	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 28					; 0000001cH
	mov	edx, DWORD PTR _pLights$[ebp]
	mov	eax, DWORD PTR [edx+ecx+16]
	mov	DWORD PTR _nMaxRange$[ebp], eax
	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 28					; 0000001cH
	mov	edx, DWORD PTR _pLights$[ebp]
	mov	eax, DWORD PTR [edx+ecx+20]
	mov	DWORD PTR _nMinRange$[ebp], eax
$L102643:

; 1548 : 			// 半径为负值，忽略这个光源
; 1549 : 			if(nMinRange < 0)

	cmp	DWORD PTR _nMinRange$[ebp], 0
	jge	SHORT $L102644

; 1551 : 				_ASSERT(0);
; 1552 : 				continue;

	jmp	$L102634
$L102644:

; 1554 : 
; 1555 : 			nMinRange = nMaxRange - 8;

	mov	ecx, DWORD PTR _nMaxRange$[ebp]
	sub	ecx, 8
	mov	DWORD PTR _nMinRange$[ebp], ecx

; 1556 : 
; 1557 : 			pLight->m_dwColor = pLights[i].dwColor;

	mov	edx, DWORD PTR _i$[ebp]
	imul	edx, 28					; 0000001cH
	mov	eax, DWORD PTR _pLight$102636[ebp]
	mov	ecx, DWORD PTR _pLights$[ebp]
	mov	edx, DWORD PTR [ecx+edx+12]
	mov	DWORD PTR [eax+20], edx

; 1558 : 			pLight->m_oPosition.nX = pLights[i].oPos.x;

	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 28					; 0000001cH
	mov	ecx, DWORD PTR _pLight$102636[ebp]
	mov	edx, DWORD PTR _pLights$[ebp]
	mov	eax, DWORD PTR [edx+eax]
	mov	DWORD PTR [ecx+8], eax

; 1559 : 			pLight->m_oPosition.nY = pLights[i].oPos.y;

	mov	ecx, DWORD PTR _i$[ebp]
	imul	ecx, 28					; 0000001cH
	mov	edx, DWORD PTR _pLight$102636[ebp]
	mov	eax, DWORD PTR _pLights$[ebp]
	mov	ecx, DWORD PTR [eax+ecx+4]
	mov	DWORD PTR [edx+12], ecx

; 1560 : 			pLight->m_oPosition.nZ = pLights[i].oPos.z;

	mov	edx, DWORD PTR _i$[ebp]
	imul	edx, 28					; 0000001cH
	mov	eax, DWORD PTR _pLight$102636[ebp]
	mov	ecx, DWORD PTR _pLights$[ebp]
	mov	edx, DWORD PTR [ecx+edx+8]
	mov	DWORD PTR [eax+16], edx

; 1561 : 			pLight->m_nRadius = nMinRange + g_Random(nMaxRange - nMinRange);

	mov	eax, DWORD PTR _nMaxRange$[ebp]
	sub	eax, DWORD PTR _nMinRange$[ebp]
	push	eax
	call	DWORD PTR __imp_?g_Random@@YAII@Z
	add	esp, 4
	mov	ecx, DWORD PTR _nMinRange$[ebp]
	add	ecx, eax
	mov	edx, DWORD PTR _pLight$102636[ebp]
	mov	DWORD PTR [edx+24], ecx

; 1562 : 			pLight->fRadius = (float)(pLight->m_nRadius);

	mov	eax, DWORD PTR _pLight$102636[ebp]
	fild	DWORD PTR [eax+24]
	mov	ecx, DWORD PTR _pLight$102636[ebp]
	fstp	DWORD PTR [ecx+36]

; 1563 : 			pLight->m_pParent = NULL;

	mov	edx, DWORD PTR _pLight$102636[ebp]
	mov	DWORD PTR [edx+4], 0

; 1564 : 			pLight->fMaxRange = (float)nMaxRange;

	fild	DWORD PTR _nMaxRange$[ebp]
	mov	eax, DWORD PTR _pLight$102636[ebp]
	fstp	DWORD PTR [eax+32]

; 1565 : 			pLight->fMinRange = (float)nMinRange;

	fild	DWORD PTR _nMinRange$[ebp]
	mov	ecx, DWORD PTR _pLight$102636[ebp]
	fstp	DWORD PTR [ecx+28]

; 1566 : 			pLight->fCycle = 100 * 0.875f + g_Random(60 / 4);

	push	15					; 0000000fH
	call	DWORD PTR __imp_?g_Random@@YAII@Z
	add	esp, 4
	mov	DWORD PTR -56+[ebp], eax
	mov	DWORD PTR -56+[ebp+4], 0
	fild	QWORD PTR -56+[ebp]
	fadd	DWORD PTR __real@4@4005af00000000000000
	mov	edx, DWORD PTR _pLight$102636[ebp]
	fstp	DWORD PTR [edx+40]

; 1567 : 			m_LightList.push_back(pLight);

	mov	eax, DWORD PTR _pLight$102636[ebp]
	mov	DWORD PTR $T103246[ebp], eax
	lea	ecx, DWORD PTR $T103246[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::push_back
$L102641:

; 1569 : 	}

	jmp	$L102634
$L102635:
$L102628:

; 1570 : }

	mov	ecx, DWORD PTR __$EHRec$[ebp]
	mov	DWORD PTR fs:__except_list, ecx
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	8
_TEXT	ENDS
text$x	SEGMENT
$L103247:
	mov	eax, DWORD PTR $T103243[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L103252:
	mov	eax, OFFSET FLAT:$T103249
	jmp	___CxxFrameHandler
text$x	ENDS
?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z ENDP ; KIpoTree::AddBuildinLight
PUBLIC	?EnableBioLights@KIpoTree@@QAEX_N@Z		; KIpoTree::EnableBioLights
_TEXT	SEGMENT
_bEnable$ = 8
_this$ = -24
_i$102656 = -4
$T103255 = -8
$T103256 = -12
$T103257 = -16
$T103258 = -20
?EnableBioLights@KIpoTree@@QAEX_N@Z PROC NEAR		; KIpoTree::EnableBioLights

; 1573 : {

	push	ebp
	mov	ebp, esp
	sub	esp, 24					; 00000018H
	mov	DWORD PTR _this$[ebp], ecx

; 1574 : 	m_bProcessBioLights = bEnable;

	mov	eax, DWORD PTR _this$[ebp]
	mov	cl, BYTE PTR _bEnable$[ebp]
	mov	BYTE PTR [eax+89], cl

; 1575 : 
; 1576 : 	if(!bEnable)

	mov	edx, DWORD PTR _bEnable$[ebp]
	and	edx, 255				; 000000ffH
	test	edx, edx
	jne	$L102661

; 1578 : 		//清除所有内建对象的光源
; 1579 : 		list<KLightBase*>::iterator i;

	lea	ecx, DWORD PTR _i$102656[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator

; 1580 : 		for (i = m_LightList.begin(); i != m_LightList.end(); )

	lea	eax, DWORD PTR $T103255[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR _i$102656[ebp], ecx
$L102660:
	lea	edx, DWORD PTR $T103256[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	push	eax
	lea	ecx, DWORD PTR _i$102656[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	SHORT $L102661

; 1582 : 			if((*i)->m_pParent == NULL)

	lea	ecx, DWORD PTR _i$102656[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	eax, DWORD PTR [eax]
	cmp	DWORD PTR [eax+4], 0
	jne	SHORT $L102662

; 1584 : 				delete (*i);

	lea	ecx, DWORD PTR _i$102656[ebp]
	call	??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR $T103257[ebp], ecx
	mov	edx, DWORD PTR $T103257[ebp]
	push	edx
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 1585 : 				i = m_LightList.erase(i);

	mov	eax, DWORD PTR _i$102656[ebp]
	push	eax
	lea	ecx, DWORD PTR $T103258[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	call	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR _i$102656[ebp], edx

; 1586 : 				continue;

	jmp	SHORT $L102660
$L102662:

; 1588 : 			++i;

	lea	ecx, DWORD PTR _i$102656[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++

; 1589 : 		}

	jmp	SHORT $L102660
$L102661:

; 1591 : }

	mov	esp, ebp
	pop	ebp
	ret	4
?EnableBioLights@KIpoTree@@QAEX_N@Z ENDP		; KIpoTree::EnableBioLights
_TEXT	ENDS
PUBLIC	?EnableDynamicLights@KIpoTree@@QAEX_N@Z		; KIpoTree::EnableDynamicLights
_TEXT	SEGMENT
_bEnable$ = 8
_this$ = -4
?EnableDynamicLights@KIpoTree@@QAEX_N@Z PROC NEAR	; KIpoTree::EnableDynamicLights

; 1594 : {

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 1595 : 	m_bDynamicLighting = bEnable;

	mov	eax, DWORD PTR _this$[ebp]
	mov	cl, BYTE PTR _bEnable$[ebp]
	mov	BYTE PTR [eax+90], cl

; 1596 : }

	mov	esp, ebp
	pop	ebp
	ret	4
?EnableDynamicLights@KIpoTree@@QAEX_N@Z ENDP		; KIpoTree::EnableDynamicLights
_TEXT	ENDS
PUBLIC	?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Buynode
;	COMDAT ??0?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@ABV?$allocator@PAVKLightBase@@@1@@Z
_TEXT	SEGMENT
_this$ = -4
__Al$ = 8
??0?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@ABV?$allocator@PAVKLightBase@@@1@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::list<KLightBase *,std::allocator<KLightBase *> >, COMDAT

; 131  : 		_Head(_Buynode()), _Size(0) {}

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __Al$[ebp]
	mov	cl, BYTE PTR [eax]
	mov	edx, DWORD PTR _this$[ebp]
	mov	BYTE PTR [edx], cl
	push	0
	push	0
	mov	ecx, DWORD PTR _this$[ebp]
	call	?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Buynode
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+4], eax
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+8], 0
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??0?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@ABV?$allocator@PAVKLightBase@@@1@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::list<KLightBase *,std::allocator<KLightBase *> >
_TEXT	ENDS
PUBLIC	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
PUBLIC	?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Freenode
;	COMDAT ??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT
_this$ = -16
$T103263 = -4
$T103264 = -8
$T103265 = -12
??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >, COMDAT

; 151  : 		{erase(begin(), end());

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx
	lea	eax, DWORD PTR $T103263[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	mov	ecx, DWORD PTR [eax]
	push	ecx
	lea	edx, DWORD PTR $T103264[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	eax, DWORD PTR [eax]
	push	eax
	lea	ecx, DWORD PTR $T103265[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase

; 152  : 		_Freenode(_Head);

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+4]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Freenode

; 153  : 		_Head = 0, _Size = 0; }

	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+4], 0
	mov	edx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [edx+8], 0
	mov	esp, ebp
	pop	ebp
	ret	0
??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
_TEXT	ENDS
PUBLIC	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
PUBLIC	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
;	COMDAT ?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ
_TEXT	SEGMENT
___$ReturnUdt$ = 8
_this$ = -8
$T103268 = -4
?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin, COMDAT

; 166  : 		{return (iterator(_Acc::_Next(_Head))); }

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+4]
	push	ecx
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	edx, DWORD PTR [eax]
	push	edx
	lea	ecx, DWORD PTR $T103268[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR ___$ReturnUdt$[ebp]
	mov	DWORD PTR [ecx], eax
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
_TEXT	ENDS
;	COMDAT ?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ
_TEXT	SEGMENT
___$ReturnUdt$ = 8
_this$ = -8
$T103271 = -4
?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::end, COMDAT

; 170  : 		{return (iterator(_Head)); }

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax+4]
	push	ecx
	lea	ecx, DWORD PTR $T103271[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
	mov	edx, DWORD PTR [eax]
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	mov	DWORD PTR [eax], edx
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
_TEXT	ENDS
;	COMDAT ?size@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEIXZ
_TEXT	SEGMENT
_this$ = -4
?size@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEIXZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::size, COMDAT

; 188  : 		{return (_Size); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax+8]
	mov	esp, ebp
	pop	ebp
	ret	0
?size@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEIXZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::size
_TEXT	ENDS
PUBLIC	?insert@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@ABQAVKLightBase@@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::insert
;	COMDAT ?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z
_TEXT	SEGMENT
_this$ = -12
__X$ = 8
$T103276 = -4
$T103277 = -8
?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::push_back, COMDAT

; 208  : 		{insert(end(), _X); }

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __X$[ebp]
	push	eax
	lea	ecx, DWORD PTR $T103276[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	mov	edx, DWORD PTR [eax]
	push	edx
	lea	eax, DWORD PTR $T103277[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?insert@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@ABQAVKLightBase@@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::insert
	mov	esp, ebp
	pop	ebp
	ret	4
?push_back@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXABQAVKLightBase@@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::push_back
_TEXT	ENDS
PUBLIC	?destroy@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@@Z ; std::allocator<KLightBase *>::destroy
PUBLIC	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
PUBLIC	?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Value
PUBLIC	?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::_Mynode
PUBLIC	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
;	COMDAT ?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z
_TEXT	SEGMENT
___$ReturnUdt$ = 8
_this$ = -12
__S$ = -4
__P$ = 12
$T103280 = -8
?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase, COMDAT

; 235  : 		{_Nodeptr _S = (_P++)._Mynode();

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	push	esi
	mov	DWORD PTR _this$[ebp], ecx
	push	0
	lea	eax, DWORD PTR $T103280[ebp]
	push	eax
	lea	ecx, DWORD PTR __P$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
	mov	ecx, eax
	call	?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::_Mynode
	mov	DWORD PTR __S$[ebp], eax

; 236  : 		_Acc::_Next(_Acc::_Prev(_S)) = _Acc::_Next(_S);

	mov	ecx, DWORD PTR __S$[ebp]
	push	ecx
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	esi, eax
	mov	edx, DWORD PTR __S$[ebp]
	push	edx
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	eax, DWORD PTR [eax]
	push	eax
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	ecx, DWORD PTR [esi]
	mov	DWORD PTR [eax], ecx

; 237  : 		_Acc::_Prev(_Acc::_Next(_S)) = _Acc::_Prev(_S);

	mov	edx, DWORD PTR __S$[ebp]
	push	edx
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	esi, eax
	mov	eax, DWORD PTR __S$[ebp]
	push	eax
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	ecx, DWORD PTR [eax]
	push	ecx
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	edx, DWORD PTR [esi]
	mov	DWORD PTR [eax], edx

; 238  : 		allocator.destroy(&_Acc::_Value(_S));

	mov	eax, DWORD PTR __S$[ebp]
	push	eax
	call	?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Value
	add	esp, 4
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?destroy@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@@Z ; std::allocator<KLightBase *>::destroy

; 239  : 		_Freenode(_S);

	mov	ecx, DWORD PTR __S$[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Freenode

; 240  : 		--_Size;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+8]
	sub	eax, 1
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+8], eax

; 241  : 		return (_P); }

	mov	edx, DWORD PTR ___$ReturnUdt$[ebp]
	mov	eax, DWORD PTR __P$[ebp]
	mov	DWORD PTR [edx], eax
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	8
?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
_TEXT	ENDS
;	COMDAT ?clear@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXXZ
_TEXT	SEGMENT
_this$ = -16
$T103283 = -4
$T103284 = -8
$T103285 = -12
?clear@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXXZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::clear, COMDAT

; 247  : 		{erase(begin(), end()); }

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx
	lea	eax, DWORD PTR $T103283[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?end@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::end
	mov	ecx, DWORD PTR [eax]
	push	ecx
	lea	edx, DWORD PTR $T103284[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?begin@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::begin
	mov	eax, DWORD PTR [eax]
	push	eax
	lea	ecx, DWORD PTR $T103285[ebp]
	push	ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
	mov	esp, ebp
	pop	ebp
	ret	0
?clear@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEXXZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::clear
_TEXT	ENDS
PUBLIC	??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator
;	COMDAT ??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator, COMDAT

; 96   : 		iterator()

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	ecx, DWORD PTR _this$[ebp]
	call	??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
_TEXT	ENDS
;	COMDAT ??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ
_TEXT	SEGMENT
_this$ = -4
??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*, COMDAT

; 101  : 			{return (_Acc::_Value(_Ptr)); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax]
	push	ecx
	call	?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Value
	add	esp, 4
	mov	esp, ebp
	pop	ebp
	ret	0
??Diterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEAAPAVKLightBase@@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator*
_TEXT	ENDS
;	COMDAT ??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ
_TEXT	SEGMENT
_this$ = -4
??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++, COMDAT

; 105  : 			{_Ptr = _Acc::_Next(_Ptr);

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax]
	push	ecx
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	DWORD PTR [edx], eax

; 106  : 			return (*this); }

	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
_TEXT	ENDS
PUBLIC	??8iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator==
;	COMDAT ??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z
_TEXT	SEGMENT
_this$ = -4
__X$ = 8
??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=, COMDAT

; 121  : 			{return (!(*this == _X)); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __X$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	??8iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator==
	and	eax, 255				; 000000ffH
	neg	eax
	sbb	eax, eax
	inc	eax
	mov	esp, ebp
	pop	ebp
	ret	4
??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
_TEXT	ENDS
PUBLIC	?construct@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@ABQAV3@@Z ; std::allocator<KLightBase *>::construct
;	COMDAT ?insert@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@ABQAVKLightBase@@@Z
_TEXT	SEGMENT
___$ReturnUdt$ = 8
_this$ = -12
__S$ = -4
__P$ = 12
__X$ = 16
$T103296 = -8
?insert@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@ABQAVKLightBase@@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::insert, COMDAT

; 218  : 		{_Nodeptr _S = _P._Mynode();

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	push	esi
	mov	DWORD PTR _this$[ebp], ecx
	lea	ecx, DWORD PTR __P$[ebp]
	call	?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::_Mynode
	mov	DWORD PTR __S$[ebp], eax

; 219  : 		_Acc::_Prev(_S) = _Buynode(_S, _Acc::_Prev(_S));

	mov	eax, DWORD PTR __S$[ebp]
	push	eax
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	ecx, DWORD PTR [eax]
	push	ecx
	mov	edx, DWORD PTR __S$[ebp]
	push	edx
	mov	ecx, DWORD PTR _this$[ebp]
	call	?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Buynode
	mov	esi, eax
	mov	eax, DWORD PTR __S$[ebp]
	push	eax
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	DWORD PTR [eax], esi

; 220  : 		_S = _Acc::_Prev(_S);

	mov	ecx, DWORD PTR __S$[ebp]
	push	ecx
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR __S$[ebp], edx

; 221  : 		_Acc::_Next(_Acc::_Prev(_S)) = _S;

	mov	eax, DWORD PTR __S$[ebp]
	push	eax
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	ecx, DWORD PTR [eax]
	push	ecx
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	edx, DWORD PTR __S$[ebp]
	mov	DWORD PTR [eax], edx

; 222  : 		allocator.construct(&_Acc::_Value(_S), _X);

	mov	eax, DWORD PTR __X$[ebp]
	push	eax
	mov	ecx, DWORD PTR __S$[ebp]
	push	ecx
	call	?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Value
	add	esp, 4
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?construct@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@ABQAV3@@Z ; std::allocator<KLightBase *>::construct

; 223  : 		++_Size;

	mov	edx, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [edx+8]
	add	eax, 1
	mov	ecx, DWORD PTR _this$[ebp]
	mov	DWORD PTR [ecx+8], eax

; 224  : 		return (iterator(_S)); }

	mov	edx, DWORD PTR __S$[ebp]
	push	edx
	lea	ecx, DWORD PTR $T103296[ebp]
	call	??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR ___$ReturnUdt$[ebp]
	mov	DWORD PTR [ecx], eax
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	12					; 0000000cH
?insert@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@ABQAVKLightBase@@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::insert
_TEXT	ENDS
;	COMDAT ?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z
_TEXT	SEGMENT
___$ReturnUdt$ = 8
_this$ = -12
__F$ = 12
__L$ = 16
$T103299 = -4
$T103300 = -8
?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase, COMDAT

; 243  : 		{while (_F != _L)

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	mov	DWORD PTR _this$[ebp], ecx
$L102752:
	lea	eax, DWORD PTR __L$[ebp]
	push	eax
	lea	ecx, DWORD PTR __F$[ebp]
	call	??9iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator!=
	and	eax, 255				; 000000ffH
	test	eax, eax
	je	SHORT $L102753

; 244  : 			erase(_F++);

	push	0
	lea	ecx, DWORD PTR $T103299[ebp]
	push	ecx
	lea	ecx, DWORD PTR __F$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
	mov	edx, DWORD PTR [eax]
	push	edx
	lea	eax, DWORD PTR $T103300[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
	jmp	SHORT $L102752
$L102753:

; 245  : 		return (_F); }

	mov	ecx, DWORD PTR ___$ReturnUdt$[ebp]
	mov	edx, DWORD PTR __F$[ebp]
	mov	DWORD PTR [ecx], edx
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	12					; 0000000cH
?erase@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AViterator@12@V312@0@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::erase
_TEXT	ENDS
PUBLIC	?_Charalloc@?$allocator@PAVKLightBase@@@std@@QAEPADI@Z ; std::allocator<KLightBase *>::_Charalloc
;	COMDAT ?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z
_TEXT	SEGMENT
_this$ = -8
__S$ = -4
__Narg$ = 8
__Parg$ = 12
?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Buynode, COMDAT

; 386  : 		{_Nodeptr _S = (_Nodeptr)allocator._Charalloc(

	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	mov	DWORD PTR _this$[ebp], ecx

; 387  : 			1 * sizeof (_Node));

	push	12					; 0000000cH
	mov	ecx, DWORD PTR _this$[ebp]
	call	?_Charalloc@?$allocator@PAVKLightBase@@@std@@QAEPADI@Z ; std::allocator<KLightBase *>::_Charalloc
	mov	DWORD PTR __S$[ebp], eax

; 388  : 		_Acc::_Next(_S) = _Narg != 0 ? _Narg : _S;

	cmp	DWORD PTR __Narg$[ebp], 0
	je	SHORT $L103303
	mov	eax, DWORD PTR __Narg$[ebp]
	mov	DWORD PTR -12+[ebp], eax
	jmp	SHORT $L103304
$L103303:
	mov	ecx, DWORD PTR __S$[ebp]
	mov	DWORD PTR -12+[ebp], ecx
$L103304:
	mov	edx, DWORD PTR __S$[ebp]
	push	edx
	call	?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
	add	esp, 4
	mov	ecx, DWORD PTR -12+[ebp]
	mov	DWORD PTR [eax], ecx

; 389  : 		_Acc::_Prev(_S) = _Parg != 0 ? _Parg : _S;

	cmp	DWORD PTR __Parg$[ebp], 0
	je	SHORT $L103305
	mov	edx, DWORD PTR __Parg$[ebp]
	mov	DWORD PTR -16+[ebp], edx
	jmp	SHORT $L103306
$L103305:
	mov	eax, DWORD PTR __S$[ebp]
	mov	DWORD PTR -16+[ebp], eax
$L103306:
	mov	ecx, DWORD PTR __S$[ebp]
	push	ecx
	call	?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
	add	esp, 4
	mov	edx, DWORD PTR -16+[ebp]
	mov	DWORD PTR [eax], edx

; 390  : 		return (_S); }

	mov	eax, DWORD PTR __S$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	8
?_Buynode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEPAU_Node@12@PAU312@0@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Buynode
_TEXT	ENDS
PUBLIC	?deallocate@?$allocator@PAVKLightBase@@@std@@QAEXPAXI@Z ; std::allocator<KLightBase *>::deallocate
;	COMDAT ?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z
_TEXT	SEGMENT
_this$ = -4
__S$ = 8
?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Freenode, COMDAT

; 392  : 		{allocator.deallocate(_S, 1); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	push	1
	mov	eax, DWORD PTR __S$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	?deallocate@?$allocator@PAVKLightBase@@@std@@QAEXPAXI@Z ; std::allocator<KLightBase *>::deallocate
	mov	esp, ebp
	pop	ebp
	ret	4
?_Freenode@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@IAEXPAU_Node@12@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Freenode
_TEXT	ENDS
;	COMDAT ?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z
_TEXT	SEGMENT
__P$ = 8
?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next, COMDAT

; 37   : 			{return ((_Nodepref)(*_P)._Next); }

	push	ebp
	mov	ebp, esp
	mov	eax, DWORD PTR __P$[ebp]
	pop	ebp
	ret	0
?_Next@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Next
_TEXT	ENDS
;	COMDAT ?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z
_TEXT	SEGMENT
__P$ = 8
?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev, COMDAT

; 39   : 			{return ((_Nodepref)(*_P)._Prev); }

	push	ebp
	mov	ebp, esp
	mov	eax, DWORD PTR __P$[ebp]
	add	eax, 4
	pop	ebp
	ret	0
?_Prev@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAU_Node@23@PAU423@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Prev
_TEXT	ENDS
;	COMDAT ?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z
_TEXT	SEGMENT
__P$ = 8
?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Value, COMDAT

; 41   : 			{return ((_Vref)(*_P)._Value); }

	push	ebp
	mov	ebp, esp
	mov	eax, DWORD PTR __P$[ebp]
	add	eax, 8
	pop	ebp
	ret	0
?_Value@_Acc@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@SAAAPAVKLightBase@@PAU_Node@23@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::_Acc::_Value
_TEXT	ENDS
PUBLIC	?_Destroy@std@@YAXPAPAVKLightBase@@@Z		; std::_Destroy
;	COMDAT ?destroy@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@@Z
_TEXT	SEGMENT
_this$ = -4
__P$ = 8
?destroy@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@@Z PROC NEAR ; std::allocator<KLightBase *>::destroy, COMDAT

; 68   : 		{_Destroy(_P); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __P$[ebp]
	push	eax
	call	?_Destroy@std@@YAXPAPAVKLightBase@@@Z	; std::_Destroy
	add	esp, 4
	mov	esp, ebp
	pop	ebp
	ret	4
?destroy@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@@Z ENDP ; std::allocator<KLightBase *>::destroy
_TEXT	ENDS
;	COMDAT ??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT
_this$ = -4
??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator, COMDAT

; 59   : 		const_iterator()

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	0
??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator
_TEXT	ENDS
;	COMDAT ?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ
_TEXT	SEGMENT
_this$ = -4
?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::_Mynode, COMDAT

; 88   : 			{return (_Ptr); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	esp, ebp
	pop	ebp
	ret	0
?_Mynode@const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBEPAU_Node@23@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::_Mynode
_TEXT	ENDS
PUBLIC	??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator
;	COMDAT ??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z
_TEXT	SEGMENT
_this$ = -4
__P$ = 8
??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator, COMDAT

; 98   : 		iterator(_Nodeptr _P)

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __P$[ebp]
	push	eax
	mov	ecx, DWORD PTR _this$[ebp]
	call	??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??0iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::iterator
_TEXT	ENDS
;	COMDAT ??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z
_TEXT	SEGMENT
___$ReturnUdt$ = 8
_this$ = -8
__Tmp$ = -4
??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++, COMDAT

; 108  : 			{iterator _Tmp = *this;

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax]
	mov	DWORD PTR __Tmp$[ebp], ecx

; 109  : 			++*this;

	mov	ecx, DWORD PTR _this$[ebp]
	call	??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAEAAV012@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++

; 110  : 			return (_Tmp); }

	mov	edx, DWORD PTR ___$ReturnUdt$[ebp]
	mov	eax, DWORD PTR __Tmp$[ebp]
	mov	DWORD PTR [edx], eax
	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	8
??Eiterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE?AV012@H@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator++
_TEXT	ENDS
;	COMDAT ??8iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z
_TEXT	SEGMENT
_this$ = -4
__X$ = 8
??8iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator==, COMDAT

; 119  : 			{return (_Ptr == _X._Ptr); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR __X$[ebp]
	mov	edx, DWORD PTR [eax]
	xor	eax, eax
	cmp	edx, DWORD PTR [ecx]
	sete	al
	mov	esp, ebp
	pop	ebp
	ret	4
??8iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QBE_NABV012@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::iterator::operator==
_TEXT	ENDS
PUBLIC	?_Allocate@std@@YAPADHPAD@Z			; std::_Allocate
;	COMDAT ?_Charalloc@?$allocator@PAVKLightBase@@@std@@QAEPADI@Z
_TEXT	SEGMENT
_this$ = -4
__N$ = 8
?_Charalloc@?$allocator@PAVKLightBase@@@std@@QAEPADI@Z PROC NEAR ; std::allocator<KLightBase *>::_Charalloc, COMDAT

; 61   : 		{return (_Allocate((difference_type)_N,

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 62   : 			(char _FARQ *)0)); }

	push	0
	mov	eax, DWORD PTR __N$[ebp]
	push	eax
	call	?_Allocate@std@@YAPADHPAD@Z		; std::_Allocate
	add	esp, 8
	mov	esp, ebp
	pop	ebp
	ret	4
?_Charalloc@?$allocator@PAVKLightBase@@@std@@QAEPADI@Z ENDP ; std::allocator<KLightBase *>::_Charalloc
_TEXT	ENDS
;	COMDAT ?deallocate@?$allocator@PAVKLightBase@@@std@@QAEXPAXI@Z
_TEXT	SEGMENT
_this$ = -4
__P$ = 8
?deallocate@?$allocator@PAVKLightBase@@@std@@QAEXPAXI@Z PROC NEAR ; std::allocator<KLightBase *>::deallocate, COMDAT

; 64   : 		{operator delete(_P); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __P$[ebp]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
	mov	esp, ebp
	pop	ebp
	ret	8
?deallocate@?$allocator@PAVKLightBase@@@std@@QAEXPAXI@Z ENDP ; std::allocator<KLightBase *>::deallocate
_TEXT	ENDS
PUBLIC	?_Construct@std@@YAXPAPAVKLightBase@@ABQAV2@@Z	; std::_Construct
;	COMDAT ?construct@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@ABQAV3@@Z
_TEXT	SEGMENT
_this$ = -4
__P$ = 8
__V$ = 12
?construct@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@ABQAV3@@Z PROC NEAR ; std::allocator<KLightBase *>::construct, COMDAT

; 66   : 		{_Construct(_P, _V); }

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR __V$[ebp]
	push	eax
	mov	ecx, DWORD PTR __P$[ebp]
	push	ecx
	call	?_Construct@std@@YAXPAPAVKLightBase@@ABQAV2@@Z ; std::_Construct
	add	esp, 8
	mov	esp, ebp
	pop	ebp
	ret	8
?construct@?$allocator@PAVKLightBase@@@std@@QAEXPAPAVKLightBase@@ABQAV3@@Z ENDP ; std::allocator<KLightBase *>::construct
_TEXT	ENDS
;	COMDAT ??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z
_TEXT	SEGMENT
_this$ = -4
__P$ = 8
??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator, COMDAT

; 61   : 		const_iterator(_Nodeptr _P)

	push	ebp
	mov	ebp, esp
	push	ecx
	mov	DWORD PTR _this$[ebp], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR __P$[ebp]
	mov	DWORD PTR [eax], ecx
	mov	eax, DWORD PTR _this$[ebp]
	mov	esp, ebp
	pop	ebp
	ret	4
??0const_iterator@?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@PAU_Node@12@@Z ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::const_iterator::const_iterator
_TEXT	ENDS
;	COMDAT ?_Destroy@std@@YAXPAPAVKLightBase@@@Z
_TEXT	SEGMENT
?_Destroy@std@@YAXPAPAVKLightBase@@@Z PROC NEAR		; std::_Destroy, COMDAT

; 38   : 	{_DESTRUCTOR(_Ty, _P); }

	push	ebp
	mov	ebp, esp
	pop	ebp
	ret	0
?_Destroy@std@@YAXPAPAVKLightBase@@@Z ENDP		; std::_Destroy
_TEXT	ENDS
;	COMDAT ?_Allocate@std@@YAPADHPAD@Z
_TEXT	SEGMENT
__N$ = 8
?_Allocate@std@@YAPADHPAD@Z PROC NEAR			; std::_Allocate, COMDAT

; 27   : 	{if (_N < 0)

	push	ebp
	mov	ebp, esp
	cmp	DWORD PTR __N$[ebp], 0
	jge	SHORT $L102880

; 28   : 		_N = 0;

	mov	DWORD PTR __N$[ebp], 0
$L102880:

; 30   : 		(_SIZT)_N * sizeof (_Ty))); }

	mov	eax, DWORD PTR __N$[ebp]
	push	eax
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	pop	ebp
	ret	0
?_Allocate@std@@YAPADHPAD@Z ENDP			; std::_Allocate
_TEXT	ENDS
PUBLIC	??2@YAPAXIPAX@Z					; operator new
;	COMDAT ?_Construct@std@@YAXPAPAVKLightBase@@ABQAV2@@Z
_TEXT	SEGMENT
__P$ = 8
__V$ = 12
$T103341 = -4
?_Construct@std@@YAXPAPAVKLightBase@@ABQAV2@@Z PROC NEAR ; std::_Construct, COMDAT

; 34   : 	{new ((void _FARQ *)_P) _T1(_V); }

	push	ebp
	mov	ebp, esp
	sub	esp, 8
	mov	eax, DWORD PTR __P$[ebp]
	push	eax
	push	4
	call	??2@YAPAXIPAX@Z				; operator new
	add	esp, 8
	mov	DWORD PTR $T103341[ebp], eax
	cmp	DWORD PTR $T103341[ebp], 0
	je	SHORT $L103342
	mov	ecx, DWORD PTR $T103341[ebp]
	mov	edx, DWORD PTR __V$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	DWORD PTR [ecx], eax
	mov	ecx, DWORD PTR $T103341[ebp]
	mov	DWORD PTR -8+[ebp], ecx
	jmp	SHORT $L103343
$L103342:
	mov	DWORD PTR -8+[ebp], 0
$L103343:
	mov	esp, ebp
	pop	ebp
	ret	0
?_Construct@std@@YAXPAPAVKLightBase@@ABQAV2@@Z ENDP	; std::_Construct
_TEXT	ENDS
;	COMDAT ??2@YAPAXIPAX@Z
_TEXT	SEGMENT
__P$ = 12
??2@YAPAXIPAX@Z PROC NEAR				; operator new, COMDAT

; 42   : 	{return (_P); }

	push	ebp
	mov	ebp, esp
	mov	eax, DWORD PTR __P$[ebp]
	pop	ebp
	ret	0
??2@YAPAXIPAX@Z ENDP					; operator new
_TEXT	ENDS
END
