
outbase       := out
outdir        := $(outbase)/MultiColor
objdir        := $(outbase)/obj
srcdir        := ../../src

###############################################################################
# This is the list of source files to be compiled
#
src := 
src += C64Color.cpp
src += MCApp.cpp
src += MCBitmap.cpp
src += MCBlock.cpp
src += MCCanvas.cpp
src += MCDoc.cpp
src += MCMainFrame.cpp
src += MCToolBase.cpp
src += MCToolCloneBrush.cpp
src += MCToolDots.cpp
src += MCToolFill.cpp
src += MCToolFreehand.cpp
src += MCToolLines.cpp
src += MCToolPanel.cpp
src += PalettePanel.cpp
src += MCDrawingModePanel.cpp
src += MCBlockPanel.cpp

###############################################################################
# This is a list of resource file to be built/copied
#
res :=
res += 16x16/clone.png
res += 16x16/color_fill.png
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
res += 16x16/tv.png
res += 16x16/undo.png
res += 16x16/zoomin.png
res += 16x16/zoomout.png
res += 24x24/clone.png
res += 24x24/color_fill.png
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
res += 24x24/save_all.png
res += 24x24/tv.png
res += 24x24/undo.png
res += 24x24/zoomin.png
res += 24x24/zoomout.png
res += README.icons.txt
res += lgpl-2.1.txt

###############################################################################
# Transform all names foo.cpp in $src to out/obj/foo.o
# 
obj := $(addprefix $(objdir)/, $(src:.cpp=.o))

###############################################################################
# Transform all names in $res to out/MultiColor/res/*
# 
outres := $(addprefix $(outdir)/res/, $(res))

###############################################################################
# Poor men's dependencies: Let all files depend from all header files
# 
headers := $(wildcard $(srcdir)/*.h)
