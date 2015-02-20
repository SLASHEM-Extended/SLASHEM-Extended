SPECIAL_LEVELS += Ass-goal.lev Ass-loca.lev Ass-strt.lev Ass-fila.lev Ass-filb.lev
dat/Ass-goal.lev: dat/Assassin.des util/lev_comp
	( cd dat ; ../util/lev_comp Assassin.des )
dat/Ass-loca.lev: dat/Ass-goal.lev
dat/Ass-strt.lev: dat/Ass-goal.lev
dat/Ass-fila.lev: dat/Ass-goal.lev
dat/Ass-filb.lev: dat/Ass-goal.lev
