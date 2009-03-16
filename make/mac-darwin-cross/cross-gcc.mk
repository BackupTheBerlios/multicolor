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
path        := $(PATH)


build-dir   := $(here)/build
archive-dir := $(here)/archive

odcctools   := odcctools-20060413
gcc         := gcc-4.3.3

odcctools-components :=  ar as include libstuff libmacho ld man misc 

cross       := i686-apple-darwin9
gccprefix   := /opt/cross/$(cross)-$(gcc)

sudo        := sudo

path        := $(gccprefix)/bin:$(path)

.PHONY: odcctools
odcctools: $(build-dir)/1-odcctools.done

###############################################################################
# odcctools

ifeq ("`uname -m | grep 64`", "")
odcc_flags :=
else
odcc_flags := "-m32"
endif

# build odcctools
$(build-dir)/1-odcctools.done: $(build-dir)/$(odcctools)
	mkdir -p $(build-dir)/obj-$(odcctools)
	cd $(build-dir)/obj-$(odcctools) && \
		CFLAGS=$(odcc_flags) LDFLAGS=$(odcc_flags) \
			$(build-dir)/$(odcctools)/configure \
			--target=$(cross) --prefix=$(gccprefix) \
			--with-sysroot=$(gccprefix)
	make -C $(build-dir)/obj-$(odcctools) COMPONENTS="$(odcctools-components)"
	$(sudo) make -C $(build-dir)/obj-$(odcctools) install \
		COMPONENTS="$(odcctools-components)"
	touch $@

# unpack odcctools
$(build-dir)/$(odcctools): $(archive-dir)/$(odcctools).tar.bz2
	mkdir -p $(build-dir)
	tar xjf $(archive-dir)/$(odcctools).tar.bz2 -C $(build-dir)
	patch -d $(build-dir)/$(odcctools) -p 1 -i $(here)/patches/odcctools-no-long-double.patch
	touch $@

# download odcctools
$(archive-dir)/$(odcctools).tar.bz2:
	mkdir -p $(archive-dir)
	cd $(archive-dir) && wget http://biolpc22.york.ac.uk/pub/linux-mac-cross/$(odcctools).tar.bz2
