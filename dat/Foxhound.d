SPECIAL_LEVELS += Fox-strt.lev Fox-loca.lev Fox-goal.lev Fox-fila.lev Fox-filb.lev
dat/Fox-strt.lev: dat/Foxhound.des util/lev_comp
	( cd dat ; ../util/lev_comp Foxhound.des )
dat/Fox-loca.lev: dat/Fox-strt.lev
dat/Fox-goal.lev: dat/Fox-strt.lev
dat/Fox-fila.lev: dat/Fox-strt.lev
dat/Fox-filb.lev: dat/Fox-strt.lev
