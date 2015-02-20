SPECIAL_LEVELS += Val-strt.lev Val-loca.lev Val-goal.lev Val-fila.lev Val-filb.lev
dat/Val-strt.lev: dat/Valkyrie.des util/lev_comp
	( cd dat ; ../util/lev_comp Valkyrie.des )
dat/Val-loca.lev: dat/Val-strt.lev
dat/Val-goal.lev: dat/Val-strt.lev
dat/Val-fila.lev: dat/Val-strt.lev
dat/Val-filb.lev: dat/Val-strt.lev
