SPECIAL_LEVELS += Zoo-strt.lev Zoo-loca.lev Zoo-goal.lev Zoo-fila.lev Zoo-filb.lev
dat/Zoo-strt.lev: dat/Zooke.des util/lev_comp
	( cd dat ; ../util/lev_comp Zooke.des )
dat/Zoo-loca.lev: dat/Zoo-strt.lev
dat/Zoo-goal.lev: dat/Zoo-strt.lev
dat/Zoo-fila.lev: dat/Zoo-strt.lev
dat/Zoo-filb.lev: dat/Zoo-strt.lev
