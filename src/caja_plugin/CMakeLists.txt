cmake_minimum_required(VERSION 3.2)

project(caja-devbox CXX)

include_directories(../../lib/posix)

set(SOURCE_FILES
	implementation.cpp
	menu.hpp
	menu.cpp
	propertypage.hpp
	propertypage.cpp
)

#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_C_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
add_library(${PROJECT_NAME} SHARED
	${SOURCE_FILES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
)
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})

install(
	TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION "/usr/lib/x86_64-linux-gnu/caja/extensions-2.0/"
)

set(CPACK_GENERATOR "DEB")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_PACKAGE_CONTACT "federico.kircheis@gmail.com")
set(CPACK_PACKAGE_NAME "caja-devbox")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "caja plugin for developers")
set(CPACK_PACKAGE_DESCRIPTION "This package contains a caja plugin for developers")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Federico Kircheis <federico.kircheis@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/fekir/devbox")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "openssl (>= 1.0.2h), libc6")
#set(CPACK_DEBIAN_CHANGELOG_FILE "...") # create from github log??
include(CPack)

## Test ##

set(TEST_FILES
	test/test_interface.cpp
)

source_group("Test Files" FILES ${TEST_FILES})

set(PROJECT_NAME_TEST "${PROJECT_NAME}Test")
add_executable(${PROJECT_NAME_TEST} test/main.cpp ${SOURCE_FILES} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME_TEST} ${OPENSSL_LIBRARIES})


set_property(TARGET ${PROJECT_NAME_TEST} PROPERTY CXX_STANDARD 11)

set_target_properties(${PROJECT_NAME_TEST}
	PROPERTIES
	LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
)
