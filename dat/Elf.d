SPECIAL_LEVELS += Pok-strt.lev Pok-loca.lev Pok-goal.lev Pok-fila.lev Pok-filb.lev
dat/Pok-strt.lev: dat/Elf.des util/lev_comp
	( cd dat ; ../util/lev_comp Elf.des )
dat/Pok-loca.lev: dat/Pok-strt.lev
dat/Pok-goal.lev: dat/Pok-strt.lev
dat/Pok-fila.lev: dat/Pok-strt.lev
dat/Pok-filb.lev: dat/Pok-strt.lev
