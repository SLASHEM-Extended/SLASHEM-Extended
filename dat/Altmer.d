SPECIAL_LEVELS += Alt-strt.lev Alt-loca.lev Alt-goal.lev Alt-fila.lev Alt-filb.lev
dat/Alt-strt.lev: dat/Altmer.des util/lev_comp
	( cd dat ; ../util/lev_comp Altmer.des )
dat/Alt-loca.lev: dat/Alt-strt.lev
dat/Alt-goal.lev: dat/Alt-strt.lev
dat/Alt-fila.lev: dat/Alt-strt.lev
dat/Alt-filb.lev: dat/Alt-strt.lev
