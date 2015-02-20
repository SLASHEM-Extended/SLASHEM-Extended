SPECIAL_LEVELS += Gan-strt.lev Gan-loca.lev Gan-goal.lev Gan-fila.lev Gan-filb.lev
dat/Gan-strt.lev: dat/Gangster.des util/lev_comp
	( cd dat ; ../util/lev_comp Gangster.des )
dat/Gan-loca.lev: dat/Gan-strt.lev
dat/Gan-goal.lev: dat/Gan-strt.lev
dat/Gan-fila.lev: dat/Gan-strt.lev
dat/Gan-filb.lev: dat/Gan-strt.lev
