##############################################################################
# Project Configuration
#

PROJECT := maxisynth
PROJECT_TYPE := synth

##############################################################################
# Sources
#

# C sources
CSRC = header.c

# C++ sources
CXXSRC = unit.cc ../Maximilian/src/maximilian.cpp ../Maximilian/src/libs/PolyBLEP/PolyBLEP.cpp

# List ASM source files here
ASMSRC = 

ASMXSRC = 

##############################################################################
# Include Paths
#

UINCDIR  = ../Maximilian/src

##############################################################################
# Library Paths
#

ULIBDIR = 

##############################################################################
# Libraries
#

ULIBS  = -lm
ULIBS += -lc

##############################################################################
# Macros
#

UDEFS = 

