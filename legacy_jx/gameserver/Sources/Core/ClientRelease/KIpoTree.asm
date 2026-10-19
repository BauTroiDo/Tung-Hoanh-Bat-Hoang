	TITLE	E:\BaiduYunDownload\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
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
;	COMDAT ??8@YAHABU_GUID@@0@Z
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
;	COMDAT __imp_?GetName@KStrNode@@UAEPADXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?SetName@KStrNode@@UAEXPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetHead@KList@@QAEPAVKNode@@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddTail@KList@@QAEXPAVKNode@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?RemoveHead@KList@@QAEPAVKNode@@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetNodeCount@KList@@QAEJXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKCacheNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1KStrNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KCacheNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?GetMemPtr@KMemClass@@QAEPAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1KITabFile@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1TTabColNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GTTabColNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1TTabLineNode@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GTTabLineNode@@UAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Release@tagSHOW_MSG_SYNC@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1KScriptNode@@UAE@XZ
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
;	COMDAT ?SetColor@KLColor@@QAEXK@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Scale@KLColor@@QAEXM@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetColor@KLColor@@QAEKXZ
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
;	COMDAT ?GetSkillStyle@KSkillManager@@AAEHK@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetSkill@KSkillManager@@QAEPAVISkill@@HH@Z
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
;	COMDAT __imp_?eof@?$char_traits@G@std@@SAGXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?assign@?$char_traits@D@std@@SAXAADABD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?length@?$char_traits@D@std@@SAIPBD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?copy@?$char_traits@D@std@@SAPADPADPBDI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?move@?$char_traits@D@std@@SAPADPADPBDI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?eof@?$char_traits@D@std@@SAHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1bad_exception@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0bad_exception@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1bad_alloc@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0bad_alloc@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??2@YAPAXIPAX@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0logic_error@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@1@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1logic_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?what@logic_error@std@@UBEPBDXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0logic_error@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1domain_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0domain_error@std@@QAE@ABV01@@Z
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
;	COMDAT __imp_??1length_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0length_error@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1out_of_range@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0out_of_range@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0runtime_error@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@1@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1runtime_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0runtime_error@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1overflow_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0overflow_error@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1underflow_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0underflow_error@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1range_error@std@@UAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0range_error@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Breath@KLightBase@@UAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KSelfBreathLight@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KLightBase@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Breath@KSelfBreathLight@@UAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??0KIpoTree@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??1KIpoTree@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Breathe@KIpoTree@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Clear@KIpoTree@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ??_GKIpotBranch@@QAEPAXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Fell@KIpoTree@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?RenderLightMap@KIpoTree@@QAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?CanLighting@KIpoTree@@AAE_NHHHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?EnableBioLights@KIpoTree@@QAEX_N@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?EnableDynamicLights@KIpoTree@@QAEX_N@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@ABV01@@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_??1?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@XZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?c_str@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEPBDXZ
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
;	COMDAT __imp_?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Nullstr@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@CAPBDXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z
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
;	COMDAT __imp_?erase@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@II@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?size@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?capacity@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAE_NI_N@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Refcnt@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEAAEPBD@Z
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
;	COMDAT __imp_?max_size@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@PBD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?allocate@?$allocator@D@std@@QAEPADIPBX@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?max_size@?$allocator@D@std@@QBEIXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __imp_?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@PBDI@Z
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
;	COMDAT ??_7TTabColNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
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
;	COMDAT ??_7TTabLineNode@@6B@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
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
PUBLIC	??0KSelfBreathLight@@QAE@XZ			; KSelfBreathLight::KSelfBreathLight
PUBLIC	?Breath@KSelfBreathLight@@UAEXXZ		; KSelfBreathLight::Breath
PUBLIC	??_7KSelfBreathLight@@6B@			; KSelfBreathLight::`vftable'
EXTRN	__imp__timeGetTime@0:NEAR
;	COMDAT ??_7KSelfBreathLight@@6B@
; File E:\BaiduYunDownload\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
??_7KSelfBreathLight@@6B@ DD FLAT:?Breath@KSelfBreathLight@@UAEXXZ ; KSelfBreathLight::`vftable'
CONST	ENDS
;	COMDAT ??0KSelfBreathLight@@QAE@XZ
_TEXT	SEGMENT
??0KSelfBreathLight@@QAE@XZ PROC NEAR			; KSelfBreathLight::KSelfBreathLight, COMDAT

