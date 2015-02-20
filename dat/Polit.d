SPECIAL_LEVELS += Pol-strt.lev Pol-fila.lev Pol-loca.lev Pol-filb.lev Pol-goal.lev
dat/Pol-strt.lev: dat/Polit.des util/lev_comp
	( cd dat ; ../util/lev_comp Polit.des )
dat/Pol-fila.lev: dat/Pol-strt.lev
dat/Pol-loca.lev: dat/Pol-strt.lev
dat/Pol-filb.lev: dat/Pol-strt.lev
dat/Pol-goal.lev: dat/Pol-strt.lev
