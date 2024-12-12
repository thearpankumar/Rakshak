# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Set the DTC overlay file BEFORE find_package
#set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/boards/arduino_uno_r4_minima.overlay)

# Find Zephyr
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(NONE)

# Add your application source files
target_sources(app PRIVATE src/main.c)
