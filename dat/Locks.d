SPECIAL_LEVELS += Loc-strt.lev Loc-loca.lev Loc-goal.lev Loc-fila.lev Loc-filb.lev
dat/Loc-strt.lev: dat/Locks.des util/lev_comp
	( cd dat ; ../util/lev_comp Locks.des )
dat/Loc-loca.lev: dat/Loc-strt.lev
dat/Loc-goal.lev: dat/Loc-strt.lev
dat/Loc-fila.lev: dat/Loc-strt.lev
dat/Loc-filb.lev: dat/Loc-strt.lev
