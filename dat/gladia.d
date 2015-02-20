SPECIAL_LEVELS += Gla-strt.lev Gla-loca.lev Gla-goal.lev Gla-fila.lev Gla-filb.lev
dat/Gla-strt.lev: dat/gladia.des util/lev_comp
	( cd dat ; ../util/lev_comp gladia.des )
dat/Gla-loca.lev: dat/Gla-strt.lev
dat/Gla-goal.lev: dat/Gla-strt.lev
dat/Gla-fila.lev: dat/Gla-strt.lev
dat/Gla-filb.lev: dat/Gla-strt.lev
