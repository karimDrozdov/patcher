#pragma once
#include "AwBreakPoint.h"
#include <vector>

using namespace std;

class BP {
public:
	BP();

	static bool AddHWBreak(DWORD addr, LPVOID pEip, LPVOID func = nullptr);
	static bool AddEipHWBreak(DWORD addr, LPVOID func = nullptr);

	static bool AddINT3Break(DWORD addr, LPVOID pEip, LPVOID func = nullptr);
	static bool AddEipINT3Break(DWORD addr, LPVOID func = nullptr);

	static bool ClearBP(DWORD addr);
};