; 25   : {

	push	esi
	mov	esi, ecx
	mov	DWORD PTR [esi], OFFSET FLAT:??_7KSelfBreathLight@@6B@ ; KSelfBreathLight::`vftable'

; 26   : 	nDir = 1;

	mov	DWORD PTR [esi+44], 1

; 27   : 	dwLastBreathTime = timeGetTime();

	call	DWORD PTR __imp__timeGetTime@0
	mov	DWORD PTR [esi+48], eax

; 28   : }

	mov	eax, esi
	pop	esi
	ret	0
??0KSelfBreathLight@@QAE@XZ ENDP			; KSelfBreathLight::KSelfBreathLight
_TEXT	ENDS
EXTRN	__ftol:NEAR
EXTRN	__fltused:NEAR
;	COMDAT ?Breath@KSelfBreathLight@@UAEXXZ
_TEXT	SEGMENT
?Breath@KSelfBreathLight@@UAEXXZ PROC NEAR		; KSelfBreathLight::Breath, COMDAT

; 31   : {

	sub	esp, 12					; 0000000cH
	push	esi
	push	edi
	mov	esi, ecx

; 32   : 	DWORD dwCurTime = timeGetTime();

	call	DWORD PTR __imp__timeGetTime@0

; 33   : 	
; 34   : 	fRadius += (dwCurTime - dwLastBreathTime) / fCycle * (fMaxRange - fMinRange) * nDir * 2;

	mov	edx, DWORD PTR [esi+48]
	mov	edi, eax
	sub	eax, edx
	mov	DWORD PTR -8+[esp+24], 0
	mov	DWORD PTR -8+[esp+20], eax
	mov	ecx, DWORD PTR [esi+44]
	fild	QWORD PTR -8+[esp+20]
	mov	DWORD PTR -12+[esp+20], ecx

; 35   : 	if(nDir == 1 && fRadius > fMaxRange)

	cmp	ecx, 1
	fdiv	DWORD PTR [esi+40]
	fld	DWORD PTR [esi+32]
	fsub	DWORD PTR [esi+28]
	fmulp	ST(1), ST(0)
	fimul	DWORD PTR -12+[esp+20]
	fadd	ST(0), ST(0)
	fadd	DWORD PTR [esi+36]
	fst	DWORD PTR [esi+36]
	jne	SHORT $L89264
	fcom	DWORD PTR [esi+32]
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L89264

; 36   : 	{
; 37   : 		nDir = -1;
; 38   : 		fRadius = fMaxRange;

	mov	ecx, DWORD PTR [esi+32]
	mov	DWORD PTR [esi+44], -1
	fstp	ST(0)
	mov	DWORD PTR [esi+36], ecx

; 44   : 	}
; 45   : 	m_nRadius = (int)fRadius;

	fld	DWORD PTR [esi+36]
	call	__ftol

; 46   : 	dwLastBreathTime = dwCurTime;

	mov	DWORD PTR [esi+48], edi
	mov	DWORD PTR [esi+24], eax
	pop	edi
	pop	esi

; 47   : }

	add	esp, 12					; 0000000cH
	ret	0
$L89264:

; 39   : 	}
; 40   : 	else if(nDir == -1 && fRadius < fMinRange)

	cmp	ecx, -1
	jne	SHORT $L90341
	fcomp	DWORD PTR [esi+28]
	fnstsw	ax
	test	ah, 1
	je	SHORT $L89266

; 41   : 	{
; 42   : 		nDir = 1;
; 43   : 		fRadius = fMinRange;

	mov	edx, DWORD PTR [esi+28]
	mov	DWORD PTR [esi+44], 1
	mov	DWORD PTR [esi+36], edx

; 44   : 	}
; 45   : 	m_nRadius = (int)fRadius;

	fld	DWORD PTR [esi+36]
	call	__ftol

; 46   : 	dwLastBreathTime = dwCurTime;

	mov	DWORD PTR [esi+48], edi
	mov	DWORD PTR [esi+24], eax
	pop	edi
	pop	esi

; 47   : }

	add	esp, 12					; 0000000cH
	ret	0
$L90341:

; 41   : 	{
; 42   : 		nDir = 1;
; 43   : 		fRadius = fMinRange;

	fstp	ST(0)
$L89266:

; 44   : 	}
; 45   : 	m_nRadius = (int)fRadius;

	fld	DWORD PTR [esi+36]
	call	__ftol

; 46   : 	dwLastBreathTime = dwCurTime;

	mov	DWORD PTR [esi+48], edi
	mov	DWORD PTR [esi+24], eax
	pop	edi
	pop	esi

; 47   : }

	add	esp, 12					; 0000000cH
	ret	0
?Breath@KSelfBreathLight@@UAEXXZ ENDP			; KSelfBreathLight::Breath
_TEXT	ENDS
PUBLIC	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
PUBLIC	??0KIpoTree@@QAE@XZ				; KIpoTree::KIpoTree
EXTRN	??2@YAPAXI@Z:NEAR				; operator new
EXTRN	__except_list:DWORD
EXTRN	___CxxFrameHandler:NEAR
EXTRN	??0KIpotBranch@@QAE@XZ:NEAR			; KIpotBranch::KIpotBranch
EXTRN	??1KIpotBranch@@QAE@XZ:NEAR			; KIpotBranch::~KIpotBranch
;	COMDAT xdata$x
; File C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE\list
xdata$x	SEGMENT
$T90415	DD	019930520H
	DD	02H
	DD	FLAT:$T90418
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T90418	DD	0ffffffffH
	DD	FLAT:$L90349
	DD	00H
	DD	FLAT:$L90350
xdata$x	ENDS
;	COMDAT ??0KIpoTree@@QAE@XZ
_TEXT	SEGMENT
_this$ = -16
$T90346 = -17
__$EHRec$ = -12
??0KIpoTree@@QAE@XZ PROC NEAR				; KIpoTree::KIpoTree, COMDAT

; 51   : {

	push	-1
	push	$L90416
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	sub	esp, 8
	push	ebx
	push	esi
	mov	esi, ecx
	push	edi
	mov	DWORD PTR _this$[esp+32], esi
	lea	ecx, DWORD PTR [esi+4]
	call	??0KIpotBranch@@QAE@XZ			; KIpotBranch::KIpotBranch
	mov	al, BYTE PTR $T90346[esp+32]
	xor	ebx, ebx
	push	12					; 0000000cH
	mov	DWORD PTR __$EHRec$[esp+44], ebx
	mov	BYTE PTR [esi+92272], al
	call	??2@YAPAXI@Z				; operator new
	mov	DWORD PTR [eax], eax
	mov	DWORD PTR [eax+4], eax
	mov	DWORD PTR [esi+92276], eax
	mov	DWORD PTR [esi+92280], ebx
	mov	al, 1

; 52   : 	m_pMainBranch = NULL;
; 53   : 	m_bProcessBioLights = true;
; 54   : 	m_bDynamicLighting = true;
; 55   : 	m_nCurrentTime = 0;
; 56   : 	m_bIsIndoor = false;
; 57   : 	m_dwAmbient = 0xff000000;
; 58   : 	memset(&m_PermanentLeaf, 0, sizeof(m_PermanentLeaf));

	mov	ecx, 6
	mov	BYTE PTR __$EHRec$[esp+44], al
	mov	BYTE PTR [esi+89], al
	mov	BYTE PTR [esi+90], al
	xor	eax, eax
	lea	edi, DWORD PTR [esi+52]
	mov	DWORD PTR [esi], ebx
	mov	DWORD PTR [esi+84], ebx
	mov	BYTE PTR [esi+88], bl
	mov	DWORD PTR [esi+92], -16777216		; ff000000H

; 59   : 
; 60   :     ptrdiff_t ulAddress;
; 61   : 
; 62   :     m_pbyLColorBase = new unsigned char [(sizeof(KLColor) * LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT) + 255];

	push	37119					; 000090ffH
	rep stosd
	call	??2@YAPAXI@Z				; operator new
	mov	DWORD PTR [esi+73828], eax

; 63   :     _ASSERT(m_pbyLColorBase);
; 64   :     ulAddress = (ptrdiff_t)m_pbyLColorBase;
; 65   :     ulAddress = (ulAddress + 255) / 256 * 256;

	add	eax, 255				; 000000ffH
	cdq
	and	edx, 255				; 000000ffH

; 66   :     pLColor = (KLColor *)ulAddress;
; 67   : 
; 68   :     m_pby_ptpBase = new unsigned char [(sizeof(KLColor) * LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT) + 255];

	push	37119					; 000090ffH
	add	eax, edx
	sar	eax, 8
	shl	eax, 8
	mov	DWORD PTR [esi+73824], eax
	call	??2@YAPAXI@Z				; operator new
	mov	DWORD PTR [esi+73836], eax

; 69   :     _ASSERT(m_pby_ptpBase);
; 70   :     ulAddress = (ptrdiff_t)m_pby_ptpBase;
; 71   :     ulAddress = (ulAddress + 255) / 256 * 256;

	add	eax, 255				; 000000ffH
	cdq
	and	edx, 255				; 000000ffH

; 72   :     ptp = (KLColor *)ulAddress;
; 73   : }

	mov	ecx, DWORD PTR __$EHRec$[esp+44]
	add	eax, edx
	add	esp, 12					; 0000000cH
	sar	eax, 8
	shl	eax, 8
	mov	DWORD PTR [esi+73832], eax
	mov	eax, esi
	pop	edi
	pop	esi
	pop	ebx
	mov	DWORD PTR fs:__except_list, ecx
	add	esp, 20					; 00000014H
	ret	0
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$L90349:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	jmp	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
$L90350:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	jmp	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
$L90416:
	mov	eax, OFFSET FLAT:$T90415
	jmp	___CxxFrameHandler
text$x	ENDS
??0KIpoTree@@QAE@XZ ENDP				; KIpoTree::KIpoTree
PUBLIC	??1KIpoTree@@QAE@XZ				; KIpoTree::~KIpoTree
PUBLIC	?Clear@KIpoTree@@QAEXXZ				; KIpoTree::Clear
EXTRN	??3@YAXPAX@Z:NEAR				; operator delete
;	COMDAT xdata$x
; File C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE\xmemory
xdata$x	SEGMENT
$T90739	DD	019930520H
	DD	02H
	DD	FLAT:$T90742
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T90742	DD	0ffffffffH
	DD	FLAT:$L90427
	DD	00H
	DD	FLAT:$L90428
xdata$x	ENDS
;	COMDAT ??1KIpoTree@@QAE@XZ
_TEXT	SEGMENT
_this$ = -16
__$EHRec$ = -12
??1KIpoTree@@QAE@XZ PROC NEAR				; KIpoTree::~KIpoTree, COMDAT

; 77   : {

	push	-1
	push	$L90741
	mov	eax, DWORD PTR fs:__except_list
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	push	ecx
	push	ebx
	push	ebp
	push	esi
	mov	esi, ecx
	push	edi
	mov	DWORD PTR _this$[esp+32], esi
	mov	DWORD PTR __$EHRec$[esp+40], 1

; 78   : 	Clear();

	call	?Clear@KIpoTree@@QAEXXZ			; KIpoTree::Clear

; 79   : 
; 80   :     if (m_pby_ptpBase)

	mov	eax, DWORD PTR [esi+73836]
	xor	ebp, ebp
	cmp	eax, ebp
	je	SHORT $L89289

; 81   :     {
; 82   :         delete []m_pby_ptpBase;

	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 83   :         m_pby_ptpBase = NULL;

	mov	DWORD PTR [esi+73836], ebp

; 84   :         ptp = NULL;

	mov	DWORD PTR [esi+73832], ebp
$L89289:

; 85   :     }
; 86   : 
; 87   :     if (m_pbyLColorBase)

	mov	eax, DWORD PTR [esi+73828]
	cmp	eax, ebp
	je	SHORT $L89291

; 88   :     {
; 89   :         delete []m_pbyLColorBase;

	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 90   :         m_pbyLColorBase = NULL;

	mov	DWORD PTR [esi+73828], ebp

; 91   : 
; 92   :         pLColor = NULL;

	mov	DWORD PTR [esi+73824], ebp
$L89291:

; 93   :     }
; 94   : }

	mov	ebx, DWORD PTR [esi+92276]
	mov	edi, DWORD PTR [ebx]
	cmp	edi, ebx
	je	SHORT $L90731
$L90730:
	mov	eax, edi
	mov	edi, DWORD PTR [edi]
	push	eax
	mov	ecx, DWORD PTR [eax+4]
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR [ecx], edx
	mov	ecx, DWORD PTR [eax]
	mov	edx, DWORD PTR [eax+4]
	mov	DWORD PTR [ecx+4], edx
	call	??3@YAXPAX@Z				; operator delete
	mov	ecx, DWORD PTR [esi+92280]
	add	esp, 4
	dec	ecx
	cmp	edi, ebx
	mov	DWORD PTR [esi+92280], ecx
	jne	SHORT $L90730
$L90731:
	mov	eax, DWORD PTR [esi+92276]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
	lea	ecx, DWORD PTR [esi+4]
	mov	DWORD PTR [esi+92276], ebp
	mov	DWORD PTR [esi+92280], ebp
	mov	DWORD PTR __$EHRec$[esp+40], -1
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	mov	ecx, DWORD PTR __$EHRec$[esp+32]
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx
	mov	DWORD PTR fs:__except_list, ecx
	add	esp, 16					; 00000010H
	ret	0
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$L90427:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 4
	jmp	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
$L90428:
	mov	ecx, DWORD PTR _this$[ebp]
	add	ecx, 92272				; 00016870H
	jmp	??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
$L90741:
	mov	eax, OFFSET FLAT:$T90739
	jmp	___CxxFrameHandler
text$x	ENDS
??1KIpoTree@@QAE@XZ ENDP				; KIpoTree::~KIpoTree
PUBLIC	?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z ; KIpoTree::Paint
PUBLIC	?RenderLightMap@KIpoTree@@QAEXXZ		; KIpoTree::RenderLightMap
EXTRN	?PaintObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@@Z:NEAR ; KIpotBranch::PaintObjectLayer
EXTRN	?PaintNoneObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@H@Z:NEAR ; KIpotBranch::PaintNoneObjectLayer
EXTRN	?g_pRepresent@@3PAUiRepresentShell@@A:DWORD	; g_pRepresent
;	COMDAT ?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z
_TEXT	SEGMENT
_pRepresentArea$ = 8
_eLayer$ = 12
?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z PROC NEAR ; KIpoTree::Paint, COMDAT

; 98   : {

	push	esi
	push	edi

; 99   : 	if(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting)

	mov	edi, DWORD PTR _eLayer$[esp+4]
	mov	esi, ecx
	cmp	edi, 1
	jne	SHORT $L89298
	mov	al, BYTE PTR [esi+90]
	test	al, al
	je	SHORT $L89300

; 100  : 	{
; 101  : 		// 渲染光照图
; 102  : 		RenderLightMap();

	call	?RenderLightMap@KIpoTree@@QAEXXZ	; KIpoTree::RenderLightMap

; 103  : 		// 设置表现模块的光照信息
; 104  : 		g_pRepresent->SetLightInfo(m_nLeftTopX, m_nLeftTopY, (unsigned int*)pLightingArray);

	mov	ecx, DWORD PTR ?g_pRepresent@@3PAUiRepresentShell@@A ; g_pRepresent
	lea	edx, DWORD PTR [esi+73840]
	push	edx
	mov	edx, DWORD PTR [esi+80]
	mov	eax, DWORD PTR [ecx]
	push	edx
	mov	edx, DWORD PTR [esi+76]
	push	edx
	call	DWORD PTR [eax+104]
$L89300:

; 113  : 	}
; 114  : 	else if (m_pMainBranch)

	mov	ecx, DWORD PTR [esi]

; 115  : 		m_pMainBranch->PaintNoneObjectLayer(pRepresentArea, eLayer);

	push	edi
	test	ecx, ecx
	je	SHORT $L89304
	mov	edx, DWORD PTR _pRepresentArea$[esp+8]
	push	edx

; 116  : 	else
; 117  : 		m_DefaultBranch.PaintNoneObjectLayer(pRepresentArea, eLayer);

	call	?PaintNoneObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@H@Z ; KIpotBranch::PaintNoneObjectLayer
	pop	edi
	pop	esi

; 118  : }

	ret	8
$L89298:

; 105  : 	}
; 106  : 	
; 107  : 	if (eLayer == IPOT_RL_OBJECT)

	cmp	edi, 2
	jne	SHORT $L89300

; 108  : 	{
; 109  : 		if (m_pMainBranch)

	mov	ecx, DWORD PTR [esi]
	test	ecx, ecx
	je	SHORT $L89301

; 110  : 			m_pMainBranch->PaintObjectLayer(pRepresentArea);

	mov	eax, DWORD PTR _pRepresentArea$[esp+4]
	push	eax
	call	?PaintObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@@Z ; KIpotBranch::PaintObjectLayer
	pop	edi
	pop	esi

; 118  : }

	ret	8
$L89301:

; 111  : 		else
; 112  : 			m_DefaultBranch.PaintObjectLayer(pRepresentArea);

	mov	ecx, DWORD PTR _pRepresentArea$[esp+4]
	push	ecx
	lea	ecx, DWORD PTR [esi+4]
	call	?PaintObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@@Z ; KIpotBranch::PaintObjectLayer
	pop	edi
	pop	esi

; 118  : }

	ret	8
$L89304:

; 116  : 	else
; 117  : 		m_DefaultBranch.PaintNoneObjectLayer(pRepresentArea, eLayer);

	mov	eax, DWORD PTR _pRepresentArea$[esp+8]
	lea	ecx, DWORD PTR [esi+4]
	push	eax
	call	?PaintNoneObjectLayer@KIpotBranch@@QAEXPAUtagRECT@@H@Z ; KIpotBranch::PaintNoneObjectLayer
	pop	edi
	pop	esi

; 118  : }

	ret	8
?Paint@KIpoTree@@QAEXPAUtagRECT@@W4IPOT_RENDER_LAYER@@@Z ENDP ; KIpoTree::Paint
_TEXT	ENDS
PUBLIC	?Breathe@KIpoTree@@QAEXXZ			; KIpoTree::Breathe
;	COMDAT ?Breathe@KIpoTree@@QAEXXZ
_TEXT	SEGMENT
?Breathe@KIpoTree@@QAEXXZ PROC NEAR			; KIpoTree::Breathe, COMDAT

; 121  : {

	push	esi
	push	edi
	mov	edi, ecx

; 122  : 	list<KLightBase*>::iterator i;
; 123  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	mov	eax, DWORD PTR [edi+92276]
	mov	esi, DWORD PTR [eax]
	cmp	esi, eax
	je	SHORT $L89315
$L90793:

; 124  : 	{
; 125  : 		if((*i)->m_pParent == NULL)

	mov	ecx, DWORD PTR [esi+8]
	mov	eax, DWORD PTR [ecx+4]
	test	eax, eax
	jne	SHORT $L89314

; 126  : 			(*i)->Breath();

	mov	eax, DWORD PTR [ecx]
	call	DWORD PTR [eax]
$L89314:
	mov	esi, DWORD PTR [esi]
	mov	eax, DWORD PTR [edi+92276]
	cmp	esi, eax
	jne	SHORT $L90793
$L89315:
	pop	edi
	pop	esi

; 127  : 	}
; 128  : }

	ret	0
?Breathe@KIpoTree@@QAEXXZ ENDP				; KIpoTree::Breathe
_TEXT	ENDS
PUBLIC	?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ ; KIpoTree::RemoveRtoGroupWithPermanentLeaf
EXTRN	?Clear@KIpotBranch@@QAEXXZ:NEAR			; KIpotBranch::Clear
;	COMDAT ?Clear@KIpoTree@@QAEXXZ
_TEXT	SEGMENT
?Clear@KIpoTree@@QAEXXZ PROC NEAR			; KIpoTree::Clear, COMDAT

; 132  : {

	push	ebx
	push	esi
	push	edi
	mov	edi, ecx

; 133  : 	if (m_pMainBranch)

	xor	ebx, ebx
	mov	esi, DWORD PTR [edi]
	cmp	esi, ebx
	je	SHORT $L89320

; 134  : 	{
; 135  : 		delete (m_pMainBranch);

	mov	ecx, esi
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	push	esi
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 136  : 		m_pMainBranch = NULL;

	mov	DWORD PTR [edi], ebx
$L89320:

; 137  : 	}
; 138  : 	m_DefaultBranch.Clear();

	lea	ecx, DWORD PTR [edi+4]
	call	?Clear@KIpotBranch@@QAEXXZ		; KIpotBranch::Clear

; 139  : 	RemoveRtoGroupWithPermanentLeaf();

	mov	ecx, edi
	call	?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ ; KIpoTree::RemoveRtoGroupWithPermanentLeaf

; 140  : 	
; 141  : 	//clear all dyna light
; 142  : 	list<KLightBase*>::iterator i;
; 143  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	mov	eax, DWORD PTR [edi+92276]
	mov	esi, DWORD PTR [eax]
	cmp	esi, eax
	je	SHORT $L89329
$L90888:

; 144  : 	{
; 145  : 		delete (*i);

	mov	eax, DWORD PTR [esi+8]
	push	eax
	call	??3@YAXPAX@Z				; operator delete

; 146  :         *i=NULL;

	mov	DWORD PTR [esi+8], ebx
	mov	esi, DWORD PTR [esi]
	mov	eax, DWORD PTR [edi+92276]
	add	esp, 4
	cmp	esi, eax
	jne	SHORT $L90888
$L89329:

; 147  : 	}
; 148  : 	m_LightList.clear();

	mov	ebx, DWORD PTR [edi+92276]
	mov	esi, DWORD PTR [ebx]
	cmp	esi, ebx
	je	SHORT $L91197
$L91196:
	mov	eax, esi
	mov	esi, DWORD PTR [esi]
	push	eax
	mov	ecx, DWORD PTR [eax+4]
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR [ecx], edx
	mov	ecx, DWORD PTR [eax]
	mov	edx, DWORD PTR [eax+4]
	mov	DWORD PTR [ecx+4], edx
	call	??3@YAXPAX@Z				; operator delete
	mov	ecx, DWORD PTR [edi+92280]
	add	esp, 4
	dec	ecx
	cmp	esi, ebx
	mov	DWORD PTR [edi+92280], ecx
	jne	SHORT $L91196
$L91197:
	pop	edi
	pop	esi
	pop	ebx

; 149  : }

	ret	0
?Clear@KIpoTree@@QAEXXZ ENDP				; KIpoTree::Clear
_TEXT	ENDS
;	COMDAT ?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ
_TEXT	SEGMENT
?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ PROC NEAR ; KIpoTree::RemoveRtoGroupWithPermanentLeaf, COMDAT

; 153  : 	KIpotRuntimeObj* pRto;
; 154  : 	while(pRto = (KIpotRuntimeObj*)m_PermanentLeaf.pBrother)

	mov	eax, DWORD PTR [ecx+56]
	xor	edx, edx
	cmp	eax, edx
	je	SHORT $L89341
	push	esi
$L89340:

; 155  : 	{
; 156  : 		m_PermanentLeaf.pBrother = pRto->pBrother;

	mov	esi, DWORD PTR [eax+4]
	mov	DWORD PTR [ecx+56], esi

; 157  : 		pRto->pAheadBrother = NULL;

	mov	DWORD PTR [eax+48], edx

; 158  : 		pRto->pBrother = NULL;

	mov	DWORD PTR [eax+4], edx
	mov	eax, DWORD PTR [ecx+56]
	cmp	eax, edx
	jne	SHORT $L89340
	pop	esi
$L89341:

; 159  : 	}
; 160  : }

	ret	0
?RemoveRtoGroupWithPermanentLeaf@KIpoTree@@AAEXXZ ENDP	; KIpoTree::RemoveRtoGroupWithPermanentLeaf
_TEXT	ENDS
PUBLIC	?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z	; KIpoTree::AddBranch
EXTRN	?AddBranch@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z:NEAR ; KIpotBranch::AddBranch
EXTRN	?AddAObject@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z:NEAR ; KIpotBranch::AddAObject
;	COMDAT xdata$x
; File E:\BaiduYunDownload\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
xdata$x	SEGMENT
$T91247	DD	019930520H
	DD	01H
	DD	FLAT:$T91249
	DD	2 DUP(00H)
	DD	2 DUP(00H)
	ORG $+4
$T91249	DD	0ffffffffH
	DD	FLAT:$L91245
xdata$x	ENDS
;	COMDAT ?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z
_TEXT	SEGMENT
$T91242 = 8
__$EHRec$ = -12
_pBranchObj$ = 8
?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z PROC NEAR ; KIpoTree::AddBranch, COMDAT

; 173  : {

	mov	eax, DWORD PTR fs:__except_list
	push	-1
	push	$L91248
	push	eax
	mov	DWORD PTR fs:__except_list, esp
	push	esi
	mov	esi, ecx
	push	edi

; 174  : 	if (m_pMainBranch)

	mov	ecx, DWORD PTR [esi]
	test	ecx, ecx
	je	SHORT $L89347

; 175  : 		m_pMainBranch->AddBranch(pBranchObj);

	mov	eax, DWORD PTR _pBranchObj$[esp+16]
	push	eax
	call	?AddBranch@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddBranch

; 180  : 	}
; 181  : }

	mov	ecx, DWORD PTR __$EHRec$[esp+20]
	mov	DWORD PTR fs:__except_list, ecx
	pop	edi
	pop	esi
	add	esp, 12					; 0000000cH
	ret	4
$L89347:

; 176  : 	else if (pBranchObj)

	mov	edi, DWORD PTR _pBranchObj$[esp+16]
	test	edi, edi
	je	SHORT $L89349

; 177  : 	{
; 178  : 		m_pMainBranch = new KIpotBranch;

	push	48					; 00000030H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	DWORD PTR $T91242[esp+16], eax
	test	eax, eax
	mov	DWORD PTR __$EHRec$[esp+28], 0
	je	SHORT $L91243
	mov	ecx, eax
	call	??0KIpotBranch@@QAE@XZ			; KIpotBranch::KIpotBranch
	jmp	SHORT $L91244
$L91243:
	xor	eax, eax
$L91244:

; 179  : 		m_pMainBranch->AddAObject(pBranchObj);

	push	edi
	mov	ecx, eax
	mov	DWORD PTR __$EHRec$[esp+32], -1
	mov	DWORD PTR [esi], eax
	call	?AddAObject@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddAObject
$L89349:

; 180  : 	}
; 181  : }

	mov	ecx, DWORD PTR __$EHRec$[esp+20]
	pop	edi
	mov	DWORD PTR fs:__except_list, ecx
	pop	esi
	add	esp, 12					; 0000000cH
	ret	4
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
$L91245:
	mov	eax, DWORD PTR $T91242[ebp-4]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	pop	ecx
	ret	0
$L91248:
	mov	eax, OFFSET FLAT:$T91247
	jmp	___CxxFrameHandler
text$x	ENDS
?AddBranch@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z ENDP	; KIpoTree::AddBranch
PUBLIC	?Fell@KIpoTree@@QAEXXZ				; KIpoTree::Fell
EXTRN	?RemoveAllRtoLeafs@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z:NEAR ; KIpotBranch::RemoveAllRtoLeafs
;	COMDAT ?Fell@KIpoTree@@QAEXXZ
_TEXT	SEGMENT
?Fell@KIpoTree@@QAEXXZ PROC NEAR			; KIpoTree::Fell, COMDAT

; 185  : {

	push	esi
	push	edi
	mov	edi, ecx

; 186  : 	if (m_pMainBranch)

	mov	ecx, DWORD PTR [edi]
	test	ecx, ecx
	je	SHORT $L89357

; 187  : 	{
; 188  : 		m_pMainBranch->RemoveAllRtoLeafs(&m_PermanentLeaf);

	lea	eax, DWORD PTR [edi+52]
	push	eax
	call	?RemoveAllRtoLeafs@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::RemoveAllRtoLeafs

; 189  : 		delete(m_pMainBranch);

	mov	esi, DWORD PTR [edi]
	test	esi, esi
	je	SHORT $L91257
	mov	ecx, esi
	call	??1KIpotBranch@@QAE@XZ			; KIpotBranch::~KIpotBranch
	push	esi
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$L91257:

; 190  : 		m_pMainBranch = NULL;

	mov	DWORD PTR [edi], 0
$L89357:

; 191  : 	}
; 192  : 	m_DefaultBranch.RemoveAllRtoLeafs(&m_PermanentLeaf);

	lea	ecx, DWORD PTR [edi+52]
	lea	esi, DWORD PTR [edi+4]
	push	ecx
	mov	ecx, esi
	call	?RemoveAllRtoLeafs@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::RemoveAllRtoLeafs

; 193  : 	m_DefaultBranch.Clear();

	mov	ecx, esi
	call	?Clear@KIpotBranch@@QAEXXZ		; KIpotBranch::Clear

; 194  : 
; 195  : 	//清除所有内建对象的光源
; 196  : 	list<KLightBase*>::iterator i;
; 197  : 	for (i = m_LightList.begin(); i != m_LightList.end(); )

	mov	eax, DWORD PTR [edi+92276]
	mov	esi, DWORD PTR [eax]
	cmp	esi, eax
	je	SHORT $L89366
	push	ebx
$L89365:

; 198  : 	{
; 199  : 		if((*i)->m_pParent == NULL)

	mov	eax, DWORD PTR [esi+8]
	mov	ecx, DWORD PTR [eax+4]
	test	ecx, ecx
	jne	SHORT $L89367

; 200  : 		{
; 201  : 			delete (*i);

	push	eax
	call	??3@YAXPAX@Z				; operator delete

; 202  : 			*i=NULL;

	mov	DWORD PTR [esi+8], 0

; 203  : 			i = m_LightList.erase(i);

	mov	edx, DWORD PTR [esi+4]
	mov	ebx, DWORD PTR [esi]
	push	esi
	mov	DWORD PTR [edx], ebx
	mov	eax, DWORD PTR [esi]
	mov	ecx, DWORD PTR [esi+4]
	mov	DWORD PTR [eax+4], ecx
	call	??3@YAXPAX@Z				; operator delete
	mov	eax, DWORD PTR [edi+92280]
	add	esp, 8
	dec	eax
	mov	esi, ebx
	mov	DWORD PTR [edi+92280], eax

; 204  : 			continue;

	jmp	SHORT $L91452
$L89367:

; 205  : 		}
; 206  : 		++i;

	mov	esi, DWORD PTR [esi]
$L91452:
	cmp	esi, DWORD PTR [edi+92276]
	jne	SHORT $L89365
	pop	ebx
$L89366:
	pop	edi
	pop	esi

; 207  : 	}
; 208  : }

	ret	0
?Fell@KIpoTree@@QAEXXZ ENDP				; KIpoTree::Fell
_TEXT	ENDS
PUBLIC	?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z ; KIpoTree::AddLeafLine
EXTRN	?AddLeafLine@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z:NEAR ; KIpotBranch::AddLeafLine
;	COMDAT ?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z
_TEXT	SEGMENT
_pLeaf$ = 8
?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z PROC NEAR ; KIpoTree::AddLeafLine, COMDAT

; 223  : {

	mov	eax, ecx

; 224  : 	if (m_pMainBranch)

	mov	ecx, DWORD PTR [eax]
	test	ecx, ecx
	je	SHORT $L89374

; 225  : 		m_pMainBranch->AddLeafLine(pLeaf);

	mov	eax, DWORD PTR _pLeaf$[esp-4]
	push	eax

; 226  : 	else
; 227  : 		m_DefaultBranch.AddLeafLine(pLeaf);

	call	?AddLeafLine@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddLeafLine

; 228  : }

	ret	4
$L89374:

; 226  : 	else
; 227  : 		m_DefaultBranch.AddLeafLine(pLeaf);

	mov	ecx, DWORD PTR _pLeaf$[esp-4]
	push	ecx
	lea	ecx, DWORD PTR [eax+4]
	call	?AddLeafLine@KIpotBranch@@QAEXPAUKIpotBuildinObj@@@Z ; KIpotBranch::AddLeafLine

; 228  : }

	ret	4
?AddLeafLine@KIpoTree@@QAEXPAUKIpotBuildinObj@@@Z ENDP	; KIpoTree::AddLeafLine
_TEXT	ENDS
PUBLIC	__real@4@3ff6b60b60b60b60b800
PUBLIC	__real@4@4006ff00000000000000
PUBLIC	?Breath@KLightBase@@UAEXXZ			; KLightBase::Breath
PUBLIC	??_7KLightBase@@6B@				; KLightBase::`vftable'
PUBLIC	?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z	; KIpoTree::AddLeafPoint
EXTRN	?IsPlayer@KNpc@@QAEHXZ:NEAR			; KNpc::IsPlayer
EXTRN	?CoreGetGameObjLightInfo@@YAXIIPAUKLightInfo@@@Z:NEAR ; CoreGetGameObjLightInfo
EXTRN	?Npc@@3PAVKNpc@@A:BYTE				; Npc
EXTRN	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z:NEAR ; KIpotBranch::AddLeafPoint
;	COMDAT ??_7KLightBase@@6B@
; File C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE\list
CONST	SEGMENT
??_7KLightBase@@6B@ DD FLAT:?Breath@KLightBase@@UAEXXZ	; KLightBase::`vftable'
CONST	ENDS
;	COMDAT __real@4@3ff6b60b60b60b60b800
CONST	SEGMENT
__real@4@3ff6b60b60b60b60b800 DD 03b360b61r	; 0.00277778
CONST	ENDS
;	COMDAT __real@4@4006ff00000000000000
CONST	SEGMENT
__real@4@4006ff00000000000000 DD 0437f0000r	; 255
CONST	ENDS
;	COMDAT ?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z
_TEXT	SEGMENT
_pLeaf$ = 8
_lightInfo$89392 = -20
_n$89398 = 8
_color$89405 = -28
?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z PROC NEAR	; KIpoTree::AddLeafPoint, COMDAT

; 232  : {

	sub	esp, 28					; 0000001cH
	push	ebx
	mov	ebx, ecx
	push	esi
	push	edi

; 233  : 	if (m_pMainBranch)

	mov	ecx, DWORD PTR [ebx]

; 234  : 		m_pMainBranch->AddLeafPoint(pLeaf);

	mov	edi, DWORD PTR _pLeaf$[esp+36]
	test	ecx, ecx
	push	edi

; 235  : 	else

	jne	SHORT $L91708

; 236  : 		m_DefaultBranch.AddLeafPoint(pLeaf);

	lea	ecx, DWORD PTR [ebx+4]
$L91708:
	call	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::AddLeafPoint

; 237  : 	// 
; 238  : 	if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)

	cmp	DWORD PTR [edi], 1
	jne	$L89407

; 239  : 	{
; 240  : 		KIpotRuntimeObj* pRtoLeaf = (KIpotRuntimeObj*)pLeaf;
; 241  : 		if (pRtoLeaf->eLayerParam & IPOT_RL_LIGHT_PROP)

	test	BYTE PTR [edi+36], 8
	je	$L89407

; 242  : 		{
; 243  : 			// 增加一个光源
; 244  : 			KLightBase *pLight = new KLightBase;

	push	28					; 0000001cH
	call	??2@YAPAXI@Z				; operator new
	mov	esi, eax
	add	esp, 4
	test	esi, esi
	je	$L89407
	mov	DWORD PTR [esi], OFFSET FLAT:??_7KLightBase@@6B@ ; KLightBase::`vftable'

; 245  : 			if(pLight)
; 246  : 			{
; 247  : 				KLightInfo lightInfo;
; 248  : 				CoreGetGameObjLightInfo(pRtoLeaf->uGenre, pRtoLeaf->nId, &lightInfo);

	mov	ecx, DWORD PTR [edi+28]
	mov	edx, DWORD PTR [edi+24]
	lea	eax, DWORD PTR _lightInfo$89392[esp+40]
	push	ebp
	push	eax
	push	ecx
	push	edx
	call	?CoreGetGameObjLightInfo@@YAXIIPAUKLightInfo@@@Z ; CoreGetGameObjLightInfo

; 249  : 				pLight->m_dwColor = lightInfo.dwColor;

	mov	eax, DWORD PTR _lightInfo$89392[esp+68]

; 250  : 				pLight->m_oPosition = lightInfo.oPosition;

	mov	edx, DWORD PTR _lightInfo$89392[esp+56]
	lea	ecx, DWORD PTR [esi+8]
	mov	DWORD PTR [esi+20], eax
	mov	eax, DWORD PTR _lightInfo$89392[esp+60]

; 251  : 				pLight->m_nRadius = lightInfo.nRadius;
; 252  : 				pLight->m_pParent = pLeaf;

	mov	DWORD PTR [esi+4], edi
	mov	DWORD PTR [ecx], edx
	mov	edx, DWORD PTR _lightInfo$89392[esp+64]
	add	esp, 12					; 0000000cH

; 253  : 				
; 254  : 				if (pRtoLeaf->uGenre == CGOG_NPC && Npc[pRtoLeaf->nId].IsPlayer())

	mov	ebp, 320				; 00000140H
	mov	DWORD PTR [ecx+4], eax
	mov	eax, DWORD PTR _lightInfo$89392[esp+60]
	mov	DWORD PTR [esi+24], eax
	mov	DWORD PTR [ecx+8], edx
	mov	eax, DWORD PTR [edi+24]
	cmp	eax, 6
	jne	$L89397
	mov	ecx, DWORD PTR [edi+28]
	imul	ecx, 41812				; 0000a354H
	add	ecx, OFFSET FLAT:?Npc@@3PAVKNpc@@A	; Npc
	call	?IsPlayer@KNpc@@QAEHXZ			; KNpc::IsPlayer
	test	eax, eax
	je	$L89397

; 255  : 				{
; 256  : 					pLight->m_dwColor = 0xffffffff;

	mov	DWORD PTR [esi+20], -1

; 257  : 					if(m_bIsIndoor)

	mov	al, BYTE PTR [ebx+88]
	test	al, al
	je	SHORT $L89394

; 258  : 					{
; 259  : 						if(m_dwAmbient == 0xff404040)

	mov	ecx, DWORD PTR [ebx+92]
	sub	ecx, -12566464				; ff404040H
	neg	ecx
	sbb	ecx, ecx
	and	ecx, ebp
	mov	DWORD PTR [esi+24], ecx

; 260  : 							pLight->m_nRadius = 0;
; 261  : 						else
; 262  : 							pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;
; 263  : 					}
; 264  : 					else

	jmp	$L89397
$L89394:

; 265  : 					{
; 266  : 						// 是主角，按时间调整光源亮度
; 267  : 						int n = 0;
; 268  : 						if(m_nCurrentTime >= 480 && m_nCurrentTime <= 960)

	mov	ecx, DWORD PTR [ebx+84]
	xor	eax, eax
	cmp	ecx, 480				; 000001e0H
	mov	DWORD PTR _n$89398[esp+40], eax
	jl	SHORT $L91710
	cmp	ecx, 960				; 000003c0H
	jg	SHORT $L89399

; 269  : 							pLight->m_nRadius = 0;

	mov	DWORD PTR [esi+24], eax

; 270  : 						else if(m_nCurrentTime < 480)

	jmp	SHORT $L89403
$L89399:
	cmp	ecx, 480				; 000001e0H
	jge	SHORT $L89401
$L91710:

; 271  : 						{
; 272  : 							n = 480 - m_nCurrentTime;

	mov	eax, 480				; 000001e0H
	sub	eax, ecx

; 273  : 							pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;
; 274  : 						}
; 275  : 						else

	jmp	SHORT $L91711
$L89401:

; 276  : 						{
; 277  : 							n = m_nCurrentTime - 960;

	lea	eax, DWORD PTR [ecx-960]
$L91711:

; 278  : 							pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;
; 279  : 						}
; 280  : 
; 281  : 						if(n > 360)

	cmp	eax, 360				; 00000168H
	mov	DWORD PTR _n$89398[esp+40], eax
	mov	DWORD PTR [esi+24], ebp
	jle	SHORT $L89403

; 282  : 							n = 360;

	mov	DWORD PTR _n$89398[esp+40], 360		; 00000168H
$L89403:

; 283  : 						float f = n / 360.0f;

	fild	DWORD PTR _n$89398[esp+40]
	fmul	DWORD PTR __real@4@3ff6b60b60b60b60b800

; 284  : 						f = f * f;

	fld	ST(0)
	fmulp	ST(1), ST(0)

; 285  : 						KLColor color;
; 286  : 						color.SetColor(pLight->m_dwColor);
; 287  : 						color.Scale(f);

	fmul	DWORD PTR __real@4@4006ff00000000000000
	call	__ftol
	mov	WORD PTR _color$89405[esp+44], ax
	mov	WORD PTR _color$89405[esp+46], ax

; 288  : 						pLight->m_dwColor = color.GetColor();

	mov	edx, DWORD PTR _color$89405[esp+44]
	mov	WORD PTR _color$89405[esp+48], ax
	mov	eax, DWORD PTR _color$89405[esp+46]
	mov	ecx, DWORD PTR _color$89405[esp+48]
	and	edx, 65535				; 0000ffffH
	and	eax, 65535				; 0000ffffH
	or	edx, -256				; ffffff00H
	and	ecx, 65535				; 0000ffffH
	shl	edx, 8
	or	edx, eax
	shl	edx, 8
	or	edx, ecx
	mov	DWORD PTR [esi+20], edx
$L89397:

; 289  : 					}
; 290  : 				}
; 291  : 
; 292  : 				pLight->m_nRadius = MAIN_PLAYER_LIGHT_RADIUS;

	mov	DWORD PTR [esi+24], ebp

; 293  : 				pLight->m_dwColor = 0xff808080;

	mov	DWORD PTR [esi+20], -8355712		; ff808080H

; 294  : 
; 295  : 				if(pLight->m_nRadius)
; 296  :                 {
; 297  : 					m_LightList.push_back(pLight);

	mov	edi, DWORD PTR [ebx+92276]
	push	12					; 0000000cH
	mov	ebp, DWORD PTR [edi+4]
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	mov	ecx, edi
	test	edi, edi
	jne	SHORT $L91663
	mov	ecx, eax
$L91663:
	test	ebp, ebp
	mov	DWORD PTR [eax], ecx
	jne	SHORT $L91665
	mov	ebp, eax
$L91665:
	mov	DWORD PTR [eax+4], ebp
	mov	DWORD PTR [edi+4], eax
	mov	edx, DWORD PTR [eax+4]
	pop	ebp
	mov	DWORD PTR [edx], eax
	add	eax, 8
	test	eax, eax
	je	SHORT $L91690
	mov	DWORD PTR [eax], esi
$L91690:
	inc	DWORD PTR [ebx+92280]
$L89407:
	pop	edi
	pop	esi
	pop	ebx

; 298  :                 }
; 299  :                 else
; 300  :                 {
; 301  :                     delete pLight;
; 302  :                     pLight = NULL;
; 303  :                 }
; 304  : 			}
; 305  : 		}
; 306  : 	}
; 307  : }

	add	esp, 28					; 0000001cH
	ret	4
