#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <bitset>

using namespace std;

typedef void(__cdecl* EXCEPTION_HANDLER)(EXCEPTION_POINTERS*);

enum DebugRegister {
	kError = -1,
	kDR0 = 0,
	kDR1 = 2,
	kDR2 = 4,
	kDR3 = 6
};
enum BreakState {
	kDisabled = 0,				// disabled			- 00
	kEnabledLocally = 1,		// task local		- 01
	kEnabledGlobally = 2,		// global			- 10
	kBreakStateMask = 3			// mask				- 11
};
enum Condition {
	kWhenExecuted = 0,			// on execution			- 00
	kWhenWritten = 1,			// on write				- 01
	kWhenWrittenOrReaden = 2,	// on read or write		- 11
	kConditionMask = 3			// mask					- 11
};
enum Size {
	kByte = 0,					// 1 byte  - 00
	kHalfWord = 1,				// 2 bytes - 01
	kWord = 3,					// 4 bytes - 11
	kDoubleWord = 2,			// 5 bytes - 10
	kSizeMask = 3				// mask      11
};

enum BreakPointType
{
	BpType_Execution = 0,
	BpType_Write = 1,
	BpType_Read_Write = 2,
	BpType_Reserved = 3,
};

struct BreakPoint
{
	DWORD TargetAddress;
	BYTE reservedByte = 0x0;
	int DebugRegister = kError;
	int Type = BpType_Execution;
	int State = kEnabledGlobally;
	int Size = kByte;
};

class AwBreakPoint {
public:

	static bool SetExceptionHandler(EXCEPTION_HANDLER);
	static bool SetHWBreakPoint(BreakPoint*);
	static bool ClearHWBreakPoint(BreakPoint*);

	static bool SetINT3BreakPoint(BreakPoint*);
	static bool ClearINT3BreakPoint(BreakPoint*);

	static EXCEPTION_HANDLER GetHandler();

private:

	static void GetMainThreadFromProcessId();

	static HANDLE m_hThread;
	static EXCEPTION_HANDLER Handler;

	static unsigned short MakeMask(DebugRegister reg);
	static unsigned short MakeFlags(DebugRegister reg, BreakState state, BreakPointType cond, Size size);
};