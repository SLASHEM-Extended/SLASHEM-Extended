SPECIAL_LEVELS += Abu-strt.lev Abu-loca.lev Abu-goal.lev Abu-fila.lev Abu-filb.lev
dat/Abu-strt.lev: dat/Abuser.des util/lev_comp
	( cd dat ; ../util/lev_comp Abuser.des )
dat/Abu-loca.lev: dat/Abu-strt.lev
dat/Abu-goal.lev: dat/Abu-strt.lev
dat/Abu-fila.lev: dat/Abu-strt.lev
dat/Abu-filb.lev: dat/Abu-strt.lev
