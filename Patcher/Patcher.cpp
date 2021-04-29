#include "Patcher.h"
JitRuntime runtime;
using namespace std;

std::vector<Patcher::ABSD> Patcher::addrs;

BOOL Patcher::PlaceJMP(DWORD pSrc, void * pDest, int nNops) {
	return PlaceJMP((LPVOID)pSrc, pDest, nNops);
}

BOOL Patcher::PlaceJMP(LPVOID pSrc, void *pDest, int nNops) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, 5 + nNops, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[5 + nNops];
	memcpy(a.ori, pSrc, 5 + nNops);
	a.Size = 5 + nNops;
	addrs.push_back(a);

	*(char*)pSrc = (char)0xE9;
	*(DWORD*)((DWORD)pSrc + 1) = (DWORD)pDest - (DWORD)pSrc - 5;

	for (int i = 0; i < nNops; ++i) {
		*(BYTE*)((DWORD)pSrc + 5 + i) = 0x90;
	}

	if (!VirtualProtect(pSrc, 5 + nNops, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::ClearJMP(DWORD pSrc) {
	return ClearJMP((LPVOID)pSrc);
}

BOOL Patcher::ClearJMP(LPVOID pSrc) {
	size_t i;
	for (i = 0; i < addrs.size() && addrs[i].addr != pSrc; i++)
		;
	if (i == addrs.size()) return false;

	DWORD OldProt;
	if (!VirtualProtect(pSrc, addrs[i].Size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	memcpy(addrs[i].addr, addrs[i].ori, addrs[i].Size);
	if (!VirtualProtect(pSrc, addrs[i].Size, OldProt, &OldProt))
		return false;
	addrs.erase(addrs.begin() + i);
	return true;
}

LPVOID Patcher::GetAsmJit(X86Assembler * x86) {
	void * cave = x86->make();
	size_t code_size = x86->getCodeSize();

	void* func = VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(func, cave, code_size);
	x86->relocCode(func);

	return func;
}

BOOL Patcher::PlaceBytes(DWORD pSrc, BYTE* bytes, int Size) {
	return PlaceBytes((LPVOID)pSrc, bytes, Size);
}

BOOL Patcher::PlaceBytes(LPVOID pSrc, BYTE* bytes, int Size) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, Size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[Size];
	memcpy(a.ori, pSrc, Size);
	a.Size = Size;
	addrs.push_back(a);

	for (int i = 0; i < Size; ++i) {
		*(BYTE*)((DWORD)pSrc + i) = bytes[i];
	}

	if (!VirtualProtect(pSrc, Size, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::SetNOPS(DWORD pSrc, int size) {
	return SetNOPS((LPVOID)pSrc, size);
}

BOOL Patcher::SetNOPS(LPVOID pSrc, int size) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[size];
	memcpy(a.ori, pSrc, size);
	a.Size = size;
	addrs.push_back(a);

	memset(pSrc, 0x90, size);

	if (!VirtualProtect(pSrc, size, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::MemSet(DWORD pSrc, BYTE val, int size) {
	return MemSet(LPVOID(pSrc), val, size);
}

BOOL Patcher::MemSet(LPVOID pSrc, BYTE val, int size) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[size];
	memcpy(a.ori, pSrc, size);
	a.Size = size;
	addrs.push_back(a);

	memset(pSrc, val, size);

	if (!VirtualProtect(pSrc, size, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::PlaceJit(DWORD pSrc, X86Assembler * x86, int Nops) {
	return PlaceJit((LPVOID)pSrc, x86, Nops);
}

BOOL Patcher::PlaceJit(LPVOID pSrc, X86Assembler * x86, int Nops) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, 5 + Nops, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = (LPVOID)pSrc;
	a.ori = new BYTE[5 + Nops];
	memcpy(a.ori, pSrc, 5 + Nops);
	a.Size = 5 + Nops;
	addrs.push_back(a);

	void * cave = x86->make();
	size_t code_size = x86->getCodeSize();

	void* func = VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(func, cave, code_size);
	x86->relocCode(func);

	*(char*)pSrc = (char)0xE9;
	*(DWORD*)((DWORD)pSrc + 1) = (DWORD)func - (DWORD)pSrc - 5;

	memset((LPVOID)((DWORD)pSrc + 5), 0x90, Nops);

	if (!VirtualProtect(pSrc, 5 + Nops, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::PlaceCode(DWORD pSrc, X86Assembler * x86) {
	return PlaceCode((LPVOID)pSrc, x86);
}

BOOL Patcher::PlaceCode(LPVOID pSrc, X86Assembler * x86) {
	void * cave = x86->make();
	size_t code_size = x86->getCodeSize();

	DWORD OldProt;
	if (!VirtualProtect(pSrc, code_size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[code_size];
	memcpy(a.ori, pSrc, code_size);
	a.Size = code_size;
	addrs.push_back(a);

	memcpy(pSrc, cave, code_size);
	x86->relocCode(pSrc);

	if (!VirtualProtect(pSrc, code_size, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::PlaceCode(DWORD pSrc, BYTE * bytes, size_t size) {
	return PlaceCode((LPVOID)pSrc, bytes, size);
}

BOOL Patcher::PlaceCode(LPVOID pSrc, BYTE * bytes, size_t size) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[size];
	memcpy(a.ori, pSrc, size);
	a.Size = size;
	addrs.push_back(a);

	memcpy(pSrc, bytes, size);

	if (!VirtualProtect(pSrc, size, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::PlaceCode(DWORD pSrc, vector<BYTE>& bytes) {
	return PlaceCode((LPVOID)pSrc, bytes);
}

BOOL Patcher::PlaceCode(LPVOID pSrc, vector<BYTE>& bytes) {
	size_t size = bytes.size();
	BYTE* bs = new BYTE[size];
	for (size_t i = 0; i < size; i++) {
		bs[i] = bytes[i];
	}

	DWORD OldProt;
	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &OldProt)) {
		delete[] bs;
		return false;
	}

	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[size];
	memcpy(a.ori, pSrc, size);
	a.Size = size;
	addrs.push_back(a);

	memcpy(pSrc, bs, size);

	if (!VirtualProtect(pSrc, size, OldProt, &OldProt)) {
		delete[] bs;
		return false;
	}

	delete[] bs;
	return true;
}

BOOL Patcher::PlaceCode(DWORD pSrc, char * bytes, size_t size) {
	DWORD OldProt;
	if (!VirtualProtect((LPVOID)pSrc, size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = (LPVOID)pSrc;
	a.ori = new BYTE[size];
	memcpy(a.ori, (LPVOID)pSrc, size);
	a.Size = size;
	addrs.push_back(a);

	memcpy((LPVOID)pSrc, bytes, size);

	if (!VirtualProtect((LPVOID)pSrc, size, OldProt, &OldProt))
		return false;
	return true;
}

BOOL Patcher::PlaceCode(LPVOID pSrc, char * bytes, size_t size) {
	DWORD OldProt;
	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	ABSD a;
	a.addr = pSrc;
	a.ori = new BYTE[size];
	memcpy(a.ori, pSrc, size);
	a.Size = size;
	addrs.push_back(a);

	memcpy(pSrc, bytes, size);

	if (!VirtualProtect(pSrc, size, OldProt, &OldProt))
		return false;
	return true;
}

BYTE* Patcher::GetOriBytes(LPVOID pSrc) {
	size_t i;
	for (i = 0; i < addrs.capacity() && addrs[i].addr != pSrc; i++)
		;
	if (i == addrs.capacity()) return NULL;

	return addrs[i].ori;
}

BYTE* Patcher::GetOriBytes(DWORD pSrc) {
	size_t i;
	for (i = 0; i < addrs.capacity() && addrs[i].addr != (LPVOID)pSrc; i++)
		;
	if (i == addrs.capacity()) return NULL;

	return addrs[i].ori;
}

BOOL Patcher::API_Hook(LPVOID ptr, LPVOID dst) {
	return Mhook_SetHook(&(PVOID&)ptr, dst);
}

BOOL Patcher::API_Unhook(LPVOID hooked) {
	return Mhook_Unhook(&(PVOID&)hooked);
}

void * __cdecl Patcher::memcpy(void * dst, void * ptr, size_t size) {
	DWORD OldProt;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &OldProt);
	void * tmp = ::memcpy(dst, ptr, size);
	VirtualProtect(dst, size, OldProt, &OldProt);
	return tmp;
}
