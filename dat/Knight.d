SPECIAL_LEVELS += Kni-strt.lev Kni-loca.lev Kni-goal.lev Kni-fila.lev Kni-filb.lev
dat/Kni-strt.lev: dat/Knight.des util/lev_comp
	( cd dat ; ../util/lev_comp Knight.des )
dat/Kni-loca.lev: dat/Kni-strt.lev
dat/Kni-goal.lev: dat/Kni-strt.lev
dat/Kni-fila.lev: dat/Kni-strt.lev
dat/Kni-filb.lev: dat/Kni-strt.lev
