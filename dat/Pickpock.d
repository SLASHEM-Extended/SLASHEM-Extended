SPECIAL_LEVELS += Pic-strt.lev Pic-fila.lev Pic-loca.lev Pic-filb.lev Pic-goal.lev
dat/Pic-strt.lev: dat/Pickpock.des util/lev_comp
	( cd dat ; ../util/lev_comp Pickpock.des )
dat/Pic-fila.lev: dat/Pic-strt.lev
dat/Pic-loca.lev: dat/Pic-strt.lev
dat/Pic-filb.lev: dat/Pic-strt.lev
dat/Pic-goal.lev: dat/Pic-strt.lev