?AddLeafPoint@KIpoTree@@QAEXPAUKIpotLeaf@@@Z ENDP	; KIpoTree::AddLeafPoint
_TEXT	ENDS
;	COMDAT ?Breath@KLightBase@@UAEXXZ
_TEXT	SEGMENT
?Breath@KLightBase@@UAEXXZ PROC NEAR			; KLightBase::Breath, COMDAT

; 50   : 	virtual void Breath(){}

	ret	0
?Breath@KLightBase@@UAEXXZ ENDP				; KLightBase::Breath
_TEXT	ENDS
PUBLIC	?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z	; KIpoTree::PluckRto
EXTRN	?Pluck@KIpotRuntimeObj@@QAEXXZ:NEAR		; KIpotRuntimeObj::Pluck
;	COMDAT ?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z
_TEXT	SEGMENT
_pLeaf$ = 8
?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z PROC NEAR ; KIpoTree::PluckRto, COMDAT

; 310  : {

	push	ebx

; 311  : 	pLeaf->Pluck();

	mov	ebx, DWORD PTR _pLeaf$[esp]
	push	esi
	push	edi
	mov	edi, ecx
	mov	ecx, ebx
	call	?Pluck@KIpotRuntimeObj@@QAEXXZ		; KIpotRuntimeObj::Pluck

; 312  : 	list<KLightBase*>::iterator i;
; 313  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	mov	eax, DWORD PTR [edi+92276]
	mov	esi, DWORD PTR [eax]
	cmp	esi, eax
	je	SHORT $L91899
$L91761:

; 314  : 	{
; 315  : 		if((*i)->m_pParent == pLeaf)

	mov	ecx, DWORD PTR [esi+8]
	cmp	DWORD PTR [ecx+4], ebx
	je	SHORT $L91909
	mov	esi, DWORD PTR [esi]
	cmp	esi, eax
	jne	SHORT $L91761
	pop	edi
	pop	esi
	pop	ebx

; 320  : 			break;
; 321  : 		}
; 322  : 	}
; 323  : }

	ret	4
$L91909:

; 316  : 		{
; 317  : 			delete (*i);

	mov	edx, DWORD PTR [esi+8]
	push	edx
	call	??3@YAXPAX@Z				; operator delete

; 318  : 			*i=NULL;

	mov	DWORD PTR [esi+8], 0

; 319  : 			m_LightList.erase(i);

	mov	eax, DWORD PTR [esi+4]
	mov	ecx, DWORD PTR [esi]
	push	esi
	mov	DWORD PTR [eax], ecx
	mov	edx, DWORD PTR [esi]
	mov	eax, DWORD PTR [esi+4]
	mov	DWORD PTR [edx+4], eax
	call	??3@YAXPAX@Z				; operator delete
	mov	eax, DWORD PTR [edi+92280]
	add	esp, 8
	dec	eax
	mov	DWORD PTR [edi+92280], eax
$L91899:
	pop	edi
	pop	esi
	pop	ebx

; 320  : 			break;
; 321  : 		}
; 322  : 	}
; 323  : }

	ret	4
?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z ENDP	; KIpoTree::PluckRto
_TEXT	ENDS
PUBLIC	?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z	; KIpoTree::SetPermanentBranchPos
EXTRN	?SetLine@KIpotBranch@@QAEXPAUtagPOINT@@0@Z:NEAR	; KIpotBranch::SetLine
;	COMDAT ?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z
_TEXT	SEGMENT
_nLeftX$ = 8
_nRightX$ = 12
_y$ = 16
_p1$ = -8
_p2$ = -16
?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z PROC NEAR	; KIpoTree::SetPermanentBranchPos, COMDAT

; 327  : 	POINT	p1, p2;
; 328  : 	if (nLeftX < nRightX)

	mov	eax, DWORD PTR _nLeftX$[esp-4]
	mov	edx, DWORD PTR _nRightX$[esp-4]
	sub	esp, 16					; 00000010H
	cmp	eax, edx
	jge	SHORT $L91914

; 329  : 	{
; 330  : 		p1.x = nLeftX;

	mov	DWORD PTR _p1$[esp+16], eax

; 331  : 		p2.x = nRightX;
; 332  : 	}
; 333  : 	else if (nLeftX > nRightX)

	jmp	SHORT $L91915
$L91914:
	jle	SHORT $L89433

; 334  : 	{
; 335  : 		p2.x = nLeftX;

	mov	DWORD PTR _p2$[esp+16], eax

; 336  : 		p1.x = nRightX;

	mov	DWORD PTR _p1$[esp+16], edx

; 337  : 	}
; 338  : 	else

	jmp	SHORT $L89434
$L89433:

; 339  : 	{
; 340  : 		p1.x = nLeftX;

	mov	DWORD PTR _p1$[esp+16], eax

; 341  : 		p2.x = nRightX + 2048;

	add	edx, 2048				; 00000800H
$L91915:
	mov	DWORD PTR _p2$[esp+16], edx
$L89434:

; 342  : 	}
; 343  : 	p2.y = p1.y = y;

	mov	eax, DWORD PTR _y$[esp+12]

; 344  : 	m_DefaultBranch.SetLine(&p1, &p2);

	lea	edx, DWORD PTR _p1$[esp+16]
	mov	DWORD PTR _p1$[esp+20], eax
	mov	DWORD PTR _p2$[esp+20], eax
	lea	eax, DWORD PTR _p2$[esp+16]
	add	ecx, 4
	push	eax
	push	edx
	call	?SetLine@KIpotBranch@@QAEXPAUtagPOINT@@0@Z ; KIpotBranch::SetLine

; 345  : }

	add	esp, 16					; 00000010H
	ret	12					; 0000000cH
?SetPermanentBranchPos@KIpoTree@@QAEXHHH@Z ENDP		; KIpoTree::SetPermanentBranchPos
_TEXT	ENDS
PUBLIC	?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ; KIpoTree::ObjectsCallback
PUBLIC	?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z	; KIpoTree::StrewRtoLeafs
EXTRN	?EnumerateObjects@KIpotBranch@@QAEXPAXP6AX0PAUKIpotLeaf@@@Z@Z:NEAR ; KIpotBranch::EnumerateObjects
;	COMDAT ?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z
_TEXT	SEGMENT
_KeepRtoArea$ = 8
?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z PROC NEAR	; KIpoTree::StrewRtoLeafs, COMDAT

; 348  : {

	push	ebx
	push	ebp
	push	esi
	mov	esi, ecx

; 349  : 	KIpotBranch* pBranch;
; 350  : 	if (m_pMainBranch)

	xor	ebx, ebx
	mov	eax, DWORD PTR [esi]
	cmp	eax, ebx

; 351  : 		pBranch = m_pMainBranch;

	mov	ebp, eax
	jne	SHORT $L91919

; 352  : 	else
; 353  : 		pBranch = &m_DefaultBranch;

	lea	ebp, DWORD PTR [esi+4]
$L91919:

; 354  : 
; 355  : 	KIpotRuntimeObj* pRto;
; 356  : 	while(pRto = (KIpotRuntimeObj*)m_PermanentLeaf.pBrother)

	mov	eax, DWORD PTR [esi+56]
	cmp	eax, ebx
	je	SHORT $L89445
	push	edi
	mov	edi, DWORD PTR _KeepRtoArea$[esp+12]
$L89444:

; 357  : 	{
; 358  : 		m_PermanentLeaf.pBrother = pRto->pBrother;

	mov	ecx, DWORD PTR [eax+4]
	mov	DWORD PTR [esi+56], ecx

; 359  : 		pRto->pAheadBrother = NULL;
; 360  : 		pRto->pBrother = NULL;
; 361  : 
; 362  : 		if (pRto->oPosition.x >= KeepRtoArea.left  ||
; 363  : 			pRto->oPosition.x <  KeepRtoArea.right ||
; 364  : 			pRto->oPosition.y >= KeepRtoArea.top   ||
; 365  : 			pRto->oPosition.y <  KeepRtoArea.bottom)

	mov	ecx, DWORD PTR [eax+16]
	mov	DWORD PTR [eax+48], ebx
	mov	DWORD PTR [eax+4], ebx
	cmp	ecx, DWORD PTR [edi]
	jge	SHORT $L89448
	cmp	ecx, DWORD PTR [edi+8]
	jl	SHORT $L89448
	mov	ecx, DWORD PTR [eax+20]
	mov	edx, DWORD PTR [edi+4]
	cmp	ecx, edx
	jge	SHORT $L89448
	cmp	ecx, DWORD PTR [edi+12]
	jl	SHORT $L89448

; 368  : 		}
; 369  : 		else
; 370  : 		{
; 371  : 			PluckRto(pRto);				

	push	eax
	mov	ecx, esi
	call	?PluckRto@KIpoTree@@QAEXPAUKIpotRuntimeObj@@@Z ; KIpoTree::PluckRto
	jmp	SHORT $L89449
$L89448:

; 366  : 		{
; 367  : 			pBranch->AddLeafPoint(pRto);

	push	eax
	mov	ecx, ebp
	call	?AddLeafPoint@KIpotBranch@@QAEXPAUKIpotLeaf@@@Z ; KIpotBranch::AddLeafPoint
$L89449:
	mov	eax, DWORD PTR [esi+56]
	cmp	eax, ebx
	jne	SHORT $L89444
	pop	edi
$L89445:

; 372  : 		}
; 373  : 	}
; 374  : 
; 375  : 	if(m_bDynamicLighting)

	cmp	BYTE PTR [esi+90], bl
	je	SHORT $L89456

; 376  : 	{
; 377  : 		// 清空遮挡信息
; 378  : 		for(int j=0; j<LIGHTING_GRID_WIDTH*LIGHTING_GRID_HEIGHT; j++)

	lea	eax, DWORD PTR [esi+96]
	mov	ecx, 4608				; 00001200H
$L89452:

; 379  : 		{
; 380  : 			pObstacle[j].nObstacle = 0;

	mov	DWORD PTR [eax], ebx
	add	eax, 16					; 00000010H
	dec	ecx
	jne	SHORT $L89452

; 381  : 		}
; 382  : 
; 383  : 		// 遍历树，计算遮挡信息
; 384  : 		if (m_pMainBranch)

	mov	ecx, DWORD PTR [esi]

; 385  : 			m_pMainBranch->EnumerateObjects(this, ObjectsCallback);

	push	OFFSET FLAT:?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ; KIpoTree::ObjectsCallback
	cmp	ecx, ebx
	push	esi

; 386  : 		else

	jne	SHORT $L91922

; 387  : 			m_DefaultBranch.EnumerateObjects(this, ObjectsCallback);

	lea	ecx, DWORD PTR [esi+4]
$L91922:
	call	?EnumerateObjects@KIpotBranch@@QAEXPAXP6AX0PAUKIpotLeaf@@@Z@Z ; KIpotBranch::EnumerateObjects
$L89456:
	pop	esi
	pop	ebp
	pop	ebx

; 388  : 	}
; 389  : }

	ret	4
