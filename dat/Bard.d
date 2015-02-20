SPECIAL_LEVELS += Brd-strt.lev Brd-loca.lev Brd-goal.lev Brd-fila.lev Brd-filb.lev
dat/Brd-strt.lev: dat/Bard.des util/lev_comp
	( cd dat ; ../util/lev_comp Bard.des )
dat/Brd-loca.lev: dat/Brd-strt.lev
dat/Brd-goal.lev: dat/Brd-strt.lev
dat/Brd-fila.lev: dat/Brd-strt.lev
dat/Brd-filb.lev: dat/Brd-strt.lev
