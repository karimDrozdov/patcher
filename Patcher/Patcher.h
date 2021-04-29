#pragma once
#include <Windows.h>
#include "asmjit\asmjit.h"
#include "Mhook\mhook-lib\mhook.h"
#include <vector>

using namespace asmjit;
using namespace asmjit::x86;

extern JitRuntime runtime;
#define ASM(varname) X86Assembler varname(&runtime)

class Patcher {
	struct ABSD {
		LPVOID addr;
		BYTE * ori;
		int Size;

		LPVOID allocated = NULL;
	};
	static std::vector<ABSD> addrs;

public:
	static BOOL PlaceJMP(DWORD pSrc, void *pDest, int nNops = 0);
	static BOOL PlaceJMP(LPVOID pSrc, void *pDest, int nNops = 0);
	static BOOL ClearJMP(DWORD pSrc);
	static BOOL ClearJMP(LPVOID pSrc);

	static LPVOID GetAsmJit(X86Assembler * x86);

	static BOOL PlaceBytes(DWORD pSrc, BYTE* bytes, int Size);
	static BOOL PlaceBytes(LPVOID pSrc, BYTE* bytes, int Size);
	static BOOL SetNOPS(DWORD pSrc, int size);
	static BOOL SetNOPS(LPVOID pSrc, int size);
	static BOOL MemSet(DWORD pSrc, BYTE val, int size);
	static BOOL MemSet(LPVOID pSrc, BYTE val, int size);

	static BOOL PlaceJit(DWORD pSrc, X86Assembler* x86, int Nops = 0);
	static BOOL PlaceJit(LPVOID pSrc, X86Assembler* x86, int Nops = 0);

	static BOOL PlaceCode(DWORD pSrc, X86Assembler* x86);
	static BOOL PlaceCode(LPVOID pSrc, X86Assembler* x86);

	static BOOL PlaceCode(DWORD pSrc, BYTE* bytes, size_t size);
	static BOOL PlaceCode(LPVOID pSrc, BYTE* bytes, size_t size);

	static BOOL PlaceCode(DWORD pSrc, std::vector<BYTE>& bytes);
	static BOOL PlaceCode(LPVOID pSrc, std::vector<BYTE>& bytes);

	static BOOL PlaceCode(DWORD pSrc, char* bytes, size_t size);
	static BOOL PlaceCode(LPVOID pSrc, char* bytes, size_t size);

	static BYTE* GetOriBytes(LPVOID pSrc); // If address include in struct
	static BYTE* GetOriBytes(DWORD pSrc); // If address include in struct

	static BOOL API_Hook(LPVOID ptr, LPVOID dst); //For void* Mhook_SetHook(&reinterpret_cast<PVOID&>(func), new_func)
	static BOOL API_Unhook(LPVOID hooked);

	static void* __cdecl memcpy(void* dst, void* ptr, size_t size);
};

#define WND_PROC(name) LRESULT CALLBACK name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
#define WND_HOOK_A(hWnd, wnd_proc) reinterpret_cast<WNDPROC>(SetWindowLongA(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)wnd_proc))
#define WND_HOOK_W(hWnd, wnd_proc) reinterpret_cast<WNDPROC>(SetWindowLongW(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)wnd_proc))

#define MHOOK_API(func, new_func) Mhook_SetHook(&reinterpret_cast<PVOID&>(func), new_func)