?StrewRtoLeafs@KIpoTree@@QAEXAAUtagRECT@@@Z ENDP	; KIpoTree::StrewRtoLeafs
_TEXT	ENDS
PUBLIC	?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z		; KIpoTree::FillLineObstacle
;	COMDAT ?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z
_TEXT	SEGMENT
_p$ = 8
_pLeaf$ = 12
?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z PROC NEAR ; KIpoTree::ObjectsCallback, COMDAT

; 394  : 	_ASSERT(p);
; 395  : 	_ASSERT(pLeaf);
; 396  : 	static int nObstacleID = 1;
; 397  : 	nObstacleID++;
; 398  : 
; 399  : 	if(pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)

	mov	eax, DWORD PTR _pLeaf$[esp-4]
	push	ebx
	push	ebp
	mov	ebp, DWORD PTR _?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA
	inc	ebp
	push	esi
	mov	DWORD PTR _?nObstacleID@?1??ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z@4HA, ebp
	mov	ecx, DWORD PTR [eax]
	cmp	ecx, 1
	push	edi
	je	$L89480

; 400  : 		return;
; 401  : 
; 402  : 	KIpoTree *pTree = (KIpoTree*)p;
; 403  : 	KIpotBuildinObj* pObj = (KIpotBuildinObj*)pLeaf;
; 404  : 	KBuildinObj* pBio = ((KIpotBuildinObj*)pLeaf)->pBio;

	mov	ecx, DWORD PTR [eax+48]

; 405  : 	if((pBio->Props & SPBIO_P_BLOCK_LIGHT_MASK) == SPBIO_P_BLOCK_LIGHT_NONE)

	mov	ecx, DWORD PTR [ecx]
	and	ecx, 48					; 00000030H
	je	$L89480

; 406  : 		return;
; 407  : 
; 408  : 	if((pBio->Props & SPBIO_P_BLOCK_LIGHT_MASK) == SPBIO_P_BLOCK_LIGHT_BOTTOM)

	cmp	ecx, 16					; 00000010H
	jne	SHORT $L89473

; 409  : 	{
; 410  : 		// 按线段处理遮挡
; 411  : 		pTree->FillLineObstacle(pObj->oPosition.x - pTree->m_nLeftTopX, pObj->oPosition.y - pTree->m_nLeftTopY, 
; 412  : 								pObj->oEndPos.x - pTree->m_nLeftTopX, pObj->oEndPos.y - pTree->m_nLeftTopY, nObstacleID);

	mov	ecx, DWORD PTR _p$[esp+12]
	mov	edi, DWORD PTR [eax+32]
	push	ebp
	mov	edx, DWORD PTR [ecx+80]
	mov	esi, DWORD PTR [ecx+76]
	sub	edi, edx
	push	edi
	mov	edi, DWORD PTR [eax+28]
	sub	edi, esi
	push	edi
	mov	edi, DWORD PTR [eax+20]
	mov	eax, DWORD PTR [eax+16]
	sub	edi, edx
	sub	eax, esi
	push	edi
	push	eax
	call	?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z	; KIpoTree::FillLineObstacle
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx

; 431  : 		}
; 432  : 	}
; 433  : }

	ret	0
$L89473:

; 413  : 	}
; 414  : 	else if((pBio->Props & SPBIO_P_BLOCK_LIGHT_MASK) == SPBIO_P_BLOCK_LIGHT_CIRCLE)

	cmp	ecx, 32					; 00000020H
	jne	SHORT $L89480

; 415  : 	{
; 416  : 		// 按圆处理光线遮挡，忽略半径，都按一个格子处理
; 417  : 		int nX = pObj->oPosition.x;
; 418  : 		int nY = pObj->oPosition.y;
; 419  : 		int nIdx;
; 420  : 		if(nX < pTree->m_nLeftTopX || nY < pTree->m_nLeftTopY ||
; 421  : 				nX >= pTree->m_nLeftTopX + AREGION_WIDTH * 3 || nY >= pTree->m_nLeftTopY + AREGION_HEIGHT * 3)

	mov	esi, DWORD PTR _p$[esp+12]
	mov	edi, DWORD PTR [eax+16]
	mov	eax, DWORD PTR [eax+20]
	mov	ebx, DWORD PTR [esi+76]
	cmp	edi, ebx
	jl	SHORT $L89480
	mov	ecx, DWORD PTR [esi+80]
	cmp	eax, ecx
	jl	SHORT $L89480
	lea	edx, DWORD PTR [ebx+1536]
	cmp	edi, edx
	jge	SHORT $L89480
	lea	edx, DWORD PTR [ecx+3072]
	cmp	eax, edx
	jge	SHORT $L89480

; 422  : 			return;
; 423  : 		else
; 424  : 		{
; 425  : 			nX = (nX - pTree->m_nLeftTopX) / LIGHTING_GRID_SIZEX;
; 426  : 			nY = (nY - pTree->m_nLeftTopY) / LIGHTING_GRID_SIZEY;

	sub	eax, ecx
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5

; 427  : 			nIdx = nY * LIGHTING_GRID_WIDTH + nX;

	lea	ecx, DWORD PTR [eax+eax*2]
	mov	eax, edi
	sub	eax, ebx
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	shl	ecx, 4
	sar	eax, 5
	add	ecx, eax

; 428  : 			pTree->pObstacle[nIdx].nObstacle = nObstacleID;

	lea	eax, DWORD PTR [ecx+6]
	shl	eax, 4

; 429  : 			// 将方向的x分量设为0，表示不考虑方向
; 430  : 			pTree->pObstacle[nIdx].vDir.fX = 0.0f;

	shl	ecx, 4
	mov	DWORD PTR [eax+esi], ebp
	mov	DWORD PTR [ecx+esi+100], 0
$L89480:
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx

; 431  : 		}
; 432  : 	}
; 433  : }

	ret	0
?ObjectsCallback@KIpoTree@@CAXPAXPAUKIpotLeaf@@@Z ENDP	; KIpoTree::ObjectsCallback
_TEXT	ENDS
PUBLIC	__real@4@40048000000000000000
PUBLIC	__real@4@c0048000000000000000
;	COMDAT __real@4@40048000000000000000
; File E:\BaiduYunDownload\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@4@40048000000000000000 DD 042000000r	; 32
CONST	ENDS
;	COMDAT __real@4@c0048000000000000000
CONST	SEGMENT
__real@4@c0048000000000000000 DD 0c2000000r	; -32
CONST	ENDS
;	COMDAT ?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z
_TEXT	SEGMENT
_x1$ = 8
_y1$ = 12
_x2$ = 16
_y2$ = 20
_nObstacleID$ = 24
_fy1$ = 12
_fdx$ = 8
_fdy$ = -36
_gStepX$ = 16
_gStepY$ = 12
_gx1$ = -28
_gy1$ = -20
_gx2$ = -16
_gy2$ = -12
_vDir$ = -8
_fFlope1$ = -24
?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z PROC NEAR	; KIpoTree::FillLineObstacle, COMDAT

; 436  : {

	sub	esp, 36					; 00000024H
	push	ebx

; 437  : 	if(x1 >= x2)

	mov	ebx, DWORD PTR _x2$[esp+36]
	push	ebp
	mov	ebp, DWORD PTR _x1$[esp+40]
	push	esi
	cmp	ebp, ebx
	push	edi
	mov	esi, ecx
	jge	$L89558

; 438  : 		return;
; 439  : 
; 440  : 	float fx1, fy1, fx2, fy2, fdx, fdy, fStepX, fStepY;
; 441  : 	int gStepX, gStepY, gx, gy, nIdx, gx1, gy1, gx2, gy2;
; 442  : 	fx1 = (float)x1, fy1 = (float)y1, fx2 = (float)x2, fy2 = (float)y2;

	fild	DWORD PTR _x1$[esp+48]
	fild	DWORD PTR _y1$[esp+48]

; 443  : 	fdx = (float)fabs(fx2 - fx1);

	fild	DWORD PTR _x2$[esp+48]
	fsub	ST(0), ST(2)
	fabs
	fstp	DWORD PTR _fdx$[esp+48]

; 444  : 	fdy = (float)fabs(fy2 - fy1);

	fild	DWORD PTR _y2$[esp+48]

; 445  : 	KVector2 vDir;
; 446  : 	vDir.fX = fdx;

	mov	eax, DWORD PTR _fdx$[esp+48]
	mov	DWORD PTR _vDir$[esp+52], eax

; 447  : 	vDir.fY = fdy;
; 448  : 	float fFlope1, fFlope2;
; 449  : 	gx1 = x1 / LIGHTING_GRID_SIZEX;

	mov	eax, ebp
	fsub	ST(0), ST(1)
	cdq
	fabs
	fstp	DWORD PTR _fdy$[esp+52]

; 450  : 	gy1 = y1 / LIGHTING_GRID_SIZEX;
; 451  : 	gx2 = x2 / LIGHTING_GRID_SIZEX;
; 452  : 	gy2 = y2 / LIGHTING_GRID_SIZEX;
; 453  : 
; 454  : 	fFlope1 = fdy / fdx;

	fld	DWORD PTR _fdy$[esp+52]
	fdiv	DWORD PTR _fdx$[esp+48]
	and	edx, 31					; 0000001fH
	mov	ecx, DWORD PTR _fdy$[esp+52]
	add	eax, edx
	mov	DWORD PTR _vDir$[esp+56], ecx
	mov	edi, eax
	mov	eax, DWORD PTR _y1$[esp+48]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	ecx, eax
	mov	eax, ebx
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	ebx, eax
	mov	eax, DWORD PTR _y2$[esp+48]
	cdq
	and	edx, 31					; 0000001fH
	add	edx, eax
	sar	edi, 5
	sar	ecx, 5
	sar	ebx, 5
	sar	edx, 5
	mov	DWORD PTR _gx1$[esp+52], edi
	mov	DWORD PTR _gy1$[esp+52], ecx
	mov	DWORD PTR _gx2$[esp+52], ebx
	mov	DWORD PTR _gy2$[esp+52], edx
	fstp	DWORD PTR -32+[esp+52]

; 455  : 
; 456  : 	// 斜率小于45度的情况
; 457  : 	if(fdx >= fdy)

	fld	DWORD PTR _fdx$[esp+48]
	fcomp	DWORD PTR _fdy$[esp+52]
	mov	eax, DWORD PTR -32+[esp+52]
	mov	DWORD PTR _fFlope1$[esp+52], eax
	fnstsw	ax
	test	ah, 1
	jne	$L89517

; 458  : 	{
; 459  : 		if(gx1 == gx2)

	cmp	edi, ebx
	je	$L91941

; 460  : 			return;
; 461  : 
; 462  : 		if(x2 > x1)

	mov	edx, DWORD PTR _x2$[esp+48]
	or	eax, -1
	cmp	edx, ebp
	mov	ecx, 1
	jle	SHORT $L89519

; 463  : 		{
; 464  : 			gStepX = 1;
; 465  : 			fStepX = LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@40048000000000000000
	mov	DWORD PTR _gStepX$[esp+48], ecx

; 466  : 		}
; 467  : 		else

	jmp	SHORT $L89520
$L89519:

; 468  : 		{
; 469  : 			gStepX = -1;
; 470  : 			fStepX = -LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@c0048000000000000000
	mov	DWORD PTR _gStepX$[esp+48], eax
$L89520:

; 471  : 		}
; 472  : 		if(y2 > y1)

	mov	edx, DWORD PTR _y1$[esp+48]
	mov	edi, DWORD PTR _y2$[esp+48]
	cmp	edi, edx
	jle	SHORT $L89521

; 473  : 		{
; 474  : 			gStepY = 1;
; 475  : 			fStepY = fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR -32+[esp+52]
	fmul	DWORD PTR __real@4@40048000000000000000
	mov	DWORD PTR _gStepY$[esp+48], ecx

; 476  : 		}
; 477  : 		else

	jmp	SHORT $L91939
$L89521:

; 478  : 		{
; 479  : 			gStepY = -1;
; 480  : 			fStepY = -fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR -32+[esp+52]
	fmul	DWORD PTR __real@4@c0048000000000000000
	mov	DWORD PTR _gStepY$[esp+48], eax
$L91939:
	mov	ebx, DWORD PTR _nObstacleID$[esp+48]
	mov	ebp, DWORD PTR _vDir$[esp+52]
$L89524:

; 481  : 		}
; 482  : 		while(1)
; 483  : 		{
; 484  : 			int nx = (int)fx1;
; 485  : 			int ny = (int)fy1;
; 486  : 
; 487  : 			gx = nx / LIGHTING_GRID_SIZEX;

	fld	ST(3)
	call	__ftol
	cdq

; 488  : 			gy = ny / LIGHTING_GRID_SIZEX;

	fld	ST(2)
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	edi, eax
	sar	edi, 5
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5

; 489  : 
; 490  : 			if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	test	edi, edi
	jle	$L89533
	cmp	edi, 47					; 0000002fH
	jge	$L89533
	test	eax, eax
	jle	$L89533
	cmp	eax, 95					; 0000005fH
	jge	$L89533

; 491  : 			{
; 492  : 				nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	lea	ecx, DWORD PTR [eax+eax*2]
	shl	ecx, 4
	add	ecx, edi

; 493  : 				pObstacle[nIdx].nObstacle = nObstacleID;

	lea	edx, DWORD PTR [ecx+6]
	shl	edx, 4
	mov	DWORD PTR [edx+esi], ebx

; 494  : 				pObstacle[nIdx].vDir = vDir;

	mov	edx, ecx
	shl	edx, 4
	mov	DWORD PTR [edx+esi+100], ebp
	mov	ebp, DWORD PTR _vDir$[esp+56]
	mov	DWORD PTR [edx+esi+104], ebp

; 495  : 
; 496  : 				fFlope2 = (float)(fabs((gy + gStepY) * 32.0f - fy1) / fabs((gx + gStepX) * 32.0f - fx1));

	mov	edx, DWORD PTR _gStepY$[esp+48]
	add	edx, eax
	mov	ebp, DWORD PTR _gStepX$[esp+48]
	mov	DWORD PTR 20+[esp+48], edx

; 497  : 				if(fFlope2 > fFlope1)

	fild	DWORD PTR 20+[esp+48]
	lea	eax, DWORD PTR [edi+ebp]
	mov	DWORD PTR 20+[esp+48], eax
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	ST(0), ST(3)
	fabs
	fild	DWORD PTR 20+[esp+48]
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	ST(0), ST(5)
	fabs
	fdivp	ST(1), ST(0)
	fcomp	DWORD PTR _fFlope1$[esp+52]
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L89532

; 498  : 				{
; 499  : 					nIdx = (gy + gStepY) * LIGHTING_GRID_WIDTH + gx;

	lea	eax, DWORD PTR [edx+edx*2]
	shl	eax, 4
	add	eax, edi

; 500  : 					pObstacle[nIdx].nObstacle = nObstacleID;
; 501  : 					pObstacle[nIdx].vDir = vDir;
; 502  : 				}
; 503  : 				else

	jmp	SHORT $L91945
$L89532:

; 504  : 				{
; 505  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx + gStepX;

	lea	eax, DWORD PTR [ecx+ebp]
$L91945:

; 506  : 					pObstacle[nIdx].nObstacle = nObstacleID;
; 507  : 					pObstacle[nIdx].vDir = vDir;

	mov	ebp, DWORD PTR _vDir$[esp+52]
	mov	edx, DWORD PTR _vDir$[esp+56]
	lea	ecx, DWORD PTR [eax+6]
	shl	ecx, 4
	shl	eax, 4
	mov	DWORD PTR [ecx+esi], ebx
	mov	DWORD PTR [eax+esi+100], ebp
	mov	DWORD PTR [eax+esi+104], edx
$L89533:

; 513  : 			gx1 += gStepX;

	mov	eax, DWORD PTR _gx1$[esp+52]
	mov	edx, DWORD PTR _gStepX$[esp+48]

; 514  : 			if(gx1 == gx2)

	mov	ecx, DWORD PTR _gx2$[esp+52]
	add	eax, edx
	fld	ST(1)
	faddp	ST(4), ST(0)
	cmp	eax, ecx
	mov	DWORD PTR _gx1$[esp+52], eax
	fld	ST(0)
	faddp	ST(3), ST(0)
	jne	$L89524

; 518  : 				if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)
; 519  : 				{
; 520  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx;
; 521  : 					pObstacle[nIdx].nObstacle = nObstacleID;
; 522  : 					pObstacle[nIdx].vDir = vDir;
; 523  : 				}
; 524  : 				return;

	jmp	$L91947
$L89517:

; 525  : 			}
; 526  : 		}
; 527  : 	}
; 528  : 	// 斜率大于45度的情况
; 529  : 	else
; 530  : 	{
; 531  : 		if(gy1 == gy2)

	cmp	ecx, edx
	je	$L91941

; 532  : 			return;
; 533  : 
; 534  : 		if(y2 > y1)

	mov	edx, DWORD PTR _y1$[esp+48]
	mov	edi, DWORD PTR _y2$[esp+48]
	or	eax, -1
	cmp	edi, edx
	mov	ecx, 1
	jle	SHORT $L89540

; 535  : 		{
; 536  : 			gStepY = 1;
; 537  : 			fStepY = LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@40048000000000000000
	mov	DWORD PTR _gStepY$[esp+48], ecx

; 538  : 		}
; 539  : 		else

	jmp	SHORT $L89541
$L89540:

; 540  : 		{
; 541  : 			gStepY = -1;
; 542  : 			fStepY = -LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@c0048000000000000000
	mov	DWORD PTR _gStepY$[esp+48], eax
$L89541:

; 543  : 		}
; 544  : 		if(x2 > x1)

	cmp	DWORD PTR _x2$[esp+48], ebp
	jle	SHORT $L89542

; 545  : 		{
; 546  : 			gStepX = 1;
; 547  : 			fStepX = fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[esp+48]
	fdiv	DWORD PTR _fdy$[esp+52]
	mov	DWORD PTR _gStepX$[esp+48], ecx
	fmul	DWORD PTR __real@4@40048000000000000000

; 548  : 		}
; 549  : 		else

	jmp	SHORT $L91940
$L89542:

; 550  : 		{
; 551  : 			gStepX = -1;
; 552  : 			fStepX = -fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[esp+48]
	fdiv	DWORD PTR _fdy$[esp+52]
	mov	DWORD PTR _gStepX$[esp+48], eax
	fmul	DWORD PTR __real@4@c0048000000000000000
$L91940:
	mov	ebx, DWORD PTR _nObstacleID$[esp+48]
	mov	ebp, DWORD PTR _vDir$[esp+52]
$L89545:

; 553  : 		}
; 554  : 		while(1)
; 555  : 		{
; 556  : 			int nx = (int)fx1;
; 557  : 			int ny = (int)fy1;
; 558  : 
; 559  : 			gx = nx / LIGHTING_GRID_SIZEX;

	fld	ST(3)
	call	__ftol
	cdq

; 560  : 			gy = ny / LIGHTING_GRID_SIZEX;

	fld	ST(2)
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	edi, eax
	sar	edi, 5
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5

; 561  : 
; 562  : 			if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	test	edi, edi
	jle	$L89554
	cmp	edi, 47					; 0000002fH
	jge	$L89554
	test	eax, eax
	jle	$L89554
	cmp	eax, 95					; 0000005fH
	jge	$L89554

; 563  : 			{
; 564  : 				nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	lea	ecx, DWORD PTR [eax+eax*2]
	shl	ecx, 4
	add	ecx, edi

; 565  : 				pObstacle[nIdx].nObstacle = nObstacleID;

	lea	edx, DWORD PTR [ecx+6]
	shl	edx, 4
	mov	DWORD PTR [edx+esi], ebx

; 566  : 				pObstacle[nIdx].vDir = vDir;

	mov	edx, ecx
	shl	edx, 4
	mov	DWORD PTR [edx+esi+100], ebp
	mov	ebp, DWORD PTR _vDir$[esp+56]
	mov	DWORD PTR [edx+esi+104], ebp

; 567  : 
; 568  : 				fFlope2 = (float)(fabs((gy + gStepY) * 32.0f - fy1) / fabs((gx + gStepX) * 32.0f - fx1));

	mov	edx, DWORD PTR _gStepY$[esp+48]
	add	edx, eax
	mov	ebp, DWORD PTR _gStepX$[esp+48]
	mov	DWORD PTR 20+[esp+48], edx

; 569  : 				if(fFlope2 > fFlope1)

	fild	DWORD PTR 20+[esp+48]
	lea	eax, DWORD PTR [edi+ebp]
	mov	DWORD PTR 20+[esp+48], eax
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	ST(0), ST(3)
	fabs
	fild	DWORD PTR 20+[esp+48]
	fmul	DWORD PTR __real@4@40048000000000000000
	fsub	ST(0), ST(5)
	fabs
	fdivp	ST(1), ST(0)
	fcomp	DWORD PTR _fFlope1$[esp+52]
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L89553

; 570  : 				{
; 571  : 					nIdx = (gy + gStepY) * LIGHTING_GRID_WIDTH + gx;

	lea	eax, DWORD PTR [edx+edx*2]
	shl	eax, 4
	add	eax, edi

; 572  : 					pObstacle[nIdx].nObstacle = nObstacleID;
; 573  : 					pObstacle[nIdx].vDir = vDir;
; 574  : 				}
; 575  : 				else

	jmp	SHORT $L91946
$L89553:

; 576  : 				{
; 577  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx + gStepX;

	lea	eax, DWORD PTR [ecx+ebp]
$L91946:

; 578  : 					pObstacle[nIdx].nObstacle = nObstacleID;
; 579  : 					pObstacle[nIdx].vDir = vDir;

	mov	ebp, DWORD PTR _vDir$[esp+52]
	mov	edx, DWORD PTR _vDir$[esp+56]
	lea	ecx, DWORD PTR [eax+6]
	shl	ecx, 4
	shl	eax, 4
	mov	DWORD PTR [ecx+esi], ebx
	mov	DWORD PTR [eax+esi+100], ebp
	mov	DWORD PTR [eax+esi+104], edx
$L89554:

; 585  : 			gy1 += gStepY;

	mov	eax, DWORD PTR _gy1$[esp+52]
	mov	edx, DWORD PTR _gStepY$[esp+48]

; 586  : 			if(gy1 == gy2)

	mov	ecx, DWORD PTR _gy2$[esp+52]
	add	eax, edx
	fld	ST(0)
	faddp	ST(4), ST(0)
	fld	ST(1)
	cmp	eax, ecx
	mov	DWORD PTR _gy1$[esp+52], eax
	faddp	ST(3), ST(0)
	jne	$L89545
$L91947:

; 580  : 				}
; 581  : 			}
; 582  : 
; 583  : 			fx1 += fStepX;
; 584  : 			fy1 += fStepY;

	fstp	ST(0)
	fstp	ST(0)
	fstp	DWORD PTR _fy1$[esp+48]

; 587  : 			{
; 588  : 				gx = ((int)fx1) / LIGHTING_GRID_SIZEX;

	call	__ftol
	cdq

; 589  : 				gy = ((int)fy1) / LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fy1$[esp+48]
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	edi, eax
	sar	edi, 5
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5

; 590  : 				if(gx >0 && gx < LIGHTING_GRID_WIDTH - 1 && gy >0 && gy < LIGHTING_GRID_HEIGHT - 1)

	test	edi, edi
	jle	SHORT $L89558
	cmp	edi, 47					; 0000002fH
	jge	SHORT $L89558
	test	eax, eax
	jle	SHORT $L89558
	cmp	eax, 95					; 0000005fH
	jge	SHORT $L89558

; 591  : 				{
; 592  : 					nIdx = gy * LIGHTING_GRID_WIDTH + gx;

	lea	eax, DWORD PTR [eax+eax*2]

; 593  : 					pObstacle[nIdx].nObstacle = nObstacleID;
; 594  : 					pObstacle[nIdx].vDir = vDir;

	mov	edx, DWORD PTR _vDir$[esp+56]
	shl	eax, 4
	add	eax, edi
	pop	edi
	lea	ecx, DWORD PTR [eax+6]
	shl	ecx, 4
	shl	eax, 4
	mov	DWORD PTR [ecx+esi], ebx
	mov	DWORD PTR [eax+esi+100], ebp
	mov	DWORD PTR [eax+esi+104], edx
	pop	esi
	pop	ebp
	pop	ebx

; 595  : 				}
; 596  : 				return;
; 597  : 			}
; 598  : 		}
; 599  : 	}
; 600  : }

	add	esp, 36					; 00000024H
	ret	20					; 00000014H
