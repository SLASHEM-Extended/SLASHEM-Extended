SPECIAL_LEVELS += Top-strt.lev Top-fila.lev Top-loca.lev Top-filb.lev Top-goal.lev
dat/Top-strt.lev: dat/Top.des util/lev_comp
	( cd dat ; ../util/lev_comp Top.des )
dat/Top-fila.lev: dat/Top-strt.lev
dat/Top-loca.lev: dat/Top-strt.lev
dat/Top-filb.lev: dat/Top-strt.lev
dat/Top-goal.lev: dat/Top-strt.lev
