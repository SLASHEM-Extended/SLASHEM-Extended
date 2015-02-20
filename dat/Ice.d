SPECIAL_LEVELS += Ice-strt.lev Ice-loca.lev Ice-goal.lev Ice-fila.lev Ice-filb.lev
dat/Ice-strt.lev: dat/Ice.des util/lev_comp
	( cd dat ; ../util/lev_comp Ice.des )
dat/Ice-loca.lev: dat/Ice-strt.lev
dat/Ice-goal.lev: dat/Ice-strt.lev
dat/Ice-fila.lev: dat/Ice-strt.lev
dat/Ice-filb.lev: dat/Ice-strt.lev
