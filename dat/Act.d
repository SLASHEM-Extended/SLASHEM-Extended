SPECIAL_LEVELS += Act-strt.lev Act-loca.lev Act-goal.lev Act-fila.lev Act-filb.lev
dat/Act-strt.lev: dat/Act.des util/lev_comp
	( cd dat ; ../util/lev_comp Act.des )
dat/Act-loca.lev: dat/Act-strt.lev
dat/Act-goal.lev: dat/Act-strt.lev
dat/Act-fila.lev: dat/Act-strt.lev
dat/Act-filb.lev: dat/Act-strt.lev
