SPECIAL_LEVELS += Sci-strt.lev Sci-loca.lev Sci-goal.lev Sci-fila.lev Sci-filb.lev
dat/Sci-strt.lev: dat/scient.des util/lev_comp
	( cd dat ; ../util/lev_comp scient.des )
dat/Sci-loca.lev: dat/Sci-strt.lev
dat/Sci-goal.lev: dat/Sci-strt.lev
dat/Sci-fila.lev: dat/Sci-strt.lev
dat/Sci-filb.lev: dat/Sci-strt.lev
