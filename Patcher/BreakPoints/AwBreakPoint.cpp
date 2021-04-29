#include "AwBreakPoint.h"

unsigned short AwBreakPoint::MakeFlags(DebugRegister reg, BreakState state, BreakPointType cond, Size size) {
	return (state | cond << 16 | size << 24) << reg;
}
unsigned short AwBreakPoint::MakeMask(DebugRegister reg) {
	return MakeFlags(reg, kBreakStateMask, BpType_Reserved, kSizeMask);
}

HANDLE AwBreakPoint::m_hThread;
EXCEPTION_HANDLER AwBreakPoint::Handler;

LONG __stdcall Ex_handler(EXCEPTION_POINTERS* ep) {
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP || ep->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
		const auto handler = AwBreakPoint::GetHandler();
		handler(ep);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void AwBreakPoint::GetMainThreadFromProcessId() {
	unsigned long uProcessId = GetCurrentProcessId();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, uProcessId);

	if (!hSnapshot)
		return;

	THREADENTRY32 lpThread;

	lpThread.dwSize = sizeof(THREADENTRY32);

	if (Thread32First(hSnapshot, &lpThread))
	{
		do {
			if (lpThread.th32OwnerProcessID == uProcessId)
			{
				break;
			}
		} while (Thread32Next(hSnapshot, &lpThread));

		CloseHandle(hSnapshot);
		AwBreakPoint::m_hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 1, lpThread.th32ThreadID);
	}
	return;
}

bool AwBreakPoint::SetExceptionHandler(EXCEPTION_HANDLER eHandler) {
	Handler = eHandler;
	AddVectoredExceptionHandler(1, Ex_handler);
	return true;
}

EXCEPTION_HANDLER AwBreakPoint::GetHandler() {
	return AwBreakPoint::Handler;
}

inline void SetBits(unsigned long& dw, int lowBit, int bits, int newValue) {
	int mask = (1 << bits) - 1; // e.g. 1 becomes 0001, 2 becomes 0011, 3 becomes 0111
	dw = (dw & ~(mask << lowBit)) | (newValue << lowBit);
}

//void SetBit(DWORD& ptr, int bit, int size, int value)
//{
//	/*bitset<32> val(value);
//	for (int i = 0; i < size; i++)
//	{
//		ptr |= (val[i] << bit + i);
//	}*/
//	//printf("%d\n", bit);
//	//printf("%d\n", size);
//	//printf("================\n");
//	bitset<32> x(ptr);
//	bitset<32> val(value);
//
//	for (int i = 0; i < size; i++)
//	{
//		//x.set(bit + i, val[i]);
//		x[bit + i] = val[i];
//	}
//	ptr = x.to_ulong();
//}

#define SetBit(reg, bit) reg |= (1<<bit)

