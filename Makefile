
# Where to install on "make install"?
inst_prefix   := /usr/local
app_name      := multicolor

cxx           := c++
cc            := gcc

ifeq ($(debug), yes)
    outbase       := out_debug
    ccflags       := -g
    cxxflags      := -g `wx-config --cxxflags`
    ldflags       := -g
else
    outbase       := out
    ccflags       := -O2
    cxxflags      := -O2 `wx-config --cxxflags`
    #cxxflags      += -DN2C
    ldflags       :=
endif

srcdir        := src

include make/common/filelist.mk
include make/common/transform.mk
include make/common/rules.mk
include make/common/install.mk

.PHONY: check-environment
check-environment:
