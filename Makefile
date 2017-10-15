#makefile for the tiny compiler

#-Wno-deprecated because flex generates obsolete C++ headers (using gcc3)
CXXFLAGS= -Wall -g -Wno-deprecated 
CXX=g++
JAVAC=javac
#

#compiler C-files
CCFILES= type.C stringpool.C symboltable.C tinylexer.C tinyparser.C tinymain.C process.C parseargs.C print.C
#abstract syntax tree C-files
ASTCFILES= node.C assign.C unop.C binop.C block.C fundecl.C funref.C ifstmt.C number.C qchar.C readstmt.C returnstmt.C vardecl.C varref.C whilestmt.C writestmt.C funcall.C lengthexp.C arrayref.C arraydecl.C arrayaccess.C break.C continue.C
#visitor C-files
VCFILES= typechecker.C constantfolder.C typecheckexc.C ictranslator.C bexptranslator.C labelset.C icoptranslator.C jvmtranslator.C livenesschecker.C tiny2java.C globaljvmtranslator.C variableset.C livenessexc.C
#operator C-files
OCFILES= plus.C times.C minus.C divides.C equal.C nequal.C less.C greater.C uminus.C negate.C operators.C evaluate.C
#instruction C-files
ICFILES = instruction.C label.C parami.C calli.C returni.C instructions.C binassigni.C unassigni.C arraccessi.C arrassigni.C opinstruction.C arrparami.C bbmapping.C dimlengthi.C multidimi.C multiarrayi.C
#optimization C-files
OPTCFILES = optimizer.C dagnode.C bb2dag.C matcher.C regcmp.C usecount.C

#all C-files
CFILES= $(CCFILES) $(ASTCFILES:%.C=syntaxtree/%.C) $(VCFILES:%.C=visitors/%.C) $(OCFILES:%.C=operators/%.C) $(ICFILES:%.C=instructions/%.C) $(OPTCFILES:%.C=optimization/%.C)

HFILES=		$(CFILES:%.C=%.h)	
OFILES=		$(CFILES:%.C=%.o)
SOURCE=		Makefile Doxyfile $(CFILES) $(HFILES) tiny.y lex.l tinyc.sh TinyRuntime.java quicksort.tiny matrixmultiply.tiny factorial.tiny
#
#
all:		tags runtime tiny tinyc
tiny:		$(OFILES)
		$(CXX) $(CXXFLAGS) -o $@ $^

#using ctags to create tag entries
tags:	
		ctags -R *
#
include	make.depend
#
#	Bison options:
#	-d	Generate tiny.tab.h containing token type definitions
#
tinyparser.C:	tiny.y
		bisonpp -d --class TinyParser tiny.y && mv y.tab.c tinyparser.C

tinylexer.C:    lex.l
		flex++ -f -t $^ >$@

runtime:	TinyRuntime.java
		$(JAVAC) $^

#this rule forces make to use g++ rather than cc for .c files
%.o:		%.c
		$(CXX) $(CXXFLAGS) -c $<

#standard rules
clean:
		@rm -f tinyparser.C tinylexer.C TinyParser.tab.h $(OFILES) tiny *.jasm *.class make.depend tinyc tiny.tab.o lex.yy.o tags
#
tar:
		tar cvf tiny.tar $(SOURCE)

doc:		
		doxygen Doxyfile
#
##
# creating dependencies
# suppress output
##
make.depend:	$(CFILES) 
		@$(CXX) $(CXXFLAGS) -M $(CFILES) >make.depend
