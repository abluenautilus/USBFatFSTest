# Project Name
TARGET = USBFatFSTest

# Sources
CPP_SOURCES = USBFatFSTest.cpp

#APP_TYPE = BOOT_QSPI
#APP_TYPE = BOOT_NONE
APP_TYPE = BOOT_SRAM

# Required to use FatFs File System
USE_FATFS = 1

# Library Locations
LIBDAISY_DIR = ${HOME}/Music/Daisy/libDaisy
DAISYSP_DIR = ${HOME}/Music/Daisy/DaisySP

# Linker flags
LDFLAGS = -u _printf_float
OPT = -Os

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

