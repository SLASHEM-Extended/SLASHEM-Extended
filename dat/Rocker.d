SPECIAL_LEVELS += Roc-strt.lev Roc-loca.lev Roc-goal.lev Roc-fila.lev Roc-filb.lev
dat/Roc-strt.lev: dat/Rocker.des util/lev_comp
	( cd dat ; ../util/lev_comp Rocker.des )
dat/Roc-loca.lev: dat/Roc-strt.lev
dat/Roc-goal.lev: dat/Roc-strt.lev
dat/Roc-fila.lev: dat/Roc-strt.lev
dat/Roc-filb.lev: dat/Roc-strt.lev
