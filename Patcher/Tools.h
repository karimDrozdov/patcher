#pragma once
#include <Windows.h>
#include <vector>

class Tools {
	static bool _is_console;
public:
	struct cmd {
		std::string command;
		std::vector<std::string> args;
	};

	static void ShowConsole(std::string name, 
		std::vector<std::string> print_lines = { "Dll Succefull Attached!\n", "Debug Console Started...\n" });
	static bool IsConsoleShowed();

	static std::vector<cmd*> ReadArgs();
};

