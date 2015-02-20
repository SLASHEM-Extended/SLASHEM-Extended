SPECIAL_LEVELS += Rog-strt.lev Rog-loca.lev Rog-goal.lev Rog-fila.lev Rog-filb.lev
dat/Rog-strt.lev: dat/Rogue.des util/lev_comp
	( cd dat ; ../util/lev_comp Rogue.des )
dat/Rog-loca.lev: dat/Rog-strt.lev
dat/Rog-goal.lev: dat/Rog-strt.lev
dat/Rog-fila.lev: dat/Rog-strt.lev
dat/Rog-filb.lev: dat/Rog-strt.lev
