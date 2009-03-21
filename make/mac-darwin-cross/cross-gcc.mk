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
archive-dir := $(here)/../archive

odcctools   := odcctools-9.2-r277
sdk         := MacOSX10.4u.sdk
gcc         := gcc-5465

odcctools-components :=  ar as include libstuff libmacho ld man misc 

cross       := i686-apple-darwin9
gccprefix   := $(HOME)/cross/$(cross)-$(gcc)

path        := $(gccprefix)/bin:$(path)

.PHONY: all
all: $(build-dir)/3-gcc-c.done

###############################################################################
# odcctools

# build odcctools
$(build-dir)/1-odcctools.done: $(build-dir)/$(odcctools)
	mkdir -p $(build-dir)/obj-$(odcctools)
	cd $(build-dir)/obj-$(odcctools) && \
		CFLAGS=-m32 LDFLAGS=-m32 \
			$(build-dir)/$(odcctools)/configure \
			--target=$(cross) --prefix=$(gccprefix) \
			--with-sysroot=$(gccprefix)
	make -C $(build-dir)/obj-$(odcctools) COMPONENTS="$(odcctools-components)"
	make -C $(build-dir)/obj-$(odcctools) install \
		COMPONENTS="$(odcctools-components)"
	touch $@

# unpack odcctools
$(build-dir)/$(odcctools): $(archive-dir)/$(odcctools).tar.bz2
	mkdir -p $(build-dir)
	tar xjf $(archive-dir)/$(odcctools).tar.bz2 -C $(build-dir)
	patch -d $(build-dir)/$(odcctools) -p 1 -i $(here)/patches/odcctools-no-long-double.patch
	patch -d $(build-dir)/$(odcctools) -p 1 -i $(here)/patches/odcctools-qsort_r.patch
	patch -d $(build-dir)/$(odcctools) -p 0 -i $(here)/patches/odcctools-as.patch
	touch $@

# download odcctools
$(archive-dir)/$(odcctools).tar.bz2:
	mkdir $(build-dir)/tmp
	cd $(build-dir)/tmp && svn co http://iphone-dev.googlecode.com/svn/branches/odcctools-9.2-ld/ $(odcctools) -r 277
	rm -rf $(build-dir)/tmp/$(odcctools)/.svn
	tar cjf $@ -C $(build-dir)/tmp $(odcctools)

###############################################################################
# SDK

# unpack odcctools
$(build-dir)/2-sdk.done: $(archive-dir)/$(sdk).tar.bz2
	mkdir -p $(build-dir)
	tar xjf $(archive-dir)/$(sdk).tar.bz2 -C $(build-dir)
	mkdir -p $(gccprefix)
	cp -rf $(build-dir)/$(sdk)/* $(gccprefix)

###############################################################################
# GCC Step 1

# build gcc - C only
$(build-dir)/3-gcc-c.done: $(build-dir)/$(gcc) \
		$(build-dir)/1-odcctools.done \
		$(build-dir)/2-sdk.done
	mkdir -p $(build-dir)/obj-$(gcc)
	cd $(build-dir)/obj-$(gcc) && PATH=$(path) CFLAGS=-m32 LDFLAGS=-m32 \
		$(build-dir)/$(gcc)/configure \
		--target=$(cross) --prefix=$(gccprefix) \
		--with-sysroot=$(gccprefix) \
		--disable-checking --enable-static --enable-shared --disable-nls \
		--disable-multilib --enable-languages=c,c++ \
		--with-as=$(gccprefix)/bin/$(cross)-as \
		--with-ld=$(gccprefix)/bin/$(cross)-ld 
	PATH=$(path) make -C $(build-dir)/obj-$(gcc)
	PATH=$(path) make -C $(build-dir)/obj-$(gcc) install
	touch $@

# unpack gcc
$(build-dir)/$(gcc): $(archive-dir)/$(gcc).tar.gz
	mkdir -p $(build-dir)
	tar xzf $(archive-dir)/$(gcc).tar.gz -C $(build-dir)
	patch -d $(build-dir)/$(gcc) -p 1 -i $(here)/patches/gcc-4.0.1-collect2-open.patch
	touch $@

# download gcc
$(archive-dir)/$(gcc).tar.gz:
	mkdir -p $(archive-dir)
	cd $(archive-dir) && wget http://www.opensource.apple.com/darwinsource/tarballs/other/$(gcc).tar.gz
