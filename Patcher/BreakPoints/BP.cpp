#include "BP.h"

struct Break {
	BreakPoint* b;
	DWORD func;
	LPVOID eip;
	bool isEip;
};
static vector<Break> breaks;

void ExceptionHandler(EXCEPTION_POINTERS* ep) {
	for (auto b : breaks) {
		if (reinterpret_cast<DWORD>(ep->ExceptionRecord->ExceptionAddress) == b.b->TargetAddress) {
			if (b.isEip) {
				if (b.func != NULL) {
					void(*pFunction)(PCONTEXT);
					reinterpret_cast<FARPROC&>(pFunction) = reinterpret_cast<FARPROC>(b.func);
					pFunction(ep->ContextRecord);
				}
			} else {
				if (b.func != NULL) {
					void(*pFunction)(PCONTEXT);
					reinterpret_cast<FARPROC&>(pFunction) = reinterpret_cast<FARPROC>(b.func);
					pFunction(ep->ContextRecord);
				}
				ep->ContextRecord->Eip = reinterpret_cast<DWORD>(b.eip);
			}
		}
	}
}

BP::BP() {
	AwBreakPoint::SetExceptionHandler(ExceptionHandler);
}

bool BP::AddHWBreak(DWORD addr, LPVOID pEip, LPVOID func) {
	auto dbg = -1;
	bool db[4] = { false };

	for (auto br : breaks) {
		switch (br.b->DebugRegister) {
			case 0:
				db[0] = true;
				break;
			case 1:
				db[1] = true;
				break;
			case 2:
				db[2] = true;
				break;
			case 3:
				db[3] = true;
				break;
			default: 
				break;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (db[i] != true) {
			dbg = i;
			break;
		}
	}

	if (dbg == -1) {
		return AddINT3Break(addr, pEip, func);
	}

	BreakPoint* buff = new BreakPoint;
	Break b;

	buff->TargetAddress = addr;
	buff->DebugRegister = dbg;

	b.eip = pEip;
	b.func = reinterpret_cast<DWORD>(func);
	b.isEip = false;
	b.b = buff;
	breaks.push_back(b);

	return AwBreakPoint::SetHWBreakPoint(buff);
}

bool BP::AddEipHWBreak(DWORD addr, LPVOID func) {
	int dbg = -1;
	bool db[4] = { false };

	for (auto br : breaks) {
		switch (br.b->DebugRegister) {
			case 0:
				db[0] = true;
				break;
			case 1:
				db[1] = true;
				break;
			case 2:
				db[2] = true;
				break;
			case 3:
				db[3] = true;
				break;
			default:
				break;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (db[i] != true) {
			dbg = i;
			break;
		}
	}

	if (dbg == -1) {
		return AddEipINT3Break(addr, func);
	}

	BreakPoint* buff = new BreakPoint;
	Break b;

	buff->TargetAddress = addr;
	buff->DebugRegister = dbg;

	b.eip = nullptr;
	b.func = reinterpret_cast<DWORD>(func);
	b.isEip = true;
	b.b = buff;
	breaks.push_back(b);

	return AwBreakPoint::SetHWBreakPoint(buff);
}

bool BP::AddINT3Break(DWORD addr, LPVOID pEip, LPVOID func) {
	BreakPoint* buff = new BreakPoint;
	Break b;

	buff->TargetAddress = addr;

	b.eip = pEip;
	b.func = reinterpret_cast<DWORD>(func);
	b.isEip = false;
	b.b = buff;
	breaks.push_back(b);

	return AwBreakPoint::SetINT3BreakPoint(buff);
}

bool BP::AddEipINT3Break(DWORD addr, LPVOID func) {
	BreakPoint* buff = new BreakPoint;
	Break b;

	buff->TargetAddress = addr;

	b.eip = NULL;
	b.func = (DWORD)func;
	b.isEip = true;
	b.b = buff;
	breaks.push_back(b);

	return AwBreakPoint::SetINT3BreakPoint(buff);
}

bool BP::ClearBP(DWORD addr) {
	for (size_t i = 0; i < breaks.size(); i++) {
		BreakPoint* bb = breaks[i].b;
		if (bb->TargetAddress == addr) {
			if (bb->DebugRegister == kError)
				AwBreakPoint::ClearINT3BreakPoint(bb);
			else
				AwBreakPoint::ClearHWBreakPoint(bb);
			breaks.erase(breaks.begin() + i);
			return true;
		}
	}
	return false;
}
