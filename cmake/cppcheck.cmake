
set(cppcheck_bin "/usr/bin/cppcheck" CACHE STRING "cppcheck executable" FORCE)

# adds target POST_BUILD with cppcheck
function(analyze_target_with_cppcheck target list_additional_includes)
	# default values

	set(prop_include "$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>")
	#set(additional_includes_param "-I$<JOIN:$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>, -I >")
	set(additional_includes_param "$<$<BOOL:${prop_include}>:-I$<JOIN:${prop_include}, -I >>")

	#set(template $<$<CXX_COMPILER_ID:gcc>:--template=gcc>)


	set(template [=[--template="[{severity}][{id}][{file}][line {line}] {message} {callstack}"]=])
	# can it differ between targets??
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(template "gcc")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(template "gcc")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(template "vs")
	endif()

	 # source files, may miss some headers and include some that are not used by the compiler
	set(prop_sources "$<TARGET_PROPERTY:${target},SOURCES>")
	set(sources "$<$<BOOL:${prop_sources}>:$<JOIN:${prop_sources},\;>>")


	 # source directory, may contain some file that are not used by the compiler
	set(prop_source_dir "$<TARGET_PROPERTY:${target},SOURCE_DIR>")

	# check if compiling with c++11
	set(std "c++11")
	set(enable "warning,style,performance,portability,information,missingInclude")
	set(library "--library=qt.cfg")

	file(
		GENERATE OUTPUT cppcheck_for_${target}.cmake
		CONTENT
		"execute_process(\n\
			COMMAND ${cppcheck_bin}\n\
			--enable=${enable}\n\
			--std=${std}\n\
			#${library}\n\
			--template=${template}\n\
			--verbose\n\
			--quiet\n\
			#${additional_includes_param}\n\
			${sources}\n\
			)"
		)

	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cppcheck_for_${target}.cmake
	)

endfunction()

# adds target POST_BUILD with cppcheck
function(analyze_directory_with_cppcheck target_name directory list_additional_includes)
	# default values

	set(template [=[--template="[{severity}][{id}][{file}][line {line}] {message} {callstack}"]=])
	# can it differ between targets??
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(template "gcc")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(template "gcc")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(template "vs")
	endif()

	# add check if compiling with c++11
	set(std "c++11")
	#add option for enabling/disabling
	set(enable "warning,style,performance,portability,information,missingInclude")
	# add option for config files
	set(library "--library=qt.cfg")

	add_custom_target(
		cppcheck_${target_name}
		COMMAND ${cppcheck_bin}
		--enable=${enable}
		--std=${std}
		#${library}
		--template=${template}
		--verbose
		--quiet
		${directory}
	)

endfunction()
