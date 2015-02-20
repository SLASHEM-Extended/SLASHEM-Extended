SPECIAL_LEVELS += Dru-strt.lev Dru-loca.lev Dru-goal.lev Dru-fila.lev Dru-filb.lev
dat/Dru-strt.lev: dat/Drunk.des util/lev_comp
	( cd dat ; ../util/lev_comp Drunk.des )
dat/Dru-loca.lev: dat/Dru-strt.lev
dat/Dru-goal.lev: dat/Dru-strt.lev
dat/Dru-fila.lev: dat/Dru-strt.lev
dat/Dru-filb.lev: dat/Dru-strt.lev
