SPECIAL_LEVELS += Nin-strt.lev Nin-loca.lev Nin-goal.lev Nin-fila.lev Nin-filb.lev
dat/Nin-strt.lev: dat/Ninja.des util/lev_comp
	( cd dat ; ../util/lev_comp Ninja.des )
dat/Nin-loca.lev: dat/Nin-strt.lev
dat/Nin-goal.lev: dat/Nin-strt.lev
dat/Nin-fila.lev: dat/Nin-strt.lev
dat/Nin-filb.lev: dat/Nin-strt.lev
