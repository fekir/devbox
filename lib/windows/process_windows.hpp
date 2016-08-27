//
// Copyright (C) 2016 Federico Kircheis
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//

/// This package contains utility functions for invoking programs

#ifndef H_PROCESS_WINDOWS_0
#define H_PROCESS_WINDOWS_0


#include "process_base.hpp"
#include "string_utils.hpp"

#include <Windows.h>
#include <WinBase.h>

#include <string>
#include <vector>
#include <exception>

struct wexec_params {
	bool waitfinishes = false; // give maximum time?
	bool captureoutput = false;
	bool parseargs = false; // if true, parses arguments and adds "\"" on every arg, otherwise it simply append them one to another with spaces
	std::vector<std::wstring> args;
	environ_var env;
};

struct wexec_result {
	int status = -1;
	std::string output;
	//pid_t pid = -1; // may be invalid
};

/// This function handles memory (otherwise it wouldn't be const-safe), so it may fail.
inline int execute_program(std::wstring program,/* const environ_var& env,*/ std::vector<std::wstring> args){
	STARTUPINFOW si{};
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi{};
	
	auto argv = program + L" " + args_to_str(args, true);
		
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx
	// FIXME: see Process Creation Flags, currently set to 0
	
	auto res = CreateProcessW(program.c_str(), &argv[0], nullptr, nullptr, FALSE, 0, 0, 0, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return !!res;
}

inline wexec_result execute_program(std::wstring program, const wexec_params& p) {
	auto argv = program + L" " + args_to_str(p.args, p.parseargs);
	STARTUPINFOW si{};
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi{};


	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx
	// FIXME: see Process Creation Flags, currently set to 0

	auto res = CreateProcessW(program.c_str(), &argv[0], nullptr, nullptr, FALSE, 0, 0, 0, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return{};
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682499%28v=vs.85%29.aspx
inline wexec_result execute_program2(const std::wstring& program, const wexec_params& p) {
	SECURITY_ATTRIBUTES saAttr{};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;
	
	auto cmdline = program + L" " + args_to_str(p.args, true);


	wexec_result res;

	// Create a pipe for the child process's STDOUT. 
	HANDLE stdoutReadHandle = nullptr;
	HANDLE stdoutWriteHandle = nullptr;
	if (p.captureoutput) {
		if (!CreatePipe(&stdoutReadHandle, &stdoutWriteHandle, &saAttr, 5000)) {
			res.status = -1;
			return res;
		}
	
		// Ensure the read handle to the pipe for STDOUT is not inherited.
		if (!SetHandleInformation(stdoutReadHandle, HANDLE_FLAG_INHERIT, 0)){
			res.status = -2;
			return res;
		}
	}

	STARTUPINFOW startupInfo{};
	startupInfo.cb = sizeof(startupInfo);
	if (p.captureoutput) {
		startupInfo.hStdError = stdoutWriteHandle;
		startupInfo.hStdOutput = stdoutWriteHandle;
		startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	}


	PROCESS_INFORMATION processInfo{};
	if (!CreateProcessW(program.c_str(), &cmdline[0], nullptr, nullptr, p.captureoutput ? TRUE : FALSE,
		CREATE_NO_WINDOW, NULL, 0, &startupInfo, &processInfo)){
		res.status = -3;
		return res;
	}

	if (p.captureoutput) {
		::CloseHandle(stdoutWriteHandle);

		for (;;) {
			std::string buffer(256, '\0');
			DWORD bytes_read;
			if (!ReadFile(stdoutReadHandle, &buffer[0], buffer.size() - 1, &bytes_read, nullptr))
			{
				break;
			}
			if (bytes_read > 0)
			{
				buffer.resize(bytes_read);
				res.output += buffer;
			}
		}
	}
	

	if (p.waitfinishes) {
		if (WaitForSingleObject(processInfo.hProcess, INFINITE) != WAIT_OBJECT_0)
		{
			res.status = -5;
			return res;
		}

		DWORD exitcode;
		if (!GetExitCodeProcess(processInfo.hProcess, &exitcode))
		{
			res.status = -6;
			return res;
		}
	}

	res.status = 0;

	::CloseHandle(processInfo.hProcess);
	::CloseHandle(processInfo.hThread); // FIXME: use RAII

	return res;
}

#endif
