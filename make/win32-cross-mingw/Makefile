#
# MultiColor - An image manipulation tool for Commodore 8-bit computers'
#              graphic formats
#
# (c) 2003-2009 Thomas Giesel
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

here := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# after adding some paths to this it will be used as PATH
path          := $(PATH)

.PHONY: all
all: world

include cross-mingw.mk
include cross-wx.mk

cxx           := $(gccprefix)/bin/$(cross)-c++
strip         := $(gccprefix)/bin/$(cross)-strip
cxxflags      := -O2 `$(wx-prefix)/bin/wx-config --static=yes --cxxflags`

include ../common/filelist.mk

###############################################################################
# Main target
#
.PHONY: world
world: $(outdir)/MultiColor.exe $(outres)

###############################################################################
# This rule can link <here>/out/obj/*.o to <here>/out/MultiColor/MultiColor.exe
# 
$(outdir)/MultiColor.exe: $(obj) | $(outdir) check-environment
	$(cxx) --static  $(obj) -o $@ \
		`$(wx-prefix)/bin/wx-config --static=yes --libs`
	$(strip) $@

include ../common/rules.mk
