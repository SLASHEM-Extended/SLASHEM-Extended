SPECIAL_LEVELS += Con-strt.lev Con-loca.lev Con-goal.lev Con-fila.lev Con-filb.lev
dat/Con-strt.lev: dat/Convict.des util/lev_comp
	( cd dat ; ../util/lev_comp Convict.des )
dat/Con-loca.lev: dat/Con-strt.lev
dat/Con-goal.lev: dat/Con-strt.lev
dat/Con-fila.lev: dat/Con-strt.lev
dat/Con-filb.lev: dat/Con-strt.lev
