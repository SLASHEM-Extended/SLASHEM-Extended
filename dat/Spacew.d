SPECIAL_LEVELS += Spa-strt.lev Spa-fila.lev Spa-loca.lev Spa-filb.lev Spa-goal.lev
dat/Spa-strt.lev: dat/Spacew.des util/lev_comp
	( cd dat ; ../util/lev_comp Spacew.des )
dat/Spa-fila.lev: dat/Spa-strt.lev
dat/Spa-loca.lev: dat/Spa-strt.lev
dat/Spa-filb.lev: dat/Spa-strt.lev
dat/Spa-goal.lev: dat/Spa-strt.lev
