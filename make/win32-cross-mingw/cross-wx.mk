#
# MultiColor - An image manipulation tool for Commodore 8-bit computers'
#              graphic formats
#
# (c) 2003-2008 Thomas Giesel
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
 
###############################################################################
# Rules to check the cross compiling environment
#
.PHONY: check-environment
check-environment: out/check-environment.ok

out/check-environment.ok:
	i586-mingw32msvc-c++ --version > /dev/null || $(MAKE) no-compiler
	$(wx-prefix)/bin/wx-config --version > /dev/null || $(MAKE) no-wx
	mkdir -p out
	touch $@

.PHONY: no-compiler
no-compiler:
	$(warning ========================================================================)
	$(warning No cross compiler found.)
	$(warning )
	$(warning You probably don't have the mingw cross compiler installed.)
	$(warning e.g. on *ubuntu you can do this with:)
	$(warning )
	$(warning sudo apt-get install mingw32)
	$(warning )
	$(warning Check http://wiki.wxwidgets.org/Install_The_Mingw_Cross-Compiler)
	$(warning for other systems.)
	$(warning ========================================================================)
	$(error stop.)

.PHONY: no-wx
no-wx:
	$(warning ========================================================================)
	$(warning No cross-wxWidgets found.)
	$(warning )
	$(warning This could mean that it is not installed or not at the place we look at:)
	$(warning $(wx-prefix)/bin/wx-config)
	$(warning )
	$(warning If you have a suitable i586-wxWidgets installed, you may want to adapt)
	$(warning the path in this makefile.)
	$(warning )
	$(warning However, it's recommended to build it with this makefile, it uses a path)
	$(warning which is unlikely to collide with other versions. Otherwise it may)
	$(warning become a pain in the ass to get the config running.)
	$(warning )
	$(warning You can install is using this makefile by invoking:)
	$(warning make install-wxwidgets)
	$(warning This needs you to be a sudoer, because some commands use sudo)
	$(warning ========================================================================)
	$(error stop.)

###############################################################################
# Rules for installing cross-wxWidgets
#
.PHONY: install-wxwidgets
install-wxwidgets: $(wx-build-path)/$(wx-version)/3-installed

$(wx-build-path)/$(wx-version):
	mkdir -p $(wx-build-path)
	cd $(wx-build-path) && wget "http://downloads.sourceforge.net/wxwindows/$(wx-version).tar.bz2"
	cd $(wx-build-path) && tar xjf $(wx-version).tar.bz2

$(wx-build-path)/$(wx-version)/1-configured: $(wx-build-path)/$(wx-version)
	cd $(wx-build-path)/$(wx-version) && \
		./configure --prefix=$(wx-prefix) --disable-shared --host=i586-mingw32msvc --build=`uname -m`-linux
	touch $@

$(wx-build-path)/$(wx-version)/2-compiled: $(wx-build-path)/$(wx-version)/1-configured
	make -C $(wx-build-path)/$(wx-version)
	touch $@

$(wx-build-path)/$(wx-version)/3-installed: $(wx-build-path)/$(wx-version)/2-compiled
	sudo make -C $(wx-build-path)/$(wx-version) install
	touch $@
