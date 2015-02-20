SPECIAL_LEVELS += Blo-strt.lev Blo-loca.lev Blo-goal.lev Blo-fila.lev Blo-filb.lev
dat/Blo-strt.lev: dat/Bloodsee.des util/lev_comp
	( cd dat ; ../util/lev_comp Bloodsee.des )
dat/Blo-loca.lev: dat/Blo-strt.lev
dat/Blo-goal.lev: dat/Blo-strt.lev
dat/Blo-fila.lev: dat/Blo-strt.lev
dat/Blo-filb.lev: dat/Blo-strt.lev
