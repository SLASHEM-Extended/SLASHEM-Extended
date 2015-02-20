SPECIAL_LEVELS += Arc-strt.lev Arc-loca.lev Arc-goal.lev Arc-fila.lev Arc-filb.lev
dat/Arc-strt.lev: dat/Arch.des util/lev_comp
	( cd dat ; ../util/lev_comp Arch.des )
dat/Arc-loca.lev: dat/Arc-strt.lev
dat/Arc-goal.lev: dat/Arc-strt.lev
dat/Arc-fila.lev: dat/Arc-strt.lev
dat/Arc-filb.lev: dat/Arc-strt.lev
