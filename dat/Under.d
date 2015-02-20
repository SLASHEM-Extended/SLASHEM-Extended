SPECIAL_LEVELS += Unt-strt.lev Unt-loca.lev Unt-goal.lev Unt-fila.lev Unt-filb.lev
dat/Unt-strt.lev: dat/Under.des util/lev_comp
	( cd dat ; ../util/lev_comp Under.des )
dat/Unt-loca.lev: dat/Unt-strt.lev
dat/Unt-goal.lev: dat/Unt-strt.lev
dat/Unt-fila.lev: dat/Unt-strt.lev
dat/Unt-filb.lev: dat/Unt-strt.lev
