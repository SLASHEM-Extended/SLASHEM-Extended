SPECIAL_LEVELS += Gun-strt.lev Gun-loca.lev Gun-goal.lev Gun-fila.lev Gun-filb.lev
dat/Gun-strt.lev: dat/Gunner.des util/lev_comp
	( cd dat ; ../util/lev_comp Gunner.des )
dat/Gun-loca.lev: dat/Gun-strt.lev
dat/Gun-goal.lev: dat/Gun-strt.lev
dat/Gun-fila.lev: dat/Gun-strt.lev
dat/Gun-filb.lev: dat/Gun-strt.lev
