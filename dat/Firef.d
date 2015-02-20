SPECIAL_LEVELS += Fir-strt.lev Fir-loca.lev Fir-goal.lev Fir-fila.lev Fir-filb.lev
dat/Fir-strt.lev: dat/Firef.des util/lev_comp
	( cd dat ; ../util/lev_comp Firef.des )
dat/Fir-loca.lev: dat/Fir-strt.lev
dat/Fir-goal.lev: dat/Fir-strt.lev
dat/Fir-fila.lev: dat/Fir-strt.lev
dat/Fir-filb.lev: dat/Fir-strt.lev
