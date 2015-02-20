SPECIAL_LEVELS += Pir-strt.lev Pir-loca.lev Pir-goal.lev Pir-fila.lev Pir-filb.lev
dat/Pir-strt.lev: dat/pirate.des util/lev_comp
	( cd dat ; ../util/lev_comp pirate.des )
dat/Pir-loca.lev: dat/Pir-strt.lev
dat/Pir-goal.lev: dat/Pir-strt.lev
dat/Pir-fila.lev: dat/Pir-strt.lev
dat/Pir-filb.lev: dat/Pir-strt.lev
