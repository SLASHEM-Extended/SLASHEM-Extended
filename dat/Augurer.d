SPECIAL_LEVELS += Aug-strt.lev Aug-loca.lev Aug-goal.lev Aug-fila.lev Aug-filb.lev
dat/Aug-strt.lev: dat/Augurer.des util/lev_comp
	( cd dat ; ../util/lev_comp Augurer.des )
dat/Aug-loca.lev: dat/Aug-strt.lev
dat/Aug-goal.lev: dat/Aug-strt.lev
dat/Aug-fila.lev: dat/Aug-strt.lev
dat/Aug-filb.lev: dat/Aug-strt.lev
