#
# Copyright (C) 2022-2024, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

include(CMakeDependentOption)

option(AU_BUILD_TESTS "Enable the tests." OFF)
option(AU_BUILD_DOCS "Generate Docs during build" OFF)
option(AU_BUILD_EXAMPLES "Enable examples" OFF)
option(AU_ENABLE_SLOW_TESTS "Option to Enable SLOW tests" OFF)
option(AU_ENABLE_BROKEN_TESTS "Option to Enable BROKEN tests" OFF)
option(AU_ENABLE_ASSERTIONS "Enable asserts in the code" OFF)
option(AU_ENABLE_OLD_API "Enable OLD alci_* APIs" OFF)
option(AU_BUILD_WITH_ASAN "Enable ASAN Options on build" OFF)
option(AU_BUILD_WITH_TSAN "Enable TSAN Options on build" OFF)
option(AU_BUILD_WITH_MEMSAN "Enable MEMSAN Options on build" OFF)
option(AU_ENABLE_CODE_COVERAGE "Enable Code coverage on build" OFF)
option(AU_BUILD_STATIC_LIBS "Build static libraries" ON)
option(AU_BUILD_SHARED_LIBS "Build shared libraries" ON)
option(AU_CMAKE_VERBOSE "Set cmake verbosity" OFF)

# Sub options for docs
# Doxygen generate graphs and enable code browsing
option(AU_DOXYGEN_FULL "Generate Doxygen documentation with all features" OFF)

# Define the processor type
string(TOUPPER "${CMAKE_SYSTEM_PROCESSOR}" upper_CMAKE_SYSTEM_PROCESSOR)
cmake_dependent_option(AU_CPU_ARCH_X86 "" OFF "${upper_CMAKE_SYSTEM_PROCESSOR} MATCHES ^x86" ON)


# Lets make the build
string(TOUPPER "${CMAKE_BUILD_TYPE}" upper_CMAKE_BUILD_TYPE)
if (CMAKE_BUILD_TYPE AND
    NOT upper_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|DEVELOPER|RELWITHDEBINFO)$")
  message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# Handle deprecated APIs
if(${AU_ENABLE_OLD_API})
    set(AU_WARN_DEPRECATION FALSE)
else()
    set(AU_WARN_DEPRECATION TRUE)
endif()

cmake_dependent_option(AU_BUILD_TYPE_RELEASE "" ON "upper_CMAKE_BUILD_TYPE STREQUAL RELEASE" OFF)
cmake_dependent_option(AU_BUILD_TYPE_DEBUG "" ON "upper_CMAKE_BUILD_TYPE STREQUAL DEBUG" OFF)
cmake_dependent_option(AU_BUILD_TYPE_DEVELOPER "" ON "upper_CMAKE_BUILD_TYPE STREQUAL DEVELOPER" OFF)
cmake_dependent_option(AU_BUILD_TYPE_RELWITHDEBINFO "" ON "upper_CMAKE_BUILD_TYPE STREQUAL RELWITHDEBINFO" OFF)

mark_as_advanced(AU_BUILD_TYPE_RELEASE
	AU_BUILD_TYPE_DEBUG
	AU_BUILD_TYPE_DEVELOPER
    AU_BUILD_TYPE_RELWITHDEBINFO
    AU_ENABLE_ASSERTIONS
    AU_WARN_DEPRECATION)

if (AU_CMAKE_VERBOSE AND FALSE)
message(
	"build type \n"
	"release:" ${AU_BUILD_TYPE_RELEASE} "\n"
	"debug:" ${AU_BUILD_TYPE_DEBUG} "\n"
	"developer:" ${AU_BUILD_TYPE_DEVELOPER} "\n"
    "relwithdebinfo:" ${AU_BUILD_TYPE_RELWITHDEBINFO} "\n"
	"assertions: " ${AU_ENABLE_ASSERTIONS})
endif()


if (AU_BUILD_TYPE_RELEASE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/Release)
elseif(AU_BUILD_TYPE_DEVELOPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Developer)
elseif(AU_BUILD_TYPE_DEBUG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Debug)
elseif(AU_BUILD_TYPE_RELWITHDEBINFO)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/RelWithDebInfo)
endif()


option(AU_BUILD_SHARED_LIBS "Build shared libraries" OFF)
