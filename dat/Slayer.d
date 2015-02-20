SPECIAL_LEVELS += Und-strt.lev Und-loca.lev Und-goal.lev Und-fila.lev Und-filb.lev
dat/Und-strt.lev: dat/Slayer.des util/lev_comp
	( cd dat ; ../util/lev_comp Slayer.des )
dat/Und-loca.lev: dat/Und-strt.lev
dat/Und-goal.lev: dat/Und-strt.lev
dat/Und-fila.lev: dat/Und-strt.lev
dat/Und-filb.lev: dat/Und-strt.lev
