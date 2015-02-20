SPECIAL_LEVELS += Sup-strt.lev Sup-loca.lev Sup-goal.lev Sup-fila.lev Sup-filb.lev
dat/Sup-strt.lev: dat/Supermar.des util/lev_comp
	( cd dat ; ../util/lev_comp Supermar.des )
dat/Sup-loca.lev: dat/Sup-strt.lev
dat/Sup-goal.lev: dat/Sup-strt.lev
dat/Sup-fila.lev: dat/Sup-strt.lev
dat/Sup-filb.lev: dat/Sup-strt.lev
