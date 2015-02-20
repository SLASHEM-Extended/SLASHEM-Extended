SPECIAL_LEVELS += Lad-strt.lev Lad-fila.lev Lad-loca.lev Lad-filb.lev Lad-goal.lev
dat/Lad-strt.lev: dat/Ladies.des util/lev_comp
	( cd dat ; ../util/lev_comp Ladies.des )
dat/Lad-fila.lev: dat/Lad-strt.lev
dat/Lad-loca.lev: dat/Lad-strt.lev
dat/Lad-filb.lev: dat/Lad-strt.lev
dat/Lad-goal.lev: dat/Lad-strt.lev
