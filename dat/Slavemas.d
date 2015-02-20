SPECIAL_LEVELS += Sla-strt.lev Sla-loca.lev Sla-goal.lev Sla-fila.lev Sla-filb.lev
dat/Sla-strt.lev: dat/Slavemas.des util/lev_comp
	( cd dat ; ../util/lev_comp Slavemas.des )
dat/Sla-loca.lev: dat/Sla-strt.lev
dat/Sla-goal.lev: dat/Sla-strt.lev
dat/Sla-fila.lev: dat/Sla-strt.lev
dat/Sla-filb.lev: dat/Sla-strt.lev
