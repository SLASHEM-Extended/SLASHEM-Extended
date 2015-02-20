SPECIAL_LEVELS += key.lev
dat/key.lev: dat/key.des util/lev_comp
	( cd dat ; ../util/lev_comp key.des )
