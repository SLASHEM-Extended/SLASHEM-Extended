SPECIAL_LEVELS += Gra-strt.lev Gra-loca.lev Gra-goal.lev Gra-fila.lev Gra-filb.lev
dat/Gra-strt.lev: dat/Gradu.des util/lev_comp
	( cd dat ; ../util/lev_comp Gradu.des )
dat/Gra-loca.lev: dat/Gra-strt.lev
dat/Gra-goal.lev: dat/Gra-strt.lev
dat/Gra-fila.lev: dat/Gra-strt.lev
dat/Gra-filb.lev: dat/Gra-strt.lev
