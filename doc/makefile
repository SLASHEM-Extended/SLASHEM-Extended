
#	NetHack Makefile.
#       SCCS Id: @(#)Makefile.doc       3.2     97/DEC/17

# WAC - added support for DOS DJGPP using GNU Groff package
# Standard distribution documents are built using nroff, tbl and col
# We'll be using groff to simulate nroff and col
# tbl is in the Groff package
# I don't see why this won't work on any other platform with GNU Groff
# If you have Groff and col or nroff and col,  use the standard Unix Makefile.doc

GUIDEBOOK = Guidebook		# regular ASCII file
#GUIDEBOOK = Guidebook.ps	# PostScript file
#GUIDEBOOK = Guidebook.dvi	# TeX device-independent file

# Use the "cat" GUIDECMD if groff and/or tbl are not installed
# DOS w/o cat users can use "type"
# GUIDECMD = cat Guidebook.txt
# GUIDECMD = type Guidebook.txt
GUIDECMD = tbl tmac.n Guidebook.mn | groff -Wall -mtty-char -Tascii -P-u -P-b 

# the basic guidebook
Guidebook:	Guidebook.mn
	$(GUIDECMD) > Guidebook

# Fancier output for those with ditroff, psdit and a PostScript printer.
Guidebook.ps:	Guidebook.mn
	tbl tmac.n Guidebook.mn | ditroff | psdit > Guidebook.ps

# Guidebook.tex is the same as Guidebook.mn but formatted with LaTeX.
# - The invocation command for LaTeX may vary in different installations.
# - To print Guidebook.dvi you need to use a suitable dvi-driver.
Guidebook.dvi:	Guidebook.tex
	latex Guidebook.tex

GAME    = slashem
#GAME    = nethack
MANDIR	= /games/slash/doc
MANEXT	= 1n

# manual installation for most SYSV-style systems
# and for man files readable in less (eg dos DJGPP+GNU)
 GAMEMANCREATE = groff -Wall -mtty-char -Tascii -man $(GAME).6 >
 LEVMANCREATE = groff -Wall -mtty-char -Tascii -man lev_comp.6 >
 DGNMANCREATE = groff -Wall -mtty-char -Tascii -man dgn_comp.6 >
 RCVRMANCREATE = groff -Wall -mtty-char -Tascii -man recover.6 >
 DLBMANCREATE = groff -Wall -mtty-char -Tascii -man dlb.6 >

manpages:
	-$(GAMEMANCREATE) $(MANDIR)/$(GAME).$(MANEXT)
	-$(LEVMANCREATE) $(MANDIR)/lev_comp.$(MANEXT)
	-$(DGNMANCREATE) $(MANDIR)/dgn_comp.$(MANEXT)
	-$(RCVRMANCREATE) $(MANDIR)/recover.$(MANEXT)
	-$(DLBMANCREATE) $(MANDIR)/dlb.$(MANEXT)

# manual creation for distribution
DISTRIB = Guidebook.txt $(GAME).txt lev_comp.txt dgn_comp.txt recover.txt dlb.txt

distrib: $(DISTRIB)
	@echo "Plain text documentation is up to date."

Guidebook.txt	: Guidebook.mn tmac.n
	tbl tmac.n Guidebook.mn | groff -Wall -mtty-char -Tascii -P-u -P-b \
	> Guidebook.txt
$(GAME).txt	: $(GAME).6
	groff -Wall -mtty-char -Tascii -P-u -P-b -man $(GAME).6 > $(GAME).txt
lev_comp.txt	: lev_comp.6
	groff -Wall -mtty-char -Tascii -P-u -P-b -man lev_comp.6 > lev_comp.txt
dgn_comp.txt	: dgn_comp.6
	groff -Wall -mtty-char -Tascii -P-u -P-b -man dgn_comp.6 > dgn_comp.txt
recover.txt	: recover.6
	groff -Wall -mtty-char -Tascii -P-u -P-b -man recover.6 > recover.txt
dlb.txt		: dlb.6
	groff -Wall -mtty-char -Tascii -P-u -P-b -man dlb.6 > dlb.txt


clean:
	-rm -f Guidebook.aux Guidebook.log

spotless: clean
	-rm -f Guidebook Guidebook.ps Guidebook.dvi

maintainer-clean: spotless
	-rm -f $(DISTRIB)
#	-rm -f Makefile
