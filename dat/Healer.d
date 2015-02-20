SPECIAL_LEVELS += Hea-strt.lev Hea-loca.lev Hea-goal.lev Hea-fila.lev Hea-filb.lev
dat/Hea-strt.lev: dat/Healer.des util/lev_comp
	( cd dat ; ../util/lev_comp Healer.des )
dat/Hea-loca.lev: dat/Hea-strt.lev
dat/Hea-goal.lev: dat/Hea-strt.lev
dat/Hea-fila.lev: dat/Hea-strt.lev
dat/Hea-filb.lev: dat/Hea-strt.lev
