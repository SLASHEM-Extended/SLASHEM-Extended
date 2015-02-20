SPECIAL_LEVELS += Jed-strt.lev Jed-loca.lev Jed-goal.lev Jed-fila.lev Jed-filb.lev
dat/Jed-strt.lev: dat/Jedi.des util/lev_comp
	( cd dat ; ../util/lev_comp Jedi.des )
dat/Jed-loca.lev: dat/Jed-strt.lev
dat/Jed-goal.lev: dat/Jed-strt.lev
dat/Jed-fila.lev: dat/Jed-strt.lev
dat/Jed-filb.lev: dat/Jed-strt.lev
