SPECIAL_LEVELS += Bos-strt.lev Bos-loca.lev Bos-goal.lev Bos-fila.lev Bos-filb.lev
dat/Bos-strt.lev: dat/Bosmer.des util/lev_comp
	( cd dat ; ../util/lev_comp Bosmer.des )
dat/Bos-loca.lev: dat/Bos-strt.lev
dat/Bos-goal.lev: dat/Bos-strt.lev
dat/Bos-fila.lev: dat/Bos-strt.lev
dat/Bos-filb.lev: dat/Bos-strt.lev
