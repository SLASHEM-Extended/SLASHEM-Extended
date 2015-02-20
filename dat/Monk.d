SPECIAL_LEVELS += Mon-strt.lev Mon-loca.lev Mon-goal.lev Mon-fila.lev Mon-filb.lev
dat/Mon-strt.lev: dat/Monk.des util/lev_comp
	( cd dat ; ../util/lev_comp Monk.des )
dat/Mon-loca.lev: dat/Mon-strt.lev
dat/Mon-goal.lev: dat/Mon-strt.lev
dat/Mon-fila.lev: dat/Mon-strt.lev
dat/Mon-filb.lev: dat/Mon-strt.lev
