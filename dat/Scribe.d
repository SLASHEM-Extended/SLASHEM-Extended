SPECIAL_LEVELS += Scr-strt.lev Scr-loca.lev Scr-goal.lev Scr-fila.lev Scr-filb.lev
dat/Scr-strt.lev: dat/Scribe.des util/lev_comp
	( cd dat ; ../util/lev_comp Scribe.des )
dat/Scr-loca.lev: dat/Scr-strt.lev
dat/Scr-goal.lev: dat/Scr-strt.lev
dat/Scr-fila.lev: dat/Scr-strt.lev
dat/Scr-filb.lev: dat/Scr-strt.lev
