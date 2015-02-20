SPECIAL_LEVELS += Nec-strt.lev Nec-loca.lev Nec-goal.lev Nec-fila.lev Nec-filb.lev
dat/Nec-strt.lev: dat/Necro.des util/lev_comp
	( cd dat ; ../util/lev_comp Necro.des )
dat/Nec-loca.lev: dat/Nec-strt.lev
dat/Nec-goal.lev: dat/Nec-strt.lev
dat/Nec-fila.lev: dat/Nec-strt.lev
dat/Nec-filb.lev: dat/Nec-strt.lev
