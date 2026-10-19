//=============================================================================
// ScriptFuns_V9_Offers.h
// Shared identity for the four V9 tong-contribution counters.
//
// The counters live as g_V9PlayerOffers[MAX_PLAYER][4] in
// ScriptFuns_V9_GeneralCoreB.cpp, but the Get/Set/Add bindings are split
// across that file and ScriptFuns_V9_GeneralCoreA.cpp, and both name the
// slot by these ids.  Defining them in one .cpp made the other one fail to
// compile (C2065 on every V9_OFFER_* use), which is how this header came to
// exist: one definition, included by both translation units.
//=============================================================================
#ifndef __SCRIPTFUNS_V9_OFFERS_H__
#define __SCRIPTFUNS_V9_OFFERS_H__

#define V9_OFFER_WEEKLY     0
#define V9_OFFER_TOTAL      1
#define V9_OFFER_GOAL       2
#define V9_OFFER_CUMULATE   3

extern int  V9OfferGet(int nPlayerIndex, int nKind);
extern void V9OfferSet(int nPlayerIndex, int nKind, int nValue);
extern void V9OfferAdd(int nPlayerIndex, int nKind, int nDelta);

#endif
