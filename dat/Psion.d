SPECIAL_LEVELS += Psi-strt.lev Psi-loca.lev Psi-goal.lev Psi-fila.lev Psi-filb.lev
dat/Psi-strt.lev: dat/Psion.des util/lev_comp
	( cd dat ; ../util/lev_comp Psion.des )
dat/Psi-loca.lev: dat/Psi-strt.lev
dat/Psi-goal.lev: dat/Psi-strt.lev
dat/Psi-fila.lev: dat/Psi-strt.lev
dat/Psi-filb.lev: dat/Psi-strt.lev
