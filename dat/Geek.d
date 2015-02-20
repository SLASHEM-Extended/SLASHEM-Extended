SPECIAL_LEVELS += Gee-strt.lev Gee-loca.lev Gee-goal.lev Gee-fila.lev Gee-filb.lev
dat/Gee-strt.lev: dat/Geek.des util/lev_comp
	( cd dat ; ../util/lev_comp Geek.des )
dat/Gee-loca.lev: dat/Gee-strt.lev
dat/Gee-goal.lev: dat/Gee-strt.lev
dat/Gee-fila.lev: dat/Gee-strt.lev
dat/Gee-filb.lev: dat/Gee-strt.lev
