SPECIAL_LEVELS += Elp-strt.lev Elp-loca.lev Elp-goal.lev Elp-fila.lev Elp-filb.lev
dat/Elp-strt.lev: dat/Elfreal.des util/lev_comp
	( cd dat ; ../util/lev_comp Elfreal.des )
dat/Elp-loca.lev: dat/Elp-strt.lev
dat/Elp-goal.lev: dat/Elp-strt.lev
dat/Elp-fila.lev: dat/Elp-strt.lev
dat/Elp-filb.lev: dat/Elp-strt.lev