$L91941:

; 508  : 				}
; 509  : 			}
; 510  : 
; 511  : 			fx1 += fStepX;
; 512  : 			fy1 += fStepY;

	fstp	ST(0)

; 515  : 			{
; 516  : 				gx = ((int)fx1) / LIGHTING_GRID_SIZEX;
; 517  : 				gy = ((int)fy1) / LIGHTING_GRID_SIZEX;

	fstp	ST(0)
$L89558:
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx

; 595  : 				}
; 596  : 				return;
; 597  : 			}
; 598  : 		}
; 599  : 	}
; 600  : }

	add	esp, 36					; 00000024H
	ret	20					; 00000014H
?FillLineObstacle@KIpoTree@@AAEXHHHHH@Z ENDP		; KIpoTree::FillLineObstacle
_TEXT	ENDS
PUBLIC	?CanLighting@KIpoTree@@AAE_NHHHH@Z		; KIpoTree::CanLighting
PUBLIC	__real@8@3fff8000000000000000
PUBLIC	__real@8@400f8000000000000000
;	COMDAT __real@8@3fff8000000000000000
; File C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE\list
CONST	SEGMENT
__real@8@3fff8000000000000000 DQ 03ff0000000000000r ; 1
CONST	ENDS
;	COMDAT __real@8@400f8000000000000000
CONST	SEGMENT
__real@8@400f8000000000000000 DQ 040f0000000000000r ; 65536
CONST	ENDS
;	COMDAT ?RenderLightMap@KIpoTree@@QAEXXZ
_TEXT	SEGMENT
_this$ = -92
_i$ = -24
_nLightPosX$89580 = -32
_nLightPosY$89581 = -36
_ur$89588 = -52
_ug$89589 = -48
_ub$89590 = -44
_j$89591 = -96
_y$89593 = -20
_gx$89594 = -68
_gy$89595 = -68
_fLightRadiusDenom$89597 = -56
_pdwLight$ = -32
?RenderLightMap@KIpoTree@@QAEXXZ PROC NEAR		; KIpoTree::RenderLightMap, COMDAT

