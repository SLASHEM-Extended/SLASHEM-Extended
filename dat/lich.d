SPECIAL_LEVELS += lich.lev
dat/lich.lev: dat/lich.des util/lev_comp
	( cd dat ; ../util/lev_comp lich.des )
