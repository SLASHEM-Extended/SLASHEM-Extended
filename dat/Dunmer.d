SPECIAL_LEVELS += Dun-strt.lev Dun-loca.lev Dun-goal.lev Dun-fila.lev Dun-filb.lev
dat/Dun-strt.lev: dat/Dunmer.des util/lev_comp
	( cd dat ; ../util/lev_comp Dunmer.des )
dat/Dun-loca.lev: dat/Dun-strt.lev
dat/Dun-goal.lev: dat/Dun-strt.lev
dat/Dun-fila.lev: dat/Dun-strt.lev
dat/Dun-filb.lev: dat/Dun-strt.lev
