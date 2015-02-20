SPECIAL_LEVELS += Ota-strt.lev Ota-loca.lev Ota-goal.lev Ota-fila.lev Ota-filb.lev
dat/Ota-strt.lev: dat/Otaku.des util/lev_comp
	( cd dat ; ../util/lev_comp Otaku.des )
dat/Ota-loca.lev: dat/Ota-strt.lev
dat/Ota-goal.lev: dat/Ota-strt.lev
dat/Ota-fila.lev: dat/Ota-strt.lev
dat/Ota-filb.lev: dat/Ota-strt.lev
