SPECIAL_LEVELS += Sai-strt.lev Sai-loca.lev Sai-goal.lev Sai-fila.lev Sai-filb.lev
dat/Sai-strt.lev: dat/Saiyan.des util/lev_comp
	( cd dat ; ../util/lev_comp Saiyan.des )
dat/Sai-loca.lev: dat/Sai-strt.lev
dat/Sai-goal.lev: dat/Sai-strt.lev
dat/Sai-fila.lev: dat/Sai-strt.lev
dat/Sai-filb.lev: dat/Sai-strt.lev
