SPECIAL_LEVELS += Zyb-strt.lev Zyb-loca.lev Zyb-goal.lev Zyb-fila.lev Zyb-filb.lev
dat/Zyb-strt.lev: dat/Cyborg.des util/lev_comp
	( cd dat ; ../util/lev_comp Cyborg.des )
dat/Zyb-loca.lev: dat/Zyb-strt.lev
dat/Zyb-goal.lev: dat/Zyb-strt.lev
dat/Zyb-fila.lev: dat/Zyb-strt.lev
dat/Zyb-filb.lev: dat/Zyb-strt.lev
