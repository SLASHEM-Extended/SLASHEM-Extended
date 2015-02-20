SPECIAL_LEVELS += Sam-strt.lev Sam-loca.lev Sam-goal.lev Sam-fila.lev Sam-filb.lev
dat/Sam-strt.lev: dat/Samurai.des util/lev_comp
	( cd dat ; ../util/lev_comp Samurai.des )
dat/Sam-loca.lev: dat/Sam-strt.lev
dat/Sam-goal.lev: dat/Sam-strt.lev
dat/Sam-fila.lev: dat/Sam-strt.lev
dat/Sam-filb.lev: dat/Sam-strt.lev
