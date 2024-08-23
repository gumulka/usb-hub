# Copyright (c) 2024 Fabian Pflug
# SPDX-License-Identifier: Apache-2.0

list(APPEND SYSCALL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SYSCALL_INCLUDE_DIRS ${SYSCALL_INCLUDE_DIRS} PARENT_SCOPE)
zephyr_include_directories(include)

add_subdirectory(drivers)
