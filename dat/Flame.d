SPECIAL_LEVELS += Fla-strt.lev Fla-loca.lev Fla-goal.lev Fla-fila.lev Fla-filb.lev
dat/Fla-strt.lev: dat/Flame.des util/lev_comp
	( cd dat ; ../util/lev_comp Flame.des )
dat/Fla-loca.lev: dat/Fla-strt.lev
dat/Fla-goal.lev: dat/Fla-strt.lev
dat/Fla-fila.lev: dat/Fla-strt.lev
dat/Fla-filb.lev: dat/Fla-strt.lev
