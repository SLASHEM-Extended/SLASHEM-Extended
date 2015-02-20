SPECIAL_LEVELS += town-1.lev town-2.lev
dat/town-1.lev: dat/town.des util/lev_comp
	( cd dat ; ../util/lev_comp town.des )
dat/town-2.lev: dat/town-1.lev
