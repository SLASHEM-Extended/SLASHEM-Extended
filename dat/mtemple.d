SPECIAL_LEVELS += mtemple.lev
dat/mtemple.lev: dat/mtemple.des util/lev_comp
	( cd dat ; ../util/lev_comp mtemple.des )
