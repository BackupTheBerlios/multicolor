#
# Makefile
#
# (c) 2003-2010 Thomas Giesel
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
#
# Thomas Giesel skoe@directbox.com
#

.PHONY: default
default: all

ifeq ($(debug), yes)
    outbase       := out$(target)_debug
    ccflags       := -g
    ldflags       := -g
else
    outbase       := out$(target)
    ccflags       := -O2
    #cxxflags      += -DN2C
    ldflags       :=
endif

ifneq "$(release)" "yes"
	version        := $(shell date +%y%m%d-%H%M)
	version_suffix :=
else
	version        := 1.0.1
	version_suffix := -$(version)
endif

ifeq ("$(target)", "win32")
include make/win32-cross-mingw/cross-mingw.mk
include make/win32-cross-mingw/cross-wx.mk
cxx           := $(gccprefix)/bin/$(cross)-c++
cc            := $(gccprefix)/bin/$(cross)-cc
strip         := $(gccprefix)/bin/$(cross)-strip
cxxflags      := -O2 `$(wx-prefix)/bin/wx-config --static=yes --cxxflags`
app_name      := MultiColor
app_suffix    := .exe
archive_suffix := zip
out_archive   := $(outbase)/MultiColor$(version_suffix).$(archive_suffix)
else
cxx           := c++
cc            := gcc
cxxflags      := -g $(shell wx-config --cxxflags)
app_name      := multicolor
app_suffix    := 
archive_suffix := tar.bz2
out_archive   := $(outbase)/multicolor$(version_suffix).$(archive_suffix)
endif

# Where to install on "make install"?
inst_prefix   := /usr/local

# to remove old installations
app_name_old  := MultiColor

###############################################################################
# This is the list of source files to be compiled
#
src :=
src += BitmapBase.cpp
src += C64Color.cpp
src += FormatInfo.cpp
src += MCApp.cpp
src += MCBlock.cpp
src += MCBitmap.cpp
src += HiResBlock.cpp
src += HiResBitmap.cpp
src += HiResDoc.cpp
src += MCCanvas.cpp
src += PreviewWindow.cpp
src += DocBase.cpp
src += DocRenderer.cpp
src += MCDoc.cpp
src += MCMainFrame.cpp
src += NewFileDialog.cpp
src += ToolBase.cpp
src += ToolCloneBrush.cpp
src += ToolColorPicker.cpp
src += ToolDots.cpp
src += ToolFill.cpp
src += ToolFreehand.cpp
src += ToolLines.cpp
src += ToolPanel.cpp
src += ToolRect.cpp
src += PalettePanel.cpp
src += MCDrawingModePanel.cpp
src += MCBlockPanel.cpp

###############################################################################
# This is a list of resource file to be built/copied
#
res :=
res += 16x16/clone.png
res += 16x16/color_fill.png
res += 16x16/colorpicker.png
res += 16x16/dots.png
res += 16x16/fileclose.png
res += 16x16/filenew.png
res += 16x16/fileopen.png
res += 16x16/filesave.png
res += 16x16/filesaveas.png
res += 16x16/freeline_unfilled.png
res += 16x16/line.png
res += 16x16/mouse.png
res += 16x16/quit.png
res += 16x16/redo.png
res += 16x16/rect.png
res += 16x16/tv.png
res += 16x16/undo.png
res += 16x16/zoomin.png
res += 16x16/zoomout.png
res += 24x24/clone.png
res += 24x24/color_fill.png
res += 24x24/colorpicker.png
res += 24x24/dots.png
res += 24x24/fileclose.png
res += 24x24/filenew.png
res += 24x24/fileopen.png
res += 24x24/filesave.png
res += 24x24/filesaveas.png
res += 24x24/freeline_unfilled.png
res += 24x24/line.png
res += 24x24/mouse.png
res += 24x24/quit.png
res += 24x24/redo.png
res += 24x24/rect.png
res += 24x24/save_all.png
res += 24x24/tv.png
res += 24x24/undo.png
res += 24x24/zoomin.png
res += 24x24/zoomout.png
res += cursors/colorpicker.png
res += cursors/clonebrush.png
res += cursors/dots.png
res += cursors/floodfill.png
res += cursors/freehand.png
res += cursors/lines.png
res += cursors/rect.png
res += README.icons.txt
res += lgpl-2.1.txt

###############################################################################
# This is a list of documents to be copied
#
doc := CHANGES COPYING README

include make/common/transform.mk
include make/common/rules.mk
include make/common/install.mk

.PHONY: check-environment
check-environment:
