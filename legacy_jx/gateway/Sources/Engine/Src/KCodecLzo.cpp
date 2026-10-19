//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodecLzo.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	LZO Encode and Decode Algorithm
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KCodecLzo.h"
//---------------------------------------------------------------------------
// 函数:	Compress
// 功能:	压缩长度为dwInLen的数据pIn到pOut，返回压缩后的长度pOutLen
// 参数:	pIn			待压缩数据指针
//			dwInLen		待压缩数据长度
//			pOut		压缩后数据指针
//			pOutLen		压缩后数据长度
// 返回:	剩余没有压缩的字节数
//---------------------------------------------------------------------------
DWORD KCodecLzo::Compress(
	 PBYTE	pIn, 		// 待压缩数据
	 DWORD	dwInLen, 	// 待压缩数据长度
	 PBYTE	pOut,		// 压缩数据
	 PDWORD	pOutLen		// 压缩数据长度
	 )
{
	BYTE *ip;
	BYTE *op;
	BYTE *in_end = pIn + dwInLen;
	BYTE *ip_end = pIn + dwInLen - 13;
	BYTE *ii;
	BYTE **dict = (BYTE **)m_WorkMem.GetMemPtr();
	
	op = pOut;
	ip = pIn;
	ii = ip;
	ip += 4;
	
	while (TRUE)
	{
		register PBYTE m_pos;
		// a distance between two addresses in this call, compared below against 0xbfff to decide
		// whether m_pos may be read at all: measuring it in 32 bits would let a dict[] entry
		// left over from an earlier call, four gigabytes away, subtract down to a small match.
		ptrdiff_t m_off;
		unsigned m_len;
		unsigned dindex;
		
		dindex = ((0x21*(((((((unsigned)(ip[3])<<6)^ip[2])<<5)^ip[1])<<5)^ip[0]))>>5) & 0x3fff;
		m_pos = dict [dindex];
		
		if	(m_pos < pIn  ||
			((m_off = ip - m_pos) <= 0) ||
			(m_off > 0xbfff))
			goto literal;
		
		if ((m_off <= 0x0800) || (m_pos[3] == ip[3]))
			goto try_match;
		
		dindex = (dindex & 0x7ff) ^ 0x201f;
		
		m_pos  =  dict [dindex];
		
		if (m_pos  < pIn   ||
			((m_off = ip - m_pos) <= 0)||
			(m_off > 0xbfff))
			goto literal;
		
		if ((m_off <= 0x0800) || (m_pos[3] == ip[3]))
			goto try_match;
		
		goto literal;
		
try_match:
		
		if ((*(unsigned short*)m_pos == *(unsigned short*)ip) &&
			(m_pos[2] == ip[2]))
			goto match;
		
literal:
		
		dict[dindex] = ip;
		++ip;
		if (ip >= ip_end)
			break;
		continue;
		
match:
		
		dict[dindex] = ip;
		if (ip - ii > 0)
		{
			register unsigned t = (unsigned)(ip - ii);	// a literal run inside one input block, whose length is the DWORD dwInLen
			
			if (t <= 3)
				op[-2] |= (BYTE)t;
			else if (t <= 18)
				*op++ = (BYTE) ( t - 3 );
			else
			{
				register unsigned tt = t - 18;
				*op++ = 0;
				while (tt > 255)
				{
					tt -= 255;
					*op++ = 0;
				}
				*op++ = (BYTE)tt;
			}
			do { *op++ = *ii++; } while (--t > 0);
		}
		
		ip += 3;
		
		if (m_pos[3] != *ip++ || m_pos[4] != *ip++ || m_pos[5] != *ip++ ||
			m_pos[6] != *ip++ || m_pos[7] != *ip++ || m_pos[8] != *ip++ )
		{
			--ip;
			m_len = ip - ii;
			
			if (m_off <= 0x0800)
			{
				--m_off;
				*op++ = (BYTE) ( ((m_len - 1) << 5) | ((m_off & 7) << 2) );
				*op++ = (BYTE) ( m_off >> 3 );
			}
			else if (m_off <= 0x4000)
			{
				-- m_off;
				*op++ = (BYTE) ( 32  | (m_len - 2) );
				goto m3_m4_offset;
			}
			else
			{
				m_off -= 0x4000;
				*op++ = (BYTE) (16 | ((m_off & 0x4000) >> 11) | (m_len - 2) );
				goto m3_m4_offset;
			}
		}
		else
		{
			{
				BYTE *end = in_end;
				BYTE *m = m_pos + 9;
				while ((ip < end) && (*m == *ip))
				{
					m++, ip++;
				}
				m_len = (ip - ii);
			}
			
			if (m_off <= 0x4000)
			{
				--m_off;
				if (m_len <= 33)
				{
					*op++ = (BYTE) ( 32  | (m_len - 2));
				}
				else
				{
					m_len -= 33;
					*op++ = 32;
					goto m3_m4_len;
				}
			}
			else
			{
				m_off -= 0x4000;
				if (m_len <= 9 )
				{
					*op++ = (BYTE)(16|((m_off & 0x4000) >> 11) | (m_len - 2));
				}
				else
				{
					m_len -= 9;
					*op++ = (BYTE) (16 | ((m_off & 0x4000) >> 11));
m3_m4_len:
					while (m_len > 255)
					{
						m_len -= 255;
						*op++ = 0;
					}
					*op++ = (BYTE)m_len;
				}
			}
			
m3_m4_offset:
			
			*op++ = (BYTE) ( (m_off & 63) << 2 );
			*op++ = (BYTE) ( m_off >> 6 );
		}
		
		ii = ip;
		if (ip >= ip_end)
			break;
	} // while(TRUE)
	
	*pOutLen = op - pOut;
	return (DWORD)(in_end - ii);
}
//---------------------------------------------------------------------------
// 函数:	Encode
// 功能:	压缩
// 参数:	pCodeInfo	待压缩数据指针
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KCodecLzo::Encode(TCodeInfo* pCodeInfo)
{
	PBYTE	op = pCodeInfo->lpPack;
	DWORD	t;
	DWORD	dwPackLen;

	// Compress treats this block as `BYTE **dict' and indexes it with `... & 0x3fff', so
	// the table is 16384 pointers wide.  65536 byte(s) is that table only while a pointer
	// is 4 bytes: measured at x64 an honest stream faulted inside Compress on 8158 of 8192
	// round-trips and the two `dict[dindex] = ip' stores ran past the block, which the
	// allocator answered at exit with a heap-corruption stop.  Sized from the index space
	// it is the same table at either width - 65536 here, 131072 at x64.  Alloc takes a
	// DWORD at both widths, so the table size is stated as one rather than narrowed in.
	if (!m_WorkMem.Alloc((DWORD)(sizeof(BYTE*) * 0x4000)))
		return FALSE;

	if (pCodeInfo->dwDataLen <= 13)
	{
		t = pCodeInfo->dwDataLen;
	}
	else
	{
		t = Compress(pCodeInfo->lpData, pCodeInfo->dwDataLen, op, &dwPackLen);
		op += dwPackLen;
	}
	
	if (t > 0)
	{
		BYTE *ii = pCodeInfo->lpData + pCodeInfo->dwDataLen - t;
		if (op == pCodeInfo->lpPack && t <= 238)
			*op++ = (BYTE) (17 + t);
		else if (t <= 3)
			op[-2] |= (BYTE)t;
		else if (t <= 18)
			*op++ = (BYTE)(t - 3);
		else
		{
			unsigned tt = t - 18;
			*op++ = 0;
			while (tt > 255)
			{
				tt -= 255;
				*op++ = 0;
			}
			*op++ = (BYTE)tt;
		}
		do { *op++ = *ii++; } while (--t > 0);
	}
	
	*op++ = 17;
	*op++ = 0;
	*op++ = 0;
	
	pCodeInfo->dwPackLen = (DWORD)(op - pCodeInfo->lpPack);
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Decode
// 功能:	解压缩
// 参数:	pCodeInfo	压缩数据指针
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
// Both cursors below are driven by the stream, so the containment of a Decode call is
// what the caller parks beside each pointer in TCodeInfo: dwDataLen is the room behind
// lpData, dwPackLen the length of the stream behind lpPack - and eof_found already
// treats lpPack + dwPackLen as the end of the packet, so the bound is the shipped
// code's own.  Without them a packet states how much gets written: measured, one
// claiming 286 literals put 288 byte(s) into a 1-byte room, one claiming 4111 put 3093
// past a 1024-byte room, and one ending at a guard page faulted its read exactly at the
// packet end, the same at both widths.  Each guard names the byte count the copy under
// it consumes; LZO_BACK keeps a back-reference inside the destination too, which the
// room ceiling above makes reachable for the first time.
#define LZO_OUT(n)	if ((DWORD64)(out_end - op) < (DWORD64)(n)) goto decode_fail
#define LZO_IN(n)	if ((DWORD64)(ip_end - ip) < (DWORD64)(n)) goto decode_fail
#define LZO_BACK(n)	if (m_pos < pCodeInfo->lpData || m_pos > out_end || (DWORD64)(out_end - m_pos) < (DWORD64)(n)) goto decode_fail
BOOL KCodecLzo::Decode(TCodeInfo* pCodeInfo)
{
	register PBYTE op;
	register PBYTE ip;
	register DWORD t;
	register PBYTE m_pos;
	// the two ends, stated by the caller and measured once, so no copy can pass either
	PBYTE const out_end = pCodeInfo->lpData + pCodeInfo->dwDataLen;
	PBYTE const ip_end = pCodeInfo->lpPack + pCodeInfo->dwPackLen;
	
	op = pCodeInfo->lpData;
	ip = pCodeInfo->lpPack;
	
	LZO_IN(1);
	if (*ip > 17)
	{
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		LZO_IN(t);
		LZO_OUT(t);
		do { *op++ = *ip++; } while (--t > 0);
		goto first_literal_run;
	}
	
	while (TRUE)
	{
		LZO_IN(1);
		t = *ip++;
		if (t >= 16) goto match;
		if (t == 0)
		{
			// the run-length scan is a read loop, and the bytes it walks over are the packet's:
			// bounded here so the length byte it stops on is the one guard below asks for.
			while (ip < ip_end && *ip == 0)
			{
				t += 255;
				ip++;
			}
			// a bounded scan can end on the end of the packet as well as on a length byte, and this step
			// reads that byte either way, so the pair is tested here in front of it rather than by a name
			if (ip >= ip_end) goto decode_fail;
			t += 15 + *ip++;
		}
		
		LZO_IN((DWORD64)t + 3);
		LZO_OUT((DWORD64)t + 3);
		* (unsigned *) op = * (unsigned *) ip;
		op += 4;
		ip += 4;
		if (--t > 0)
		{
			if (t >= 4)
			{
				do
				{
					* (unsigned *) op = * (unsigned *) ip;
					op += 4;
					ip += 4;
					t -= 4;
				}
				while (t >= 4);
				if (t > 0) do { *op++ = *ip++; } while (--t > 0);
			}
			else do { *op++ = *ip++; } while (--t > 0);
		}
		
first_literal_run:
		
		LZO_IN(1);
		t = *ip++;
		if (t >= 16)
			goto match;
		
		m_pos = op - 0x0801;
		m_pos -= t >> 2;
		LZO_IN(1);
		m_pos -= *ip++ << 2;
		LZO_BACK(3);
		LZO_OUT(3);
		
		*op++ = *m_pos++;
		*op++ = *m_pos++;
		*op++ = *m_pos;
		
		goto match_done;
		
		while (TRUE)
		{
			
match:

		if (t >= 64)
		{
			m_pos = op - 1;
			m_pos -= (t >> 2) & 7;
			LZO_IN(1);
			m_pos -= *ip++ << 3;
			t = (t >> 5) - 1;
			goto copy_match;
		}
		else if (t >= 32)
		{
			t &= 31;
			if (t == 0)
			{
				while (ip < ip_end && *ip == 0)
				{
					t += 255;
					ip++;
				}
				// a bounded scan can end on the end of the packet as well as on a length byte, and this step
				// reads that byte either way, so the pair is tested here in front of it rather than by a name
				if (ip >= ip_end) goto decode_fail;
				t += 31 + *ip++;
			}
			m_pos = op - 1;
			LZO_IN(2);
			m_pos -= (* (PWORD) ip) >> 2;
			ip += 2;
		}
		else if (t >= 16)
		{
			m_pos = op;
			m_pos -= (t & 8) << 11;
			t &= 7;
			if (t == 0)
			{
				while (ip < ip_end && *ip == 0)
				{
					t += 255;
					ip++;
				}
				// a bounded scan can end on the end of the packet as well as on a length byte, and this step
				// reads that byte either way, so the pair is tested here in front of it rather than by a name
				if (ip >= ip_end) goto decode_fail;
				t += 7 + *ip++;
			}
			// the two bytes read here are the shipped eof marker too (17,0,0 ends a stream by
			// landing m_pos back on op), so a valid packet always has them inside dwPackLen
			LZO_IN(2);
			m_pos -= (* (PWORD) ip) >> 2;
			ip += 2;
			if (m_pos == op)
				goto eof_found;
			m_pos -= 0x4000;
		}
		else
		{
			m_pos = op - 1;
			m_pos -= t >> 2;
			LZO_IN(1);
			m_pos -= *ip++ << 2;
			LZO_BACK(2);
			LZO_OUT(2);
			*op++ = *m_pos++; *op++ = *m_pos;
			goto match_done;
		}
		
		if (t >= 6 && (op - m_pos) >= 4)
		{
			LZO_BACK((DWORD64)t + 2);
			LZO_OUT((DWORD64)t + 2);
			* (unsigned *) op = * (unsigned *) m_pos;
			op += 4;
			m_pos += 4;
			t -= 2;
			do
			{
				* (unsigned *) op = * (unsigned *) m_pos;
				op += 4;
				m_pos += 4;
				t -= 4;
			}
			while (t >= 4);
			if (t > 0) do { *op++ = *m_pos++; } while (--t > 0);
		}
		else
		{

copy_match:

	LZO_BACK((DWORD64)t + 2);
	LZO_OUT((DWORD64)t + 2);

		*op++ = *m_pos++;
		*op++ = *m_pos++;
		do { *op++ = *m_pos++; } while (--t > 0);
		}
		
match_done:

		t = ip[-2] & 3;
		if (t == 0) break;
		
match_next:

		LZO_OUT(t);
		// the run plus the one byte the line after it reads for the next token
		LZO_IN((DWORD64)t + 1);
		do { *op++ = *ip++; } while (--t > 0);
		t = *ip++;
		
		}// while(TRUE)
		
	}// while(TRUE)
	
// the stream asked for more destination, or more packet, than the caller said it holds.
// Answer the way the shipped eof mismatch below already does, so a caller that checks
// the length it stated (KIniFile::LoadPack) or the flag (KNetClient::DeCompress) sees a
// refusal instead of a length that was never written.
decode_fail:
	pCodeInfo->dwDataLen = 0;
	return FALSE;

eof_found:
	
	if (ip != (pCodeInfo->lpPack + pCodeInfo->dwPackLen))
	{
		pCodeInfo->dwDataLen = 0;
		return FALSE;
	}

	pCodeInfo->dwDataLen = (DWORD)(op - pCodeInfo->lpData);
	return TRUE;
}
//---------------------------------------------------------------------------

