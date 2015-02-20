SPECIAL_LEVELS += Cou-strt.lev Cou-loca.lev Cou-goal.lev Cou-fila.lev Cou-filb.lev
dat/Cou-strt.lev: dat/Courier.des util/lev_comp
	( cd dat ; ../util/lev_comp Courier.des )
dat/Cou-loca.lev: dat/Cou-strt.lev
dat/Cou-goal.lev: dat/Cou-strt.lev
dat/Cou-fila.lev: dat/Cou-strt.lev
dat/Cou-filb.lev: dat/Cou-strt.lev
