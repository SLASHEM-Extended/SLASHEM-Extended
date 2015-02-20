SPECIAL_LEVELS += Pal-strt.lev Pal-loca.lev Pal-goal.lev Pal-fila.lev Pal-filb.lev
dat/Pal-strt.lev: dat/Paladin.des util/lev_comp
	( cd dat ; ../util/lev_comp Paladin.des )
dat/Pal-loca.lev: dat/Pal-strt.lev
dat/Pal-goal.lev: dat/Pal-strt.lev
dat/Pal-fila.lev: dat/Pal-strt.lev
dat/Pal-filb.lev: dat/Pal-strt.lev
