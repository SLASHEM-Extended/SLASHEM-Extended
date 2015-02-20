SPECIAL_LEVELS += Che-strt.lev Che-loca.lev Che-goal.lev Che-fila.lev Che-filb.lev
dat/Che-strt.lev: dat/Cheval.des util/lev_comp
	( cd dat ; ../util/lev_comp Cheval.des )
dat/Che-loca.lev: dat/Che-strt.lev
dat/Che-goal.lev: dat/Che-strt.lev
dat/Che-fila.lev: dat/Che-strt.lev
dat/Che-filb.lev: dat/Che-strt.lev