bool AwBreakPoint::SetHWBreakPoint(BreakPoint* bp) {
	GetMainThreadFromProcessId();

	CONTEXT c = { CONTEXT_DEBUG_REGISTERS };

	SuspendThread(AwBreakPoint::m_hThread);

	GetThreadContext(AwBreakPoint::m_hThread, &c);

	switch (bp->DebugRegister)
	{
	case 0:
		c.Dr0 = bp->TargetAddress;
	//	c.Dr7 = (1 << 0); // set 0th bit
		break;

	case 1:
		c.Dr1 = bp->TargetAddress;
	//	c.Dr7 |= (1 << 2); // set 2nd bit
		break;

	case 2:
		c.Dr2 = bp->TargetAddress;
	//	c.Dr7 |= (1 << 4); // set 4th bit
		break;

	case 3:
		c.Dr3 = bp->TargetAddress;
	//	c.Dr7 |= (1 << 6); // set 6th bit
		break;

	default:
		ResumeThread(AwBreakPoint::m_hThread);
		return false;
	}
	
	SetBits(c.Dr7, 16 + bp->DebugRegister * 4, 2, bp->Type);
	SetBits(c.Dr7, 18 + bp->DebugRegister * 4, 2, bp->Size);
	SetBits(c.Dr7, bp->DebugRegister * 2, 1, 1);

	//Sett(c.Dr7, bp->DebugRegister * 2, 2, bp->State);
	//cout << bitset<32>(bp->State).to_string() << endl;;
	//SetBit(c.Dr7, bp->DebugRegister * 2, 2, bp->State);
	//SetBit(c.Dr7, 16 + (bp->DebugRegister * 4), 2, bp->Type);
	//SetBit(c.Dr7, 18 + (bp->DebugRegister * 4), 2, bp->Size);

	//bitset<32> d(c.Dr7);
	//cout << d.to_string() << endl;
	//SetBits(c.Dr7, 16 + (bp->DebugRegister * 4), 2, bp->Type);
	//SetBits(c.Dr7, 18 + (bp->DebugRegister * 4), 2, bp->Size);
	/*SetBits(c.Dr7, 16 + (bp->DebugRegister * 4), 2, bp->Type);
	SetBits(c.Dr7, 18 + (bp->DebugRegister * 4), 2, bp->Size);
	SetBits(c.Dr7, bp->DebugRegister * 2, 1, 1);*/

	//printf("%X\n", c.Dr7);

	c.Dr6 = 0;

	SetThreadContext(AwBreakPoint::m_hThread, &c);

	ResumeThread(AwBreakPoint::m_hThread);

	return true;
}

bool AwBreakPoint::ClearHWBreakPoint(BreakPoint* bp) {
	if (bp->DebugRegister == kError)
		return false;

	GetMainThreadFromProcessId();

	CONTEXT c = { CONTEXT_DEBUG_REGISTERS };

	SuspendThread(AwBreakPoint::m_hThread);

	GetThreadContext(AwBreakPoint::m_hThread, &c);

	//switch (bp->DebugRegister)
	//{
	//case 0:
	//	c.Dr7 = (0 << 0); // set 0th bit
	//	break;

	//case 1:
	//	c.Dr7 |= (0 << 2); // set 2nd bit
	//	break;

	//case 2:
	//	c.Dr7 |= (0 << 4); // set 4th bit
	//	break;

	//case 3:
	//	c.Dr7 |= (0 << 6); // set 6th bit
	//	break;

	//default:
	//	return false;
	//}

	switch (bp->DebugRegister)
	{
	case 0:
		c.Dr0 = 0;
		break;
	case 1:
		c.Dr1 = 0;
		break;
	case 2:
		c.Dr2 = 0;
		break;
	case 3:
		c.Dr3 = 0;
		break;
	default: 
		break;
	}
	SetBits(c.Dr7, bp->DebugRegister * 2, 1, 0);

	c.Dr6 = 0;

	SetThreadContext(AwBreakPoint::m_hThread, &c);

	ResumeThread(AwBreakPoint::m_hThread);

	return true;
}

bool AwBreakPoint::SetINT3BreakPoint(BreakPoint *bp) {
	DWORD OldProt;
	if (!VirtualProtect((LPVOID)bp->TargetAddress, 1, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;

	bp->reservedByte = *(BYTE*)bp->TargetAddress;
	*(BYTE*)bp->TargetAddress = 0xCC;

	if (!VirtualProtect((LPVOID)bp->TargetAddress, 1, OldProt, &OldProt))
		return false;

	return true;
}

bool AwBreakPoint::ClearINT3BreakPoint(BreakPoint *bp) {
	DWORD OldProt;
	if (!VirtualProtect((LPVOID)bp->TargetAddress, 1, PAGE_EXECUTE_READWRITE, &OldProt))
		return false;
	*(char*)bp->TargetAddress = bp->reservedByte;
	bp->reservedByte = 0;
	if (!VirtualProtect((LPVOID)bp->TargetAddress, 1, OldProt, &OldProt))
		return false;
	return 1;
}
