SPECIAL_LEVELS += Gam-strt.lev Gam-loca.lev Gam-goal.lev Gam-fila.lev Gam-filb.lev
dat/Gam-strt.lev: dat/Gamer.des util/lev_comp
	( cd dat ; ../util/lev_comp Gamer.des )
dat/Gam-loca.lev: dat/Gam-strt.lev
dat/Gam-goal.lev: dat/Gam-strt.lev
dat/Gam-fila.lev: dat/Gam-strt.lev
dat/Gam-filb.lev: dat/Gam-strt.lev
