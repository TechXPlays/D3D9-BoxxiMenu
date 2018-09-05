#include "SystemIncludes.h"


void PrintText(char pString[], int x, int y, D3DCOLOR col, ID3DXFont *font)
{
    RECT FontRect = { x, y, x+500, y+30 };
    font->DrawText( NULL, pString, -1, &FontRect, DT_LEFT | DT_WORDBREAK, col);
}

void FillRGB( int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice )
{
    if( w < 0 )w = 1;
    if( h < 0 )h = 1;
    if( x < 0 )x = 1;
    if( y < 0 )y = 1;

    D3DRECT rec = { x, y, x + w, y + h };
    pDevice->Clear( 1, &rec, D3DCLEAR_TARGET, color, 0, 0 );
}

void DrawBorder( int x, int y, int w, int h, int px, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice )
{
    FillRGB( x, (y + h - px), w, px,    BorderColor, pDevice );
    FillRGB( x, y, px, h,                BorderColor, pDevice );
    FillRGB( x, y, w, px,                BorderColor, pDevice );
    FillRGB( (x + w - px), y, px, h,    BorderColor, pDevice );
}

void DrawBox( int x, int y, int w, int h, D3DCOLOR BoxColor, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice )
{
    FillRGB( x, y, w, h,        BoxColor, pDevice );
    DrawBorder( x, y, w, h, 1,    BorderColor, pDevice );
}  

bool isMouseinRegion(int x1, int y1, int x2, int y2) 
{
        POINT cPos;
        GetCursorPos(&cPos);
        if(cPos.x > x1 && cPos.x < x2 && cPos.y > y1 && cPos.y < y2){
                return true;
        } else {
                return false;
        }
}

bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
        for(;*szMask;++szMask,++pData,++bMask)
                if(*szMask=='x' && *pData!=*bMask)  
                        return 0;
        return (*szMask) == NULL;
}

DWORD FindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
        for(DWORD i=0; i<dwLen; i++)
                if (bCompare((BYTE*)(dwAddress+i),bMask,szMask))  
                        return (DWORD)(dwAddress+i);
        return 0;
}

/*void *DetourFunction (BYTE *src, const BYTE *dst, const int len)
{
                BYTE *jmp = (BYTE*)malloc(len+5);
        DWORD dwBack;

        VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwBack);
        memcpy(jmp, src, len);  
        jmp += len;
        jmp[0] = 0xE9;
        *(DWORD*)(jmp+1) = (DWORD)(src+len - jmp) - 5;
        src[0] = 0xE9;
        *(DWORD*)(src+1) = (DWORD)(dst - src) - 5;
        for (int i=5; i<len; i++)  src[i]=0x90;
        VirtualProtect(src, len, dwBack, &dwBack);
        return (jmp-len);
}*/


//-----------------------------------------------------------------------------
//     Copyright © November/2011. MPGH/Qmoa. All rights reserved.
//                  ||  Do not forget to credits ||
//-----------------------------------------------------------------------------
// flavor from the Base cheat ... need to Edit ok, and the creation of your abilities
//( Clue : discard the unnecessary )
//-----------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>

#include "ADE32.h"

#define DETOUR_LEN_AUTO 0      // Finds the detour length automatically

enum
{
	DETOUR_TYPE_JMP,          // min detour len: 5
	DETOUR_TYPE_PUSH_RET,     // min detour len: 6
	DETOUR_TYPE_NOP_JMP,      // min detour len: 6
	DETOUR_TYPE_NOP_NOP_JMP,  // min detour len: 7
	DETOUR_TYPE_STC_JC,       // min detour len: 7
	DETOUR_TYPE_CLC_JNC,      // min detour len: 7
};

LPVOID DetourCreate(LPVOID lpFuncOrig, LPVOID lpFuncDetour, int patchType, int detourLen=DETOUR_LEN_AUTO);
LPVOID DetourCreate(LPCSTR lpModuleName, LPCSTR lpProcName, LPVOID lpFuncDetour, int patchType, int detourLen=DETOUR_LEN_AUTO);
BOOL DetourRemove(LPVOID lpDetourCreatePtr);

