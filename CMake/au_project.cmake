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

include(GNUInstallDirs)

# All project dependant variables are defined here
# Figure out where to put the Config.h
# Input for this is config.h.in in ${PROJECT_SOURCE_DIR}/Library
if (NOT DEFINED AU_CONFIG_DIR)
  set(AU_CONFIG_DIR "${PROJECT_BINARY_DIR}/generated")
endif()

if (NOT DEFINED AU_CONFIG_OUTPUT_FILE)
  set(AU_CONFIG_OUTPUT_FILE ${AU_CONFIG_DIR}/Au/Config.h)
endif()

if (NOT DEFINED AU_VERSION_OUTPUT_FILE)
  set(AU_VERSION_OUTPUT_FILE ${AU_CONFIG_DIR}/version.cc)
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

#include_directories("${AU_CONFIG_DIR}")
set(AU_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/SDK/Include
    ${PROJECT_SOURCE_DIR}/Library/Private
  ${AU_CONFIG_DIR}
)

set(AU_INSTALL_INCLUDE_DIR          "${CMAKE_INSTALL_INCLUDEDIR}")
set(AU_INSTALL_BIN_DIR              "${CMAKE_INSTALL_BINDIR}")
set(AU_INSTALL_TEST_DIR             "${CMAKE_INSTALL_DIR}/tests")
set(AU_INSTALL_LIB_DIR              "${CMAKE_INSTALL_LIBDIR}")
set(AU_INSTALL_ARCHIVE_DIR          "${CMAKE_INSTALL_LIBDIR}")
set(AU_INSTALL_FRAMEWORK_DIR        "${CMAKE_INSTALL_LIBDIR}")
set(AU_INSTALL_CMAKE_CONFIG_DIR     "${CMAKE_INSTALL_LIBDIR}/CMake")
set(AU_INSTALL_ADDITIONAL_FILES_DIR "${CMAKE_INSTALL_DATADIR}/aocl-utils")
