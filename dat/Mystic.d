SPECIAL_LEVELS += Mys-strt.lev Mys-loca.lev Mys-goal.lev Mys-fila.lev Mys-filb.lev
dat/Mys-strt.lev: dat/Mystic.des util/lev_comp
	( cd dat ; ../util/lev_comp Mystic.des )
dat/Mys-loca.lev: dat/Mys-strt.lev
dat/Mys-goal.lev: dat/Mys-strt.lev
dat/Mys-fila.lev: dat/Mys-strt.lev
dat/Mys-filb.lev: dat/Mys-strt.lev
