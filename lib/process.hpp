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

#ifndef H_PROCESS_0
#define H_PROCESS_0

#include <posix_memory.hpp>
#include <string_utils.hpp>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string>
#include <vector>
#include <exception>

#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

// converts the vector of argument and program name to a vector that can be used as argv.
// Notice that argv needs to be "char* const", and vector<const char*> would be "const char* const"
inline std::vector<char*> to_argv(std::string& program, std::vector<std::string>& arguments){
	std::vector<char*> toreturn; toreturn.reserve(arguments.size()+2);
	toreturn.push_back(&program[0]);
	for(auto& arg : arguments) {
		toreturn.push_back(&arg[0]);
	}
	toreturn.push_back(nullptr);
	return toreturn;
}

inline std::vector<char*> to_argv(std::vector<std::string>& arguments){
	std::vector<char*> toreturn; toreturn.reserve(arguments.size()+1);
	for(auto& arg : arguments) {
		toreturn.push_back(&arg[0]);
	}
	toreturn.push_back(nullptr);
	return toreturn;
}

/// use struct instead of char* const*, for avoiding confusione between env and args
struct environ_var{
	char* const* envp = nullptr;
	explicit environ_var(char* const envp_[]) : envp(envp_){}
	explicit environ_var(){}
	char* const* getenv() const {return envp;}
};

/// This function handles memory (otherwise it wouldn't be const-safe), so it may fail.
/// Prefer the variadic variant if the number of arguments is know at compile-time
inline int execute(std::string filename, const environ_var& env, std::vector<std::string> args){
	const auto argv = to_argv(filename, args);
	if(env.getenv() != nullptr){
		return execvpe(filename.c_str(), &argv[0], env.getenv());
	}
	return execvp(filename.c_str(), &argv[0]);
}

inline int execute(FILE* file, const environ_var& env, std::vector<std::string> args){
	const int file_no = fileno(file);
	const auto argv = to_argv(args);
	return fexecve(file_no, &argv[0], env.getenv());
}

// for using const char* and std::string inside the variadic execute
inline const char* to_c_str(const std::string& s){
	return s.c_str();
}
inline const char* to_c_str(const char* s){
	return s;
}

template<typename... Args>
inline int execute(const std::string& filename, const environ_var& env, const Args... args){
	//static_assert(std::is_same<Args..., std::string>::value, "must be stringtype (std::string or const char*)");
	if(env.getenv() != nullptr){
		return execle(filename.c_str(), filename.c_str(), to_c_str(args)..., nullptr, env.getenv());
	}
	return execlp(filename.c_str(), filename.c_str(), to_c_str(args)..., nullptr);
}

// dumb name, but it's what it does
// using _Exit and not _exit or exit for avoiding flushing buffers, deleting tmpfiles, and so on
struct exit_on_exit_in_child{
	explicit exit_on_exit_in_child(){}
	~exit_on_exit_in_child(){
		_Exit(EXIT_FAILURE);
	}
};

// FIXME: It should be possible to create a variation of popen_and_execute that takes a std::tuple/std::array
// if the number of arguments is known at compile time.
struct exec_params{
	bool waitfinishes = false; // give maximum time?
	bool captureoutput = false;
	std::vector<std::string> args;
	environ_var env;
};

struct exec_result{
	int status = -1;
	std::string output;
	//pid_t pid = -1; // may be invalid
};

// this function will fork and captures the output of program if requested
// FIXME: with posix_spawn it may reduce code complexity
inline exec_result fork_and_execute(std::string program, const exec_params& p){
	exec_result res;
	int pipefd[2] = {};
	if(p.captureoutput){
		if(::pipe(pipefd) != 0){ //create a pipe
			return res;
		}
	}
	const auto pid = ::fork();
	if(pid < 0){
		return res;
	}
	if(pid == 0){
		exit_on_exit_in_child _;
		if(p.captureoutput){
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			dup2(pipefd[1], STDERR_FILENO);
		}
		execute(program, p.env, p.args);
	}

	//res.pid = pid;

	if(p.captureoutput){
		close(pipefd[1]);
		FILE_handle output(fdopen(pipefd[0], "r"));
		std::string buffer;
		std::string result;
		while(mygetline(buffer, output.get())){
			result += buffer + "\n";
		}
		res.output = result;
	}
	if(p.waitfinishes){
		int status = 0;
		::waitpid(pid, &status, 0);
	}
	res.status = 0;
	return res;
}

// FIXME: this is a test function (it does not work currently)
inline exec_result fork_and_execute_in_mate_term(std::string program, const exec_params& p){
	exec_result res; (void)p; (void)program;

	int fd_p2m[2] = {};

	if(::pipe(fd_p2m) != 0){ //create a pipe
		res.status = -2;
		return res;
	}

	const auto pidmate = ::fork();
	if(pidmate < 0){
		res.status = -3;
		return res;
	}
	if(pidmate == 0){
		exit_on_exit_in_child _;
		if(dup2(fd_p2m[0], STDIN_FILENO) != -1 /*&& close(fd_p2m[0]) == 0 && close(fd_p2m[1]) == 0*/){
			execlp("mate-terminal", "mate-terminal", (char*)NULL);
		}
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::string s = "echo a\n";

	FILE_handle input(fdopen(fd_p2m[1], "w"));
	if(fputs(s.c_str(), input.get())<0){
		res.status = -4;
		return res;
	}/*
	if(write(fd_p2m[1], s.c_str(), s.size()) != s.size()){
		res.status = -4;
		return res;
	}*/

/*
	const auto pid = ::fork();
	if(pid < 0){
		return res;
	}
	if (pid == 0) {
		exit_on_exit_in_child _(pid);
		//close(opipe[0]);
		dup2(p2m[1], STDOUT_FILENO);
		dup2(p2m[1], STDERR_FILENO);
		execute(program, p.env, p.args);
	}


*/
	res.status = 0;
	return res;
}

#endif
