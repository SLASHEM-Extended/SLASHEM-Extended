SPECIAL_LEVELS += Tra-strt.lev Tra-fila.lev Tra-loca.lev Tra-filb.lev Tra-goal.lev
dat/Tra-strt.lev: dat/Trans.des util/lev_comp
	( cd dat ; ../util/lev_comp Trans.des )
dat/Tra-fila.lev: dat/Tra-strt.lev
dat/Tra-loca.lev: dat/Tra-strt.lev
dat/Tra-filb.lev: dat/Tra-strt.lev
dat/Tra-goal.lev: dat/Tra-strt.lev
