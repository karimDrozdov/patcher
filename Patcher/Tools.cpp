#include "Tools.h"
#include <io.h>

bool Tools::_is_console = false;

void Tools::ShowConsole(std::string name, std::vector<std::string> print_lines) {
	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		*stdout = *stderr = *_fdopen(_open_osfhandle(long(GetStdHandle(STD_OUTPUT_HANDLE)), 2), "w");

		if (!name.empty())
			SetConsoleTitleA(name.c_str());
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		for (auto line : print_lines)
			printf(line.c_str());
		_is_console = true;
	}
}

bool Tools::IsConsoleShowed() {
	return _is_console;
}

std::vector<Tools::cmd*> Tools::ReadArgs() {
	std::vector<cmd*> out;
	auto count = 0;
	const auto lp_argv = CommandLineToArgvW(GetCommandLineW(), &count);
	std::vector<char*> tmp;
	for (auto i = 0; i < count; ++i) {
		const int size = wcslen(lp_argv[i]) + 1;
		auto ptr = new char[size];
		wcstombs(ptr, lp_argv[i], size);
		tmp.push_back(ptr);
	}
	for (auto it = tmp.begin(); it < tmp.end(); ++it) {
		const auto t = *it;
		if (t[0] == '-') {
			auto c = new cmd;
			c->command = t;
			for (++it; it != tmp.end(); ++it) {
				if (*it[0] != '-')
					c->args.emplace_back(*it);
				else {
					--it;
					break;
				}
			}
			out.push_back(c);
		}
	}
	LocalFree(lp_argv);
	return out;
}
