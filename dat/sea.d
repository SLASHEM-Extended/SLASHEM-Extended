SPECIAL_LEVELS += sea.lev
dat/sea.lev: dat/sea.des util/lev_comp
	( cd dat ; ../util/lev_comp sea.des )
