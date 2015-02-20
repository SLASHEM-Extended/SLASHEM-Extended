SPECIAL_LEVELS += rndmaze.lev
dat/rndmaze.lev: dat/rndmaze.des util/lev_comp
	( cd dat ; ../util/lev_comp rndmaze.des )
