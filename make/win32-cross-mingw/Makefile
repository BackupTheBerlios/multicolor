
wx-version    := wxMSW-2.8.9
wx-build-path := wx-win32-build
wx-prefix     := /usr/local/i586-mingw32-$(wx-version)

cxx           := i586-mingw32msvc-c++
strip         := i586-mingw32msvc-strip
cxxflags      := -O2 `$(wx-prefix)/bin/wx-config --static=yes --cxxflags`

include ../common/filelist.mk

###############################################################################
# Main target
#
.PHONY: all
all: $(outdir)/MultiColor.exe $(outres) $(outdir)/mingwm10.dll

###############################################################################
# This rule can link <here>/out/obj/*.o to <here>/out/MultiColor/MultiColor.exe
# 
$(outdir)/MultiColor.exe: $(obj) | $(outdir) check-environment
	$(cxx) --static  $(obj) -o $@ \
		`$(wx-prefix)/bin/wx-config --static=yes --libs`
	$(strip) $@

###############################################################################
# This rule can copy mingw10.dll from here to the out dir
# 
$(outdir)/mingwm10.dll: mingwm10.dll | $(outdir)
	cp $^ $@

include ../common/rules.mk
include cross-wx.mk
