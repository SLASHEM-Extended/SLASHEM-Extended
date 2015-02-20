SPECIAL_LEVELS += Mah-strt.lev Mah-fila.lev Mah-loca.lev Mah-filb.lev Mah-goal.lev
dat/Mah-strt.lev: dat/Mahou.des util/lev_comp
	( cd dat ; ../util/lev_comp Mahou.des )
dat/Mah-fila.lev: dat/Mah-strt.lev
dat/Mah-loca.lev: dat/Mah-strt.lev
dat/Mah-filb.lev: dat/Mah-strt.lev
dat/Mah-goal.lev: dat/Mah-strt.lev
