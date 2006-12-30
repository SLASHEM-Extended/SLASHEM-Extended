#	NetHack Makefile.
#       SCCS Id: @(#)Makefile.doc       3.4     1996/03/23

# WAC - added support for DOS DJGPP using GNU Groff package
# Standard distribution documents are built using nroff, tbl and col
# We'll be using groff to simulate nroff and col
# tbl is in the Groff package
# I don't see why this won't work on any other platform with GNU Groff
# If you have Groff and col or nroff and col, use the standard Unix Makefile.doc

# for Atari
# SHELL=E:/GEMINI2/MUPFEL.TTP

GUIDEBOOK = Guidebook		# regular ASCII file
#GUIDEBOOK = Guidebook.ps	# PostScript file
#GUIDEBOOK = Guidebook.dvi	# TeX device-independent file

# Some versions of col need -x to keep them from converting spaces to tabs;
# some versions of col don't do the conversion by default and don't
# recognize the option.  Sigh.
COLCMD = col -bx
#COLCMD = col -b

# The command to use to generate a PostScript file
# PSCMD = ditroff | psdit
PSCMD = groff -Wall -Tps

# Use the "cat" GUIDECMD if groff and/or tbl are not installed
# Not appropriate for creating Guidebook.txt.
# DOS w/o cat users can use "type"
# GUIDECMD = cat Guidebook.txt
# GUIDECMD = type Guidebook.txt
# The following work better with groff-1.18, eg on Linux
# GUIDECMD = tbl tmac.n Guidebook.mn | nroff -c -Tascii | $(COLCMD)
# GUIDECMD = tbl tmac.n Guidebook.mn | groff -Wall -mtty-char -Tascii -P-u -P-b 
GUIDECMD = tbl tmac.n Guidebook.mn | nroff | $(COLCMD)

# the basic guidebook
Guidebook:	Guidebook.mn
	$(GUIDECMD) > Guidebook

# Fancier output for those with ditroff, psdit and a PostScript printer.
Guidebook.ps:	Guidebook.mn
	tbl tmac.n Guidebook.mn | $(PSCMD) > Guidebook.ps

# Guidebook.tex is the same as Guidebook.mn but formatted with LaTeX.
# - The invocation command for LaTeX may vary in different installations.
# - To print Guidebook.dvi you need to use a suitable dvi-driver.
Guidebook.dvi:	Guidebook.tex
	latex Guidebook.tex
# Or for those with groff
# Guidebook.dvi:	Guidebook.mn
#	tbl tmac.n Guidebook.mn | groff -Wall -Tdvi > Guidebook.dvi

GAME    = slashem
MANDIR	= /usr/man/man6
MANEXT	= 6
FRMANDIR= $(MANDIR:man6=fr/man6)

# manual installation for most BSD-style systems
GAMEMANCREATE = cp $(GAME).6
LEVMANCREATE = cp lev_comp.6
DGNMANCREATE = cp dgn_comp.6
RCVRMANCREATE = cp recover.6
DLBMANCREATE = cp dlb.6
FRGAMEMANCREATE = cp fr/$(GAME).6
# manual installation for most SYSV-style systems
# and for man files readable in less (eg dos DJGPP+GNU)
# GAMEMANCREATE = groff -Wall -mtty-char -Tascii -man $(GAME).6 >
# LEVMANCREATE = groff -Wall -mtty-char -Tascii -man lev_comp.6 >
# DGNMANCREATE = groff -Wall -mtty-char -Tascii -man dgn_comp.6 >
# RCVRMANCREATE = groff -Wall -mtty-char -Tascii -man recover.6 >
# DLBMANCREATE = groff -Wall -mtty-char -Tascii -man dlb.6 >
# FRGAMEMANCREATE = groff -Wall -mtty-char -Tutf8 -man fr/$(GAME).6 >

manpages:
	-$(GAMEMANCREATE) $(MANDIR)/$(GAME).$(MANEXT)
	-$(LEVMANCREATE) $(MANDIR)/lev_comp.$(MANEXT)
	-$(DGNMANCREATE) $(MANDIR)/dgn_comp.$(MANEXT)
	-$(RCVRMANCREATE) $(MANDIR)/recover.$(MANEXT)
	-$(DLBMANCREATE) $(MANDIR)/dlb.$(MANEXT)
	-$(FRGAMEMANCREATE) $(FRMANDIR)/$(GAME).$(MANEXT)

# manual creation for distribution
DISTRIB = Guidebook.txt $(GAME).txt lev_comp.txt dgn_comp.txt recover.txt dlb.txt

distrib: $(DISTRIB)
	@echo "Plain text documentation is up to date."

Guidebook.txt	: Guidebook.mn tmac.n
	$(GUIDECMD) > Guidebook.txt
$(GAME).txt	: $(GAME).6
	nroff -man $(GAME).6 | $(COLCMD) > $(GAME).txt
lev_comp.txt	: lev_comp.6
	nroff -man lev_comp.6 | $(COLCMD) > lev_comp.txt
dgn_comp.txt	: dgn_comp.6
	nroff -man dgn_comp.6 | $(COLCMD) > dgn_comp.txt
recover.txt	: recover.6
	nroff -man recover.6 | $(COLCMD) > recover.txt
dlb.txt		: dlb.6
	nroff -man dlb.6 | $(COLCMD) > dlb.txt


clean:
	-rm -f Guidebook.aux Guidebook.log

spotless: clean
	-rm -f Guidebook Guidebook.ps Guidebook.dvi

maintainer-clean: spotless
	-rm -f $(DISTRIB)
#	-rm -f Makefile
