SPECIAL_LEVELS += Art-strt.lev Art-loca.lev Art-goal.lev Art-fila.lev Art-filb.lev
dat/Art-strt.lev: dat/Artist.des util/lev_comp
	( cd dat ; ../util/lev_comp Artist.des )
dat/Art-loca.lev: dat/Art-strt.lev
dat/Art-goal.lev: dat/Art-strt.lev
dat/Art-fila.lev: dat/Art-strt.lev
dat/Art-filb.lev: dat/Art-strt.lev
