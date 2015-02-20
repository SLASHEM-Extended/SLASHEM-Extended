SPECIAL_LEVELS += rndmazea.lev
dat/rndmazea.lev: dat/rndmazea.des util/lev_comp
	( cd dat ; ../util/lev_comp rndmazea.des )