#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma warning(disable: 4244)

#define DETOUR_MAX_SRCH_OPLEN 64

#define JMP32_SZ 5
#define BIT32_SZ 4

// jmp32 sig
#define SIG_SZ 3
#define SIG_OP_0 0xCC
#define SIG_OP_1 0x90
#define SIG_OP_2 0xC3

static DWORD Silamguard;

int GetDetourLen(int patchType);
int GetDetourLenAuto(PBYTE &pbFuncOrig, int minDetLen);

// Thin wrapper for APIs
LPVOID DetourCreate(LPCSTR lpModuleName, LPCSTR lpProcName, LPVOID lpFuncDetour, int patchType, int detourLen)
{
	LPVOID lpFuncOrig = NULL;
		
	if((lpFuncOrig = GetProcAddress(GetModuleHandle(lpModuleName), lpProcName)) == NULL)
		return NULL;

	return DetourCreate(lpFuncOrig, lpFuncDetour, patchType, detourLen);
}

LPVOID DetourCreate(LPVOID lpFuncOrig, LPVOID lpFuncDetour, int patchType, int detourLen)
{
	LPVOID lpMallocPtr = NULL;
	DWORD dwProt = NULL;
	PBYTE NewocPBrs = NULL;
	PBYTE pbFuncOrig = (PBYTE)lpFuncOrig;
	PBYTE pbFuncDetour = (PBYTE)lpFuncDetour;
	PBYTE pbPatchBuf = NULL;
	int minDetLen = 0;
	int detLen = 0;

	// Get detour length
	if((minDetLen = GetDetourLen(patchType)) == 0)
		return NULL;

	if(detourLen != DETOUR_LEN_AUTO)
		detLen = detourLen;

	else if((detLen = GetDetourLenAuto(pbFuncOrig, minDetLen)) < minDetLen)
		return NULL;

	// Alloc mem for the overwritten bytes
	if((lpMallocPtr = (LPVOID)malloc(detLen+JMP32_SZ+SIG_SZ)) == NULL)
		return NULL;

	NewocPBrs = (PBYTE)lpMallocPtr;

	// Enable writing to original
	VirtualProtect(lpFuncOrig, detLen, PAGE_READWRITE, &dwProt);

	// Write overwritten bytes to the malloc
	memcpy(lpMallocPtr, lpFuncOrig, detLen);
	NewocPBrs += detLen;
	NewocPBrs[0] = 0xE9;
	*(DWORD*)(NewocPBrs+1) = (DWORD)((pbFuncOrig+detLen)-NewocPBrs)-JMP32_SZ;
	NewocPBrs += JMP32_SZ;
	NewocPBrs[0] = SIG_OP_0;
	NewocPBrs[1] = SIG_OP_1;
	NewocPBrs[2] = SIG_OP_2;

	// Create a buffer to prepare the detour bytes
	pbPatchBuf = new BYTE[detLen];
	memset(pbPatchBuf, 0x90, detLen);

	switch(patchType)
	{
		case DETOUR_TYPE_JMP:
			pbPatchBuf[0] = 0xE9;
			*(DWORD*)&pbPatchBuf[1] = (DWORD)(pbFuncDetour - pbFuncOrig) - 5;
			break;

		case DETOUR_TYPE_PUSH_RET:
			pbPatchBuf[0] = 0x68;
			*(DWORD*)&pbPatchBuf[1] = (DWORD)pbFuncDetour;
			pbPatchBuf[5] = 0xC3;
			break;

		case DETOUR_TYPE_NOP_JMP:
			pbPatchBuf[0] = 0x90;
			pbPatchBuf[1] = 0xE9;
			*(DWORD*)&pbPatchBuf[2] = (DWORD)(pbFuncDetour - pbFuncOrig) - 6;
			break;

		case DETOUR_TYPE_NOP_NOP_JMP:
			pbPatchBuf[0] = 0x90;
			pbPatchBuf[1] = 0x90;
			pbPatchBuf[2] = 0xE9;
			*(DWORD*)&pbPatchBuf[3] = (DWORD)(pbFuncDetour - pbFuncOrig) - 7;
			break;

		case DETOUR_TYPE_STC_JC:
			pbPatchBuf[0] = 0xF9;
			pbPatchBuf[1] = 0x0F;
			pbPatchBuf[2] = 0x82;
			*(DWORD*)&pbPatchBuf[3] = (DWORD)(pbFuncDetour - pbFuncOrig) - 7;
			break;

		case DETOUR_TYPE_CLC_JNC:
			pbPatchBuf[0] = 0xF8;
			pbPatchBuf[1] = 0x0F;
			pbPatchBuf[2] = 0x83;
			*(DWORD*)&pbPatchBuf[3] = (DWORD)(pbFuncDetour - pbFuncOrig) - 7;
			break;
		
		default:
			return NULL;
	}

	// Write the detour
	for(int i=0; i<detLen; i++)
		pbFuncOrig[i] = pbPatchBuf[i];

	delete [] pbPatchBuf;

	// Reset original mem flags
	VirtualProtect(lpFuncOrig, detLen, dwProt, &Silamguard);

	return lpMallocPtr;
}

