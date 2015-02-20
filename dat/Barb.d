SPECIAL_LEVELS += Bar-strt.lev Bar-loca.lev Bar-goal.lev Bar-fila.lev Bar-filb.lev
dat/Bar-strt.lev: dat/Barb.des util/lev_comp
	( cd dat ; ../util/lev_comp Barb.des )
dat/Bar-loca.lev: dat/Bar-strt.lev
dat/Bar-goal.lev: dat/Bar-strt.lev
dat/Bar-fila.lev: dat/Bar-strt.lev
dat/Bar-filb.lev: dat/Bar-strt.lev
