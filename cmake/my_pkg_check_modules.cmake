
macro(my_check_modules)
	pkg_check_modules(${ARGV})

	# For some reason, pkg_check_modules defines _CFLAGS with semicolon separators,
	# but the COMPILE_FLAGS property takes space-separated values.
	string(REPLACE ";" " " MY_STRIPPED "${${ARGV0}_CFLAGS}")
	set(${ARGV0}_CFLAGS "${MY_STRIPPED}")

	# Same with _LDFLAGS
	string(REPLACE ";" " " MY_STRIPPED "${${ARGV0}_LDFLAGS}")
	set(${ARGV0}_LDFLAGS "${MY_STRIPPED}")
endmacro()
