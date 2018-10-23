.PHONY: preview dist clean

#
# Configuration
#

# Paths
BUILD = build

# Commands
TEX = xelatex -shell-escape -halt-on-error -output-directory=$(BUILD)
BIB = bibtex

#
# Phony targets
#

preview: $(BUILD)/doc.pdf
	open $(BUILD)/doc.pdf

clean:
	rm -rf $(BUILD)/*
	ln -s ../doc.pyg $(BUILD)
	ln -s ../doc.out.pyg $(BUILD)
#
# File targers
#

$(BUILD)/doc.pdf: doc.tex
	$(TEX) doc.tex
	$(TEX) doc.tex
	$(TEX) doc.tex