BOOL DetourRemove(LPVOID lpDetourCreatePtr)
{
	PBYTE NewocPBrs = NULL;
	DWORD NewOrig = NULL;
	DWORD NewProt = NULL;
	int i=0;

	if((NewocPBrs = (PBYTE)lpDetourCreatePtr) == NULL)	
		return FALSE;

	// Find the orig jmp32 opcode sig
	for(i=0; i<=DETOUR_MAX_SRCH_OPLEN; i++)
	{
		if(NewocPBrs[i] == SIG_OP_0 
			&& NewocPBrs[i+1] == SIG_OP_1
			&& NewocPBrs[i+2] == SIG_OP_2)
			break;

		if(i == DETOUR_MAX_SRCH_OPLEN)
			return FALSE;
	}

	// Calculate the original address
	NewocPBrs += (i-JMP32_SZ+1);        // Inc to jmp
	NewOrig = *(DWORD*)NewocPBrs;       // Get 32bit jmp
	NewocPBrs += BIT32_SZ;              // Inc to end of jmp
	NewOrig += (DWORD)NewocPBrs;        // Add this addr to 32bit jmp
	NewOrig -= (i-JMP32_SZ);            // Dec by detour len to get to start of orig

	// Write the overwritten bytes back to the original
	VirtualProtect((LPVOID)NewOrig, (i-JMP32_SZ), PAGE_READWRITE, &NewProt);
	memcpy((LPVOID)NewOrig, lpDetourCreatePtr, (i-JMP32_SZ));
	VirtualProtect((LPVOID)NewOrig, (i-JMP32_SZ), NewProt, &Silamguard);

	// Memory cleanup
	free(lpDetourCreatePtr);

	return TRUE;
}

int GetDetourLen(int patchType)
{
	switch(patchType)
	{
		case DETOUR_TYPE_JMP:
			return 5;

		case DETOUR_TYPE_PUSH_RET:
		case DETOUR_TYPE_NOP_JMP:
			return 6;
		
		case DETOUR_TYPE_NOP_NOP_JMP:
		case DETOUR_TYPE_STC_JC:
		case DETOUR_TYPE_CLC_JNC:
			return 7;
		
		default:
			return 0;
	}
}

int GetDetourLenAuto(PBYTE &pbFuncOrig, int minDetLen)
{
	int len = 0;
	PBYTE pbCurOp = pbFuncOrig;

	while(len < minDetLen)
	{
		int i = oplen(pbCurOp);
		
		if(i == 0 || i == -1)
			return 0;

		if(len > DETOUR_MAX_SRCH_OPLEN)
			return 0;

		len += i;
		pbCurOp += i;
	}

	return len;
}