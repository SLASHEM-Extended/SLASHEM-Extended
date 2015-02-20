SPECIAL_LEVELS += Tou-strt.lev Tou-loca.lev Tou-goal.lev Tou-fila.lev Tou-filb.lev
dat/Tou-strt.lev: dat/Tourist.des util/lev_comp
	( cd dat ; ../util/lev_comp Tourist.des )
dat/Tou-loca.lev: dat/Tou-strt.lev
dat/Tou-goal.lev: dat/Tou-strt.lev
dat/Tou-fila.lev: dat/Tou-strt.lev
dat/Tou-filb.lev: dat/Tou-strt.lev
