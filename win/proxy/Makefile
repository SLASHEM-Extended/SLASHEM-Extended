# $Id: Makefile,v 1.1 2001-09-06 18:06:38 j_ali Exp $
# Copyright (c) Slash'EM Development Team 2001
# NetHack may be freely redistributed.  See license for details.
#
# This makefile is for maintainer use. It is not needed when building the game.

AWK = awk

CFLAGS = -I../../include
#RPCLIB = -lrpclib

# ----------------------------------------
#
# Nothing below this line should have to be changed.

# timestamps for primary header files, matching src/Makefile
CONFIG_H = ../../src/config.h-t
HACK_H  = ../../src/hack.h-t

CSRC = nhxdr.c nhextxdr.c

all:	tests

FORCE:

tests:	test_xdr rpcgen-test_xdr FORCE
	@echo ""
	@echo The output of the two tests should be identical:
	@echo ============================
	- test_xdr -w | rpcgen-test_xdr -r
	@echo ============================
	- rpcgen-test_xdr -w | test_xdr -r
	@echo ============================

test_xdr:	test_xdr.c nhxdr.o nhextxdr.o
	$(CC) $(CFLAGS) -URPCGEN -o test_xdr test_xdr.c nhxdr.o nhextxdr.o

rpcgen-test_xdr:	test_xdr.c
	$(CC) $(CFLAGS) -DRPCGEN -o rpcgen-test_xdr test_xdr.c $(RPCLIB)

rpcgen-nhext.h rpcgen-nhext_xdr.c:	rpcgen-nhext.x
	rpcgen rpcgen-nhext.x

rpcgen-nhext.x:	ext_protocol.html
	$(AWK) '/<h3>init<\/h3>/ { init=1; } /<pre>/ { if (init) output=1; next;} /<\/pre>/ { output=0; } { if (output) print }' < ext_protocol.html > rpcgen-nhext.x

clean:
	$(RM) *.o test_xdr rpcgen-test_xdr

spotless:	clean
	$(RM) rpcgen-nhext.h rpcgen-nhext_xdr.c rpcgen-nhext.x

depend: ../../sys/unix/depend.awk $(CSRC)
	$(AWK) -f ../../sys/unix/depend.awk ../../include/*.h $(CSRC) | \
	sed -e 's:\.\./include:\.\./\.\./include:' \
	    -e 's:\.\./\.\./include/nhxdr.h:nhxdr.h:' >makedep
	@echo '/^# DO NOT DELETE THIS LINE OR CHANGE ANYTHING BEYOND IT/+2,$$d' >eddep
	@echo '$$r makedep' >>eddep
	@echo 'w' >>eddep
	@cp Makefile Makefile.bak
	ed - Makefile < eddep
	@rm -f eddep makedep
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@echo '# see make depend above' >> Makefile
	- diff Makefile.bak Makefile
	@rm -f Makefile.bak

# DO NOT DELETE THIS LINE OR CHANGE ANYTHING BEYOND IT

# config.h timestamp
$(CONFIG_H): ../../include/config.h
	touch $(CONFIG_H)
# hack.h timestamp
$(HACK_H): ../../include/hack.h
	touch $(HACK_H)
#
nhxdr.o: nhxdr.c $(HACK_H) nhxdr.h
nhextxdr.o: nhextxdr.c $(HACK_H) nhxdr.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
