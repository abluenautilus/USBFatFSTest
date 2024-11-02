# Project Name
TARGET = USBFatFSTest

# Sources
CPP_SOURCES = USBFatFSTest.cpp
#DEBUG=1
#APP_TYPE = BOOT_SRAM
APP_TYPE = BOOT_QSPI
#APP_TYPE = BOOT_NONE

USE_FATFS = 1

# Library Locations
LIBDAISY_DIR = /Users/jonask/Music/Daisy/libDaisy
DAISYSP_DIR = /Users/jonask/Music/Daisy/DaisySP

# Linker flags
# This is not really required, used only for profiling! Increases executable size by ~8kB
LDFLAGS = -u _printf_float
# OPT = -Os
# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

