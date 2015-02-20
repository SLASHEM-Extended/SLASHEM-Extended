SPECIAL_LEVELS += Sag-strt.lev Sag-loca.lev Sag-goal.lev Sag-fila.lev Sag-filb.lev
dat/Sag-strt.lev: dat/Sage.des util/lev_comp
	( cd dat ; ../util/lev_comp Sage.des )
dat/Sag-loca.lev: dat/Sag-strt.lev
dat/Sag-goal.lev: dat/Sag-strt.lev
dat/Sag-fila.lev: dat/Sag-strt.lev
dat/Sag-filb.lev: dat/Sag-strt.lev
