
# Where to install on "make install"?
inst_prefix   := /usr
bin_inst_dir  := $(inst_prefix)/bin
res_inst_dir  := $(inst_prefix)/share/multicolor
desktop_inst_dir := $(inst_prefix)/share/applications

cxx           := c++

ifeq ($(DEBUG), YES)
cxxflags      := -g `wx-config --cxxflags`
ldflags       := -g
outbase       := debug
else
cxxflags      := -O2 `wx-config --cxxflags`
#cxxflags      += -DN2C
ldflags       :=
outbase       := release
endif

include ../common/filelist.mk

###############################################################################
# Main targets
#
.PHONY: all
all: $(outdir)/multicolor $(outres)

.PHONY: install
install: all
	mkdir -p $(bin_inst_dir)
	cp $(outdir)/multicolor $(bin_inst_dir)
	mkdir -p $(res_inst_dir)
	cp -r  $(outdir)/res $(res_inst_dir)
	mkdir -p $(desktop_inst_dir)
	cp $(srcdir)/../res/multicolor.desktop $(desktop_inst_dir)

###############################################################################
# This rule can link <here>/out/obj/*.o to <here>/out/MultiColor/MultiColor.exe
#
$(outdir)/multicolor: $(obj) | $(outdir) check-environment
	$(cxx) $(ldflags) $(obj) -o $@ \
		`wx-config --libs`

include ../common/rules.mk

.PHONY: check-environment
check-environment:
