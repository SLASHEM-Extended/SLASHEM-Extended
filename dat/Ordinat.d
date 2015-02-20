SPECIAL_LEVELS += Ord-strt.lev Ord-loca.lev Ord-goal.lev Ord-fila.lev Ord-filb.lev
dat/Ord-strt.lev: dat/Ordinat.des util/lev_comp
	( cd dat ; ../util/lev_comp Ordinat.des )
dat/Ord-loca.lev: dat/Ord-strt.lev
dat/Ord-goal.lev: dat/Ord-strt.lev
dat/Ord-fila.lev: dat/Ord-strt.lev
dat/Ord-filb.lev: dat/Ord-strt.lev
