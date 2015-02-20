SPECIAL_LEVELS += cav2fill.lev
dat/cav2fill.lev: dat/giants.des util/lev_comp
	( cd dat ; ../util/lev_comp giants.des )
