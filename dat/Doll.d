SPECIAL_LEVELS += Dol-strt.lev Dol-fila.lev Dol-loca.lev Dol-filb.lev Dol-goal.lev
dat/Dol-strt.lev: dat/Doll.des util/lev_comp
	( cd dat ; ../util/lev_comp Doll.des )
dat/Dol-fila.lev: dat/Dol-strt.lev
dat/Dol-loca.lev: dat/Dol-strt.lev
dat/Dol-filb.lev: dat/Dol-strt.lev
dat/Dol-goal.lev: dat/Dol-strt.lev
