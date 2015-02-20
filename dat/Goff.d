SPECIAL_LEVELS += Gof-strt.lev Gof-fila.lev Gof-loca.lev Gof-filb.lev Gof-goal.lev
dat/Gof-strt.lev: dat/Goff.des util/lev_comp
	( cd dat ; ../util/lev_comp Goff.des )
dat/Gof-fila.lev: dat/Gof-strt.lev
dat/Gof-loca.lev: dat/Gof-strt.lev
dat/Gof-filb.lev: dat/Gof-strt.lev
dat/Gof-goal.lev: dat/Gof-strt.lev
