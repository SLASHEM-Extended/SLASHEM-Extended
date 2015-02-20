SPECIAL_LEVELS += Cav-strt.lev Cav-loca.lev Cav-goal.lev Cav-fila.lev Cav-filb.lev
dat/Cav-strt.lev: dat/Caveman.des util/lev_comp
	( cd dat ; ../util/lev_comp Caveman.des )
dat/Cav-loca.lev: dat/Cav-strt.lev
dat/Cav-goal.lev: dat/Cav-strt.lev
dat/Cav-fila.lev: dat/Cav-strt.lev
dat/Cav-filb.lev: dat/Cav-strt.lev
