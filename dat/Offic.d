SPECIAL_LEVELS += Off-strt.lev Off-loca.lev Off-goal.lev Off-fila.lev Off-filb.lev
dat/Off-strt.lev: dat/Offic.des util/lev_comp
	( cd dat ; ../util/lev_comp Offic.des )
dat/Off-loca.lev: dat/Off-strt.lev
dat/Off-goal.lev: dat/Off-strt.lev
dat/Off-fila.lev: dat/Off-strt.lev
dat/Off-filb.lev: dat/Off-strt.lev
