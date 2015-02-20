SPECIAL_LEVELS += Nob-strt.lev Nob-loca.lev Nob-goal.lev Nob-fila.lev Nob-filb.lev
dat/Nob-strt.lev: dat/Noble.des util/lev_comp
	( cd dat ; ../util/lev_comp Noble.des )
dat/Nob-loca.lev: dat/Nob-strt.lev
dat/Nob-goal.lev: dat/Nob-strt.lev
dat/Nob-fila.lev: dat/Nob-strt.lev
dat/Nob-filb.lev: dat/Nob-strt.lev
