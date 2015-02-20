SPECIAL_LEVELS += Wiz-strt.lev Wiz-loca.lev Wiz-goal.lev Wiz-fila.lev Wiz-filb.lev
dat/Wiz-strt.lev: dat/Wizard.des util/lev_comp
	( cd dat ; ../util/lev_comp Wizard.des )
dat/Wiz-loca.lev: dat/Wiz-strt.lev
dat/Wiz-goal.lev: dat/Wiz-strt.lev
dat/Wiz-fila.lev: dat/Wiz-strt.lev
dat/Wiz-filb.lev: dat/Wiz-strt.lev