; 937  : {

	sub	esp, 96					; 00000060H
	push	ebx
	mov	ebx, ecx
	push	ebp
	push	esi

; 938  : 	int nn = m_LightList.size();
; 939  : 	int j, nLightID=1;
; 940  : 
; 941  : 	m_dwAmbient = 0xff101010;
; 942  : 
; 943  : 	// 按环境光清空光照图
; 944  : 	DWORD dwR, dwG, dwB;
; 945  : 	dwR = (m_dwAmbient & 0x00ff0000) >>16;
; 946  : 	dwG = (m_dwAmbient & 0x0000ff00) >>8;
; 947  : 	dwB = m_dwAmbient & 0x000000ff;
; 948  :     KLColor *pTempColor = pLColor;

	mov	eax, DWORD PTR [ebx+73824]
	push	edi
	mov	DWORD PTR _this$[esp+112], ebx
	mov	DWORD PTR [ebx+92], -15724528		; ff101010H
	mov	edx, 1152				; 00000480H
$L89569:

; 949  :     
; 950  :     // 如果每一个颜色分量是2个字节,可以考虑多一个项,然后用一个MMX的寄存器存放
; 951  :     // 正好8个字节
; 952  : 	for(j=0; j < LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT / 4; j++)
; 953  : 	{
; 954  : 			pTempColor[0].r = dwR;

	mov	ecx, 16					; 00000010H
	mov	WORD PTR [eax], cx

; 955  : 			pTempColor[1].r = dwR;

	mov	WORD PTR [eax+8], cx

; 956  : 			pTempColor[2].r = dwR;

	mov	WORD PTR [eax+16], cx

; 957  : 			pTempColor[3].r = dwR;

	mov	WORD PTR [eax+24], cx

; 958  : 
; 959  : 			pTempColor[0].g = dwG;

	mov	WORD PTR [eax+2], cx

; 960  : 			pTempColor[1].g = dwG;

	mov	WORD PTR [eax+10], cx

; 961  : 			pTempColor[2].g = dwG;

	mov	WORD PTR [eax+18], cx

; 962  : 			pTempColor[3].g = dwG;

	mov	WORD PTR [eax+26], cx

; 963  : 
; 964  : 
; 965  : 			pTempColor[0].b = dwB;

	mov	WORD PTR [eax+4], cx

; 966  : 			pTempColor[1].b = dwB;

	mov	WORD PTR [eax+12], cx

; 967  : 			pTempColor[2].b = dwB;

	mov	WORD PTR [eax+20], cx

; 968  : 			pTempColor[3].b = dwB;

	mov	WORD PTR [eax+28], cx

; 969  :             
; 970  :             pTempColor += 4;

	add	eax, 32					; 00000020H
	dec	edx
	jne	SHORT $L89569

; 971  : 	}
; 972  : 
; 973  : 	list<KLightBase*>::iterator i;
; 974  : 	// 计算每一个光源的光照结果
; 975  : 	for (i = m_LightList.begin(); i != m_LightList.end(); ++i)

	mov	eax, DWORD PTR [ebx+92276]
	mov	esi, DWORD PTR [eax]
	cmp	esi, eax
	mov	DWORD PTR _i$[esp+112], esi
	je	$L89578
	jmp	SHORT $L91999
$L92072:
	mov	esi, DWORD PTR _i$[esp+112]
$L91999:

; 976  : 	{
; 977  : 		nLightID++;
; 978  : 		KLightBase *pLight = *i;

	mov	ebp, DWORD PTR [esi+8]

; 979  : 		// 光源相对于九区域左上角的坐标
; 980  : 		int nLightPosX = pLight->m_oPosition.nX - m_nLeftTopX;

	mov	edi, DWORD PTR [ebx+76]

; 981  : 		int nLightPosY = pLight->m_oPosition.nY - m_nLeftTopY;

	mov	edx, DWORD PTR [ebx+80]
	mov	eax, DWORD PTR [ebp+8]
	mov	ecx, DWORD PTR [ebp+12]
	sub	eax, edi
	sub	ecx, edx
	mov	DWORD PTR _nLightPosX$89580[esp+112], eax
	mov	DWORD PTR _nLightPosY$89581[esp+112], ecx

; 982  : 		// 光源的格子坐标
; 983  : 		int nLightGridX = nLightPosX / LIGHTING_GRID_SIZEX;

	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	edi, eax

; 984  : 		int nLightGridY = nLightPosY / LIGHTING_GRID_SIZEY;

	mov	eax, ecx
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	ecx, eax
	sar	edi, 5
	sar	ecx, 5

; 985  : 		if(nLightGridX < 0 || nLightGridX >= LIGHTING_GRID_WIDTH || 
; 986  : 			nLightGridY < 0 || nLightGridY >= LIGHTING_GRID_HEIGHT)

	test	edi, edi
	jl	$L89577
	cmp	edi, 48					; 00000030H
	jge	$L89577
	test	ecx, ecx
	jl	$L89577
	cmp	ecx, 96					; 00000060H
	jge	$L89577

; 987  : 			continue;
; 988  : 
; 989  : 		// 光源所在格子中心的坐标
; 990  : 		int nLightGridCenterX = nLightGridX * LIGHTING_GRID_SIZEX + LIGHTING_GRID_SIZEX / 2;
; 991  : 		int nLightGridCenterY = nLightGridY * LIGHTING_GRID_SIZEY + LIGHTING_GRID_SIZEY / 2;
; 992  : 
; 993  : 		// 获取光源的r，g，b分量
; 994  : 		unsigned int ur, ug, ub;
; 995  : 
; 996  : 		ur = (pLight->m_dwColor & 0x00ff0000) >> 18;

	mov	eax, DWORD PTR [ebp+20]

; 997  : 		ug = (pLight->m_dwColor & 0x0000ff00) >> 10;
; 998  : 		ub = (pLight->m_dwColor & 0x000000ff) >> 2;
; 999  : 
; 1000 : 		//float fr = (float)pLight->m_nRadius * (float)pLight->m_nRadius;
; 1001 : 
; 1002 : 		int j, x, y, gx, gy;
; 1003 : 		int nGridRadius = pLight->m_nRadius / LIGHTING_GRID_SIZEX;

	mov	ebp, DWORD PTR [ebp+24]
	mov	DWORD PTR -16+[esp+112], ebp
	mov	edx, eax

; 1004 :         float fLightRadiusDenom =  1.0 / ((float)pLight->m_nRadius);

	fild	DWORD PTR -16+[esp+112]
	shr	edx, 18					; 00000012H
	and	edx, 63					; 0000003fH
	mov	esi, edi
	fdivr	QWORD PTR __real@8@3fff8000000000000000
	mov	DWORD PTR _ur$89588[esp+112], edx
	mov	edx, eax
	shr	eax, 2
	shr	edx, 10					; 0000000aH
	and	eax, 63					; 0000003fH
	and	edx, 63					; 0000003fH
	mov	DWORD PTR _ub$89590[esp+112], eax
	mov	eax, ebp
	mov	DWORD PTR _ug$89589[esp+112], edx
	mov	ebx, ecx
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	shl	esi, 5
	shl	ebx, 5
	sar	eax, 5
	add	esi, 16					; 00000010H
	add	ebx, 16					; 00000010H

; 1005 :         //fLightRadiusDenom *= fLightRadiusDenom;
; 1006 :         unsigned int uDistance65536 = 0;
; 1007 :         KLColor *pCurPos = NULL;
; 1008 : 
; 1009 : 		
; 1010 : 		gy = nLightGridY - (nGridRadius - 1);
; 1011 : 		// 从中心点向外扩展计算光照强度
; 1012 : 		for(int r = 0; r < nGridRadius; r++)

	test	eax, eax
	fstp	DWORD PTR _fLightRadiusDenom$89597[esp+112]
	jle	$L92078
	lea	ebp, DWORD PTR [ecx+ecx*2]
	or	edx, -1
	shl	ebp, 4
	mov	DWORD PTR -28+[esp+112], ebp
	lea	ebp, DWORD PTR [ecx+1]
	mov	DWORD PTR -80+[esp+112], ebp
	mov	DWORD PTR -40+[esp+112], edi
	lea	ebp, DWORD PTR [ebp+ebp*2]
	mov	DWORD PTR -88+[esp+112], edx
	shl	ebp, 4
	mov	DWORD PTR -64+[esp+112], ebp
	lea	ebp, DWORD PTR [ebx+32]
	sub	ebx, esi
	sub	ecx, edi
	mov	DWORD PTR -60+[esp+112], esi
	mov	DWORD PTR -84+[esp+112], edi
	mov	DWORD PTR -76+[esp+112], esi
	mov	DWORD PTR -72+[esp+112], ebp
	mov	DWORD PTR -8+[esp+112], ebx
	mov	DWORD PTR -12+[esp+112], ecx
	mov	DWORD PTR -16+[esp+112], eax
$L89602:

; 1013 : 		{
; 1014 : 			gx = nLightGridX - r;

	mov	eax, DWORD PTR -84+[esp+112]

; 1015 : 			gy = nLightGridY - r;
; 1016 : 			if (!
; 1017 : 				((gx < 0) || (gy < 0))
; 1018 : 			)

	test	eax, eax
	mov	DWORD PTR _gx$89594[esp+112], eax
	jl	$L92075
	mov	ecx, DWORD PTR -80+[esp+112]
	dec	ecx
	test	ecx, ecx
	jl	$L92075

; 1019 : 			{
; 1020 : 				x = nLightGridCenterX - r * LIGHTING_GRID_SIZEX;
; 1021 : 				y = nLightGridCenterY - r * LIGHTING_GRID_SIZEY;
; 1022 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	esi, DWORD PTR _this$[esp+112]
	mov	ecx, DWORD PTR -64+[esp+112]
	mov	edi, DWORD PTR -76+[esp+112]
	add	ecx, eax
	mov	esi, DWORD PTR [esi+73824]

; 1023 : 				for(j=0; j<(r * 2 + 1); j++)

	mov	DWORD PTR _j$89591[esp+112], 0
	lea	esi, DWORD PTR [esi+ecx*8-384]
	lea	ecx, DWORD PTR [edx+2]
	test	ecx, ecx
	jle	$L92075
$L89606:

; 1024 : 				{
; 1025 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	edx, DWORD PTR -72+[esp+112]
	mov	ebp, DWORD PTR _nLightPosY$89581[esp+112]
	mov	ebx, DWORD PTR _nLightPosX$89580[esp+112]
	mov	ecx, DWORD PTR _this$[esp+112]
	push	ebp
	lea	eax, DWORD PTR [edx-32]
	push	ebx
	push	eax
	push	edi
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	test	al, al
	je	SHORT $L89614

; 1026 : 					{
; 1027 : 						//float f = 65536.0 - ((float)(
; 1028 : 						//    (
; 1029 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1030 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1031 : 						//    ) * 
; 1032 : 						//    fLightRadiusDenom *
; 1033 : 						//    65536.0
; 1034 : 						//));
; 1035 : 						float f = 65536.0 - ((float)(
; 1036 : 							sqrt(
; 1037 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1038 : 								(y - nLightPosY) * (y - nLightPosY)
; 1039 : 							) * 
; 1040 : 							fLightRadiusDenom * 
; 1041 : 							65536.0
; 1042 : 						));

	mov	eax, DWORD PTR -72+[esp+112]
	mov	ecx, edi
	sub	ecx, ebx
	add	eax, -32				; ffffffe0H
	mov	edx, ecx
	sub	eax, ebp
	imul	edx, ecx
	mov	ecx, eax
	imul	ecx, eax
	add	edx, ecx

; 1043 : 
; 1044 : 						uDistance65536 = (int)(f);

	mov	DWORD PTR -20+[esp+112], edx
	fild	DWORD PTR -20+[esp+112]
	fsqrt
	fmul	DWORD PTR _fLightRadiusDenom$89597[esp+112]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fsubr	QWORD PTR __real@8@400f8000000000000000
	call	__ftol

; 1045 : 
; 1046 : 						if (((int)uDistance65536) > 0)

	test	eax, eax
	jle	SHORT $L89614

; 1047 : 						{
; 1048 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	edx, eax

; 1049 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	ecx, eax
	imul	edx, DWORD PTR _ur$89588[esp+112]
	imul	ecx, DWORD PTR _ug$89589[esp+112]

; 1050 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	imul	eax, DWORD PTR _ub$89590[esp+112]
	shr	edx, 16					; 00000010H
	add	WORD PTR [esi], dx
	shr	ecx, 16					; 00000010H
	add	WORD PTR [esi+2], cx
	shr	eax, 16					; 00000010H
	add	WORD PTR [esi+4], ax
$L89614:

; 1051 : 						}
; 1052 : 					}
; 1053 : 
; 1054 : 					gx++;

	mov	eax, DWORD PTR _gx$89594[esp+112]
	inc	eax

; 1055 :                     if (gx >= LIGHTING_GRID_WIDTH)

	cmp	eax, 48					; 00000030H
	mov	DWORD PTR _gx$89594[esp+112], eax
	jge	SHORT $L92067
	mov	edx, DWORD PTR -88+[esp+112]
	mov	eax, DWORD PTR _j$89591[esp+112]

; 1056 :                         break;
; 1057 : 
; 1058 : 					x += LIGHTING_GRID_SIZEX;

	add	edi, 32					; 00000020H

; 1059 : 					pCurPos++;

	add	esi, 8
	inc	eax
	lea	ecx, DWORD PTR [edx+2]
	cmp	eax, ecx
	mov	DWORD PTR _j$89591[esp+112], eax
	jl	$L89606
$L92067:

; 1023 : 				for(j=0; j<(r * 2 + 1); j++)

	mov	edx, DWORD PTR -88+[esp+112]
	mov	eax, DWORD PTR -84+[esp+112]
	jmp	SHORT $L92048
$L92075:
	mov	ebp, DWORD PTR _nLightPosY$89581[esp+112]
	mov	ebx, DWORD PTR _nLightPosX$89580[esp+112]
$L92048:

; 1060 : 				}
; 1061 : 			}
; 1062 : 
; 1063 : 			if(r == 0)

	cmp	edx, -1
	je	$L89603

; 1064 : 				continue;
; 1065 : 
; 1066 : 			gx = nLightGridX - r;
; 1067 : 			gy = nLightGridY + r;
; 1068 : 			if (!
; 1069 : 					((gx < 0) || (gy >= LIGHTING_GRID_HEIGHT))
; 1070 : 			)

	test	eax, eax
	mov	DWORD PTR _gx$89594[esp+112], eax
	jl	$L92049
	mov	ecx, DWORD PTR -12+[esp+112]
	mov	esi, DWORD PTR -40+[esp+112]
	add	ecx, esi
	cmp	ecx, 96					; 00000060H
	jge	$L92049

; 1071 : 			{
; 1072 : 				x = nLightGridCenterX - r * LIGHTING_GRID_SIZEX;
; 1073 : 				y = nLightGridCenterY + r * LIGHTING_GRID_SIZEY;

	mov	ecx, DWORD PTR -8+[esp+112]
	mov	esi, DWORD PTR -60+[esp+112]
	add	ecx, esi

; 1074 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	esi, DWORD PTR _this$[esp+112]
	mov	DWORD PTR _y$89593[esp+112], ecx
	mov	ecx, DWORD PTR -28+[esp+112]
	mov	esi, DWORD PTR [esi+73824]
	mov	edi, DWORD PTR -76+[esp+112]
	add	ecx, eax

; 1075 : 				for(j=0; j<(r * 2 + 1); j++)

	mov	DWORD PTR _j$89591[esp+112], 0
	lea	esi, DWORD PTR [esi+ecx*8]
	lea	ecx, DWORD PTR [edx+2]
	test	ecx, ecx
	jle	$L92049
$L89618:

; 1076 : 				{
; 1077 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	edx, DWORD PTR _y$89593[esp+112]
	mov	ecx, DWORD PTR _this$[esp+112]
	push	ebp
	push	ebx
	push	edx
	push	edi
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	test	al, al
	je	SHORT $L89626

; 1078 : 					{
; 1079 : 						//float f = 65536.0 - ((float)(
; 1080 : 						//    (
; 1081 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1082 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1083 : 						//    ) * 
; 1084 : 						//    fLightRadiusDenom *
; 1085 : 						//    65536.0
; 1086 : 						//));
; 1087 : 						float f = 65536.0 - ((float)(
; 1088 : 							sqrt(
; 1089 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1090 : 								(y - nLightPosY) * (y - nLightPosY)
; 1091 : 							) * 
; 1092 : 							fLightRadiusDenom * 
; 1093 : 							65536.0
; 1094 : 						));

	mov	eax, DWORD PTR _y$89593[esp+112]
	mov	ecx, edi
	sub	ecx, ebx
	sub	eax, ebp
	mov	edx, ecx
	imul	edx, ecx
	mov	ecx, eax
	imul	ecx, eax
	add	edx, ecx

; 1095 : 
; 1096 : 						uDistance65536 = (int)(f);

	mov	DWORD PTR -4+[esp+112], edx
	fild	DWORD PTR -4+[esp+112]
	fsqrt
	fmul	DWORD PTR _fLightRadiusDenom$89597[esp+112]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fsubr	QWORD PTR __real@8@400f8000000000000000
	call	__ftol

; 1097 : 
; 1098 : 						if (((int)uDistance65536) > 0)

	test	eax, eax
	jle	SHORT $L89626

; 1099 : 						{
; 1100 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	edx, eax

; 1101 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	ecx, eax
	imul	edx, DWORD PTR _ur$89588[esp+112]
	imul	ecx, DWORD PTR _ug$89589[esp+112]

; 1102 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	imul	eax, DWORD PTR _ub$89590[esp+112]
	shr	edx, 16					; 00000010H
	add	WORD PTR [esi], dx
	shr	ecx, 16					; 00000010H
	add	WORD PTR [esi+2], cx
	shr	eax, 16					; 00000010H
	add	WORD PTR [esi+4], ax
$L89626:

; 1103 : 						}
; 1104 : 					}
; 1105 : 
; 1106 : 					gx++;

	mov	eax, DWORD PTR _gx$89594[esp+112]
	inc	eax

; 1107 :                     if (gx >= LIGHTING_GRID_WIDTH)

	cmp	eax, 48					; 00000030H
	mov	DWORD PTR _gx$89594[esp+112], eax
	jge	SHORT $L92068
	mov	edx, DWORD PTR -88+[esp+112]
	mov	eax, DWORD PTR _j$89591[esp+112]

; 1108 :                         break;
; 1109 : 
; 1110 : 					x += LIGHTING_GRID_SIZEX;

	add	edi, 32					; 00000020H

; 1111 : 					pCurPos++;

	add	esi, 8
	inc	eax
	lea	ecx, DWORD PTR [edx+2]
	cmp	eax, ecx
	mov	DWORD PTR _j$89591[esp+112], eax
	jl	$L89618
$L92068:

; 1075 : 				for(j=0; j<(r * 2 + 1); j++)

	mov	edx, DWORD PTR -88+[esp+112]
	mov	eax, DWORD PTR -84+[esp+112]
$L92049:

; 1112 : 				}
; 1113 : 			}
; 1114 : 
; 1115 : 			gx = nLightGridX - r;
; 1116 : 			gy = nLightGridY - (r - 1);

	mov	ecx, DWORD PTR -80+[esp+112]

; 1117 : 			if (!
; 1118 : 				((gx < 0) || (gy < 0))
; 1119 : 			)

	test	eax, eax
	mov	DWORD PTR _gy$89595[esp+112], ecx
	jl	$L92050
	test	ecx, ecx
	jl	$L92050

; 1120 : 			{
; 1121 : 				x = nLightGridCenterX - r * LIGHTING_GRID_SIZEX;
; 1122 : 				y = nLightGridCenterY - (r - 1) * LIGHTING_GRID_SIZEY;
; 1123 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	esi, DWORD PTR _this$[esp+112]
	mov	ecx, DWORD PTR -64+[esp+112]
	mov	edi, DWORD PTR -72+[esp+112]
	add	ecx, eax
	mov	esi, DWORD PTR [esi+73824]

; 1124 : 				for(j=0; j<(r * 2 - 1); j++)

	mov	DWORD PTR _j$89591[esp+112], 0
	test	edx, edx
	lea	esi, DWORD PTR [esi+ecx*8]
	jle	$L92050
$L89629:

; 1125 : 				{
; 1126 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	edx, DWORD PTR -76+[esp+112]
	mov	ecx, DWORD PTR _this$[esp+112]
	push	ebp
	push	ebx
	push	edi
	push	edx
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	test	al, al
	je	SHORT $L89637

; 1127 : 					{
; 1128 : 						//float f = 65536.0 - ((float)(
; 1129 : 						//    (
; 1130 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1131 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1132 : 						//    ) * 
; 1133 : 						//    fLightRadiusDenom *
; 1134 : 						//    65536.0
; 1135 : 						//));
; 1136 : 						float f = 65536.0 - ((float)(
; 1137 : 							sqrt(
; 1138 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1139 : 								(y - nLightPosY) * (y - nLightPosY)
; 1140 : 							) * 
; 1141 : 							fLightRadiusDenom * 
; 1142 : 							65536.0
; 1143 : 						));

	mov	ecx, DWORD PTR -76+[esp+112]
	mov	eax, edi
	sub	ecx, ebx
	sub	eax, ebp
	mov	edx, ecx
	imul	edx, ecx
	mov	ecx, eax
	imul	ecx, eax
	add	edx, ecx

; 1144 : 
; 1145 : 						uDistance65536 = (int)(f);

	mov	DWORD PTR -4+[esp+112], edx
	fild	DWORD PTR -4+[esp+112]
	fsqrt
	fmul	DWORD PTR _fLightRadiusDenom$89597[esp+112]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fsubr	QWORD PTR __real@8@400f8000000000000000
	call	__ftol

; 1146 : 
; 1147 : 						if (((int)uDistance65536) > 0)

	test	eax, eax
	jle	SHORT $L89637

; 1148 : 						{
; 1149 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	edx, eax

; 1150 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	ecx, eax
	imul	edx, DWORD PTR _ur$89588[esp+112]
	imul	ecx, DWORD PTR _ug$89589[esp+112]

; 1151 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	imul	eax, DWORD PTR _ub$89590[esp+112]
	shr	edx, 16					; 00000010H
	add	WORD PTR [esi], dx
	shr	ecx, 16					; 00000010H
	add	WORD PTR [esi+2], cx
	shr	eax, 16					; 00000010H
	add	WORD PTR [esi+4], ax
$L89637:

; 1152 : 						}
; 1153 : 					}
; 1154 : 
; 1155 : 					gy++;

	mov	eax, DWORD PTR _gy$89595[esp+112]
	inc	eax

; 1156 :                     if (gy >= LIGHTING_GRID_HEIGHT)

	cmp	eax, 96					; 00000060H
	mov	DWORD PTR _gy$89595[esp+112], eax
	jge	SHORT $L92069
	mov	eax, DWORD PTR _j$89591[esp+112]
	mov	ecx, DWORD PTR -88+[esp+112]

; 1157 :                         break;
; 1158 : 
; 1159 : 					y += LIGHTING_GRID_SIZEX;

	add	edi, 32					; 00000020H

; 1160 : 					pCurPos += LIGHTING_GRID_WIDTH;

	add	esi, 384				; 00000180H
	inc	eax
	cmp	eax, ecx
	mov	DWORD PTR _j$89591[esp+112], eax
	jl	$L89629
$L92069:

; 1124 : 				for(j=0; j<(r * 2 - 1); j++)

	mov	edx, DWORD PTR -88+[esp+112]
	mov	eax, DWORD PTR -84+[esp+112]
$L92050:

; 1161 : 				}
; 1162 : 			}
; 1163 : 
; 1164 : 			
; 1165 : 			gx = nLightGridX + r;
; 1166 : 			gy = nLightGridY - (r - 1);
; 1167 : 			if (!
; 1168 : 				((gx >= LIGHTING_GRID_WIDTH ) || (gy < 0))
; 1169 : 			)

	mov	ecx, DWORD PTR -40+[esp+112]
	mov	esi, DWORD PTR -80+[esp+112]
	cmp	ecx, 48					; 00000030H
	mov	DWORD PTR _gy$89595[esp+112], esi
	jge	$L89603
	test	esi, esi
	jl	$L89603

; 1170 : 			{
; 1171 : 				x = nLightGridCenterX + r * LIGHTING_GRID_SIZEX;
; 1172 : 				y = nLightGridCenterY - (r - 1) * LIGHTING_GRID_SIZEY;
; 1173 : 				pCurPos = &pLColor[gy * LIGHTING_GRID_WIDTH + gx];

	mov	esi, DWORD PTR -64+[esp+112]
	mov	edi, DWORD PTR -72+[esp+112]
	add	esi, ecx
	mov	ecx, DWORD PTR _this$[esp+112]

; 1174 : 				for(j=0; j<(r * 2 - 1); j++)

	test	edx, edx
	mov	ecx, DWORD PTR [ecx+73824]
	mov	DWORD PTR _j$89591[esp+112], 0
	lea	esi, DWORD PTR [ecx+esi*8]
	jle	$L89603
$L89640:

; 1175 : 				{
; 1176 : 					if(CanLighting(x, y, nLightPosX, nLightPosY))

	mov	edx, DWORD PTR -60+[esp+112]
	mov	ecx, DWORD PTR _this$[esp+112]
	push	ebp
	push	ebx
	push	edi
	push	edx
	call	?CanLighting@KIpoTree@@AAE_NHHHH@Z	; KIpoTree::CanLighting
	test	al, al
	je	SHORT $L89648

; 1177 : 					{
; 1178 : 						//float f = 65536.0 - ((float)(
; 1179 : 						//    (
; 1180 : 						//        ((float)(x - nLightPosX)) * ((float)(x - nLightPosX)) + 
; 1181 : 						//        ((float)(y - nLightPosY)) * ((float)(y - nLightPosY))
; 1182 : 						//    ) * 
; 1183 : 						//    fLightRadiusDenom *
; 1184 : 						//    65536.0
; 1185 : 						//));
; 1186 : 						float f = 65536.0 - ((float)(
; 1187 : 							sqrt(
; 1188 : 								(x - nLightPosX) * (x - nLightPosX) + 
; 1189 : 								(y - nLightPosY) * (y - nLightPosY)
; 1190 : 							) * 
; 1191 : 							fLightRadiusDenom * 
; 1192 : 							65536.0
; 1193 : 						));

	mov	ecx, DWORD PTR -60+[esp+112]
	mov	eax, edi
	sub	ecx, ebx
	sub	eax, ebp
	mov	edx, ecx
	imul	edx, ecx
	mov	ecx, eax
	imul	ecx, eax
	add	edx, ecx

; 1194 : 
; 1195 : 						uDistance65536 = (int)(f);

	mov	DWORD PTR -4+[esp+112], edx
	fild	DWORD PTR -4+[esp+112]
	fsqrt
	fmul	DWORD PTR _fLightRadiusDenom$89597[esp+112]
	fmul	QWORD PTR __real@8@400f8000000000000000
	fsubr	QWORD PTR __real@8@400f8000000000000000
	call	__ftol

; 1196 : 
; 1197 : 						if (((int)uDistance65536) > 0)

	test	eax, eax
	jle	SHORT $L89648

; 1198 : 						{
; 1199 : 							pCurPos->r += (ur * uDistance65536 / 65536);

	mov	edx, eax

; 1200 : 							pCurPos->g += (ug * uDistance65536 / 65536);

	mov	ecx, eax
	imul	edx, DWORD PTR _ur$89588[esp+112]
	imul	ecx, DWORD PTR _ug$89589[esp+112]

; 1201 : 							pCurPos->b += (ub * uDistance65536 / 65536);

	imul	eax, DWORD PTR _ub$89590[esp+112]
	shr	edx, 16					; 00000010H
	add	WORD PTR [esi], dx
	shr	ecx, 16					; 00000010H
	add	WORD PTR [esi+2], cx
	shr	eax, 16					; 00000010H
	add	WORD PTR [esi+4], ax
$L89648:

; 1202 : 						}
; 1203 : 					}
; 1204 : 
; 1205 : 					gy++;

	mov	eax, DWORD PTR _gy$89595[esp+112]
	inc	eax

; 1206 :                     if (gy >= LIGHTING_GRID_HEIGHT)

	cmp	eax, 96					; 00000060H
	mov	DWORD PTR _gy$89595[esp+112], eax
	jge	SHORT $L92070
	mov	eax, DWORD PTR _j$89591[esp+112]
	mov	ecx, DWORD PTR -88+[esp+112]

; 1207 :                         break;
; 1208 : 
; 1209 : 					y += LIGHTING_GRID_SIZEX;

	add	edi, 32					; 00000020H

; 1210 : 					pCurPos += LIGHTING_GRID_WIDTH;

	add	esi, 384				; 00000180H
	inc	eax
	cmp	eax, ecx
	mov	DWORD PTR _j$89591[esp+112], eax
	jl	$L89640
$L92070:

; 1174 : 				for(j=0; j<(r * 2 - 1); j++)

	mov	edx, DWORD PTR -88+[esp+112]
	mov	eax, DWORD PTR -84+[esp+112]
$L89603:
	mov	ebx, DWORD PTR -28+[esp+112]
	mov	edi, DWORD PTR -40+[esp+112]
	mov	ecx, 48					; 00000030H
	mov	esi, DWORD PTR -72+[esp+112]
	add	ebx, ecx
	inc	edi
	dec	eax
	mov	ebp, DWORD PTR -64+[esp+112]
	mov	DWORD PTR -84+[esp+112], eax
	mov	eax, DWORD PTR -80+[esp+112]
	dec	eax
	mov	DWORD PTR -28+[esp+112], ebx
	mov	ebx, DWORD PTR -60+[esp+112]
	mov	DWORD PTR -40+[esp+112], edi
	mov	edi, DWORD PTR -76+[esp+112]
	mov	DWORD PTR -80+[esp+112], eax
	mov	eax, 32					; 00000020H
	sub	ebp, ecx
	add	ebx, eax
	sub	edi, eax
	sub	esi, eax
	mov	eax, DWORD PTR -16+[esp+112]
	add	edx, 2
	dec	eax
	mov	DWORD PTR -64+[esp+112], ebp
	mov	DWORD PTR -60+[esp+112], ebx
	mov	DWORD PTR -76+[esp+112], edi
	mov	DWORD PTR -72+[esp+112], esi
	mov	DWORD PTR -88+[esp+112], edx
	mov	DWORD PTR -16+[esp+112], eax
	jne	$L89602
$L92078:

; 1005 :         //fLightRadiusDenom *= fLightRadiusDenom;
; 1006 :         unsigned int uDistance65536 = 0;
; 1007 :         KLColor *pCurPos = NULL;
; 1008 : 
; 1009 : 		
; 1010 : 		gy = nLightGridY - (nGridRadius - 1);
; 1011 : 		// 从中心点向外扩展计算光照强度
; 1012 : 		for(int r = 0; r < nGridRadius; r++)

	mov	esi, DWORD PTR _i$[esp+112]
	mov	ebx, DWORD PTR _this$[esp+112]
$L89577:
	mov	esi, DWORD PTR [esi]
	mov	eax, DWORD PTR [ebx+92276]
	cmp	esi, eax
	mov	DWORD PTR _i$[esp+112], esi
	jne	$L92072
$L89578:

; 1211 : 				}
; 1212 : 			}
; 1213 : 		}
; 1214 : 	}
; 1215 : 
; 1216 :     //memcpy(ptp, pLColor, sizeof(ptp));
; 1217 : 
; 1218 :     
; 1219 : 	// 对光照图进行过滤，使阴影边沿平滑
; 1220 :     
; 1221 : 
; 1222 : 	int m;
; 1223 :     int n;
; 1224 :     DWORD   *pdwLight;
; 1225 :     KLColor *pO, *pL, *pR, *pU, *pD, *pDest;
; 1226 :     int nIdx = 1 * LIGHTING_GRID_WIDTH + 1;

	mov	esi, 392				; 00000188H
	lea	edx, DWORD PTR [ebx+74036]
	mov	DWORD PTR -28+[esp+112], esi
	mov	DWORD PTR -24+[esp+112], edx
	mov	DWORD PTR -40+[esp+112], 94		; 0000005eH
	jmp	SHORT $L89660
$L92071:
	mov	esi, DWORD PTR -28+[esp+112]
	mov	ebx, DWORD PTR _this$[esp+112]
$L89660:

; 1227 : 	for(m=1; m<LIGHTING_GRID_HEIGHT-1;m++)
; 1228 : 	{
; 1229 : 		pDest = ptp + nIdx;
; 1230 :         pdwLight = pLightingArray + nIdx;

	mov	eax, DWORD PTR -24+[esp+112]
	mov	edx, DWORD PTR [ebx+73832]
	mov	ecx, esi
	mov	DWORD PTR _pdwLight$[esp+112], eax

; 1231 : 		pO = pLColor + nIdx;

	mov	eax, DWORD PTR [ebx+73824]
	add	ecx, edx
	add	esi, eax
	mov	DWORD PTR -36+[esp+112], 46		; 0000002eH

; 1232 : 		pL = pO - 1;

	lea	edi, DWORD PTR [esi-8]

; 1233 : 		pR = pO + 1;
; 1234 : 		pU = pO - LIGHTING_GRID_WIDTH;

	lea	ebx, DWORD PTR [esi-384]

; 1235 : 		pD = pO + LIGHTING_GRID_WIDTH;

	lea	ebp, DWORD PTR [esi+384]
$L89663:

; 1236 : 		for(n=1; n<LIGHTING_GRID_WIDTH-1;n++)
; 1237 : 		{
; 1238 : 			//pDest->r = (pO->r + pR->r + pL->r + pU->r + pD->r) / 5;
; 1239 : 			//pDest->g = (pO->g + pR->g + pL->g + pU->g + pD->g) / 5;
; 1240 : 			//pDest->b = (pO->b + pR->b + pL->b + pU->b + pD->b) / 5;
; 1241 : 			//pDest->r = (pO->r + pR->r + pL->r + (pU->r + pD->r) / 2) / 4;
; 1242 : 			//pDest->g = (pO->g + pR->g + pL->g + (pU->g + pD->g) / 2) / 4;
; 1243 : 			//pDest->b = (pO->b + pR->b + pL->b + (pU->b + pD->b) / 2) / 4;
; 1244 : 			//pDest->r = (pO->r + (pR->r + pL->r) / 2 + pU->r + pD->r) / 4;
; 1245 : 			//pDest->g = (pO->g + (pR->g + pL->g) / 2 + pU->g + pD->g) / 4;
; 1246 : 			//pDest->b = (pO->b + (pR->b + pL->b) / 2 + pU->b + pD->b) / 4;
; 1247 : 			
; 1248 :             pDest->r = (pO->r + pL->r + pU->r + pD->r) / 4;

	xor	eax, eax
	xor	edx, edx
	mov	ax, WORD PTR [ebp]
	mov	dx, WORD PTR [ebx]
	add	eax, edx
	xor	edx, edx
	mov	dx, WORD PTR [edi]
	add	eax, edx
	xor	edx, edx
	mov	dx, WORD PTR [esi]
	add	eax, edx
	cdq
	and	edx, 3
	add	eax, edx

; 1249 : 		    pDest->g = (pO->g + pL->g + pU->g + pD->g) / 4;

	xor	edx, edx
	sar	eax, 2
	mov	WORD PTR [ecx], ax
	mov	dx, WORD PTR [ebx+2]
	xor	eax, eax
	mov	ax, WORD PTR [ebp+2]
	add	eax, edx
	xor	edx, edx
	mov	dx, WORD PTR [edi+2]
	add	eax, edx
	xor	edx, edx
	mov	dx, WORD PTR [esi+2]
	add	eax, edx
	cdq
	and	edx, 3
	add	eax, edx

; 1250 : 			pDest->b = (pO->b + pL->b + pU->b + pD->b) / 4;

	xor	edx, edx
	sar	eax, 2
	mov	WORD PTR [ecx+2], ax
	mov	dx, WORD PTR [ebx+4]
	xor	eax, eax
	mov	ax, WORD PTR [ebp+4]
	add	eax, edx
	xor	edx, edx
	mov	dx, WORD PTR [edi+4]
	add	eax, edx
	xor	edx, edx
	mov	dx, WORD PTR [esi+4]
	add	eax, edx
	cdq
	and	edx, 3
	add	eax, edx
	sar	eax, 2

; 1251 : 
; 1252 :             *pdwLight = 0xff000000 |
; 1253 :                 (((pDest->r > 0xff) ? 0xff : pDest->r) << 16) |
; 1254 :                 (((pDest->g > 0xff) ? 0xff : pDest->g) << 8) |
; 1255 :                 (((pDest->b > 0xff) ? 0xff : pDest->b));

	cmp	WORD PTR [ecx], 255			; 000000ffH
	mov	WORD PTR [ecx+4], ax
	jbe	SHORT $L91953
	mov	eax, 255				; 000000ffH
	jmp	SHORT $L91954
$L91953:
	xor	eax, eax
	mov	ax, WORD PTR [ecx]
$L91954:
	mov	dx, WORD PTR [ecx+2]
	cmp	dx, 255					; 000000ffH
	jbe	SHORT $L91955
	mov	DWORD PTR -16+[esp+112], 255		; 000000ffH
	jmp	SHORT $L91956
$L91955:
	and	edx, 65535				; 0000ffffH
	mov	DWORD PTR -16+[esp+112], edx
$L91956:
	mov	dx, WORD PTR [ecx+4]
	cmp	dx, 255					; 000000ffH
	jbe	SHORT $L91957
	mov	DWORD PTR -20+[esp+112], 255		; 000000ffH
	jmp	SHORT $L91958
$L91957:
	and	edx, 65535				; 0000ffffH
	mov	DWORD PTR -20+[esp+112], edx
$L91958:
	mov	edx, DWORD PTR -16+[esp+112]
	or	eax, -256				; ffffff00H
	shl	eax, 8
	or	eax, edx
	mov	edx, DWORD PTR -20+[esp+112]
	shl	eax, 8
	or	eax, edx
	mov	edx, DWORD PTR _pdwLight$[esp+112]

; 1256 : 
; 1257 : 
; 1258 :             //pDest->r = pO->r;
; 1259 : 		    //pDest->g = pO->g;
; 1260 : 			//pDest->b = pO->b;
; 1261 : 
; 1262 : 			pDest++, pO++, pL++, pR++, pU++, pD++, pdwLight++;

	add	ecx, 8
	add	esi, 8
	mov	DWORD PTR [edx], eax
	mov	eax, DWORD PTR -36+[esp+112]
	add	edi, 8
	add	ebx, 8
	add	ebp, 8
	add	edx, 4
	dec	eax
	mov	DWORD PTR _pdwLight$[esp+112], edx
	mov	DWORD PTR -36+[esp+112], eax
	jne	$L89663

; 1263 : 		}
; 1264 : 		nIdx += LIGHTING_GRID_WIDTH;

	mov	edx, DWORD PTR -24+[esp+112]
	mov	ecx, DWORD PTR -28+[esp+112]
	mov	eax, DWORD PTR -40+[esp+112]
	add	edx, 192				; 000000c0H
	add	ecx, 384				; 00000180H
	dec	eax
	mov	DWORD PTR -24+[esp+112], edx
	mov	DWORD PTR -28+[esp+112], ecx
	mov	DWORD PTR -40+[esp+112], eax
	jne	$L92071

; 1265 : 	}
; 1266 :     
; 1267 : 
; 1268 : 	// 将r，g，b的值限制在0xff之内，防止色彩错误
; 1269 : //	for(j = 0; j < LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT; j++)
; 1270 : //	{
; 1271 : //        //pLightingArray[j] = 0xff000000 |
; 1272 : //        //    ((((-(ptp[j].r > 0xff)) | ptp[j].r) & 0xff) << 16) |
; 1273 : //        //    ((((-(ptp[j].g > 0xff)) | ptp[j].g) & 0xff) << 8) |
; 1274 : //        //    ((((-(ptp[j].b > 0xff)) | ptp[j].b) & 0xff));
; 1275 : //
; 1276 : //		//if(ptp[j].r > 0xff)
; 1277 : //		//	ptp[j].r = 0xff;
; 1278 : //        
; 1279 : //		//if(ptp[j].g > 0xff)
; 1280 : //		//	ptp[j].g = 0xff;
; 1281 : //		//if(ptp[j].b > 0xff)
; 1282 : //		//	ptp[j].b = 0xff;
; 1283 : //		//pLightingArray[j] = 0xff000000 | (ptp[j].r<<16) | (ptp[j].g<<8) | ptp[j].b;
; 1284 : //
; 1285 : //        pLightingArray[j] = 0xff000000 |
; 1286 : //            (((ptp[j].r > 0xff) ? 0xff : ptp[j].r) << 16) |
; 1287 : //            (((ptp[j].g > 0xff) ? 0xff : ptp[j].g) << 8) |
; 1288 : //            (((ptp[j].b > 0xff) ? 0xff : ptp[j].b));
; 1289 : //
; 1290 : //	}
; 1291 :     for (m = 0; m < LIGHTING_GRID_HEIGHT; m++)

	mov	eax, DWORD PTR _this$[esp+112]
	mov	edx, 96					; 00000060H
	lea	ecx, DWORD PTR [eax+74028]
$L89666:

; 1292 :     {
; 1293 :         pLightingArray[m * LIGHTING_GRID_WIDTH + 0] = m_dwAmbient | 0xff000000;

	mov	esi, DWORD PTR [eax+92]
	add	ecx, 192				; 000000c0H
	or	esi, -16777216				; ff000000H
	mov	DWORD PTR [ecx-380], esi

; 1294 :         pLightingArray[m * LIGHTING_GRID_WIDTH + LIGHTING_GRID_WIDTH - 1] = m_dwAmbient | 0xff000000;

	mov	esi, DWORD PTR [eax+92]
	or	esi, -16777216				; ff000000H
	dec	edx
	mov	DWORD PTR [ecx-192], esi
	jne	SHORT $L89666

; 1295 :     }
; 1296 : 
; 1297 :     for (n = 0; n < LIGHTING_GRID_WIDTH; n++)

	lea	ecx, DWORD PTR [eax+92080]
	mov	edx, 48					; 00000030H
$L89669:

; 1298 :     {
; 1299 :         pLightingArray[n] = m_dwAmbient | 0xff000000;

	mov	esi, DWORD PTR [eax+92]
	add	ecx, 4
	or	esi, -16777216				; ff000000H
	mov	DWORD PTR [ecx-18244], esi

; 1300 :         pLightingArray[(LIGHTING_GRID_HEIGHT - 1) * LIGHTING_GRID_WIDTH + n] = m_dwAmbient | 0xff000000;

	mov	esi, DWORD PTR [eax+92]
	or	esi, -16777216				; ff000000H
	dec	edx
	mov	DWORD PTR [ecx-4], esi
	jne	SHORT $L89669
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx

; 1301 :     }
; 1302 :     //pLightingArray[0] = m_dwAmbient;    // 设置环境光
; 1303 : }

	add	esp, 96					; 00000060H
	ret	0
