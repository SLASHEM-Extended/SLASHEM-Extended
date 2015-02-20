SPECIAL_LEVELS += Pri-strt.lev Pri-loca.lev Pri-goal.lev Pri-fila.lev Pri-filb.lev
dat/Pri-strt.lev: dat/Priest.des util/lev_comp
	( cd dat ; ../util/lev_comp Priest.des )
dat/Pri-loca.lev: dat/Pri-strt.lev
dat/Pri-goal.lev: dat/Pri-strt.lev
dat/Pri-fila.lev: dat/Pri-strt.lev
dat/Pri-filb.lev: dat/Pri-strt.lev
