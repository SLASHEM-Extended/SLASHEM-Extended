SPECIAL_LEVELS += Bul-strt.lev Bul-loca.lev Bul-goal.lev Bul-fila.lev Bul-filb.lev
dat/Bul-strt.lev: dat/Bully.des util/lev_comp
	( cd dat ; ../util/lev_comp Bully.des )
dat/Bul-loca.lev: dat/Bul-strt.lev
dat/Bul-goal.lev: dat/Bul-strt.lev
dat/Bul-fila.lev: dat/Bul-strt.lev
dat/Bul-filb.lev: dat/Bul-strt.lev
