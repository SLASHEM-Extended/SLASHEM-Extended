SPECIAL_LEVELS += Bin-strt.lev Bin-loca.lev Bin-goal.lev Bin-fila.lev Bin-filb.lev
dat/Bin-strt.lev: dat/Binder.des util/lev_comp
	( cd dat ; ../util/lev_comp Binder.des )
dat/Bin-loca.lev: dat/Bin-strt.lev
dat/Bin-goal.lev: dat/Bin-strt.lev
dat/Bin-fila.lev: dat/Bin-strt.lev
dat/Bin-filb.lev: dat/Bin-strt.lev
