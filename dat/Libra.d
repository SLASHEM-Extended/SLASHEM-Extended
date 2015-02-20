SPECIAL_LEVELS += Lib-strt.lev Lib-loca.lev Lib-goal.lev Lib-fila.lev Lib-filb.lev
dat/Lib-strt.lev: dat/Libra.des util/lev_comp
	( cd dat ; ../util/lev_comp Libra.des )
dat/Lib-loca.lev: dat/Lib-strt.lev
dat/Lib-goal.lev: dat/Lib-strt.lev
dat/Lib-fila.lev: dat/Lib-strt.lev
dat/Lib-filb.lev: dat/Lib-strt.lev
