SPECIAL_LEVELS += Lun-strt.lev Lun-loca.lev Lun-goal.lev Lun-fila.lev Lun-filb.lev
dat/Lun-strt.lev: dat/Lunatic.des util/lev_comp
	( cd dat ; ../util/lev_comp Lunatic.des )
dat/Lun-loca.lev: dat/Lun-strt.lev
dat/Lun-goal.lev: dat/Lun-strt.lev
dat/Lun-fila.lev: dat/Lun-strt.lev
dat/Lun-filb.lev: dat/Lun-strt.lev
