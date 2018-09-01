# LaTeX-Makefile
#
# Author: George Ungureanu
# Adapted from Ingo Sander original Makefile.
#
# Date: 20170205
#
# Description:
# This Makefile assumes the following project structure,
#
# base_dir
# | main.tex 	-- Main tex file
# | Makefile	-- This Makefile
# | src/	-- All included tex files (e.g. chapters)
# | include/	-- Style files
# | figs_src/	-- Source for figures (e.g. tikz, svg)
# | figs/	-- Compiled figures
#
# The name of the main file should be explicitly set in the DOCNAME
# variable. During execution of the Makefile, LaTeX-compatible figures
# are generated in the directory ./figs. For compilation of the main
# file, latexmk is invoked to deal with the multi step nature of LaTeX
# compilation.
#
# The following external tools are required:
# - TeXLive 2016.
# - imagemagick, for figures convertion.
#
# This Makefile assumes a *nix/MacOSX structure for application
# locations. Customize it for your own needs.
#
# The following rules are supported:
# make
# make clean
# make view

# Directory structure
FIGS_DIR 	= ..
S_FIGS_DIR	= .

# Tools
RM = rm -f
CP = cp
MV = mv
MKDIR = mkdir -p

# Figures source files
S_TIKZ_FIGS	:= $(shell find $(S_FIGS_DIR) -name '*.tex')

# Figures targets
TIKZ_FIGS	:= $(patsubst $(S_FIGS_DIR)/%.tex, $(FIGS_DIR)/%.png, $(S_TIKZ_FIGS))

# Targets
all: $(TIKZ_FIGS)

clean: 
	@$(RM) *.run.xml *.synctex.gz
	@$(RM) *.d  *.thm  *.pdf *.aux *.log


### Rules for figures
### These are Pattern Rules and will match in the
### order they appear in the Makefile

# Rules for Tikz and LaTeX figures
../%.png: $(S_FIGS_DIR)/%.tex
	@echo $(@F)
	pdflatex $(*F)
	convert -density 150 $(*F).pdf -quality 90 $@

.PHONY: all clean 

# Include auto-generated dependencies
-include *.d
