/* 
 *   MIRACL compiler/hardware definitions - mirdef.h
 *   This version suitable for use with most 32-bit computers
 *   e.g. 80386+ PC, VAX, ARM etc. Assembly language versions of muldiv,
 *   muldvm, muldvd and muldvd2 will be necessary. See mrmuldv.any 
 *
 *   Also suitable for DJGPP GNU C Compiler
 *   ... but change __int64 to long long
 */


#define MR_LITTLE_ENDIAN
#define MIRACL 64
#define mr_utype __int64
#define MR_IBITS 32
#define MR_LBITS 32
#define mr_unsign32 unsigned int
#define mr_unsign64 unsigned __int64
#define MR_FLASH 52
#define MAXBASE ((mr_small)1<<(MIRACL-1))
#define BITSINCHAR 8