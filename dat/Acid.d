SPECIAL_LEVELS += Aci-strt.lev Aci-loca.lev Aci-goal.lev Aci-fila.lev Aci-filb.lev
dat/Aci-strt.lev: dat/Acid.des util/lev_comp
	( cd dat ; ../util/lev_comp Acid.des )
dat/Aci-loca.lev: dat/Aci-strt.lev
dat/Aci-goal.lev: dat/Aci-strt.lev
dat/Aci-fila.lev: dat/Aci-strt.lev
dat/Aci-filb.lev: dat/Aci-strt.lev
