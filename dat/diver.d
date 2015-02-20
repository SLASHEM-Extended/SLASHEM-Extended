SPECIAL_LEVELS += Div-strt.lev Div-loca.lev Div-goal.lev Div-fila.lev Div-filb.lev
dat/Div-strt.lev: dat/diver.des util/lev_comp
	( cd dat ; ../util/lev_comp diver.des )
dat/Div-loca.lev: dat/Div-strt.lev
dat/Div-goal.lev: dat/Div-strt.lev
dat/Div-fila.lev: dat/Div-strt.lev
dat/Div-filb.lev: dat/Div-strt.lev