?RenderLightMap@KIpoTree@@QAEXXZ ENDP			; KIpoTree::RenderLightMap
_TEXT	ENDS
PUBLIC	__real@4@00000000000000000000
;	COMDAT __real@4@00000000000000000000
; File E:\BaiduYunDownload\SwordOnline\Sources\Core\Src\Scene\KIpoTree.cpp
CONST	SEGMENT
__real@4@00000000000000000000 DD 000000000r	; 0
CONST	ENDS
;	COMDAT ?CanLighting@KIpoTree@@AAE_NHHHH@Z
_TEXT	SEGMENT
_x$ = 8
_y$ = 12
_nLightPosX$ = 16
_nLightPosY$ = 20
_nGridLightPosX$ = -24
_nGridLightPosY$ = -20
_nObstacle$ = -28
_vDir$ = -16
_vLight$ = -8
_fdx$ = -32
_fdy$ = 20
?CanLighting@KIpoTree@@AAE_NHHHH@Z PROC NEAR		; KIpoTree::CanLighting, COMDAT

; 1307 : {

	sub	esp, 32					; 00000020H

; 1308 : 	int nGridX, nGridY, nGridLightPosX, nGridLightPosY, nGridStep, nCurrentGridX, nCurrentGridY, nIdx;
; 1309 : 	nGridX = x / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _x$[esp+28]
	push	ebx
	cdq
	and	edx, 31					; 0000001fH
	push	ebp
	add	eax, edx
	push	esi
	mov	ebx, eax

; 1310 : 	nGridY = y / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _y$[esp+40]
	cdq
	and	edx, 31					; 0000001fH
	push	edi
	add	eax, edx
	mov	ebp, ecx
	mov	edi, eax

; 1311 : 	nGridLightPosX = nLightPosX / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nLightPosX$[esp+44]
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	mov	ecx, eax

; 1312 : 	nGridLightPosY = nLightPosY / LIGHTING_GRID_SIZEX;

	mov	eax, DWORD PTR _nLightPosY$[esp+44]
	cdq
	and	edx, 31					; 0000001fH
	add	edx, eax
	sar	ebx, 5
	sar	ecx, 5
	sar	edi, 5
	sar	edx, 5

; 1313 : 	if(nGridX == nGridLightPosX && nGridY == nGridLightPosY)

	cmp	ebx, ecx
	mov	DWORD PTR _nGridLightPosX$[esp+48], ecx
	mov	DWORD PTR _nGridLightPosY$[esp+48], edx
	jne	SHORT $L89687
	cmp	edi, edx
	jne	SHORT $L89687
	pop	edi
	pop	esi
	pop	ebp

; 1314 : 		return true;

	mov	al, 1
	pop	ebx

; 1415 : 		}
; 1416 : 	}
; 1417 : }

	add	esp, 32					; 00000020H
	ret	16					; 00000010H
$L89687:

; 1315 : 
; 1316 : 	int nObstacle = pObstacle[nGridY * LIGHTING_GRID_WIDTH + nGridX].nObstacle;

	lea	eax, DWORD PTR [edi+edi*2]
	shl	eax, 4
	add	eax, ebx
	lea	esi, DWORD PTR [eax+6]
	shl	esi, 4

; 1317 : 	KVector2 vDir = pObstacle[nGridY * LIGHTING_GRID_WIDTH + nGridX].vDir;

	shl	eax, 4
	mov	esi, DWORD PTR [esi+ebp]
	mov	DWORD PTR _nObstacle$[esp+48], esi
	mov	esi, DWORD PTR [eax+ebp+100]
	mov	eax, DWORD PTR [eax+ebp+104]
	mov	DWORD PTR _vDir$[esp+48], esi

; 1318 : 	KVector2 vLight;
; 1319 : 	vLight.fX = (float)(x - nLightPosX);

	mov	esi, DWORD PTR _nLightPosX$[esp+44]
	mov	DWORD PTR _vDir$[esp+52], eax
	mov	eax, DWORD PTR _x$[esp+44]
	sub	eax, esi

; 1320 : 	vLight.fY = (float)(y - nLightPosY);

	mov	esi, DWORD PTR _nLightPosY$[esp+44]
	mov	DWORD PTR -32+[esp+48], eax
	mov	eax, DWORD PTR _y$[esp+44]
	fild	DWORD PTR -32+[esp+48]
	sub	eax, esi
	mov	DWORD PTR -32+[esp+48], eax
	fstp	DWORD PTR _vLight$[esp+48]
	fild	DWORD PTR -32+[esp+48]
	fstp	DWORD PTR _vLight$[esp+52]

; 1321 : 
; 1322 : 	float fx, fy, fLightPosX, fLightPosY, fdx, fdy, fStepX, fStepY;
; 1323 : 	fx = (float)x, fy =(float) y, fLightPosX = (float)nLightPosX, fLightPosY = (float)nLightPosY;

	fild	DWORD PTR _x$[esp+44]
	fild	DWORD PTR _y$[esp+44]

; 1324 : 	fdx = (float)fabs(fLightPosX - fx);

	fild	DWORD PTR _nLightPosX$[esp+44]
	fsub	ST(0), ST(2)
	fabs
	fst	DWORD PTR _fdx$[esp+48]

; 1325 : 	fdy = (float)fabs(fLightPosY - fy);

	fild	DWORD PTR _nLightPosY$[esp+44]
	fsub	ST(0), ST(2)
	fabs
	fstp	DWORD PTR _fdy$[esp+44]

; 1326 : 
; 1327 : 	// 斜率小于45度的情况
; 1328 : 	if(fdx >= fdy)

	fcomp	DWORD PTR _fdy$[esp+44]
	fnstsw	ax
	test	ah, 1
	jne	$L89707

; 1329 : 	{
; 1330 : 		// 计算线段前进累加值
; 1331 : 		if(nLightPosX > x)

	mov	edx, DWORD PTR _x$[esp+44]
	mov	eax, DWORD PTR _nLightPosX$[esp+44]
	cmp	eax, edx
	jle	SHORT $L89708

; 1332 : 		{
; 1333 : 			nGridStep = 1;
; 1334 : 			fStepX = LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@40048000000000000000
	mov	edi, 1

; 1335 : 		}
; 1336 : 		else

	jmp	SHORT $L89709
$L89708:

; 1337 : 		{
; 1338 : 			nGridStep = -1;
; 1339 : 			fStepX = -LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@c0048000000000000000
	or	edi, -1
$L89709:

; 1340 : 		}
; 1341 : 		if(nLightPosY > y)

	mov	eax, DWORD PTR _y$[esp+44]

; 1342 : 			fStepY = fdy / fdx * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdy$[esp+44]
	fdiv	DWORD PTR _fdx$[esp+48]
	cmp	esi, eax
	jle	SHORT $L89710
	fmul	DWORD PTR __real@4@40048000000000000000

; 1343 : 		else

	jmp	SHORT $L89713
$L89710:

; 1344 : 			fStepY = -fdy / fdx * LIGHTING_GRID_SIZEX;

	fmul	DWORD PTR __real@4@c0048000000000000000

; 1345 : 
; 1346 : 		while(1)

	jmp	SHORT $L89713
$L92106:
	mov	ecx, DWORD PTR _nGridLightPosX$[esp+48]
$L89713:

; 1347 : 		{
; 1348 : 			fx += fStepX;

	fld	ST(1)
	faddp	ST(4), ST(0)

; 1349 : 			fy += fStepY;
; 1350 : 			nGridX += nGridStep;

	add	ebx, edi

; 1351 : 			if(nGridX == nGridLightPosX)

	cmp	ebx, ecx
	fld	ST(0)
	faddp	ST(3), ST(0)
	je	SHORT $L92098

; 1358 : 					else
; 1359 : 						return false;
; 1360 : 				}
; 1361 : 				else
; 1362 : 					return true;
; 1363 : 			}
; 1364 : 
; 1365 : 			nCurrentGridX = ((int)fx) / LIGHTING_GRID_SIZEX;
; 1366 : 			nCurrentGridY = ((int)fy) / LIGHTING_GRID_SIZEX;
; 1367 : 			nIdx = nCurrentGridY * LIGHTING_GRID_WIDTH + nCurrentGridX;

	fld	ST(2)
	call	__ftol
	cdq
	fld	ST(3)
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	lea	esi, DWORD PTR [eax+eax*2]
	shl	esi, 4
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	add	esi, eax

