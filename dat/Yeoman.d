SPECIAL_LEVELS += Yeo-strt.lev Yeo-fila.lev Yeo-loca.lev Yeo-filb.lev Yeo-goal.lev
dat/Yeo-strt.lev: dat/Yeoman.des util/lev_comp
	( cd dat ; ../util/lev_comp Yeoman.des )
dat/Yeo-fila.lev: dat/Yeo-strt.lev
dat/Yeo-loca.lev: dat/Yeo-strt.lev
dat/Yeo-filb.lev: dat/Yeo-strt.lev
dat/Yeo-goal.lev: dat/Yeo-strt.lev
