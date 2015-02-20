SPECIAL_LEVELS += Ama-strt.lev Ama-fila.lev Ama-loca.lev Ama-filb.lev Ama-goal.lev
dat/Ama-strt.lev: dat/Amazon.des util/lev_comp
	( cd dat ; ../util/lev_comp Amazon.des )
dat/Ama-fila.lev: dat/Ama-strt.lev
dat/Ama-loca.lev: dat/Ama-strt.lev
dat/Ama-filb.lev: dat/Ama-strt.lev
dat/Ama-goal.lev: dat/Ama-strt.lev
