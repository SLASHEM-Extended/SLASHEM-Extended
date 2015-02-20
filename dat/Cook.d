SPECIAL_LEVELS += Coo-strt.lev Coo-loca.lev Coo-goal.lev Coo-fila.lev Coo-filb.lev
dat/Coo-strt.lev: dat/Cook.des util/lev_comp
	( cd dat ; ../util/lev_comp Cook.des )
dat/Coo-loca.lev: dat/Coo-strt.lev
dat/Coo-goal.lev: dat/Coo-strt.lev
dat/Coo-fila.lev: dat/Coo-strt.lev
dat/Coo-filb.lev: dat/Coo-strt.lev
