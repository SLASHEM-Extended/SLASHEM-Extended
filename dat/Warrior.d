SPECIAL_LEVELS += War-strt.lev War-loca.lev War-goal.lev War-fila.lev War-filb.lev
dat/War-strt.lev: dat/Warrior.des util/lev_comp
	( cd dat ; ../util/lev_comp Warrior.des )
dat/War-loca.lev: dat/War-strt.lev
dat/War-goal.lev: dat/War-strt.lev
dat/War-fila.lev: dat/War-strt.lev
dat/War-filb.lev: dat/War-strt.lev
