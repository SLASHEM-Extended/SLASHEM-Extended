SPECIAL_LEVELS += Dea-strt.lev Dea-loca.lev Dea-goal.lev Dea-fila.lev Dea-filb.lev
dat/Dea-strt.lev: dat/Deatheat.des util/lev_comp
	( cd dat ; ../util/lev_comp Deatheat.des )
dat/Dea-loca.lev: dat/Dea-strt.lev
dat/Dea-goal.lev: dat/Dea-strt.lev
dat/Dea-fila.lev: dat/Dea-strt.lev
dat/Dea-filb.lev: dat/Dea-strt.lev
