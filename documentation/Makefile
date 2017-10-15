FILES=tiny.tex

# targets
# #######

all:	$(FILES:%.tex=%.ps) $(FILES:%.tex=%.pdf) # $(FILES:%.tex=%.html)

clean_junk:	
	rm -f *.aux *.log *.toc *.blg *.bbl internals.pl labels.pl
	images.pl

clean:	
	rm -f *.aux *.log *.pdf *.ps *.dvi *.toc *.blg *.bbl *.html internals.pl labels.pl *.css *.png WARNINGS images.pl images.tex

again: clean all
	
# extension dependencies
# ######################

%.dvi:	%.tex
	latex $* && latex $* && latex $*
#latex $* && bibtex $* && latex $* && latex $*


%.ps:	%.dvi
	dvips -o $@ $<

%.pdf:	%.dvi
	dvipdf $<
	
%.html:	
	latex2html -split 0 -no_subdir -show_section_numbers -numbered_footnotes $(FILES)
