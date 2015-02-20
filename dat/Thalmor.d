SPECIAL_LEVELS += Tha-strt.lev Tha-loca.lev Tha-goal.lev Tha-fila.lev Tha-filb.lev
dat/Tha-strt.lev: dat/Thalmor.des util/lev_comp
	( cd dat ; ../util/lev_comp Thalmor.des )
dat/Tha-loca.lev: dat/Tha-strt.lev
dat/Tha-goal.lev: dat/Tha-strt.lev
dat/Tha-fila.lev: dat/Tha-strt.lev
dat/Tha-filb.lev: dat/Tha-strt.lev