; 1368 : 			// 检查是否碰到挡光障碍
; 1369 : 			if(pObstacle[nIdx].nObstacle != 0 && pObstacle[nIdx].nObstacle != nObstacle)

	add	esi, 6
	shl	esi, 4
	mov	eax, DWORD PTR [esi+ebp]
	test	eax, eax
	je	SHORT $L92106
	cmp	eax, DWORD PTR _nObstacle$[esp+48]
	jne	$L92097

; 1345 : 
; 1346 : 		while(1)

	jmp	SHORT $L92106
$L92098:

; 1352 : 			{
; 1353 : 				// 没有碰到障碍，根据这点本身是否障碍和障碍方向判断是否受光
; 1354 : 				if(nObstacle && vDir.fX != 0.0f)

	mov	eax, DWORD PTR _nObstacle$[esp+48]
	fstp	ST(0)
	fstp	ST(0)
	fstp	ST(0)
	test	eax, eax
	fstp	ST(0)
	je	SHORT $L89716
	fld	DWORD PTR _vDir$[esp+48]
	fcomp	DWORD PTR __real@4@00000000000000000000
	fnstsw	ax
	test	ah, 64					; 00000040H
	jne	SHORT $L89716

; 1355 : 				{
; 1356 : 					if(vDir.fX * vLight.fY - vDir.fY * vLight.fX < 0.0f)

	fld	DWORD PTR _vDir$[esp+48]
	fmul	DWORD PTR _vLight$[esp+52]
	fld	DWORD PTR _vDir$[esp+52]
	fmul	DWORD PTR _vLight$[esp+48]
	fsubp	ST(1), ST(0)
	fcomp	DWORD PTR __real@4@00000000000000000000
	fnstsw	ax
	test	ah, 1
	je	$L89733
$L89716:
	pop	edi
	pop	esi
	pop	ebp

; 1357 : 						return true;

	mov	al, 1
	pop	ebx

; 1415 : 		}
; 1416 : 	}
; 1417 : }

	add	esp, 32					; 00000020H
	ret	16					; 00000010H
$L89707:

; 1370 : 				return false;
; 1371 : 		}
; 1372 : 	}
; 1373 : 	// 斜率大于45度的情况
; 1374 : 	else
; 1375 : 	{
; 1376 : 		if(nLightPosY > y)

	cmp	esi, DWORD PTR _y$[esp+44]
	jle	SHORT $L89724

; 1377 : 		{
; 1378 : 			nGridStep = 1;
; 1379 : 			fStepY = LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@40048000000000000000
	mov	ebx, 1

; 1380 : 		}
; 1381 : 		else

	jmp	SHORT $L89725
$L89724:

; 1382 : 		{
; 1383 : 			nGridStep = -1;
; 1384 : 			fStepY = -LIGHTING_GRID_SIZEX;

	fld	DWORD PTR __real@4@c0048000000000000000
	or	ebx, -1
$L89725:

; 1385 : 		}
; 1386 : 		if(nLightPosX > x)

	mov	eax, DWORD PTR _x$[esp+44]
	mov	ecx, DWORD PTR _nLightPosX$[esp+44]

; 1387 : 			fStepX = fdx / fdy * LIGHTING_GRID_SIZEX;

	fld	DWORD PTR _fdx$[esp+48]
	fdiv	DWORD PTR _fdy$[esp+44]
	cmp	ecx, eax
	jle	SHORT $L89726
	fmul	DWORD PTR __real@4@40048000000000000000

; 1388 : 		else

	jmp	SHORT $L89729
$L89726:

; 1389 : 			fStepX = -fdx / fdy * LIGHTING_GRID_SIZEX;

	fmul	DWORD PTR __real@4@c0048000000000000000

; 1390 : 		while(1)

	jmp	SHORT $L89729
$L92107:
	mov	edx, DWORD PTR _nGridLightPosY$[esp+48]
$L89729:

; 1391 : 		{
; 1392 : 			fx += fStepX;

	fld	ST(0)
	faddp	ST(4), ST(0)

; 1393 : 			fy += fStepY;
; 1394 : 			nGridY += nGridStep;

	add	edi, ebx
	fld	ST(1)

; 1395 : 			if(nGridY == nGridLightPosY)

	cmp	edi, edx
	faddp	ST(3), ST(0)
	je	$L92098

; 1396 : 			{
; 1397 : 				// 没有碰到障碍，根据这点本身是否障碍和障碍方向判断是否受光
; 1398 : 				if(nObstacle && vDir.fX != 0.0f)
; 1399 : 				{
; 1400 : 					if(vDir.fX * vLight.fY - vDir.fY * vLight.fX < 0.0f)
; 1401 : 						return true;
; 1402 : 					else
; 1403 : 						return false;
; 1404 : 				}
; 1405 : 				else
; 1406 : 					return true;
; 1407 : 			}
; 1408 : 
; 1409 : 			nCurrentGridX = ((int)fx) / LIGHTING_GRID_SIZEX;
; 1410 : 			nCurrentGridY = ((int)fy) / LIGHTING_GRID_SIZEX;
; 1411 : 			nIdx = nCurrentGridY * LIGHTING_GRID_WIDTH + nCurrentGridX;

	fld	ST(2)
	call	__ftol
	cdq
	fld	ST(3)
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	lea	esi, DWORD PTR [eax+eax*2]
	shl	esi, 4
	call	__ftol
	cdq
	and	edx, 31					; 0000001fH
	add	eax, edx
	sar	eax, 5
	add	esi, eax

; 1412 : 			// 检查是否碰到挡光障碍
; 1413 : 			if(pObstacle[nIdx].nObstacle != 0 && pObstacle[nIdx].nObstacle != nObstacle)

	add	esi, 6
	shl	esi, 4
	mov	eax, DWORD PTR [esi+ebp]
	test	eax, eax
	je	SHORT $L92107
	cmp	eax, DWORD PTR _nObstacle$[esp+48]
	je	SHORT $L92107
$L92097:

; 1396 : 			{
; 1397 : 				// 没有碰到障碍，根据这点本身是否障碍和障碍方向判断是否受光
; 1398 : 				if(nObstacle && vDir.fX != 0.0f)
; 1399 : 				{
; 1400 : 					if(vDir.fX * vLight.fY - vDir.fY * vLight.fX < 0.0f)
; 1401 : 						return true;
; 1402 : 					else
; 1403 : 						return false;
; 1404 : 				}
; 1405 : 				else
; 1406 : 					return true;
; 1407 : 			}
; 1408 : 
; 1409 : 			nCurrentGridX = ((int)fx) / LIGHTING_GRID_SIZEX;
; 1410 : 			nCurrentGridY = ((int)fy) / LIGHTING_GRID_SIZEX;
; 1411 : 			nIdx = nCurrentGridY * LIGHTING_GRID_WIDTH + nCurrentGridX;

	fstp	ST(0)
	fstp	ST(0)
	fstp	ST(0)
	fstp	ST(0)
$L89733:
	pop	edi
	pop	esi
	pop	ebp

; 1414 : 				return false;

	xor	al, al
	pop	ebx

; 1415 : 		}
; 1416 : 	}
; 1417 : }

	add	esp, 32					; 00000020H
	ret	16					; 00000010H
?CanLighting@KIpoTree@@AAE_NHHHH@Z ENDP			; KIpoTree::CanLighting
_TEXT	ENDS
PUBLIC	__real@4@4005af00000000000000
PUBLIC	?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z ; KIpoTree::AddBuildinLight
EXTRN	__imp_?g_Random@@YAII@Z:NEAR
;	COMDAT __real@4@4005af00000000000000
; File C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE\list
CONST	SEGMENT
__real@4@4005af00000000000000 DD 042af0000r	; 87.5
CONST	ENDS
;	COMDAT ?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z
_TEXT	SEGMENT
_pLights$ = 8
_nCount$ = 12
_this$ = -20
_nMinRange$ = -12
_nMaxRange$ = 12
?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z PROC NEAR ; KIpoTree::AddBuildinLight, COMDAT

; 1534 : {

	sub	esp, 20					; 00000014H

; 1535 : 	if(!m_bProcessBioLights)

	mov	al, BYTE PTR [ecx+89]
	mov	DWORD PTR _this$[esp+20], ecx
	test	al, al
	je	$L89750

; 1536 : 		return;
; 1537 : 
; 1538 : 	int nMinRange, nMaxRange;
; 1539 : 
; 1540 : 	for(int i=0; i<nCount; i++)

	mov	eax, DWORD PTR _nCount$[esp+16]
	test	eax, eax
	jle	$L89750
	mov	ecx, DWORD PTR _pLights$[esp+16]
	push	ebx
	push	ebp
	push	esi
	push	edi
	lea	edi, DWORD PTR [ecx+20]
	mov	DWORD PTR 8+[esp+32], eax
$L89748:

; 1541 : 	{
; 1542 : 		// 增加一个光源
; 1543 : 		KSelfBreathLight *pLight = new KSelfBreathLight;

	push	52					; 00000034H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	test	eax, eax
	je	$L89749
	mov	ecx, eax
	call	??0KSelfBreathLight@@QAE@XZ		; KSelfBreathLight::KSelfBreathLight
	mov	esi, eax

; 1544 : 		if(pLight)

	test	esi, esi
	je	$L89749

; 1545 : 		{
; 1546 : 			// 保证nMaxRange大于nMinRange
; 1547 : 			if(pLights[i].nMaxRange >= pLights[i].nMinRange)

	mov	ecx, DWORD PTR [edi]
	mov	edx, DWORD PTR [edi-4]
	cmp	ecx, edx
	jl	SHORT $L89757

; 1548 : 				nMaxRange = pLights[i].nMaxRange, nMinRange = pLights[i].nMinRange;

	mov	eax, ecx
	mov	ecx, edx
	mov	DWORD PTR _nMaxRange$[esp+32], eax

; 1549 : 			else

	jmp	SHORT $L89758
$L89757:

; 1550 : 				nMaxRange = pLights[i].nMinRange, nMinRange = pLights[i].nMaxRange;

	mov	eax, edx
	mov	DWORD PTR _nMaxRange$[esp+32], eax
$L89758:

; 1551 : 			// 半径为负值，忽略这个光源
; 1552 : 			if(nMinRange < 0)

	test	ecx, ecx
	jl	$L89749

; 1553 : 			{
; 1554 : 				_ASSERT(0);
; 1555 : 				continue;
; 1556 : 			}
; 1557 : 
; 1558 : 			nMinRange = nMaxRange - 8;
; 1559 : 
; 1560 : 			pLight->m_dwColor = pLights[i].dwColor;

	mov	edx, DWORD PTR [edi-8]

; 1561 : 			pLight->m_oPosition.nX = pLights[i].oPos.x;
; 1562 : 			pLight->m_oPosition.nY = pLights[i].oPos.y;
; 1563 : 			pLight->m_oPosition.nZ = pLights[i].oPos.z;
; 1564 : 			pLight->m_nRadius = nMinRange + g_Random(nMaxRange - nMinRange);

	mov	ebx, DWORD PTR __imp_?g_Random@@YAII@Z
	mov	DWORD PTR [esi+20], edx
	mov	ecx, DWORD PTR [edi-20]
	mov	DWORD PTR [esi+8], ecx
	mov	edx, DWORD PTR [edi-16]
	lea	ebp, DWORD PTR [eax-8]
	mov	DWORD PTR [esi+12], edx
	mov	ecx, DWORD PTR [edi-12]
	sub	eax, ebp
	push	eax
	mov	DWORD PTR _nMinRange$[esp+40], ebp
	mov	DWORD PTR [esi+16], ecx
	call	ebx
	add	eax, ebp

; 1565 : 			pLight->fRadius = (float)(pLight->m_nRadius);
; 1566 : 			pLight->m_pParent = NULL;

	xor	ebp, ebp
	mov	DWORD PTR -16+[esp+40], eax

; 1567 : 			pLight->fMaxRange = (float)nMaxRange;
; 1568 : 			pLight->fMinRange = (float)nMinRange;
; 1569 : 			pLight->fCycle = 100 * 0.875f + g_Random(60 / 4);

	push	15					; 0000000fH
	fild	DWORD PTR -16+[esp+44]
	mov	DWORD PTR [esi+24], eax
	mov	DWORD PTR [esi+4], ebp
	fstp	DWORD PTR [esi+36]
	fild	DWORD PTR _nMaxRange$[esp+40]
	fstp	DWORD PTR [esi+32]
	fild	DWORD PTR _nMinRange$[esp+44]
	fstp	DWORD PTR [esi+28]
	call	ebx
	mov	DWORD PTR -8+[esp+44], eax
	mov	DWORD PTR -8+[esp+48], ebp
	fild	QWORD PTR -8+[esp+44]

; 1570 : 			m_LightList.push_back(pLight);

	mov	edx, DWORD PTR _this$[esp+44]
	push	12					; 0000000cH
	fadd	DWORD PTR __real@4@4005af00000000000000
	fstp	DWORD PTR [esi+40]
	mov	ebp, DWORD PTR [edx+92276]
	mov	ebx, DWORD PTR [ebp+4]
	call	??2@YAPAXI@Z				; operator new
	add	esp, 12					; 0000000cH
	mov	ecx, ebp
	test	ebp, ebp
	jne	SHORT $L92298
	mov	ecx, eax
$L92298:
	test	ebx, ebx
	mov	DWORD PTR [eax], ecx
	jne	SHORT $L92300
	mov	ebx, eax
$L92300:
	mov	DWORD PTR [eax+4], ebx
	mov	DWORD PTR [ebp+4], eax
	mov	ecx, DWORD PTR [eax+4]
	mov	DWORD PTR [ecx], eax
	add	eax, 8
	test	eax, eax
	je	SHORT $L92314
	mov	DWORD PTR [eax], esi
$L92314:
	mov	eax, DWORD PTR _this$[esp+36]
	inc	DWORD PTR [eax+92280]
$L89749:
	mov	eax, DWORD PTR 8+[esp+32]
	add	edi, 28					; 0000001cH
	dec	eax
	mov	DWORD PTR 8+[esp+32], eax
	jne	$L89748
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx
$L89750:

; 1571 : 		}
; 1572 : 	}
; 1573 : }

	add	esp, 20					; 00000014H
	ret	8
?AddBuildinLight@KIpoTree@@QAEXPAUKBuildInLightInfo@@H@Z ENDP ; KIpoTree::AddBuildinLight
_TEXT	ENDS
PUBLIC	?EnableBioLights@KIpoTree@@QAEX_N@Z		; KIpoTree::EnableBioLights
;	COMDAT ?EnableBioLights@KIpoTree@@QAEX_N@Z
_TEXT	SEGMENT
_bEnable$ = 8
?EnableBioLights@KIpoTree@@QAEX_N@Z PROC NEAR		; KIpoTree::EnableBioLights, COMDAT

; 1577 : 	m_bProcessBioLights = bEnable;

	mov	al, BYTE PTR _bEnable$[esp-4]
	push	esi
	push	edi
	mov	edi, ecx

; 1578 : 
; 1579 : 	if(!bEnable)

	test	al, al
	mov	BYTE PTR [edi+89], al
	jne	SHORT $L89776

; 1580 : 	{
; 1581 : 		//清除所有内建对象的光源
; 1582 : 		list<KLightBase*>::iterator i;
; 1583 : 		for (i = m_LightList.begin(); i != m_LightList.end(); )

	mov	eax, DWORD PTR [edi+92276]
	mov	esi, DWORD PTR [eax]
	cmp	esi, eax
	je	SHORT $L89776
	push	ebp
$L89775:

; 1584 : 		{
; 1585 : 			if((*i)->m_pParent == NULL)

	mov	eax, DWORD PTR [esi+8]
	mov	ecx, DWORD PTR [eax+4]
	test	ecx, ecx
	jne	SHORT $L89777

; 1586 : 			{
; 1587 : 				delete (*i);

	push	eax
	call	??3@YAXPAX@Z				; operator delete

; 1588 : 				*i=NULL;

	mov	DWORD PTR [esi+8], 0

; 1589 : 				i = m_LightList.erase(i);

	mov	eax, DWORD PTR [esi+4]
	mov	ebp, DWORD PTR [esi]
	push	esi
	mov	DWORD PTR [eax], ebp
	mov	ecx, DWORD PTR [esi]
	mov	edx, DWORD PTR [esi+4]
	mov	DWORD PTR [ecx+4], edx
	call	??3@YAXPAX@Z				; operator delete
	mov	eax, DWORD PTR [edi+92280]
	add	esp, 8
	dec	eax
	mov	esi, ebp
	mov	DWORD PTR [edi+92280], eax

; 1590 : 				continue;

	jmp	SHORT $L92541
$L89777:

; 1591 : 			}
; 1592 : 			++i;

	mov	esi, DWORD PTR [esi]
$L92541:
	cmp	esi, DWORD PTR [edi+92276]
	jne	SHORT $L89775
	pop	ebp
$L89776:
	pop	edi
	pop	esi

; 1593 : 		}
; 1594 : 	}
; 1595 : }

	ret	4
?EnableBioLights@KIpoTree@@QAEX_N@Z ENDP		; KIpoTree::EnableBioLights
_TEXT	ENDS
PUBLIC	?EnableDynamicLights@KIpoTree@@QAEX_N@Z		; KIpoTree::EnableDynamicLights
;	COMDAT ?EnableDynamicLights@KIpoTree@@QAEX_N@Z
_TEXT	SEGMENT
_bEnable$ = 8
?EnableDynamicLights@KIpoTree@@QAEX_N@Z PROC NEAR	; KIpoTree::EnableDynamicLights, COMDAT

; 1599 : 	m_bDynamicLighting = bEnable;

	mov	al, BYTE PTR _bEnable$[esp-4]
	mov	BYTE PTR [ecx+90], al

; 1600 : }

	ret	4
?EnableDynamicLights@KIpoTree@@QAEX_N@Z ENDP		; KIpoTree::EnableDynamicLights
_TEXT	ENDS
;	COMDAT ??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ
_TEXT	SEGMENT
??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ PROC NEAR ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >, COMDAT

; 151  : 		{erase(begin(), end());

	push	ebx
	push	esi
	push	edi
	mov	edi, ecx
	mov	ebx, DWORD PTR [edi+4]
	mov	esi, DWORD PTR [ebx]
	cmp	esi, ebx
	je	SHORT $L92767
$L92766:
	mov	eax, esi
	mov	esi, DWORD PTR [esi]
	push	eax
	mov	ecx, DWORD PTR [eax+4]
	mov	edx, DWORD PTR [eax]
	mov	DWORD PTR [ecx], edx
	mov	ecx, DWORD PTR [eax]
	mov	edx, DWORD PTR [eax+4]
	mov	DWORD PTR [ecx+4], edx
	call	??3@YAXPAX@Z				; operator delete
	mov	ecx, DWORD PTR [edi+8]
	add	esp, 4
	dec	ecx
	cmp	esi, ebx
	mov	DWORD PTR [edi+8], ecx
	jne	SHORT $L92766
$L92767:

; 152  : 		_Freenode(_Head);

	mov	eax, DWORD PTR [edi+4]
	push	eax
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4

; 153  : 		_Head = 0, _Size = 0; }

	xor	eax, eax
	mov	DWORD PTR [edi+4], eax
	mov	DWORD PTR [edi+8], eax
	pop	edi
	pop	esi
	pop	ebx
	ret	0
??1?$list@PAVKLightBase@@V?$allocator@PAVKLightBase@@@std@@@std@@QAE@XZ ENDP ; std::list<KLightBase *,std::allocator<KLightBase *> >::~list<KLightBase *,std::allocator<KLightBase *> >
_TEXT	ENDS
END
