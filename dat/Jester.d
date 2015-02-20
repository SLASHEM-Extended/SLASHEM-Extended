SPECIAL_LEVELS += Jes-strt.lev Jes-loca.lev Jes-goal.lev Jes-fila.lev Jes-filb.lev
dat/Jes-strt.lev: dat/Jester.des util/lev_comp
	( cd dat ; ../util/lev_comp Jester.des )
dat/Jes-loca.lev: dat/Jes-strt.lev
dat/Jes-goal.lev: dat/Jes-strt.lev
dat/Jes-fila.lev: dat/Jes-strt.lev
dat/Jes-filb.lev: dat/Jes-strt.lev
