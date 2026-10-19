/* ms64.lib (MIRACL 7.0.1, built by the v100 toolset with whole-program optimization) reaches the
   standard streams through the accessor the pre-UCRT CRT exported, so an x64 link of this module ends
   with LNK2001 on __imp___iob_func and LNK1120: no image.  UCRT has no such accessor and no installed
   library answers the name; msvcrt.dll still exports it, but binding this module to a second C
   runtime would make one image of the product disagree with every other one.

   What the archive does with the returned pointer is fixed by its own generated code, which the
   linker emits only after it resolves the name: every site is

       call    [__imp___iob_func]     ; rax = the base this function returns
       lea     rdx, [rax+0x30]        ; 0x30 = the stride 2010's headers gave a FILE record
       call    [__imp_fputc]          ; and the callee is libucrt's fputc

   so the base has to put UCRT's real stdout exactly one stride in.  That offset is index 1 of the
   array the old accessor returned, and it is the only one the archive forms: nine sites in a link of
   this archive all read the same 0x30, while an image built on any other base dies in its first fputc
   with STATUS_INVALID_HANDLE.  The base itself is never dereferenced, so returning what UCRT's stdout
   minus one stride is - rather than a fabricated stream record - keeps the object the archive writes
   to genuine.

   The Win32 configuration needs no answer: ms32.lib references _iob as data, a symbol the x86 UCRT
   import libraries do define.
*/
#ifdef _WIN64

#include <stdio.h>

#define MR_FILE_STRIDE   0x30u  /* sizeof(FILE) as the archive's own 2010 headers defined it */
#define MR_STREAM_INDEX  1u     /* the index of that array whose stream the archive writes */

static FILE *mr_iob_func(void)
{
    return (FILE *)((char *)stdout - (long)(MR_STREAM_INDEX * MR_FILE_STRIDE));
}

FILE *(*__imp___iob_func)(void) = mr_iob_func;

#endif
