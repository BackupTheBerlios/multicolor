
cxx           := c++

ifeq ($(DEBUG), YES)
cxxflags      := -g `wx-config --cxxflags`
ldflags       := -g
else
cxxflags      := -O3 `wx-config --cxxflags`
ldflags       :=
endif

include ../common/filelist.mk

###############################################################################
# Main target
#
.PHONY: all
all: $(outdir)/multicolor $(outres)

###############################################################################
# This rule can link <here>/out/obj/*.o to <here>/out/MultiColor/MultiColor.exe
# 
$(outdir)/multicolor: $(obj) | $(outdir) check-environment
	$(cxx) $(ldflags) $(obj) -o $@ \
		`wx-config --libs`

include ../common/rules.mk

.PHONY: check-environment
check-environment:
