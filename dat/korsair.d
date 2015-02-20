SPECIAL_LEVELS += Kor-strt.lev Kor-loca.lev Kor-goal.lev Kor-fila.lev Kor-filb.lev
dat/Kor-strt.lev: dat/korsair.des util/lev_comp
	( cd dat ; ../util/lev_comp korsair.des )
dat/Kor-loca.lev: dat/Kor-strt.lev
dat/Kor-goal.lev: dat/Kor-strt.lev
dat/Kor-fila.lev: dat/Kor-strt.lev
dat/Kor-filb.lev: dat/Kor-strt.lev
