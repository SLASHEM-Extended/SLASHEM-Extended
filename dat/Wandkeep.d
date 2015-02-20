SPECIAL_LEVELS += Wan-strt.lev Wan-loca.lev Wan-goal.lev Wan-fila.lev Wan-filb.lev
dat/Wan-strt.lev: dat/Wandkeep.des util/lev_comp
	( cd dat ; ../util/lev_comp Wandkeep.des )
dat/Wan-loca.lev: dat/Wan-strt.lev
dat/Wan-goal.lev: dat/Wan-strt.lev
dat/Wan-fila.lev: dat/Wan-strt.lev
dat/Wan-filb.lev: dat/Wan-strt.lev
