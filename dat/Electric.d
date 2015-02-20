SPECIAL_LEVELS += Ele-strt.lev Ele-loca.lev Ele-goal.lev Ele-fila.lev Ele-filb.lev
dat/Ele-strt.lev: dat/Electric.des util/lev_comp
	( cd dat ; ../util/lev_comp Electric.des )
dat/Ele-loca.lev: dat/Ele-strt.lev
dat/Ele-goal.lev: dat/Ele-strt.lev
dat/Ele-fila.lev: dat/Ele-strt.lev
dat/Ele-filb.lev: dat/Ele-strt.lev
