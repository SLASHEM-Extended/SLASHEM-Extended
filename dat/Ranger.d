SPECIAL_LEVELS += Ran-strt.lev Ran-loca.lev Ran-goal.lev Ran-fila.lev Ran-filb.lev
dat/Ran-strt.lev: dat/Ranger.des util/lev_comp
	( cd dat ; ../util/lev_comp Ranger.des )
dat/Ran-loca.lev: dat/Ran-strt.lev
dat/Ran-goal.lev: dat/Ran-strt.lev
dat/Ran-fila.lev: dat/Ran-strt.lev
dat/Ran-filb.lev: dat/Ran-strt.lev
